#include "outstation/devices.h"

#include <random>

// IDevice
void IDevice::RegisterScreen(ftxui::ScreenInteractive* screenPtr)
{
    this->screen = screenPtr;
}

void IDevice::ForceRedraw()
{
    if (!screen)
    {
        return;
    }
    screen->PostEvent(ftxui::Event::Custom);
}

// SimpleDevice

std::shared_ptr<SimpleDevice> SimpleDevice::Create(const std::string& name)
{
    return std::make_shared<SimpleDevice>(name);
}

void SimpleDevice::AssignInputPoint(std::shared_ptr<Binary> input)
{
    status = input;
    status->SetName(name + "_status");
}

std::shared_ptr<BinaryOutput> SimpleDevice::CreateControlSignal()
{
    if (!status)
    {
        status = Binary::Create();
    }
    control = LatchOutput::Create(status, false);
    control->SetName(name + "_control");
    return control;
}

ftxui::Element SimpleDevice::Render()
{
    using namespace ftxui;

    std::string model = "Latch Model";
    if (status->Read())
    {
        return window(text(name), vbox({gauge(1) | color(Color::Red), paragraph(model)})) | size(WIDTH, EQUAL, 18)
            | notflex;
    }
    return window(text(name), vbox({gauge(0) | color(Color::Red), paragraph(model)})) | size(WIDTH, EQUAL, 18)
        | notflex;
}

// Breaker

Breaker::Breaker(const std::string& name, TwoSignalControlModel model)
    : name(name),
      control(model,
              std::make_shared<BinaryAction>(std::bind(&Breaker::Trip, this)),
              std::make_shared<BinaryAction>(std::bind(&Breaker::Close, this)))
{
    control.SetPointNames(name);
};

std::shared_ptr<Breaker> Breaker::Create(const std::string& name, TwoSignalControlModel model)
{
    return std::make_shared<Breaker>(name, model);
}

void Breaker::Trip()
{
    if (!status)
    {
        return;
    }

    status->Write(false);
}

void Breaker::Close()
{
    if (!status)
    {
        return;
    }

    status->Write(true);
}

void Breaker::AssignInputPoint(std::shared_ptr<Binary> input)
{
    status = input;
    status->SetName(name + "_status");
}

std::vector<std::shared_ptr<BinaryOutput>> Breaker::CreateControlSignal()
{
    return control.GetOutputs();
}

ftxui::Element Breaker::Render()
{
    using namespace ftxui;

    std::string model = TwoSignalControlModelSpec::to_human_string(control.GetModel());
    if (status->Read())
    {
        return window(text(name), vbox({gauge(1) | color(Color::Red), paragraph(model)})) | size(WIDTH, EQUAL, 18)
            | notflex;
    }
    return window(text(name), vbox({gauge(0) | color(Color::Red), paragraph(model)})) | size(WIDTH, EQUAL, 18)
        | notflex;
}

// SlowDevice

SlowDevice::SlowDevice(const std::string& name, uint32_t runtimeMs, TwoSignalControlModel model)
    : name(name),
      runtimeMs(runtimeMs),
      control(model,
              std::make_shared<BinaryAction>(std::bind(&SlowDevice::Trip, this)),
              std::make_shared<BinaryAction>(std::bind(&SlowDevice::Close, this)))
{
    control.SetPointNames(name);
};

SlowDevice::~SlowDevice()
{
    stopMoving();
}

std::shared_ptr<SlowDevice> SlowDevice::Create(const std::string& name, uint32_t runtimeMs, TwoSignalControlModel model)
{
    return std::make_shared<SlowDevice>(name, runtimeMs, model);
}

void SlowDevice::stopMoving()
{
    if (moving.load())
    {
        std::lock_guard<std::mutex> lock(mtx);
        moving.store(false);
        cv.notify_all();
    }
    if (moveThread.joinable())
    {
        moveThread.join();
    }
}

void SlowDevice::setPosition(double pos)
{
    opendnp3::DoubleBit newStatus;
    if (pos < 0.1)
    {
        position.store(0);
        newStatus = opendnp3::DoubleBit::DETERMINED_OFF;
    }
    else if (pos > 0.9)
    {
        position.store(1);
        newStatus = opendnp3::DoubleBit::DETERMINED_ON;
    }
    else
    {
        position.store(pos);
        newStatus = opendnp3::DoubleBit::INTERMEDIATE;
    }

    if (status->Read() != newStatus)
    {
        status->Write(newStatus);
    }
}

