#ifndef CALDERADNP3_OUTSTATION_IO_TABLE_H
#define CALDERADNP3_OUTSTATION_IO_TABLE_H

#include "outstation/devices.h"

#include <opendnp3/outstation/DatabaseConfig.h>
#include <opendnp3/outstation/ICommandHandler.h>

#include <ftxui/dom/table.hpp>

#include <functional>
#include <vector>

class IOTable : public opendnp3::ICommandHandler
{
private:
    std::vector<std::shared_ptr<Binary>> binaryInputs;
    std::vector<std::shared_ptr<DoubleBit>> doubleBitInputs;
    std::vector<std::shared_ptr<Analog>> analogInputs;
    std::vector<std::shared_ptr<BinaryOutput>> binaryOutputs;
    std::vector<std::shared_ptr<AnalogOutput>> analogOutputs;

public:
    IOTable() = default;
    ~IOTable() override = default;
    static std::shared_ptr<IOTable> Create();

    opendnp3::DatabaseConfig ConfigureDatabase();
    void RegisterOutstation(std::shared_ptr<opendnp3::IOutstation> outstation);

    ftxui::Element RenderInputs();
    ftxui::Element RenderOutputs();

    // Add measurements to the IOTable
    void RegisterBinaryInput(std::shared_ptr<Binary> input);
    void RegisterDoubleBitInput(std::shared_ptr<DoubleBit> input);
    void RegisterAnalogInput(std::shared_ptr<Analog> input);
    void RegisterBinaryOutput(std::shared_ptr<BinaryOutput> output);
    void RegisterAnalogOutput(std::shared_ptr<AnalogOutput> output);

    // Add devices
    std::shared_ptr<SimpleDevice> CreateSimpleDevice(const std::string& name);
    std::shared_ptr<Breaker> CreateBreaker(const std::string& name, TwoSignalControlModel model);
    std::shared_ptr<SlowDevice> CreateSlowDevice(const std::string& name,
                                                 uint32_t runtimeMs,
                                                 TwoSignalControlModel model);
    std::shared_ptr<SetpointController> CreateSetpointController(const std::string& name,
                                                                 double setpoint,
                                                                 double variance,
                                                                 bool readOnly);

    // Implement opendnp3::ICommandHandler
    void Begin() override {};
    void End() override {};

    opendnp3::CommandStatus Select(const opendnp3::ControlRelayOutputBlock& command, uint16_t index) override;
    opendnp3::CommandStatus Operate(const opendnp3::ControlRelayOutputBlock& command,
                                    uint16_t index,
                                    opendnp3::IUpdateHandler& handler,
                                    opendnp3::OperateType opType) override;

    opendnp3::CommandStatus Select(const opendnp3::AnalogOutputInt16& command, uint16_t index) override;
    opendnp3::CommandStatus Operate(const opendnp3::AnalogOutputInt16& command,
                                    uint16_t index,
                                    opendnp3::IUpdateHandler& handler,
                                    opendnp3::OperateType opType) override;

    opendnp3::CommandStatus Select(const opendnp3::AnalogOutputInt32& command, uint16_t index) override;
    opendnp3::CommandStatus Operate(const opendnp3::AnalogOutputInt32& command,
                                    uint16_t index,
                                    opendnp3::IUpdateHandler& handler,
                                    opendnp3::OperateType opType) override;

    opendnp3::CommandStatus Select(const opendnp3::AnalogOutputFloat32& command, uint16_t index) override;
    opendnp3::CommandStatus Operate(const opendnp3::AnalogOutputFloat32& command,
                                    uint16_t index,
                                    opendnp3::IUpdateHandler& handler,
                                    opendnp3::OperateType opType) override;

    opendnp3::CommandStatus Select(const opendnp3::AnalogOutputDouble64& command, uint16_t index) override;
    opendnp3::CommandStatus Operate(const opendnp3::AnalogOutputDouble64& command,
                                    uint16_t index,
                                    opendnp3::IUpdateHandler& handler,
                                    opendnp3::OperateType opType) override;
};

#endif // CALDERADNP3_OUTSTATION_IO_TABLE_H
