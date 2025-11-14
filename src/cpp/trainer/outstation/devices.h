#ifndef CALDERADNP3_OUTSTATION_DEVICES_H
#define CALDERADNP3_OUTSTATION_DEVICES_H

#include "outstation/types.h"

#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/dom/elements.hpp>

#include <atomic>
#include <condition_variable>
#include <cstdint>
#include <mutex>
#include <optional>
#include <thread>

class IDevice
{
private:
    ftxui::ScreenInteractive* screen = nullptr;

public:
    IDevice() = default;
    virtual ~IDevice() = default;
    virtual ftxui::Element Render() = 0;
    void RegisterScreen(ftxui::ScreenInteractive* screenPtr);
    void ForceRedraw();
};

class SimpleDevice : public IDevice
{
private:
    std::string name = "Simple Device";
    std::shared_ptr<Binary> status;
    std::shared_ptr<BinaryOutput> control;

public:
    SimpleDevice() = default;
    explicit SimpleDevice(const std::string& name) : name(name) {};
    static std::shared_ptr<SimpleDevice> Create(const std::string& name);

    void AssignInputPoint(std::shared_ptr<Binary> input);
    std::shared_ptr<BinaryOutput> CreateControlSignal();

    ftxui::Element Render() override;
};

class Breaker : public IDevice
{
private:
    std::string name = "Breaker";
    std::shared_ptr<Binary> status;
    TwoSignalControl control;

public:
    explicit Breaker(const std::string& name, TwoSignalControlModel model);
    static std::shared_ptr<Breaker> Create(const std::string& name, TwoSignalControlModel model);

    void Trip();
    void Close();

    void AssignInputPoint(std::shared_ptr<Binary> input);
    std::vector<std::shared_ptr<BinaryOutput>> CreateControlSignal();

    ftxui::Element Render() override;
};

class SlowDevice : public IDevice
{
private:
    std::string name = "SlowDevice";
    std::shared_ptr<DoubleBit> status;
    uint32_t runtimeMs = 5000;
    TwoSignalControl control;

    std::atomic<double> position{0.0};
    std::mutex mtx;
    std::condition_variable cv;
    std::atomic<bool> moving{false};
    std::thread moveThread;

    void stopMoving();
    void setPosition(double pos);
    void doTrip();
    void doClose();

public:
    explicit SlowDevice(const std::string& name, uint32_t runtimeMs, TwoSignalControlModel model);
    ~SlowDevice() override;
    static std::shared_ptr<SlowDevice> Create(const std::string& name, uint32_t runtimeMs, TwoSignalControlModel model);

    void Trip();
    void Close();

    void AssignInputPoint(std::shared_ptr<DoubleBit> input);
    std::vector<std::shared_ptr<BinaryOutput>> CreateControlSignal();

    ftxui::Element Render() override;
};

class SetpointController : public IDevice
{
private:
    std::string name = "Setpoint";
    std::shared_ptr<Analog> status;
    std::shared_ptr<AnalogOutput> control;
    std::atomic<double> baseValue{100.0};
    double variance = 0.2;

    std::mutex mtx;
    std::condition_variable cv;
    std::atomic<bool> running{false};
    std::thread runThread;

    void stop();

public:
    explicit SetpointController(const std::string& name, double setpoint, double variance);
    ~SetpointController() override;
    static std::shared_ptr<SetpointController> Create(const std::string& name, double setpoint, double variance);

    void Set(double value);

    void AssignInputPoint(std::shared_ptr<Analog> input);
    std::shared_ptr<AnalogOutput> CreateControlSignal();

    ftxui::Element Render() override;
};

#endif // CALDERADNP3_OUTSTATION_DEVICES_H