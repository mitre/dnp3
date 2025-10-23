#ifndef CALDERADNP3_OUTSTATION_TYPES_H
#define CALDERADNP3_OUTSTATION_TYPES_H

#include <opendnp3/app/MeasurementTypes.h>
#include <opendnp3/outstation/ICommandHandler.h>
#include <opendnp3/outstation/IOutstation.h>
#include <opendnp3/outstation/MeasurementConfig.h>

#include <atomic>
#include <chrono>
#include <cstdint>
#include <functional>
#include <optional>

class IMeasurement
{
private:
    std::string name = "";
    std::optional<uint16_t> index = std::nullopt;
    std::shared_ptr<opendnp3::IOutstation> outstation = nullptr;

public:
    IMeasurement() = default;
    virtual ~IMeasurement() = default;

    void SetName(const std::string& point_name);
    const std::string& GetName() const;
    void SetIndex(uint16_t idx);
    std::optional<uint16_t> GetIndex() const;
    virtual uint8_t GetStaticGroup() const = 0;
    void RegisterOutstation(std::shared_ptr<opendnp3::IOutstation> os);
    void UpdateOutstation(const opendnp3::Updates& updates);
};

class Binary : public IMeasurement
{
private:
    std::atomic<bool> value{false};

    opendnp3::PointClass point_class = opendnp3::PointClass::Class3;
    opendnp3::StaticBinaryVariation svariation = opendnp3::StaticBinaryVariation::Group1Var2;
    opendnp3::EventBinaryVariation evariation = opendnp3::EventBinaryVariation::Group2Var2;

public:
    Binary() = default;
    ~Binary() override = default;
    static std::shared_ptr<Binary> Create();

    bool Read() const;
    void Write(bool v);

    opendnp3::BinaryConfig Config() const;
    void SetClass(opendnp3::PointClass c);
    void SetVariation(opendnp3::StaticBinaryVariation v);
    void SetEventVariation(opendnp3::EventBinaryVariation v);
    uint8_t GetStaticGroup() const override;
};

class BinaryOutput : public Binary
{
public:
    virtual opendnp3::CommandStatus Operate(opendnp3::ControlRelayOutputBlock crob,
                                            opendnp3::IUpdateHandler& handler,
                                            opendnp3::OperateType opType);
};

enum class TwoSignalControlModel : uint8_t
{
    ACTIVATION = 0,
    COMPLEMENTARY_TWO_OUTPUT = 1,
    UNDEFINED = 127
};

struct TwoSignalControlModelSpec
{
    using enum_type_t = TwoSignalControlModel;

    static uint8_t to_type(TwoSignalControlModel arg);
    static TwoSignalControlModel from_type(uint8_t arg);
    static char const* to_string(TwoSignalControlModel arg);
    static char const* to_human_string(TwoSignalControlModel arg);
    static TwoSignalControlModel from_string(const std::string& arg);
};

using BinaryAction = std::function<void()>;

class TwoSignalControl
{
private:
    TwoSignalControlModel controlModel;
    std::shared_ptr<BinaryOutput> outputOne;
    std::shared_ptr<BinaryOutput> outputTwo;

public:
    explicit TwoSignalControl(TwoSignalControlModel model,
                              std::shared_ptr<BinaryAction> tripAction,
                              std::shared_ptr<BinaryAction> closeAction);

    TwoSignalControlModel GetModel();
    std::vector<std::shared_ptr<BinaryOutput>> GetOutputs();
    void SetPointNames(const std::string& name);
};

/**
 * LatchOutput controls a single binary output.
 *
 * The output is a Binary passed the LatchOutput constructor.
 * NUL / LATCH_ON will set the controlledPoint to TRUE / ON / CLOSED
 * NUL / LATCH_OFF will set the controlledPoint to FALSE / OFF / TRIPPED
 * If the controlledPoint has an index in the IOTable, it will be controlled
 * directly by the Operate function.
 */
class LatchOutput : public BinaryOutput
{
private:
    std::shared_ptr<Binary> controlledPoint = nullptr;
    bool requireSelect = false;

public:
    explicit LatchOutput(std::shared_ptr<Binary> controlledPoint, bool requireSelect);
    static std::shared_ptr<LatchOutput> Create(std::shared_ptr<Binary> controlledPoint, bool requireSelect);
    opendnp3::CommandStatus Operate(opendnp3::ControlRelayOutputBlock crob,
                                    opendnp3::IUpdateHandler& handler,
                                    opendnp3::OperateType opType) override;
};

/**
 * ActivationOutput initiates an action.
 *
 * The controlled action is represented by the callback function
 * passed to the ActivationOutput constructor. When sent a PULSE_ON
 * CROB, the function is called. All TCCs are accepted.
 */
class ActivationOutput : public BinaryOutput
{
private:
    std::shared_ptr<BinaryAction> action;
    bool requireSelect = false;

public:
    explicit ActivationOutput(std::shared_ptr<BinaryAction> action, bool requireSelect);
    static std::shared_ptr<ActivationOutput> Create(std::shared_ptr<BinaryAction> action, bool requireSelect);
    opendnp3::CommandStatus Operate(opendnp3::ControlRelayOutputBlock crob,
                                    opendnp3::IUpdateHandler& handler,
                                    opendnp3::OperateType opType) override;
};

/**
 * TwoOutput controls two output events or action.
 *
 * The controlled actions are represented by callback functions
 * named tripAction and closeAction. When sent a TRIP/PULSE_ON
 * CROB, the tripAction is called. When send a CLOSE/PULSE_ON
 * CROB, the closeAction is called.
 */
