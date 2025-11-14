#ifndef CALDERADNP3_OUTSTATION_LOG_DISPLAYER_H
#define CALDERADNP3_OUTSTATION_LOG_DISPLAYER_H

#include <opendnp3/channel/IChannelListener.h>
#include <opendnp3/logging/ILogHandler.h>

#include <ftxui/component/component.hpp>
#include <ftxui/component/screen_interactive.hpp>

#include <memory>
#include <mutex>

class TuiLogger final : public opendnp3::ILogHandler, public opendnp3::IChannelListener
{
private:
    ftxui::ScreenInteractive* screen = nullptr;
    ftxui::Sender<std::string> sender;
    bool printLocation;
    std::mutex mutex;

public:
    explicit TuiLogger(ftxui::Sender<std::string> sender, bool printLocation);
    static std::shared_ptr<TuiLogger> Create(ftxui::Sender<std::string> sender);
    static std::shared_ptr<TuiLogger> Create(ftxui::Sender<std::string> sender, bool printLocation);

    void RegisterScreen(ftxui::ScreenInteractive* screen_ptr);

    void log(opendnp3::ModuleId module_id,
             const char* id,
             opendnp3::LogLevel level,
             char const* location,
             char const* message) override;
    void OnStateChange(opendnp3::ChannelState state) override;
};

class LogDisplayer : public ftxui::ComponentBase
{
private:
    std::vector<std::string> lines = {};

    int selected = 0;
    int n_lines = 0;

    float x_focus = 0.0f;
    float y_focus = 1.0f;

    ftxui::Element hscroll_indicator();
    ftxui::Element debug_info();

public:
    LogDisplayer() = default;
    static std::shared_ptr<LogDisplayer> Create();

    ftxui::Element Render() override;
    virtual ftxui::Element RenderLine(const std::string& line, int lineno);
    void AddLine(const std::string& line);
    bool OnEvent(ftxui::Event) override;
    int GetSelected() const;
    bool Focusable() const final;
};

#endif // CALDERADNP3_OUTSTATION_LOG_DISPLAYER_H