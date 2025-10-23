#include "outstation/io_table.h"

std::shared_ptr<IOTable> IOTable::Create()
{
    return std::make_shared<IOTable>();
}

opendnp3::DatabaseConfig IOTable::ConfigureDatabase()
{
    opendnp3::DatabaseConfig config;

    for (size_t i = 0; i < binaryInputs.size(); ++i)
    {
        auto idx = static_cast<uint16_t>(i);
        config.binary_input[idx] = binaryInputs[idx]->Config();
    }

    for (size_t i = 0; i < doubleBitInputs.size(); ++i)
    {
        auto idx = static_cast<uint16_t>(i);
        config.double_binary[idx] = doubleBitInputs[idx]->Config();
    }

    for (size_t i = 0; i < analogInputs.size(); ++i)
    {
        auto idx = static_cast<uint16_t>(i);
        config.analog_input[idx] = analogInputs[idx]->Config();
    }

    return config;
}

void IOTable::RegisterOutstation(std::shared_ptr<opendnp3::IOutstation> outstation)
{
    // Write(Read()) inserted to initialize values in the outstation.
    for (const auto& input : binaryInputs)
    {
        input->RegisterOutstation(outstation);
        input->Write(input->Read());
    }

    for (const auto& input : doubleBitInputs)
    {
        input->RegisterOutstation(outstation);
        input->Write(input->Read());
    }

    for (const auto& input : analogInputs)
    {
        input->RegisterOutstation(outstation);
        input->Write(input->Read());
    }
}

ftxui::Element IOTable::RenderInputs()
{
    std::vector<std::vector<std::string>> data;
    data.push_back({"Group", "Index", "Name", "Value"});

    for (auto binary : binaryInputs)
    {
        auto group = binary->GetStaticGroup();
        auto groupStr = std::to_string(group);

        auto index = binary->GetIndex();
        if (!index.has_value())
        {
            continue;
        }
        auto indexStr = std::to_string(index.value());

        std::string value = "FALSE";
        if (binary->Read())
        {
            value = "TRUE";
        }

        data.push_back({
            groupStr,
            indexStr,
            binary->GetName(),
            value,
        });
    }

    for (auto doubleBit : doubleBitInputs)
    {
        auto group = doubleBit->GetStaticGroup();
        auto groupStr = std::to_string(group);

        auto index = doubleBit->GetIndex();
        if (!index.has_value())
        {
            continue;
        }
        auto indexStr = std::to_string(index.value());

        auto value = opendnp3::DoubleBitSpec::to_human_string(doubleBit->Read());
        data.push_back({
            groupStr,
            indexStr,
            doubleBit->GetName(),
            value,
        });
    }

    for (auto analog : analogInputs)
    {
        auto group = analog->GetStaticGroup();
        auto groupStr = std::to_string(group);

        auto index = analog->GetIndex();
        if (!index.has_value())
        {
            continue;
        }
        auto indexStr = std::to_string(index.value());

        auto value = std::to_string(analog->Read());
        data.push_back({
            groupStr,
            indexStr,
            analog->GetName(),
            value,
        });
    }

    auto table = ftxui::Table(data);

    // Add border around full table
    table.SelectAll().Border(ftxui::LIGHT);

    // Add border between input types
    table.SelectRows(0, size(binaryInputs)).Border(ftxui::LIGHT);
    table.SelectRows(0, size(binaryInputs) + size(doubleBitInputs)).Border(ftxui::LIGHT);

    // Decorate header row
    table.SelectRow(0).Decorate(ftxui::bold);
    table.SelectRow(0).SeparatorVertical(ftxui::LIGHT);
    table.SelectRow(0).Border(ftxui::DOUBLE);

    return table.Render();
}

ftxui::Element IOTable::RenderOutputs()
{
    std::vector<std::vector<std::string>> data;
    data.push_back({"Index", "Name"});

    for (auto binary : binaryOutputs)
    {
        auto index = binary->GetIndex();
        if (!index.has_value())
        {
            continue;
        }

        auto indexStr = std::to_string(index.value());
        data.push_back({
            indexStr,
            binary->GetName(),
        });
    }

    for (auto analog : analogOutputs)
    {
        auto index = analog->GetIndex();
        if (!index.has_value())
        {
            continue;
        }

        auto indexStr = std::to_string(index.value());
        data.push_back({
            indexStr,
            analog->GetName(),
        });
    }

    auto table = ftxui::Table(data);

    // Add border around full table
    table.SelectAll().Border(ftxui::LIGHT);

    // Add border between output types
    table.SelectRows(0, size(binaryOutputs)).Border(ftxui::LIGHT);

    // Decorate header row
    table.SelectRow(0).Decorate(ftxui::bold);
    table.SelectRow(0).SeparatorVertical(ftxui::LIGHT);
    table.SelectRow(0).Border(ftxui::DOUBLE);

    return table.Render();
}

void IOTable::RegisterBinaryInput(std::shared_ptr<Binary> input)
{
    binaryInputs.push_back(input);
    uint16_t idx = static_cast<uint16_t>(binaryInputs.size()) - 1;
    input->SetIndex(idx);
}

void IOTable::RegisterDoubleBitInput(std::shared_ptr<DoubleBit> input)
{
    doubleBitInputs.push_back(input);
    uint16_t idx = static_cast<uint16_t>(doubleBitInputs.size()) - 1;
    input->SetIndex(idx);
}

