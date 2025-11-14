#include "ui/main_component.h"

using namespace ftxui;

MainComponent::MainComponent()
{
    auto option = MenuOption::HorizontalAnimated();
    option.underline.color_active = Color::Green;
    menu = Menu(&pages, &selected_page, option);

    Add(Container::Vertical({
        menu,
        Container::Tab(
            {
                system_page,
                io_page,
                log_displayer,
            },
            &selected_page),
    }));
}

std::shared_ptr<MainComponent> MainComponent::Create()
{
    return std::make_shared<MainComponent>();
}

bool MainComponent::OnEvent(Event event)
{
    while (log_receiver && log_receiver->HasPending())
    {
        std::string line;
        log_receiver->Receive(&line);
        log_displayer->AddLine(line);
    }

    return ComponentBase::OnEvent(event);
}

Element MainComponent::Render()
{
    if (selected_page == 1)
    {
        return vbox({
            menu->Render(),
            io_page->Render(),
        });
    }
    else if (selected_page == 2)
    {
        return vbox({
            menu->Render(),
            log_displayer->Render(),
        });
    }
    return vbox({
        menu->Render(),
        system_page->Render(),
    });
}

void MainComponent::RegisterLogReceiver(ftxui::Receiver<std::string> receiver)
{
    log_receiver = std::move(receiver);
}

void MainComponent::RegisterDevices(std::vector<std::shared_ptr<IDevice>> devices)
{
    system_page->RegisterDevices(devices);
}

void MainComponent::RegisterIOTable(std::shared_ptr<IOTable> io_table)
{
    io_page->RegisterIOTable(io_table);
}