class TwoOutput : public BinaryOutput
{
private:
    std::shared_ptr<BinaryAction> tripAction;
    std::shared_ptr<BinaryAction> closeAction;
    bool requireSelect = false;

public:
    explicit TwoOutput(std::shared_ptr<BinaryAction> tripAction,
                       std::shared_ptr<BinaryAction> closeAction,
                       bool requireSelect);
    static std::shared_ptr<TwoOutput> Create(std::shared_ptr<BinaryAction> tripAction,
                                             std::shared_ptr<BinaryAction> closeAction,
                                             bool requireSelect);
    opendnp3::CommandStatus Operate(opendnp3::ControlRelayOutputBlock crob,
                                    opendnp3::IUpdateHandler& handler,
                                    opendnp3::OperateType opType) override;
};

class Analog : public IMeasurement
{
private:
    std::atomic<double> value{100.0};
    double deadband = 0.4;

    opendnp3::PointClass point_class = opendnp3::PointClass::Class3;
    opendnp3::StaticAnalogVariation svariation = opendnp3::StaticAnalogVariation::Group30Var5;
    opendnp3::EventAnalogVariation evariation = opendnp3::EventAnalogVariation::Group32Var7;

public:
    Analog() = default;
    explicit Analog(double value);
    static std::shared_ptr<Analog> Create();
    static std::shared_ptr<Analog> Create(double value);

    double Read() const;
    void Write(double v);

    opendnp3::AnalogConfig Config() const;
    void SetClass(opendnp3::PointClass c);
    void SetDeadband(double db);
    uint8_t GetStaticGroup() const override;
};

using AnalogAction = std::function<void(double)>;

class AnalogOutput : public Analog
{
private:
    std::shared_ptr<AnalogAction> action;
    bool requireSelect = false;

public:
    explicit AnalogOutput(std::shared_ptr<AnalogAction> action, bool requireSelect);
    static std::shared_ptr<AnalogOutput> Create(std::shared_ptr<AnalogAction> action, bool requireSelect);
    opendnp3::CommandStatus Operate(const opendnp3::AnalogOutputDouble64& command,
                                    opendnp3::IUpdateHandler& handler,
                                    opendnp3::OperateType opType);
};

class DoubleBit : public IMeasurement
{
private:
    std::atomic<uint8_t> value{0x1};

    opendnp3::PointClass point_class = opendnp3::PointClass::Class3;
    opendnp3::StaticDoubleBinaryVariation svariation = opendnp3::StaticDoubleBinaryVariation::Group3Var2;
    opendnp3::EventDoubleBinaryVariation evariation = opendnp3::EventDoubleBinaryVariation::Group4Var2;

public:
    static std::shared_ptr<DoubleBit> Create();

    opendnp3::DoubleBit Read() const;
    void Write(opendnp3::DoubleBit v);

    opendnp3::DoubleBitBinaryConfig Config() const;
    void SetClass(opendnp3::PointClass c);
    void SetVariation(opendnp3::StaticDoubleBinaryVariation v);
    void SetEventVariation(opendnp3::EventDoubleBinaryVariation v);
    uint8_t GetStaticGroup() const override;
};

class OctetString
{
private:
    std::atomic<const char*> value{""};

    opendnp3::PointClass point_class = opendnp3::PointClass::Class3;
    opendnp3::StaticOctetStringVariation svariation = opendnp3::StaticOctetStringVariation::Group110Var0;
    opendnp3::EventOctetStringVariation evariation = opendnp3::EventOctetStringVariation::Group111Var0;

public:
    OctetString() = default;
    explicit OctetString(const char* value);
    ~OctetString() = default;
    static std::shared_ptr<OctetString> Create();
    static std::shared_ptr<OctetString> Create(const char* value);

    const char* Read() const;
    void Write(const char* v);

    opendnp3::OctetStringConfig Config() const;
    void SetClass(opendnp3::PointClass c);
};

class Counter
{
private:
    std::atomic<uint32_t> value{0};

    opendnp3::PointClass point_class = opendnp3::PointClass::Class3;
    opendnp3::StaticCounterVariation svariation = opendnp3::StaticCounterVariation::Group20Var1;
    opendnp3::EventCounterVariation evariation = opendnp3::EventCounterVariation::Group22Var5;

public:
    Counter() = default;
    ~Counter() = default;
    static std::shared_ptr<Counter> Create();

    uint32_t Read() const;
    void Write(uint32_t v);

    opendnp3::CounterConfig Config() const;
    void SetClass(opendnp3::PointClass c);
    void SetVariation(opendnp3::StaticCounterVariation v);
    void SetEventVariation(opendnp3::EventCounterVariation v);
};

class TimeAndInterval
{
private:
    std::atomic<uint64_t> time{0};
    std::atomic<uint32_t> interval{0};
    std::atomic<uint8_t> units{0};

    opendnp3::StaticTimeAndIntervalVariation svariation = opendnp3::StaticTimeAndIntervalVariation::Group50Var4;

public:
    TimeAndInterval(uint64_t time, uint32_t interval, uint8_t units);
    ~TimeAndInterval() = default;
    static std::shared_ptr<TimeAndInterval> Create(uint64_t time, uint32_t interval, uint8_t units);

    opendnp3::TimeAndInterval Read() const;

    opendnp3::TimeAndIntervalConfig Config() const;
};

#endif // CALDERADNP3_OUTSTATION_TYPES_H