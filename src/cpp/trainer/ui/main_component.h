#ifndef CALDERADNP3_OUTSTATION_MAIN_COMPONENT_H
#define CALDERADNP3_OUTSTATION_MAIN_COMPONENT_H

#include "ui/io_page.h"
#include "ui/log_displayer.h"
#include "ui/system_page.h"

#include <ftxui/component/component.hpp>
#include <ftxui/component/receiver.hpp>

#include <functional>
#include <string>

class MainComponent : public ftxui::ComponentBase
{
private:
    ftxui::Receiver<std::string> log_receiver = nullptr;

    int selected_page = 0;
    std::vector<std::string> pages = {
        "System View",
        "IO Table",
        "Logs",
    };

    ftxui::Component menu = ftxui::Menu(&pages, &selected_page, ftxui::MenuOption::HorizontalAnimated());
    std::shared_ptr<SystemPage> system_page = SystemPage::Create();
    std::shared_ptr<LogDisplayer> log_displayer = LogDisplayer::Create();
    std::shared_ptr<IOPage> io_page = IOPage::Create();

public:
    MainComponent();
    static std::shared_ptr<MainComponent> Create();

    ftxui::Element Render() override;
    bool OnEvent(ftxui::Event) override;

    void RegisterLogReceiver(ftxui::Receiver<std::string> receiver);
    void RegisterDevices(std::vector<std::shared_ptr<IDevice>> devices);
    void RegisterIOTable(std::shared_ptr<IOTable> io_table);
};

#endif // CALDERADNP3_OUTSTATION_MAIN_COMPONENT_H