#include "ui/io_page.h"

using namespace ftxui;

std::shared_ptr<IOPage> IOPage::Create()
{
    return std::make_shared<IOPage>();
}

Element IOPage::Render()
{
    if (!io_table)
    {
        return vbox() | border | flex;
    }

    auto box_color = color(Color::Default);
    if (Focused())
    {
        box_color = color(Color::Green);
    }

    auto inputs = vbox({
                      text("Inputs"),
                      io_table->RenderInputs(),
                  })
        | color(Color::Default);

    auto outputs = vbox({
                       text("Outputs"),
                       io_table->RenderOutputs(),
                   })
        | color(Color::Default);

    auto tables = hbox({inputs, separatorEmpty(), outputs}) | focusPositionRelative(x_focus, y_focus)
        | vscroll_indicator | frame | flex;

    return vbox({tables, hscroll_indicator()}) | border | box_color | flex;
}

bool IOPage::OnEvent(Event event)
{
    if (!Focused())
    {
        return false;
    }

    float start_x_focus = x_focus;
    float start_y_focus = y_focus;

    // Use arrows or vim motions to navigate logs
    if ((event == ftxui::Event::ArrowUp || event == ftxui::Event::Character('k')) && y_focus > 0.1f)
    {
        y_focus = y_focus - 0.1f;
    }
    if ((event == ftxui::Event::ArrowDown || event == ftxui::Event::Character('j')) && y_focus < 1.0f)
    {
        y_focus = y_focus + 0.1f;
    }
    if ((event == ftxui::Event::ArrowLeft || event == ftxui::Event::Character('h')) && x_focus > 0.1f)
    {
        x_focus = x_focus - 0.1f;
    }
    if ((event == ftxui::Event::ArrowRight || event == ftxui::Event::Character('l')) && x_focus < 1.0f)
    {
        x_focus = x_focus + 0.1f;
    }

    // Jump to newest (last) line on Enter
    if (event == ftxui::Event::Return)
    {
        x_focus = 0.0f;
        y_focus = 1.0f;
    }

    return (x_focus != start_x_focus) || (y_focus != start_y_focus);
}

Element IOPage::hscroll_indicator()
{
    int x_dim = ftxui::Terminal::Size().dimx;

    int text_width = x_dim / 10;
    int indicator_x = x_focus * (x_dim - text_width);
    int indicator_width = indicator_x + text_width;

    std::wstring hscroll_text(text_width, L'─');
    return hbox(text(hscroll_text) | bold | align_right | color(Color::Default) | size(WIDTH, EQUAL, indicator_width));
}

void IOPage::RegisterIOTable(std::shared_ptr<IOTable> io_table)
{
    this->io_table = io_table;
}

bool IOPage::Focusable() const
{
    if (!io_table)
    {
        return false;
    }
    return true;
}