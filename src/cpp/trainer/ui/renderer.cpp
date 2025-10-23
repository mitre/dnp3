#include "ui/renderer.h"

#include "ui/main_component.h"

#include <ftxui/component/screen_interactive.hpp>

std::shared_ptr<TuiRenderer> TuiRenderer::Create()
{
    return std::make_shared<TuiRenderer>();
}

std::shared_ptr<TuiLogger> TuiRenderer::GetLogger()
{
    auto log_receiver = ftxui::MakeReceiver<std::string>();
    auto log_sender = log_receiver->MakeSender();

    app->RegisterLogReceiver(std::move(log_receiver));
    auto logger = TuiLogger::Create(std::move(log_sender));
    logger->RegisterScreen(&screen);

    return logger;
}

void TuiRenderer::RegisterDevices(std::vector<std::shared_ptr<IDevice>> devices)
{
    for (auto device : devices)
    {
        device->RegisterScreen(&screen);
    }
    app->RegisterDevices(devices);
}

void TuiRenderer::RegisterIOTable(std::shared_ptr<IOTable> io_table)
{
    app->RegisterIOTable(io_table);
}

void TuiRenderer::Init()
{
    auto controlled_app = ftxui::CatchEvent(app, [this](ftxui::Event event) {
        if (event == ftxui::Event::Character('q'))
        {
            screen.ExitLoopClosure()();
            return true;
        }
        return false;
    });

    screen.Loop(controlled_app);
}
