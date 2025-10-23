#include "ui/log_displayer.h"

#include "date/date.h"

using namespace ftxui;

// TuiLogger

TuiLogger::TuiLogger(ftxui::Sender<std::string> sender, bool printLocation)
    : sender(std::move(sender)), printLocation(printLocation)
{
}

std::shared_ptr<TuiLogger> TuiLogger::Create(ftxui::Sender<std::string> sender)
{
    return std::make_shared<TuiLogger>(std::move(sender), false);
};

std::shared_ptr<TuiLogger> TuiLogger::Create(ftxui::Sender<std::string> sender, bool printLocation)
{
    return std::make_shared<TuiLogger>(std::move(sender), printLocation);
};

void TuiLogger::RegisterScreen(ftxui::ScreenInteractive* screen_ptr)
{
    screen = screen_ptr;
}

void TuiLogger::log(
    opendnp3::ModuleId module_id, const char* id, opendnp3::LogLevel level, char const* location, char const* message)
{
    using namespace date;
    std::ostringstream oss;

    auto now = std::chrono::system_clock::now();
    oss << now << "Z ";
    oss << "[ " << LogFlagToString(level) << "] ";
    oss << "(" << id << ") ";
    if (printLocation)
    {
        oss << location << " ";
    }
    oss << message;

    std::unique_lock<std::mutex> lock(mutex);
    sender->Send(oss.str());

    if (screen)
    {
        screen->PostEvent(ftxui::Event::Custom);
    }
}

void TuiLogger::OnStateChange(opendnp3::ChannelState state)
{
    using namespace date;
    std::ostringstream oss;

    auto now = std::chrono::system_clock::now();
    oss << now << "Z [ INFO   ] (channel) state change: " << opendnp3::ChannelStateSpec::to_human_string(state);

    sender->Send(oss.str());
}

// LogDisplayer

std::shared_ptr<LogDisplayer> LogDisplayer::Create()
{
    return std::make_shared<LogDisplayer>();
}

Element LogDisplayer::Render()
{
    Elements list;
    int index = 0;
    for (const std::string& line : lines)
    {
        Element document = RenderLine(line, index);
        list.push_back(document);
        index++;
    }

    auto box_color = color(Color::Default);
    if (Focused())
    {
        box_color = color(Color::Green);
    }

    auto text_box = vbox(std::move(list)) | focusPositionRelative(x_focus, y_focus) | vscroll_indicator | frame | flex;

    return vbox({
               text_box,
               hscroll_indicator(),
           })
        | border | box_color | flex;
}

Element LogDisplayer::RenderLine(const std::string& line, int lineno)
{
    bool line_focus = (lineno == GetSelected());
    bool box_focus = Focused();

    Element lineno_element = text(std::to_string(lineno)) | align_right | dim | size(WIDTH, EQUAL, 4);
    Element document = hbox({lineno_element, separator(), text(line) | flex});

    Decorator line_decorator = color(Color::Default);
    if (line_focus && box_focus)
    {
        line_decorator = color(Color::GrayDark) | bgcolor(Color::Green);
    }
    else if (line_focus && !box_focus)
    {
        line_decorator = inverted;
    }
    return document | line_decorator;
}

Element LogDisplayer::hscroll_indicator()
{
    int x_dim = ftxui::Terminal::Size().dimx;

    int text_width = x_dim / 10;
    int indicator_x = x_focus * (x_dim - text_width);
    int indicator_width = indicator_x + text_width;

    std::wstring hscroll_text(text_width, L'─');
    return hbox(text(hscroll_text) | bold | align_right | color(Color::Default) | size(WIDTH, EQUAL, indicator_width));
}

bool LogDisplayer::OnEvent(Event event)
{
    if (!Focused())
    {
        return false;
    }

    int start_selected = selected;
    float start_x_focus = x_focus;
    float start_y_focus = y_focus;

    // Use arrows or vim motions to navigate logs
    if ((event == ftxui::Event::ArrowUp || event == ftxui::Event::Character('k')) && selected > 0)
    {
        selected--;
    }
    if ((event == ftxui::Event::ArrowDown || event == ftxui::Event::Character('j')) && selected < n_lines - 1)
    {
        selected++;
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
        selected = n_lines - 1;
        x_focus = 0.0f;
    }

    // Calculate y_focus from selected
    if (n_lines <= 1)
    {
        y_focus = 1.0f;
    }
    else
    {
        y_focus = static_cast<float>(selected) / static_cast<float>(n_lines - 1);
    }

    return (selected != start_selected) || (x_focus != start_x_focus) || (y_focus != start_y_focus);
}

void LogDisplayer::AddLine(const std::string& line)
{
    bool last_line_selected = (selected == n_lines - 1) || (n_lines == 0);

    lines.push_back(line);
    n_lines = lines.size();

    if (last_line_selected)
    {
        selected = n_lines - 1;
    }
}

Element LogDisplayer::debug_info()
{
    return hbox({
        text(std::to_string(x_focus)),
        separator(),
        text(std::to_string(y_focus)),
        separator(),
        text(std::to_string(selected) + " / " + std::to_string(n_lines)),
    });
}

int LogDisplayer::GetSelected() const
{
    return selected;
}

bool LogDisplayer::Focusable() const
{
    return true;
}