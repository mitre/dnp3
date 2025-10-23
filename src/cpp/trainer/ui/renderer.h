#ifndef CALDERADNP3_OUTSTATION_RENDERER_H
#define CALDERADNP3_OUTSTATION_RENDERER_H

#include "outstation/devices.h"
#include "ui/main_component.h"

#include <ftxui/component/screen_interactive.hpp>

#include <atomic>
#include <memory>
#include <thread>

class TuiRenderer
{
private:
    ftxui::ScreenInteractive screen = ftxui::ScreenInteractive::Fullscreen();
    std::shared_ptr<MainComponent> app = MainComponent::Create();

public:
    TuiRenderer() = default;
    ~TuiRenderer() = default;
    static std::shared_ptr<TuiRenderer> Create();

    void RegisterDevices(std::vector<std::shared_ptr<IDevice>> devices);
    void RegisterIOTable(std::shared_ptr<IOTable> io_table);
    std::shared_ptr<TuiLogger> GetLogger();

    void Init();
};

#endif // CALDERADNP3_OUTSTATION_RENDERER_H