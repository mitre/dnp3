#include "logger.hpp"
#include "outstation/io_table.h"
#include "ui/renderer.h"

#include <opendnp3/DNP3Manager.h>
#include <opendnp3/outstation/DefaultOutstationApplication.h>

#include <CLI11/CLI11.hpp>
#include <fcntl.h>

#include <atomic>
#include <cstdio>
#include <iostream>

int main(int argc, char* argv[])
{
    bool tuiEnabled = false;
    std::string addr = "127.0.0.1";
    uint16_t clientLink = 1;
    uint16_t outstationLink = 1024;
    uint16_t port = 20000;

    auto logLevels = opendnp3::levels::NORMAL | opendnp3::levels::ALL_COMMS;

    CLI::App cli{"MITRE Caldera for OT DNP3 Outstation Simulator"};
    cli.add_flag("--tui", tuiEnabled, "enable the outstation visualization");
    cli.add_option("--ip", addr, "IP address to serve the outstation on (default: 127.0.0.1)")->check(CLI::ValidIPV4);
    cli.add_option("-p,--port", port, "port number of the outstation (default: 20000)");
    cli.add_option("--local", clientLink, "link layer address of the DNP3 client (default: 1)");
    cli.add_option("--remote", outstationLink, "link layer address of the DNP3 outstation (default: 1024)");
    CLI11_PARSE(cli, argc, argv);

    std::shared_ptr<TuiRenderer> tui = nullptr;
    std::shared_ptr<opendnp3::ILogHandler> logHandler;
    std::shared_ptr<opendnp3::IChannelListener> chListener;

    if (tuiEnabled)
    {
        tui = TuiRenderer::Create();
        auto logger = tui->GetLogger();
        logHandler = logger;
        chListener = logger;
    }
    else
    {
        auto logger = StdErrLogger::Create();
        logHandler = logger;
        chListener = logger;
    }

    const int numThreads = 1;
    opendnp3::DNP3Manager manager(numThreads, logHandler);
    auto channel = std::shared_ptr<opendnp3::IChannel>(nullptr);
    auto ip = opendnp3::IPEndpoint(addr, port);
    try
    {
        channel = manager.AddTCPServer("server", logLevels, opendnp3::ServerAcceptMode::CloseExisting, ip, chListener);
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << std::endl;
        std::cout << "failed to start server, exiting" << std::endl;
        return 1;
    }

    // Add devices to the outstation
    auto table = IOTable::Create();
    std::vector<std::shared_ptr<IDevice>> devices;

    auto temp = table->CreateSetpointController("Temperature", 62, 0.5, true);
    devices.push_back(temp);

    auto voltage = table->CreateSetpointController("Voltage", 480, 0.4, false);
    devices.push_back(voltage);

    auto freq = table->CreateSetpointController("Frequency", 60, 0.2, false);
    devices.push_back(freq);

    for (int i = 0; i < 5; ++i)
    {
        auto name = "Device " + std::to_string(i);
        auto device = table->CreateSimpleDevice(name);
        devices.push_back(device);
    }

    int breaker_num = 0;
    for (int i = 0; i < 5; ++i)
    {
        auto name = "Breaker " + std::to_string(breaker_num);
        auto device = table->CreateBreaker(name, TwoSignalControlModel::COMPLEMENTARY_TWO_OUTPUT);
        devices.push_back(device);
        breaker_num++;
    }
    for (int i = 0; i < 5; ++i)
    {
        auto name = "Breaker " + std::to_string(breaker_num);
        auto device = table->CreateBreaker(name, TwoSignalControlModel::ACTIVATION);
        devices.push_back(device);
        breaker_num++;
    }
    for (int i = 0; i < 3; ++i)
    {
        auto name = "Switch " + std::to_string(i);
        auto device = table->CreateSlowDevice(name, 10000, TwoSignalControlModel::ACTIVATION);
        devices.push_back(device);
    }

    if (tui)
    {
        tui->RegisterDevices(devices);
        tui->RegisterIOTable(table);
    }

    // Configure outstation
    opendnp3::OutstationStackConfig config(table->ConfigureDatabase());
    config.outstation.eventBufferConfig = opendnp3::EventBufferConfig::AllTypes(10);
    config.outstation.params.allowUnsolicited = true;
    config.link.KeepAliveTimeout = opendnp3::TimeDuration::Max();
    config.link.LocalAddr = outstationLink;
    config.link.RemoteAddr = clientLink;

    // Initialize the outstation and add the outstation to the channel
    auto app = opendnp3::DefaultOutstationApplication::Create();
    auto outstation = channel->AddOutstation("outstation", table, app, config);
    table->RegisterOutstation(outstation);
    outstation->Enable();

    if (tui)
    {
        tui->Init();
    }
    else
    {
        std::string input;
        while (true)
        {
            std::cin >> input;
            if ((input == "quit") || (input == "x") || (input == "exit") || (input == "q"))
            {
                return 0;
            }
        }
    }
    return EXIT_SUCCESS;
}