void SlowDevice::doTrip()
{
    stopMoving();
    moving.store(true);
    moveThread = std::thread([this]() {
        auto incrementTime = std::chrono::milliseconds(runtimeMs / 10);
        while (position.load() > 0)
        {
            auto newPosition = position.load() - 0.1;
            newPosition = newPosition < 0 ? 0 : newPosition;
            setPosition(newPosition);
            ForceRedraw();
            if (std::unique_lock<std::mutex> lock(mtx); cv.wait_for(lock, incrementTime) == std::cv_status::no_timeout)
            {
                break;
            }
        }
        moving.store(false);
    });
}

void SlowDevice::doClose()
{
    stopMoving();
    moving.store(true);
    moveThread = std::thread([this]() {
        auto incrementTime = std::chrono::milliseconds(runtimeMs / 10);
        while (position.load() < 1)
        {
            auto newPosition = position.load() + 0.1;
            newPosition = newPosition > 1 ? 1 : newPosition;
            setPosition(newPosition);
            ForceRedraw();
            if (std::unique_lock<std::mutex> lock(mtx); cv.wait_for(lock, incrementTime) == std::cv_status::no_timeout)
            {
                break;
            }
        }
        moving.store(false);
    });
}

void SlowDevice::Trip()
{
    if (status->Read() != opendnp3::DoubleBit::DETERMINED_OFF)
    {
        doTrip();
    }
}

void SlowDevice::Close()
{
    if (status->Read() != opendnp3::DoubleBit::DETERMINED_ON)
    {
        doClose();
    }
}

void SlowDevice::AssignInputPoint(std::shared_ptr<DoubleBit> input)
{
    status = input;
    status->SetName(name + "_status");
}

std::vector<std::shared_ptr<BinaryOutput>> SlowDevice::CreateControlSignal()
{
    return control.GetOutputs();
}

ftxui::Element SlowDevice::Render()
{
    using namespace ftxui;

    std::string model = TwoSignalControlModelSpec::to_human_string(control.GetModel());
    auto fill = static_cast<float>(position.load());
    return window(text(name), vbox({gauge(fill) | color(Color::Red), paragraph(model)})) | size(WIDTH, EQUAL, 18)
        | notflex;
}

// SetpointController

SetpointController::SetpointController(const std::string& name, double setpoint, double variance)
    : name(name), variance(variance)
{
    baseValue.store(setpoint);
    running.store(true);

    runThread = std::thread([this]() {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<> dis(-1 * this->variance, this->variance);

        auto looptime = std::chrono::milliseconds(1000);
        while (running.load())
        {
            auto newValue = baseValue.load() + dis(gen);
            status->Write(newValue);
            ForceRedraw();
            if (std::unique_lock<std::mutex> lock(mtx); cv.wait_for(lock, looptime) == std::cv_status::no_timeout)
            {
                break;
            }
        }
    });
}

SetpointController::~SetpointController()
{
    stop();
}

void SetpointController::stop()
{
    if (running.load())
    {
        std::lock_guard<std::mutex> lock(mtx);
        running.store(false);
        cv.notify_all();
    }
    if (runThread.joinable())
    {
        runThread.join();
    }
}

std::shared_ptr<SetpointController> SetpointController::Create(const std::string& name,
                                                               double setpoint,
                                                               double variance)
{
    return std::make_shared<SetpointController>(name, setpoint, variance);
}

void SetpointController::Set(double value)
{
    baseValue.store(value);
}

void SetpointController::AssignInputPoint(std::shared_ptr<Analog> input)
{
    status = input;
    status->SetDeadband(variance * 2);
    status->SetName(name + "_status");
}

std::shared_ptr<AnalogOutput> SetpointController::CreateControlSignal()
{
    auto action = std::make_shared<std::function<void(double)>>([this](double value) { this->Set(value); });
    control = AnalogOutput::Create(action, false);
    control->SetName(name + "_control");
    return control;
}

ftxui::Element SetpointController::Render()
{
    using namespace ftxui;

    std::string model = "Analog Output (Double64)";
    if (!control)
    {
        model = "Read-Only";
    }
    auto value = status->Read();
    return window(text(name), vbox({text(std::to_string(value)) | color(Color::Red), paragraph(model)}))
        | size(WIDTH, EQUAL, 18) | notflex;
}
