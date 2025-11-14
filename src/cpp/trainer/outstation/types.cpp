#include "outstation/types.h"

#include <opendnp3/outstation/UpdateBuilder.h>

#include <unordered_set>

opendnp3::DNPTime now()
{
    auto duration = std::chrono::system_clock::now().time_since_epoch();
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();
    return opendnp3::DNPTime(ms);
}

// IMeasurement

void IMeasurement::SetName(const std::string& name_input)
{
    std::string modified_name = name_input;
    std::unordered_set<char> invalid_chars = {' ', '.', ','};
    for (char& ch : modified_name)
    {
        if (invalid_chars.find(ch) != invalid_chars.end())
        {
            ch = '_';
        }
    }
    name = modified_name;
}

const std::string& IMeasurement::GetName() const
{
    return name;
}

void IMeasurement::SetIndex(uint16_t idx)
{
    index = idx;
}

std::optional<uint16_t> IMeasurement::GetIndex() const
{
    return index;
}

void IMeasurement::RegisterOutstation(std::shared_ptr<opendnp3::IOutstation> os)
{
    outstation = os;
}

void IMeasurement::UpdateOutstation(const opendnp3::Updates& updates)
{
    if (!outstation)
    {
        return;
    }
    outstation->Apply(updates);
}

// Binary

std::shared_ptr<Binary> Binary::Create()
{
    return std::make_shared<Binary>();
}

bool Binary::Read() const
{
    return value.load();
}

void Binary::Write(bool v)
{
    value.store(v);

    auto idx = GetIndex();
    if (!idx.has_value())
    {
        return;
    }

    auto builder = opendnp3::UpdateBuilder();
    builder.Update(opendnp3::Binary(v, opendnp3::Flags(0x1), now()), idx.value());
    UpdateOutstation(builder.Build());
}

opendnp3::BinaryConfig Binary::Config() const
{
    return {svariation, point_class, evariation};
}

void Binary::SetClass(opendnp3::PointClass c)
{
    point_class = c;
}

void Binary::SetVariation(opendnp3::StaticBinaryVariation v)
{
    svariation = v;
}

void Binary::SetEventVariation(opendnp3::EventBinaryVariation v)
{
    evariation = v;
}

uint8_t Binary::GetStaticGroup() const
{
    return 1;
}

// BinaryOutput

opendnp3::CommandStatus BinaryOutput::Operate(opendnp3::ControlRelayOutputBlock crob,
                                              opendnp3::IUpdateHandler& handler,
                                              opendnp3::OperateType opType)
{
    return opendnp3::CommandStatus::NOT_SUPPORTED;
}

// TwoSignalControlModel

uint8_t TwoSignalControlModelSpec::to_type(TwoSignalControlModel arg)
{
    return static_cast<uint8_t>(arg);
}

TwoSignalControlModel TwoSignalControlModelSpec::from_type(uint8_t arg)
{
    switch (arg)
    {
    case 0:
        return TwoSignalControlModel::ACTIVATION;
    case 1:
        return TwoSignalControlModel::COMPLEMENTARY_TWO_OUTPUT;
    default:
        return TwoSignalControlModel::UNDEFINED;
    }
}

char const* TwoSignalControlModelSpec::to_string(TwoSignalControlModel arg)
{
    switch (arg)
    {
    case TwoSignalControlModel::ACTIVATION:
        return "ACTIVATION";
    case TwoSignalControlModel::COMPLEMENTARY_TWO_OUTPUT:
        return "COMPLEMENTARY_TWO_OUTPUT";
    default:
        return "UNDEFINED";
    }
}

char const* TwoSignalControlModelSpec::to_human_string(TwoSignalControlModel arg)
{
    switch (arg)
    {
    case TwoSignalControlModel::ACTIVATION:
        return "Activation Model";
    case TwoSignalControlModel::COMPLEMENTARY_TWO_OUTPUT:
        return "Complementary Two-Output Model";
    default:
        return "Undefined";
    }
}