void IOTable::RegisterAnalogInput(std::shared_ptr<Analog> input)
{
    analogInputs.push_back(input);
    uint16_t idx = static_cast<uint16_t>(analogInputs.size()) - 1;
    input->SetIndex(idx);
}

void IOTable::RegisterBinaryOutput(std::shared_ptr<BinaryOutput> output)
{
    binaryOutputs.push_back(output);
    uint16_t idx = static_cast<uint16_t>(binaryOutputs.size()) - 1;
    output->SetIndex(idx);
}

void IOTable::RegisterAnalogOutput(std::shared_ptr<AnalogOutput> output)
{
    analogOutputs.push_back(output);
    uint16_t idx = static_cast<uint16_t>(analogOutputs.size()) - 1;
    output->SetIndex(idx);
}

std::shared_ptr<SimpleDevice> IOTable::CreateSimpleDevice(const std::string& name)
{
    auto input = Binary::Create();
    RegisterBinaryInput(input);

    auto device = SimpleDevice::Create(name);
    device->AssignInputPoint(input);

    auto control = device->CreateControlSignal();
    RegisterBinaryOutput(control);

    return device;
}

std::shared_ptr<Breaker> IOTable::CreateBreaker(const std::string& name, TwoSignalControlModel model)
{
    auto input = Binary::Create();
    RegisterBinaryInput(input);

    auto breaker = Breaker::Create(name, model);
    breaker->AssignInputPoint(input);

    for (auto control : breaker->CreateControlSignal())
    {
        RegisterBinaryOutput(control);
    }

    return breaker;
}

std::shared_ptr<SlowDevice> IOTable::CreateSlowDevice(const std::string& name,
                                                      uint32_t runtimeMs,
                                                      TwoSignalControlModel model)
{
    auto input = DoubleBit::Create();
    RegisterDoubleBitInput(input);

    auto device = SlowDevice::Create(name, runtimeMs, model);
    device->AssignInputPoint(input);

    for (auto control : device->CreateControlSignal())
    {
        RegisterBinaryOutput(control);
    }

    return device;
}

std::shared_ptr<SetpointController> IOTable::CreateSetpointController(const std::string& name,
                                                                      double setpoint,
                                                                      double variance,
                                                                      bool readOnly)
{
    auto input = Analog::Create();
    RegisterAnalogInput(input);

    auto device = SetpointController::Create(name, setpoint, variance);
    device->AssignInputPoint(input);

    if (readOnly)
    {
        return device;
    }

    auto output = device->CreateControlSignal();
    RegisterAnalogOutput(output);

    return device;
}

// Implement ICommandHandler

opendnp3::CommandStatus IOTable::Select(const opendnp3::ControlRelayOutputBlock& command, uint16_t index)
{
    if (index >= binaryOutputs.size())
    {
        return opendnp3::CommandStatus::OUT_OF_RANGE;
    }

    return opendnp3::CommandStatus::SUCCESS;
}

opendnp3::CommandStatus IOTable::Operate(const opendnp3::ControlRelayOutputBlock& command,
                                         uint16_t index,
                                         opendnp3::IUpdateHandler& handler,
                                         opendnp3::OperateType opType)
{
    if (index >= binaryOutputs.size())
    {
        return opendnp3::CommandStatus::OUT_OF_RANGE;
    }

    return binaryOutputs[index]->Operate(command, handler, opType);
}

opendnp3::CommandStatus IOTable::Select(const opendnp3::AnalogOutputInt16& command, uint16_t index)
{
    return opendnp3::CommandStatus::NOT_SUPPORTED;
}

opendnp3::CommandStatus IOTable::Operate(const opendnp3::AnalogOutputInt16& command,
                                         uint16_t index,
                                         opendnp3::IUpdateHandler& handler,
                                         opendnp3::OperateType opType)
{
    return opendnp3::CommandStatus::NOT_SUPPORTED;
}

opendnp3::CommandStatus IOTable::Select(const opendnp3::AnalogOutputInt32& command, uint16_t index)
{
    return opendnp3::CommandStatus::NOT_SUPPORTED;
}

opendnp3::CommandStatus IOTable::Operate(const opendnp3::AnalogOutputInt32& command,
                                         uint16_t index,
                                         opendnp3::IUpdateHandler& handler,
                                         opendnp3::OperateType opType)
{
    return opendnp3::CommandStatus::NOT_SUPPORTED;
}

opendnp3::CommandStatus IOTable::Select(const opendnp3::AnalogOutputFloat32& command, uint16_t index)
{
    return opendnp3::CommandStatus::NOT_SUPPORTED;
}

opendnp3::CommandStatus IOTable::Operate(const opendnp3::AnalogOutputFloat32& command,
                                         uint16_t index,
                                         opendnp3::IUpdateHandler& handler,
                                         opendnp3::OperateType opType)
{
    return opendnp3::CommandStatus::NOT_SUPPORTED;
}

opendnp3::CommandStatus IOTable::Select(const opendnp3::AnalogOutputDouble64& command, uint16_t index)
{
    if (index >= analogOutputs.size())
    {
        return opendnp3::CommandStatus::OUT_OF_RANGE;
    }

    return opendnp3::CommandStatus::SUCCESS;
}

opendnp3::CommandStatus IOTable::Operate(const opendnp3::AnalogOutputDouble64& command,
                                         uint16_t index,
                                         opendnp3::IUpdateHandler& handler,
                                         opendnp3::OperateType opType)
{
    if (index >= analogOutputs.size())
    {
        return opendnp3::CommandStatus::OUT_OF_RANGE;
    }

    return analogOutputs[index]->Operate(command, handler, opType);
}