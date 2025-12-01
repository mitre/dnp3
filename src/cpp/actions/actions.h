#ifndef CALDERADNP3_ACTIONS_H
#define CALDERADNP3_ACTIONS_H

#include "logger.hpp"

#include <opendnp3/ConsoleLogger.h>
#include <opendnp3/DNP3Manager.h>
#include <opendnp3/channel/PrintingChannelListener.h>
#include <opendnp3/gen/OperateType.h>
#include <opendnp3/gen/OperationType.h>
#include <opendnp3/gen/TripCloseCode.h>
#include <opendnp3/logging/LogLevels.h>
#include <opendnp3/master/CommandResultCallbackT.h>
#include <opendnp3/master/DefaultMasterApplication.h>
#include <opendnp3/master/PrintingCommandResultCallback.h>
#include <opendnp3/master/PrintingSOEHandler.h>
#include <opendnp3/util/StaticOnly.h>

#include <memory>
#include <string>
#include <vector>

class ConnectionSettings
{
public:
    // Shared
    uint16_t local_link = 1;
    uint16_t remote_link = 1024;
    opendnp3::ChannelRetry retry = opendnp3::ChannelRetry::Default();
    std::shared_ptr<opendnp3::IChannelListener> listener = StdErrLogger::Create();

    // TCP
    std::string ip = "127.0.0.1";
    uint16_t port = 20000;

    // Serial
    std::string device_name = "COM1";
    int baud = 9600;
    int data_bits = 8;
    std::string stop_bits = "One";
    std::string parity = "None";
    std::string flow_control = "None";
    int64_t delay = 500;

    ConnectionSettings() = default;
    ~ConnectionSettings() = default;

    opendnp3::SerialSettings SerialConfig() const
    {
        auto serial_config = opendnp3::SerialSettings();
        serial_config.deviceName = device_name;
        serial_config.baud = baud;
        serial_config.dataBits = data_bits;
        serial_config.stopBits = opendnp3::StopBitsSpec::from_string(stop_bits);
        serial_config.parity = opendnp3::ParitySpec::from_string(parity);
        serial_config.flowType = opendnp3::FlowControlSpec::from_string(flow_control);
        serial_config.asyncOpenDelay = opendnp3::TimeDuration::Milliseconds(delay);

        return serial_config;
    }

    opendnp3::IPEndpoint CreateEndpoint() const
    {
        return opendnp3::IPEndpoint(ip, port);
    }
};

class CrobSettings
{
public:
    std::string op_type = "PULSE_ON";
    std::string trip_code = "NUL";
    uint32_t on_time = 100;
    uint32_t off_time = 100;
    uint8_t count = 1;
    bool clear = false;

    CrobSettings() = default;
    ~CrobSettings() = default;

    opendnp3::ControlRelayOutputBlock Create() const
    {
        opendnp3::OperationType op = opendnp3::OperationTypeSpec::from_string(op_type);
        opendnp3::TripCloseCode tcc = opendnp3::TripCloseCodeSpec::from_string(trip_code);
        opendnp3::ControlRelayOutputBlock crob(op, tcc, clear, count, on_time, off_time);
        return crob;
    }
};

std::shared_ptr<opendnp3::IMaster> init_client(std::shared_ptr<opendnp3::IChannel> channel,
                                               uint16_t local_link,
                                               uint16_t remote_link,
                                               const std::string& id);

std::shared_ptr<opendnp3::IMaster> init_client(std::shared_ptr<opendnp3::IChannel> channel,
                                               uint16_t local_link,
                                               uint16_t remote_link);

enum class OperateMode
{
    DIRECT_OPERATE,
    SELECT_BEFORE_OPERATE,
    UNDEFINED
};

struct OperateModeSpec
{
    static OperateMode from_string(const std::string& arg)
    {
        if (arg == "DIRECT_OPERATE" || arg == "DO")
            return OperateMode::DIRECT_OPERATE;
        if (arg == "SELECT_BEFORE_OPERATE" || arg == "SELECT_OPERATE" || arg == "SBO")
            return OperateMode::SELECT_BEFORE_OPERATE;
        else
            return OperateMode::UNDEFINED;
    }
};

void operate(std::shared_ptr<opendnp3::IMaster> client,
             const opendnp3::ControlRelayOutputBlock& crob,
             const std::vector<uint16_t>& indices,
             OperateMode mode);

void read(std::shared_ptr<opendnp3::IMaster> client,
          uint8_t group,
          uint8_t variation,
          int32_t start_range,
          int32_t end_range);

void integrity_poll(std::shared_ptr<opendnp3::IMaster> client);

void restart_outstation(std::shared_ptr<opendnp3::IMaster> client, opendnp3::RestartType restart_type);

void disable_unsolicited_messages(std::shared_ptr<opendnp3::IMaster> client, const std::vector<uint8_t>& classes);

void enable_unsolicited_messages(std::shared_ptr<opendnp3::IMaster> client, const std::vector<uint8_t>& classes);

enum class AnalogOutputType
{
    INT16,
    INT32,
    FLOAT32,
    DOUBLE64,
    UNDEFINED
};

struct AnalogOutputTypeSpec
{
    static AnalogOutputType from_string(const std::string& arg)
    {
        if (arg == "INT16")
            return AnalogOutputType::INT16;
        if (arg == "INT32")
            return AnalogOutputType::INT32;
        if (arg == "FLOAT32")
            return AnalogOutputType::FLOAT32;
        if (arg == "DOUBLE64")
            return AnalogOutputType::DOUBLE64;
        else
            return AnalogOutputType::UNDEFINED;
    }
};

void set_analog_outputs(std::shared_ptr<opendnp3::IMaster> client,
                        const std::vector<uint16_t>& indices,
                        const std::vector<double>& values,
                        AnalogOutputType output_type);

#endif // CALDERADNP3_ACTIONS_H