TwoSignalControlModel TwoSignalControlModelSpec::from_string(const std::string& arg)
{
    if (arg == "ACTIVATION")
        return TwoSignalControlModel::ACTIVATION;
    if (arg == "COMPLEMENTARY_TWO_OUTPUT")
        return TwoSignalControlModel::COMPLEMENTARY_TWO_OUTPUT;
    return TwoSignalControlModel::UNDEFINED;
}

// TwoSignalControl

TwoSignalControl::TwoSignalControl(TwoSignalControlModel model,
                                   std::shared_ptr<BinaryAction> tripAction,
                                   std::shared_ptr<BinaryAction> closeAction)
    : controlModel(model)
{
    switch (controlModel)
    {
    case TwoSignalControlModel::ACTIVATION:
        outputOne = ActivationOutput::Create(tripAction, false);
        outputTwo = ActivationOutput::Create(closeAction, false);
        break;
    case TwoSignalControlModel::COMPLEMENTARY_TWO_OUTPUT:
        outputOne = TwoOutput::Create(tripAction, closeAction, false);
        break;
    default:
        break;
    }
}

TwoSignalControlModel TwoSignalControl::GetModel()
{
    return controlModel;
}

std::vector<std::shared_ptr<BinaryOutput>> TwoSignalControl::GetOutputs()
{
    switch (controlModel)
    {
    case TwoSignalControlModel::ACTIVATION:
        return {outputOne, outputTwo};
    case TwoSignalControlModel::COMPLEMENTARY_TWO_OUTPUT:
        return {outputOne};
    default:
        return {};
    }
}

void TwoSignalControl::SetPointNames(const std::string& name)
{
    switch (controlModel)
    {
    case TwoSignalControlModel::ACTIVATION:
        outputOne->SetName(name + "_trip");
        outputTwo->SetName(name + "_close");
        break;
    case TwoSignalControlModel::COMPLEMENTARY_TWO_OUTPUT:
        outputOne->SetName(name + "_control");
        break;
    default:
        break;
    }
}

// LatchOutput

LatchOutput::LatchOutput(std::shared_ptr<Binary> controlledPoint, bool requireSelect)
    : controlledPoint(controlledPoint), requireSelect(requireSelect)
{
}

std::shared_ptr<LatchOutput> LatchOutput::Create(std::shared_ptr<Binary> controlledPoint, bool requireSelect)
{
    return std::make_shared<LatchOutput>(controlledPoint, requireSelect);
}

opendnp3::CommandStatus LatchOutput::Operate(opendnp3::ControlRelayOutputBlock crob,
                                             opendnp3::IUpdateHandler& handler,
                                             opendnp3::OperateType opType)
{
    if (requireSelect && (opType != opendnp3::OperateType::SelectBeforeOperate))
    {
        return opendnp3::CommandStatus::NO_SELECT;
    }

    auto index = controlledPoint->GetIndex();
    if ((crob.tcc == opendnp3::TripCloseCode::NUL) && (crob.opType == opendnp3::OperationType::NUL))
    {
        return opendnp3::CommandStatus::SUCCESS;
    }
    else if ((crob.tcc == opendnp3::TripCloseCode::NUL) && (crob.opType == opendnp3::OperationType::LATCH_ON))
    {
        controlledPoint->Write(true);
        if (index.has_value())
        {
            handler.Update(opendnp3::Binary(controlledPoint->Read(), opendnp3::Flags(0x1), now()), index.value());
        }
        return opendnp3::CommandStatus::SUCCESS;
    }
    else if ((crob.tcc == opendnp3::TripCloseCode::NUL) && (crob.opType == opendnp3::OperationType::LATCH_OFF))
    {
        controlledPoint->Write(false);
        if (index.has_value())
        {
            handler.Update(opendnp3::Binary(controlledPoint->Read(), opendnp3::Flags(0x1), now()), index.value());
        }
        return opendnp3::CommandStatus::SUCCESS;
    }
    return opendnp3::CommandStatus::NOT_SUPPORTED;
}

// ActivationOutput

ActivationOutput::ActivationOutput(std::shared_ptr<BinaryAction> action, bool requireSelect)
    : action(action), requireSelect(requireSelect)
{
}

