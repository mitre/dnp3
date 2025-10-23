#ifndef CALDERADNP3_OUTSTATION_SYSTEM_PAGE_H
#define CALDERADNP3_OUTSTATION_SYSTEM_PAGE_H

#include "outstation/devices.h"

#include <ftxui/component/component.hpp>

#include <memory>

class SystemPage : public ftxui::ComponentBase
{
public:
    SystemPage() = default;
    static std::shared_ptr<SystemPage> Create();
    ftxui::Element Render() override;
    bool OnEvent(ftxui::Event) override;
    void RegisterDevices(std::vector<std::shared_ptr<IDevice>> devices);

private:
    std::vector<std::shared_ptr<IDevice>> devices{};
};

#endif // CALDERADNP3_OUTSTATION_SYSTEM_PAGE_H