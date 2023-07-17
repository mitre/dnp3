#include "CLI11.hpp"
#include "dnp3_actions.h"
#include "config.h"

#include <opendnp3/ConsoleLogger.h>
#include <opendnp3/DNP3Manager.h>
#include <opendnp3/channel/PrintingChannelListener.h>
#include <opendnp3/gen/OperateType.h>
#include <opendnp3/gen/OperationType.h>
#include <opendnp3/gen/TripCloseCode.h>
#include <opendnp3/logging/LogLevels.h>
#include <opendnp3/master/DefaultMasterApplication.h>
#include <opendnp3/master/PrintingCommandResultCallback.h>
#include <opendnp3/master/PrintingSOEHandler.h>

#include <list>
#include <Windows.h>
using namespace std;
using namespace opendnp3;

int main(int argc, char* argv[])
{
    CLI::App app{"DNP3 Action CLI"};
    std::cout << "Version: " << PROJECT_VER << std::endl;
    app.require_subcommand();

    // DNP3 Connection Options
    std::string ip{"127.0.0.1"};
    int port{20000};
    int local_link{0};
    int remote_link{0};

    app.add_option("IP", ip, "The IP address of the outstation to connect to.")->required();
    app.add_option("local-link", local_link, "The link layer address of the local DNP3 client.")->required();
    app.add_option("remote-link", remote_link, "The link layer address of the remote DNP3 outstation.")->required();
    app.add_option("-p,--port", port, "The port number of the outstation to connect to.");

    /****************************
      Read subcommand
    ****************************/
    int group = 0;
    int variation = 0;
    int start = 0;
    int end = 0;
    int step = 1;
    OperationType opType = OperationType::NUL;
    uint8_t tripCode = 0;

    auto read = app.add_subcommand("read",
                                   "Read the specified values from the outstation. This ability allows you to specify "
                                   "the object group/variation and indicies from which to read.");

    read->add_option("--group", group, "The object group of the indicies to read from.")->required();
    read->add_option("--variation", variation, "The object variation of the indicies to read from.")->required();
    read->add_option("--start", start, "The starting index from which to read from.")->required();
    read->add_option("--end", end, "The ending index from which to read from.")->required();

    /****************************
      Integrity poll subcommand
     ****************************/
    auto integ_poll = app.add_subcommand(
        "integrity-poll",
        "Collect all data from the outstation. This ability performs an integrity poll and allows you to collect "
        "all data for the object/group variations present on the outstation.");

    /****************************
      Cold Restart subcommand
    ****************************/
    auto cold_restart = app.add_subcommand(
        "cold-restart",
        "Perform a cold restart of the outstation. This ability allows you to perform a full restart of the "
        "outstation. WARNING: In previous runs, this has left the outstation in a broken state.");

    /****************************
      Warm Restart subcommand
    ****************************/
    auto warm_restart = app.add_subcommand(
        "warm-restart",
        "Perform a warm restart of the outstation. This ability allows you to perform a partial restart of the "
        "outstation. WARNING: In previous runs, this has left the outstation in a broken state.");

    /*****************************************
      Disable Unsolicited Messages subcommand
    ******************************************/
    std::vector<int> classes;
    auto disable_messages
        = app.add_subcommand("disable-messages",
                             "Disable unsolicited messages on the outstation for the specified classes. "
                             "This ability allows you to disable unsolicited messages on the outstation for the "
                             "specified classess which can result in clients connected to the outstation from not "
                             "receiving event data that would otherwise be self-reported by the outstation.");
    disable_messages
        ->add_option("--classes", classes,
                     "The message classes to disable unsolicited messages for (space-separated list).")
        ->required()
        ->expected(1, 4)
        ->check(CLI::Range(0, 3));

    /*****************************************
      Enable Unsolicited Messages subcommand
    ******************************************/
    auto enable_messages
        = app.add_subcommand("enable-messages",
                             "Enable unsolicited messages on the outstation for the specified classes. "
                             "This ability allows you to enable unsolicited messages on the outstation for the "
                             "specified classes which can result in the outstation self-reporting event data.");

    enable_messages->add_option("--classes", classes,
                     "The message classes to enable unsolicited messages for (space-separated list).")
        ->required()
        ->expected(1, 4)
        ->check(CLI::Range(0, 3));

    /*****************************************
      Select-before-Operate (SBO) subcommand
    ******************************************/
    auto sbo = app.add_subcommand("sbo",
                                  "Toggle on all specified points. This ability allows you to toggle on "
                                  "the range of specified points using the select-before-operate command sequence.");

    sbo->add_option("--op-type", opType, "...")->required();
    sbo->add_option("-t,--tcc", tripCode,
                    "The trip close code to use when pulsing the breaker on (NUL = 0, CLOSE = 1, TRIP = 2).")
        ->required();
    sbo->add_option("--start", start, "The starting index of breakers to toggle on.")->required();
    sbo->add_option("--end", end, "The ending index of breakers to toggle on.")->required();
    sbo->add_option("--step", step, "How much to increment on each iteration between start and end indexes.")
        ->required();

    /*****************************************
      SBO Pulse On - Range subcommand
    ******************************************/
    auto sbo_on = app.add_subcommand("sbo-on", "Select-before-Operate Pulse On for a range of indexes.");

    sbo_on->add_option("--start", start, "The start point index.")->required();
    sbo_on->add_option("--end", end, "The end point index.")->required();
    sbo_on->add_option("--step", step, "How much to increment on each iteration between start and end indexes.")
        ->required();
    sbo_on
        ->add_option("-t,--tcc", tripCode,
                     "The trip close code to use when pulsing the breaker on (NUL = 0, CLOSE = 1, TRIP = 2).")
        ->required();

    /*****************************************
      SBO Pulse Off - Range subcommand
    ******************************************/
    auto sbo_off = app.add_subcommand("sbo-off", "Select-before-Operate Pulse Off for a range of indexes.");
    sbo_off->add_option("--start", start, "The start point index.")->required();
    sbo_off->add_option("--end", end, "The end point index.")->required();
    sbo_off->add_option("--step", step, "How much to increment on each iteration between start and end indexes.")
        ->required();
    sbo_off
        ->add_option("-t,--tcc", tripCode,
                     "The trip close code to use when pulsing the breaker on (NUL = 0, CLOSE = 1, TRIP = 2).")
        ->required();

    /*****************************************
      Direct Operate (DO) Toggle On subcommand
    ******************************************/
    auto toggle_on_do
        = app.add_subcommand("toggle-on-do",
                             "Toggle on all specified breakers. This ability allows you to toggle on "
                             "the range of specified breakers using the direct-operate command sequence.");

    toggle_on_do->add_option("--start", start, "The starting index of breakers to toggle on.")->required();
    toggle_on_do->add_option("--end", end, "The ending index of breakers to toggle on.")->required();

    /*****************************************
      Direct Operate (DO) Toggle Off subcommand
    ******************************************/
    auto toggle_off_do
        = app.add_subcommand("toggle-off-do",
                             "Toggle off all specified breakers. This ability allows you to toggle off the range of "
                             "specified breakers using the direct-operate command sequence.");

    toggle_off_do->add_option("--start", start, "The starting index of breakers to toggle off.")->required();
    toggle_off_do->add_option("--end", end, "The ending index of breakers to toggle off.")->required();

    // Toggle breakers off/on select-before-operate subcommand
    int index = 0;
    int iterations = 1;
    int trip_on = 0;
    int trip_off = 0;
    int on_time = 0;
    int off_time = 0;
    auto toggle_sbo = app.add_subcommand("toggle-sbo",
                                         "Toggle the specified breaker on the outstation at a high frequency. This "
                                         "ability allows you to toggle the specified breaker on the outstation off and "
                                         "on at a high frequency using the select-before-operate command sequence.");
    toggle_sbo->add_option("--index", index, "The index of the breaker to toggle off and on at a high frequency.")
        ->required();
    toggle_sbo->add_option("--iterations", iterations, "The number of iterations to toggle the breaker off an on.");
    toggle_sbo
        ->add_option("--trip-on", trip_on,
                     "The trip close code to use when pulsing the breaker on (NUL = 0, CLOSE = 1, TRIP = 2).")
        ->required()
        ->check(CLI::Range(0, 2));
    toggle_sbo
        ->add_option("--trip-off", trip_off,
                     "The trip close code to use when pulsing the breaker off (NUL = 0, CLOSE = 1, TRIP = 2).")
        ->required()
        ->check(CLI::Range(0, 2));
    toggle_sbo->add_option("--on-time", on_time, "The time in milliseconds to pulse the breaker on.")->required();
    toggle_sbo->add_option("--off-time", off_time, "The time in milliseconds to pulse the breaker off.")->required();

    auto toggle_sbo_range
        = app.add_subcommand("toggle-sbo-range",
                             "Toggle the specified breaker on the outstation at a high frequency. This ability allows "
                             "you to toggle the specified breaker on the outstation off and on at a high frequency "
                             "using the select-before-operate command sequence.");
    toggle_sbo_range->add_option("--start", start, "The starting index of breakers to toggle off.")->required();
    toggle_sbo_range->add_option("--end", end, "The ending index of breakers to toggle off.")->required();
    toggle_sbo_range
        ->add_option("-i,--iterations", iterations, "The number of iterations to toggle the breaker off an on.")
        ->required();
    toggle_sbo_range
        ->add_option("--trip-on", trip_on,
                     "The trip close code to use when pulsing the breaker on (NUL = 0, CLOSE = 1, TRIP = 2).")
        ->required()
        ->check(CLI::Range(0, 2));
    toggle_sbo_range
        ->add_option("--trip-off", trip_off,
                     "The trip close code to use when pulsing the breaker off (NUL = 0, CLOSE = 1, TRIP = 2).")
        ->required()
        ->check(CLI::Range(0, 2));
    toggle_sbo_range->add_option("--on-time", on_time, "The time in milliseconds to pulse the breaker on.")->required();
    toggle_sbo_range->add_option("--off-time", off_time, "The time in milliseconds to pulse the breaker off.")
        ->required();

    // Toggle on breakers direct-operate subcommand
    auto toggle_do = app.add_subcommand("toggle-do",
                                        "Toggle the specified breaker on the outstation at a high frequency. This "
                                        "ability allows you to toggle the specified breaker on the outstation off and "
                                        "on at a high frequency using the direct-operate command sequence.");
    toggle_do->add_option("--index", index, "The index of the breaker to toggle off and on at a high frequency.")
        ->required();
    toggle_do->add_option("--iterations", iterations, "The number of iterations to toggle the breaker off an on.")
        ->required();
    toggle_do
        ->add_option("--trip-on", trip_on,
                     "The trip close code to use when pulsing the breaker on (NUL = 0, CLOSE = 1, TRIP = 2).")
        ->required()
        ->check(CLI::Range(0, 2));
    toggle_do
        ->add_option("--trip-off", trip_off,
                     "The trip close code to use when pulsing the breaker off (NUL = 0, CLOSE = 1, TRIP = 2).")
        ->required()
        ->check(CLI::Range(0, 2));
    toggle_do->add_option("--on-time", on_time, "The time in milliseconds to pulse the breaker on.");
    toggle_do->add_option("--off-time", off_time, "The time in milliseconds to pulse the breaker off.");

    /*****************************************
      Deny TCP Sessions subcommand
    ******************************************/
    int run_time = 0;
    int add_clients = 0;
    auto deny_sessions = app.add_subcommand("deny-sessions",
                                  "Repeatedly attempt TCP handshakes with one or more clients in order to prevent other sessions. "
                                  "Many devices only make available a small number of allowed active TCP sessions.");
    deny_sessions->add_option("run-time", run_time, "Length of time to do handshake for, 0 is infinite.")->required();
    deny_sessions->add_option("-c, --add-clients", add_clients, "Number of additional clients to use in addition to the base client.");

    // Parse arguments
    CLI11_PARSE(app, argc, argv);

    /**********************************
      Create DNP3 TCP Client
    ***********************************/
    std::cout << "Creating master client..." << std::endl;
    //auto master = config_master_client(ip, port, local_link, remote_link);
    // Specify logging level
    const auto logging = levels::NORMAL | levels::ALL_APP_COMMS;
    DNP3Manager manager(1, ConsoleLogger::Create());
    auto endpoint = IPEndpoint(ip, port);
    auto channel = manager.AddTCPClient("tcpclient", logging, ChannelRetry::Default(), {endpoint},
                                        "0.0.0.0", PrintingChannelListener::Create());

    // Master configuration
    MasterStackConfig config;
    config.master.responseTimeout = TimeDuration::Seconds(120);
    config.master.disableUnsolOnStartup = false;
    config.master.unsolClassMask = ClassField::None();
    config.master.startupIntegrityClassMask = ClassField::None();
    config.link.LocalAddr = local_link;
    config.link.RemoteAddr = remote_link;

    auto master = channel->AddMaster("master", PrintingSOEHandler::Create(), DefaultMasterApplication::Create(), config);
    master->Enable();

    // Let client get set up first
    this_thread::sleep_for(chrono::milliseconds(2000));

    // Based on subcommand specified, run the appropriate code.
    if (integ_poll->parsed())
    {
        CollectAllDataFromOutstation(master);
    }

    if (read->parsed())
    {
        ReadFromOutstation(master, group, variation, start, end);
    }

    if (cold_restart->parsed())
    {
        RestartOutstation(master, RestartType::COLD);
    }

    if (warm_restart->parsed())
    {
        RestartOutstation(master, RestartType::WARM);
    }

    if (disable_messages->parsed())
    {
        DisableUnsolicitedMessagesOnOutstation(master, classes);
    }

    if (enable_messages->parsed())
    {
        EnableUnsolicitedMessagesOnOutstation(master, classes);
    }

    if (sbo->parsed())
    {
        Sbo(master, opType, tripCode, start, end, step);
    }

    if (sbo_on->parsed())
    {
        Sbo(master, OperationType::PULSE_ON, tripCode, start, end, step);
    }

    if (sbo_off->parsed())
    {
        Sbo(master, OperationType::PULSE_OFF, tripCode, start, end, step);
    }

    if (toggle_sbo_range->parsed())
    {
        BruteForceOutstation(master, 0x0, start, end, iterations, trip_on, trip_off, on_time, off_time);
    }

    if (toggle_on_do->parsed())
    {
        ToggleOnAllBreakers(master, 0x1, start, end, step);
    }

    if (toggle_off_do->parsed())
    {
        ToggleOffAllBreakers(master, 0x1, start, end, step);
    }

    if (toggle_sbo->parsed())
    {
        BruteForceOutstation(master, 0x0, index, index, iterations, trip_on, trip_off, on_time, off_time);
    }

    if (toggle_do->parsed())
    {
        BruteForceOutstation(master, 0x1, index, index, iterations, trip_on, trip_off, on_time, off_time);
    }

    if (deny_sessions->parsed())
    {
        //DenyTcpSessions(shared_ptr<IMaster> master, DNP3Manager* manager, IPEndpoint* endpoint, MasterStackConfig config, int add_clients, int run_time)
        DenyTcpSessions(master, &manager, &endpoint, config, add_clients, run_time);
    }

    return 0;
}