std::shared_ptr<ActivationOutput> ActivationOutput::Create(std::shared_ptr<BinaryAction> action, bool requireSelect)
{
    return std::make_shared<ActivationOutput>(action, requireSelect);
}

opendnp3::CommandStatus ActivationOutput::Operate(opendnp3::ControlRelayOutputBlock crob,
                                                  opendnp3::IUpdateHandler& handler,
                                                  opendnp3::OperateType opType)
{
    if (requireSelect && (opType != opendnp3::OperateType::SelectBeforeOperate))
    {
        return opendnp3::CommandStatus::NO_SELECT;
    }

    if ((crob.tcc == opendnp3::TripCloseCode::NUL) && (crob.opType == opendnp3::OperationType::NUL))
    {
        return opendnp3::CommandStatus::SUCCESS;
    }
    else if (crob.opType == opendnp3::OperationType::PULSE_ON)
    {
        (*action)();
        return opendnp3::CommandStatus::SUCCESS;
    }

    return opendnp3::CommandStatus::NOT_SUPPORTED;
}

// TwoOutput

TwoOutput::TwoOutput(std::shared_ptr<BinaryAction> tripAction,
                     std::shared_ptr<BinaryAction> closeAction,
                     bool requireSelect)
    : tripAction(tripAction), closeAction(closeAction), requireSelect(requireSelect)
{
}

std::shared_ptr<TwoOutput> TwoOutput::Create(std::shared_ptr<BinaryAction> tripAction,
                                             std::shared_ptr<BinaryAction> closeAction,
                                             bool requireSelect)
{
    return std::make_shared<TwoOutput>(tripAction, closeAction, requireSelect);
}

opendnp3::CommandStatus TwoOutput::Operate(opendnp3::ControlRelayOutputBlock crob,
                                           opendnp3::IUpdateHandler& handler,
                                           opendnp3::OperateType opType)
{
    if (requireSelect && (opType != opendnp3::OperateType::SelectBeforeOperate))
    {
        return opendnp3::CommandStatus::NO_SELECT;
    }

    if (((crob.tcc == opendnp3::TripCloseCode::NUL) && (crob.opType == opendnp3::OperationType::NUL))
        || ((crob.tcc == opendnp3::TripCloseCode::NUL) && (crob.opType == opendnp3::OperationType::PULSE_ON)))
    {
        return opendnp3::CommandStatus::SUCCESS;
    }
    else if ((crob.tcc == opendnp3::TripCloseCode::TRIP) && (crob.opType == opendnp3::OperationType::PULSE_ON))
    {
        (*tripAction)();
        return opendnp3::CommandStatus::SUCCESS;
    }
    else if ((crob.tcc == opendnp3::TripCloseCode::CLOSE) && (crob.opType == opendnp3::OperationType::PULSE_ON))
    {
        (*closeAction)();
        return opendnp3::CommandStatus::SUCCESS;
    }

    return opendnp3::CommandStatus::NOT_SUPPORTED;
}

// Analog

Analog::Analog(double value)
{
    this->value.store(value);
}

std::shared_ptr<Analog> Analog::Create()
{
    return std::make_shared<Analog>();
}
std::shared_ptr<Analog> Analog::Create(double value)
{
    return std::make_shared<Analog>(value);
}
double Analog::Read() const
{
    return value.load();
}

void Analog::Write(double v)
{
    value.store(v);

    auto idx = GetIndex();
    if (!idx.has_value())
    {
        return;
    }

    auto builder = opendnp3::UpdateBuilder();
    builder.Update(opendnp3::Analog(v, opendnp3::Flags(0x1), now()), idx.value());
    UpdateOutstation(builder.Build());
}

opendnp3::AnalogConfig Analog::Config() const
{
    return {svariation, point_class, evariation, deadband};
}

void Analog::SetClass(opendnp3::PointClass c)
{
    point_class = c;
}

void Analog::SetDeadband(double db)
{
    deadband = db;
}

uint8_t Analog::GetStaticGroup() const
{
    return 30;
}

// AnalogOutput

AnalogOutput::AnalogOutput(std::shared_ptr<AnalogAction> action, bool requireSelect)
    : action(action), requireSelect(requireSelect)
{
}

