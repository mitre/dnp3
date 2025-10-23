#ifndef CALDERADNP3_OUTSTATION_IO_PAGE_H
#define CALDERADNP3_OUTSTATION_IO_PAGE_H

#include "outstation/io_table.h"

#include <ftxui/component/component.hpp>

#include <memory>

class IOPage : public ftxui::ComponentBase
{
private:
    std::shared_ptr<IOTable> io_table = nullptr;
    float x_focus = 0.0f;
    float y_focus = 0.0f;
    ftxui::Element hscroll_indicator();

public:
    IOPage() = default;
    static std::shared_ptr<IOPage> Create();
    ftxui::Element Render() override;
    bool Focusable() const final;
    bool OnEvent(ftxui::Event event) override;
    void RegisterIOTable(std::shared_ptr<IOTable> io_table);
};

#endif // CALDERADNP3_OUTSTATION_IO_PAGE_H