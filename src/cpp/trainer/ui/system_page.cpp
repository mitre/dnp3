#include "ui/system_page.h"

using namespace ftxui;

std::shared_ptr<SystemPage> SystemPage::Create()
{
    return std::make_shared<SystemPage>();
}

Element SystemPage::Render()
{
    Elements list;
    for (auto device : devices)
    {
        list.push_back(device->Render());
    }

    return vflow(std::move(list)) | border | flex;
}

bool SystemPage::OnEvent(Event)
{
    return false;
}

void SystemPage::RegisterDevices(std::vector<std::shared_ptr<IDevice>> devices)
{
    this->devices = devices;
}