std::shared_ptr<AnalogOutput> AnalogOutput::Create(std::shared_ptr<AnalogAction> action, bool requireSelect)
{
    return std::make_shared<AnalogOutput>(action, requireSelect);
}

opendnp3::CommandStatus AnalogOutput::Operate(const opendnp3::AnalogOutputDouble64& command,
                                              opendnp3::IUpdateHandler& handler,
                                              opendnp3::OperateType opType)
{
    if (requireSelect && (opType != opendnp3::OperateType::SelectBeforeOperate))
    {
        return opendnp3::CommandStatus::NO_SELECT;
    }

    (*action)(command.value);
    return opendnp3::CommandStatus::SUCCESS;
}

// DoubleBit

std::shared_ptr<DoubleBit> DoubleBit::Create()
{
    return std::make_shared<DoubleBit>();
}

opendnp3::DoubleBit DoubleBit::Read() const
{
    uint8_t v = value.load();
    return opendnp3::DoubleBitSpec::from_type(v);
}

void DoubleBit::Write(opendnp3::DoubleBit v)
{
    value.store(uint8_t(v));

    auto idx = GetIndex();
    if (!idx.has_value())
    {
        return;
    }

    auto builder = opendnp3::UpdateBuilder();
    builder.Update(opendnp3::DoubleBitBinary(v, opendnp3::Flags(0x1), now()), idx.value());
    UpdateOutstation(builder.Build());
}

opendnp3::DoubleBitBinaryConfig DoubleBit::Config() const
{
    return {svariation, point_class, evariation};
}

void DoubleBit::SetClass(opendnp3::PointClass c)
{
    point_class = c;
}

void DoubleBit::SetVariation(opendnp3::StaticDoubleBinaryVariation v)
{
    svariation = v;
}

void DoubleBit::SetEventVariation(opendnp3::EventDoubleBinaryVariation v)
{
    evariation = v;
}

uint8_t DoubleBit::GetStaticGroup() const
{
    return 3;
}

// OctetString

OctetString::OctetString(const char* value)
{
    this->value.store(value);
}
std::shared_ptr<OctetString> OctetString::Create()
{
    return std::make_shared<OctetString>();
}
std::shared_ptr<OctetString> OctetString::Create(const char* value)
{
    return std::make_shared<OctetString>(value);
}

const char* OctetString::Read() const
{
    return value.load();
}

void OctetString::Write(const char* v)
{
    value.store(v);
}

opendnp3::OctetStringConfig OctetString::Config() const
{
    return {svariation, point_class, evariation};
}

void OctetString::SetClass(opendnp3::PointClass c)
{
    point_class = c;
}

// Counter

std::shared_ptr<Counter> Counter::Create()
{
    return std::make_shared<Counter>();
}

uint32_t Counter::Read() const
{
    return value.load();
}

void Counter::Write(uint32_t v)
{
    value.store(v);
}

opendnp3::CounterConfig Counter::Config() const
{
    return {svariation, point_class, evariation};
}

void Counter::SetClass(opendnp3::PointClass c)
{
    point_class = c;
}

void Counter::SetVariation(opendnp3::StaticCounterVariation v)
{
    svariation = v;
}

void Counter::SetEventVariation(opendnp3::EventCounterVariation v)
{
    evariation = v;
}

// TimeAndInterval

TimeAndInterval::TimeAndInterval(uint64_t time, uint32_t interval, uint8_t units)
{
    this->time.store(time);
    this->interval.store(interval);
    this->units.store(units);
}

std::shared_ptr<TimeAndInterval> TimeAndInterval::Create(uint64_t time, uint32_t interval, uint8_t units)
{
    return std::make_shared<TimeAndInterval>(time, interval, units);
}

opendnp3::TimeAndInterval TimeAndInterval::Read() const
{
    auto t = opendnp3::DNPTime(time.load());
    return opendnp3::TimeAndInterval(t, interval.load(), units.load());
}

opendnp3::TimeAndIntervalConfig TimeAndInterval::Config() const
{
    return {svariation};
}