#include "actions.h"
#include "config.h"

#include <CLI11/CLI11.hpp>

#include <array>
#include <functional>
#include <thread>

int main(int argc, char* argv[])
{

    CLI::App app{"DNP3 Client Action Library " + std::string(PROJECT_VER)};
    app.set_help_all_flag("--help-all", "Expand all help");
    app.require_subcommand(1); // require 1 connection type subcommand
    auto display_version = [](int) { std::cout << "DNP3 Client Action Library " << PROJECT_VER << std::endl; };
    app.add_flag_function("--version", display_version, "display version");

    opendnp3::DNP3Manager manager(1, StdErrLogger::Create());
    std::shared_ptr<opendnp3::IMaster> client;
    const opendnp3::LogLevels levels = opendnp3::levels::NORMAL | opendnp3::levels::ALL_APP_COMMS;

    ConnectionSettings connection_args;
    const uint16_t startup_ms = 10; 
    const uint16_t shutdown_ms = 10; 

    /*************************
      TCP Client Subcommmand
    **************************/

    auto tcp = app.add_subcommand("tcp", "establish a TCP connection with an outstation.");
    tcp->add_option("ip", connection_args.ip, "IP address of the outstation to connect to")
        ->check(CLI::ValidIPV4)
        ->required();
    tcp->add_option("local", connection_args.local_link, "link layer address of the local DNP3 client")->required();
    tcp->add_option("remote", connection_args.remote_link, "link layer address of the remote DNP3 outstation")
        ->required();
    tcp->add_option("-p,--port", connection_args.port, "port number of the outstation (default: 20000)");
    tcp->require_subcommand(1); // require 1 action subcommand
    tcp->parse_complete_callback([&connection_args, &manager, &levels, &client, &startup_ms]() {
        auto ip = connection_args.CreateEndpoint();
        auto channel = manager.AddTCPClient("tcpchannel", levels, connection_args.retry, {ip}, "0.0.0.0",
                                            connection_args.listener);
        client = init_client(channel, connection_args.local_link, connection_args.remote_link);
        client->Enable();

        // Sleep to allow client get set up
        std::this_thread::sleep_for(std::chrono::milliseconds(startup_ms));
    });

    /****************************
      Serial Client Subcommmand
    *****************************/
    std::array<std::string, 4> stop_bit_opts = {"One", "OnePointFive", "Two", "None"};
    std::array<std::string, 3> parity_opts = {"Even", "Odd", "None"};
    std::array<std::string, 3> flow_control_opts = {"Hardware", "XONXOFF", "None"};

    auto serial = app.add_subcommand("serial", "Establish a serial connection with an outstation.");
    serial->add_option("device-name", connection_args.device_name, "serial device name (e.g COM1, /dev/ttyS0)")
        ->required();
    serial->add_option("local", connection_args.local_link, "link layer address of the local DNP3 client")->required();
    serial->add_option("remote", connection_args.remote_link, "link layer address of the remote DNP3 outstation")
        ->required();
    serial->add_option("-b,--baud", connection_args.baud, "baud rate of the serial device (default: 9600)");
    serial->add_option("--databits", connection_args.data_bits, "data bits (default: 8)");
    serial->add_option("--stopbits", connection_args.stop_bits, "stop bits (default: One)")
        ->check(CLI::IsMember(stop_bit_opts));
    serial->add_option("--parity", connection_args.parity, "parity setting for the port (default: None)")
        ->check(CLI::IsMember(parity_opts));
    serial->add_option("--flowtype", connection_args.flow_control, "flow control setting (default: None)")
        ->check(CLI::IsMember(flow_control_opts));
    serial->add_option("--delay", connection_args.delay, "delay time in milliseconds before first tx (default: 500)");
    serial->require_subcommand(1); // require 1 action subcommand
    serial->parse_complete_callback([&connection_args, &manager, &levels, &client, &startup_ms]() {
        opendnp3::SerialSettings config = connection_args.SerialConfig();
        auto channel
            = manager.AddSerial("serialchannel", levels, connection_args.retry, config, connection_args.listener);
        client = init_client(channel, connection_args.local_link, connection_args.remote_link);
        client->Enable();

        // Sleep to allow client get set up
        std::this_thread::sleep_for(std::chrono::milliseconds(startup_ms));
    });

    std::vector<CLI::App*> subcommands = app.get_subcommands({});

    /***********************
      'operate' subcommand
    ************************/
    CrobSettings crob_args;
    std::vector<uint16_t> indices = {1};
    std::array<std::string, 5> op_modes = {"DIRECT_OPERATE", "DO", "SELECT_BEFORE_OPERATE", "SELECT_OPERATE", "SBO"};
    std::string op_mode = op_modes[0]; // Default to DIRECT_OPERATE
    std::array<std::string, 5> op_types = {"NUL", "PULSE_ON", "PULSE_OFF", "LATCH_ON", "LATCH_OFF"};
    std::array<std::string, 3> trip_codes = {"NUL", "CLOSE", "TRIP"};

    for (CLI::App* sub : subcommands)
    {
        auto operate_cmd = sub->add_subcommand("operate", "operate specified points.");
        operate_cmd->add_option("mode", op_mode, "command sequence")->required()->check(CLI::IsMember(op_modes));
        operate_cmd->add_option("--indices", indices, "comma separated list of points to operate")
            ->required()
            ->delimiter(',');
        operate_cmd->add_option("--op-type", crob_args.op_type, "control operation (default: PULSE_ON)")
            ->check(CLI::IsMember(op_types));
        operate_cmd->add_option("--tcc", crob_args.trip_code, "trip close code (default: NUL)")
            ->check(CLI::IsMember(trip_codes));
        operate_cmd->add_option("--on", crob_args.on_time, "on-time value in ms (default: 100ms)");
        operate_cmd->add_option("--off", crob_args.off_time, "off-time value in ms (default: 100ms)");
        operate_cmd->add_option("--count", crob_args.count, "number of times to send the signal (default: 1)");
        operate_cmd->add_flag("--clear", crob_args.clear, "set the control code clear bit");
        operate_cmd->final_callback([&client, &op_mode, &crob_args, &indices]() {
            auto mode = OperateModeSpec::from_string(op_mode);
            auto crob = crob_args.Create();
            operate(client, crob, indices, mode);
        });
    }

    /********************
      'read' subcommand
    *********************/
    uint8_t group = 0;
    uint8_t variation = 0;
    int32_t start = -1;
    int32_t end = -1;

    for (CLI::App* sub : subcommands)
    {
        auto read_cmd = sub->add_subcommand("read", "read the specified values from the outstation.");
        read_cmd->add_option("group", group, "group of indices to read")->required();
        read_cmd->add_option("--start", start, "first index to read (inclusive)");
        read_cmd->add_option("--end", end, "last index to read (inclusive)");
        read_cmd->add_option("--variation", variation, "variation (format) to read");
        read_cmd->final_callback(
            [&client, &group, &variation, &start, &end]() { read(client, group, variation, start, end); });
    }
    
    /******************************
      'integrity-poll' subcommand
    *******************************/
    for (auto sub : subcommands)
    {
        auto integ_poll = sub->add_subcommand("integrity-poll", "read all data groups present on the outstation.");
        integ_poll->final_callback([&client]() { integrity_poll(client); });
    }

    /****************************
      'cold-restart' subcommand
    ****************************/
    for (auto sub : subcommands)
    {
        auto cold_restart = sub->add_subcommand("cold-restart",
                                                "perform a cold restart of the outstation. WARNING: "
                                                "may leave the outstation in a broken state.");
        cold_restart->final_callback([&client]() { restart_outstation(client, opendnp3::RestartType::COLD); });
    }

    /****************************
      'warm-restart' subcommand
    ****************************/
    for (auto sub : subcommands)
    {
        auto warm_restart = sub->add_subcommand("warm-restart",
                                                "perform a warm restart of the outstation. WARNING: "
                                                "may leave the outstation in a broken state.");
        warm_restart->final_callback([&client]() { restart_outstation(client, opendnp3::RestartType::WARM); });
    }

    /**********************************
     'disable-unsolicited' subcommand
    ***********************************/
    std::vector<uint8_t> classes;
    for (auto sub : subcommands)
    {
        auto disable_unsol = sub->add_subcommand(
            "disable-unsolicited", "disable unsolicited messages on the outstation for the specified classes.");
        disable_unsol->add_option("classes", classes, "comma separated list of message classes to disable")
            ->required()
            ->delimiter(',')
            ->expected(1, 4)
            ->check(CLI::Range(0, 3));
        disable_unsol->final_callback([&client, &classes]() { disable_unsolicited_messages(client, classes); });
    }

    /**********************************
      'enable-unsolicited' subcommand
    ***********************************/
    for (auto sub : subcommands)
    {
        auto enable_unsol = sub->add_subcommand(
            "enable-unsolicited", "enable unsolicited messages on the outstation for the specified classes.");
        enable_unsol->add_option("classes", classes, "comma separated list of message classes to disable")
            ->required()
            ->delimiter(',')
            ->expected(1, 4)
            ->check(CLI::Range(0, 3));
        enable_unsol->final_callback([&client, &classes]() { enable_unsolicited_messages(client, classes); });
    }

    /**************************
      Toggle command settings
    ***************************/
    std::vector<uint16_t> trip_indices;
    std::vector<uint16_t> close_indices;
    CrobSettings toggle_crob_args;
    uint32_t delay_ms = 2000;
    uint8_t iterations = 1;

    /*********************************
      'toggle-activation' subcommand
    **********************************/
    for (CLI::App* sub : subcommands)
    {
        auto toggle_cmd = sub->add_subcommand(
            "toggle-activation", "Toggle breaker(s) using activation model (uses separate trip and close indices).");
        toggle_cmd->add_option("mode", op_mode, "command sequence")->required()->check(CLI::IsMember(op_modes));
        toggle_cmd->add_option("--trip-indices", trip_indices, "comma separated list of points to trip")
            ->required()
            ->delimiter(',');
        toggle_cmd->add_option("--close-indices", close_indices, "comma separated list of points to close")
            ->required()
            ->delimiter(',');
        toggle_cmd->add_option("--on", toggle_crob_args.on_time, "signal on-time in ms (default: 100ms)");
        toggle_cmd->add_option("--off", toggle_crob_args.off_time, "signal off-time in ms (default: 100ms)");
        toggle_cmd->add_option("--iterations", iterations, "number of times to repeat the operation (default: 1)");
        toggle_cmd->add_option("--delay", delay_ms, "delay between trip and close operations in ms (default: 2000ms)");

        toggle_cmd->final_callback(
            [&client, &op_mode, &toggle_crob_args, &trip_indices, &close_indices, &iterations, &delay_ms]() {

            auto mode = OperateModeSpec::from_string(op_mode);
            // Trip crob: PULSE_ON / TRIP
            toggle_crob_args.trip_code = "TRIP";
            auto tripCrob = toggle_crob_args.Create();
            // Close crob: PULSE_ON / CLOSE
            toggle_crob_args.trip_code = "CLOSE";
            auto closeCrob = toggle_crob_args.Create();

            for (uint8_t i = 0; i < iterations; i++)
            {
                operate(client, tripCrob, trip_indices, mode);
                std::this_thread::sleep_for(std::chrono::milliseconds(delay_ms));
                operate(client, closeCrob, close_indices, mode);

                if (i < iterations - 1)
                {
                    std::this_thread::sleep_for(std::chrono::milliseconds(delay_ms));
                }
            }
        });
    }

    /*********************************
      'toggle-two-output' subcommand
    **********************************/
    for (CLI::App* sub : subcommands)
    {
        auto toggle_cmd = sub->add_subcommand(
            "toggle-two-output",
            "Toggle breaker(s) using complementary two-output model (uses TRIP/CLOSE trip codes on the same index).");
        toggle_cmd->add_option("mode", op_mode, "command sequence")->required()->check(CLI::IsMember(op_modes));
        toggle_cmd->add_option("--indices", indices, "comma separated list of points to operate")
            ->required()
            ->delimiter(',');
        toggle_cmd->add_option("--on", toggle_crob_args.on_time, "signal on-time in ms (default: 100ms)");
        toggle_cmd->add_option("--off", toggle_crob_args.off_time, "signal off-time in ms (default: 100ms)");
        toggle_cmd->add_option("--iterations", iterations, "number of times to repeat the operation (default: 1)");
        toggle_cmd->add_option("--delay", delay_ms, "delay between trip and close operations in ms (default: 2000ms)");

        toggle_cmd->final_callback([&client, &op_mode, &toggle_crob_args, &indices, &iterations, &delay_ms]() {

            auto mode = OperateModeSpec::from_string(op_mode);
            // Trip crob: PULSE_ON / TRIP
            toggle_crob_args.trip_code = "TRIP";
            auto tripCrob = toggle_crob_args.Create();
            // Close crob: PULSE_ON / CLOSE
            toggle_crob_args.trip_code = "CLOSE";
            auto closeCrob = toggle_crob_args.Create();

            for (uint8_t i = 0; i < iterations; i++)
            {
                operate(client, tripCrob, indices, mode);
                std::this_thread::sleep_for(std::chrono::milliseconds(delay_ms));
                operate(client, closeCrob, indices, mode);

                if (i < iterations - 1)
                {
                    std::this_thread::sleep_for(std::chrono::milliseconds(delay_ms));
                }
            }
        });
    }

    /**************************
      'set-analog' subcommand
    ***************************/
    std::vector<uint16_t> analog_indices;
    std::vector<double> analog_values;
    std::array<std::string, 4> dtypes = {"INT16", "INT32", "FLOAT32", "DOUBLE64"};
    std::string dtype = dtypes[3]; // default to DOUBLE64

    for (CLI::App* sub : subcommands)
    {
        auto set_analog_cmd = sub->add_subcommand("set-analog", "set analog output value(s)");
        set_analog_cmd->add_option("--indices", analog_indices, "comma separated list of analog point indices")
            ->required()
            ->delimiter(',');
        set_analog_cmd->add_option("--values", analog_values, "comma separated list of values to set")
            ->required()
            ->delimiter(',');
        set_analog_cmd->add_option("--dtype", dtype, "datatype of values (default: DOUBLE64)")
            ->check(CLI::IsMember(dtypes));
        set_analog_cmd->final_callback([&client, &analog_indices, &analog_values, &dtype]() {
            auto output_type = AnalogOutputTypeSpec::from_string(dtype);
            set_analog_outputs(client, analog_indices, analog_values, output_type);
        });
    }

    // Parse arguments
    CLI11_PARSE(app, argc, argv);
    // Sleep to allow communications thread to execute 
    std::this_thread::sleep_for(std::chrono::milliseconds(shutdown_ms));
    return 0;
}
