#ifndef CALDERADNP3_OUTSTATION_LOGGER_HPP
#define CALDERADNP3_OUTSTATION_LOGGER_HPP

#include <opendnp3/ConsoleLogger.h>
#include <opendnp3/DNP3Manager.h>
#include <opendnp3/logging/LogLevels.h>
#include <opendnp3/master/ISOEHandler.h>

#include <date/date.h>

#include <ctime>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>

class SOEHandler : public opendnp3::ISOEHandler
{
private:
    static void PrintHeading(const opendnp3::HeaderInfo& info)
    {
        PrintGroupVar(info);
        std::cout << std::left << std::setw(8) << "index"
                  << "| " << std::left << std::setw(15) << "value"
                  << "| " << std::left << std::setw(9) << "flags"
                  << "| " << std::left << "time" << std::endl;

        std::cout << std::setw(8) << std::setfill('=') << "="
                  << "|" << std::setw(16) << "="
                  << "|" << std::setw(10) << "="
                  << "|" << std::setw(10) << "="
                  << std::setfill(' ') << std::endl;
    }

    static void PrintGroupVar(const opendnp3::HeaderInfo& info)
    {
        std::cout << opendnp3::GroupVariationSpec::to_string(info.gv) << ": ";
        std::cout << opendnp3::GroupVariationSpec::to_human_string(info.gv);
        std::cout << std::endl;
    }

    template<class T>
    static void PrintAll(const opendnp3::HeaderInfo& info, const opendnp3::ICollection<opendnp3::Indexed<T>>& values)
    {
        auto print = [&info](const opendnp3::Indexed<T>& pair) { Print<T>(info, pair.value, pair.index); };
        values.ForeachItem(print);
        std::cout << std::endl;
    }

    template<class T> static void Print(const opendnp3::HeaderInfo& info, const T& value, uint16_t index)
    {
        auto value_string = ValueToString(value);
        auto flags = FlagsToString(value.flags);
        auto time = ValueToString(value.time);

        std::ostringstream oss;

        // Print index and value
        oss << std::left << std::setw(8) << index << "| " << std::left << std::setw(15) << value_string << "| ";

        // Set padding for flags
        oss << std::left << std::setw(9);
        if (info.flagsValid)
        {
            oss << flags << "| ";
        }
        else
        {
            oss << "" << "| ";
        }

        // Print time if valid
        if (info.tsquality != opendnp3::TimestampQuality::INVALID)
        {
            oss << time;
        }

        std::cout << oss.str() << std::endl;
    }

    static std::string ValueToString(const opendnp3::OctetString& meas)
    {
        std::ostringstream oss;
        auto buffer = meas.ToBuffer();
        for (std::size_t i = 0; i < buffer.length; ++i)
        {
            oss << std::hex << std::setw(2) << std::setfill('0')
                << static_cast<int>(buffer.data[i])
                << std::setfill(' ');
        }
        return oss.str();
    }

    static std::string ValueToString(const opendnp3::DNPTime& meas)
    {
        using namespace date;

        if (meas.value == 0)
        {
            return "";
        }

        std::chrono::milliseconds duration(meas.value);
        std::chrono::time_point<std::chrono::system_clock> epoch;

        auto time_point = epoch + duration;
        std::ostringstream oss;
        oss << time_point;
        return oss.str();
    }

    static std::string ValueToString(const opendnp3::DoubleBitBinary& meas)
    {
        return opendnp3::DoubleBitSpec::to_human_string(meas.value);
    }

    template<class T> static std::string ValueToString(const T& meas)
    {
        std::ostringstream oss;
        oss << meas.value;
        return oss.str();
    }

    static void PrintOctetString(const opendnp3::OctetString& value, uint16_t index)
    {
        auto value_string = ValueToString(value);

        std::cout << std::left << std::setw(8) << index << "| "
                  << std::left << std::setw(15) << value_string
                  << std::endl;
    }

    static void PrintTimeAndInterval(const opendnp3::TimeAndInterval& value, uint16_t index)
    {
        auto interval = opendnp3::IntervalUnitsSpec::from_type(value.units);
        auto units = opendnp3::IntervalUnitsSpec::to_human_string(interval);
        auto time = SOEHandler::ValueToString(value.time);

        std::cout << std::left << std::setw(8) << index << "| " 
                  << std::left << std::setw(24) << time << "| "
                  << std::left << std::setw(9) << units 
                  << std::endl;
    }

    static void PrintBinaryCommandEvent(const opendnp3::BinaryCommandEvent& value, uint16_t index)
    {
        auto status = opendnp3::CommandStatusSpec::to_human_string(value.status);
        std::string result = value.value ? "Latch On / Close" : "Latch Off / Trip / NULL";

        std::cout << std::left << std::setw(8) << index << "| " 
                  << std::left << std::setw(18) << status << "| "
                  << std::left << result 
                  << std::endl;
    }

    static void PrintAnalogCommandEvent(const opendnp3::AnalogCommandEvent& value, uint16_t index)
    {
        auto status = opendnp3::CommandStatusSpec::to_human_string(value.status);

        std::cout << std::left << std::setw(8) << index << "| " 
                  << std::left << std::setw(18) << status << "| "
                  << std::left << value.value
                  << std::endl;
    }

    static std::string FlagsToString(const opendnp3::Flags flags)
    {
        std::ostringstream oss;
        for (int i = 7; i >= 0; --i)
        {
            oss << ((flags.value >> i) & 1);
        }
        return oss.str();
    }

public:
    SOEHandler() = default;
    ~SOEHandler() override = default;

    static std::shared_ptr<SOEHandler> Create()
    {
        return std::make_shared<SOEHandler>();
    }

    void BeginFragment(const opendnp3::ResponseInfo& info) override {}
    void EndFragment(const opendnp3::ResponseInfo& info) override {}

    void Process(const opendnp3::HeaderInfo& info,
                 const opendnp3::ICollection<opendnp3::Indexed<opendnp3::Binary>>& values) override
    {
        PrintHeading(info);
        PrintAll(info, values);
    }

    void Process(const opendnp3::HeaderInfo& info,
                 const opendnp3::ICollection<opendnp3::Indexed<opendnp3::DoubleBitBinary>>& values) override
    {
        PrintHeading(info);
        PrintAll(info, values);
    }

    void Process(const opendnp3::HeaderInfo& info,
                 const opendnp3::ICollection<opendnp3::Indexed<opendnp3::Analog>>& values) override
    {
        PrintHeading(info);
        PrintAll(info, values);
    }

    void Process(const opendnp3::HeaderInfo& info,
                 const opendnp3::ICollection<opendnp3::Indexed<opendnp3::Counter>>& values) override
    {
        PrintHeading(info);
        PrintAll(info, values);
    }

    void Process(const opendnp3::HeaderInfo& info,
                 const opendnp3::ICollection<opendnp3::Indexed<opendnp3::FrozenCounter>>& values) override
    {
        PrintHeading(info);
        PrintAll(info, values);
    }

    void Process(const opendnp3::HeaderInfo& info,
                 const opendnp3::ICollection<opendnp3::Indexed<opendnp3::BinaryOutputStatus>>& values) override
    {
        PrintHeading(info);
        PrintAll(info, values);
    }

    void Process(const opendnp3::HeaderInfo& info,
                 const opendnp3::ICollection<opendnp3::Indexed<opendnp3::AnalogOutputStatus>>& values) override
    {
        PrintHeading(info);
        PrintAll(info, values);
    }

    void Process(const opendnp3::HeaderInfo& info,
                 const opendnp3::ICollection<opendnp3::Indexed<opendnp3::OctetString>>& values) override
    {
        PrintGroupVar(info);

        std::cout << std::left << std::setw(8) << "index"
                  << "| " << std::left << std::setw(15) << "value"
                  << std::endl;

        std::cout << std::setw(8) << std::setfill('=') << "="
                  << "|" << std::setw(16) << "=" 
                  << std::setfill(' ') << std::endl;

        auto print = [](const opendnp3::Indexed<opendnp3::OctetString>& pair) {
            PrintOctetString(pair.value, pair.index);
        };

        values.ForeachItem(print);
        std::cout << std::endl;
    }

    void Process(const opendnp3::HeaderInfo& info,
                 const opendnp3::ICollection<opendnp3::Indexed<opendnp3::TimeAndInterval>>& values) override
    {
        PrintGroupVar(info);

        std::cout << std::left << std::setw(8) << "index" << "| " 
                  << std::left << std::setw(24) << "time" << "| "
                  << std::left << std::setw(9) << "units"
                  << std::endl;

        std::cout << std::setw(8) << std::setfill('=') << "="
                  << "|" << std::setw(25) << "="
                  << "|" << std::setw(10) << "=" 
                  << std::setfill(' ') << std::endl;

        auto print = [](const opendnp3::Indexed<opendnp3::TimeAndInterval>& pair) {
            PrintTimeAndInterval(pair.value, pair.index);
        };

        values.ForeachItem(print);
        std::cout << std::endl;
    }

    void Process(const opendnp3::HeaderInfo& info,
                 const opendnp3::ICollection<opendnp3::Indexed<opendnp3::BinaryCommandEvent>>& values) override
    {
        PrintGroupVar(info);

        std::cout << std::left << std::setw(8) << "index" << "| " 
                  << std::left << std::setw(18) << "status" << "| "
                  << std::left << "value" 
                  << std::endl;

        std::cout << std::setw(8) << std::setfill('=') << "="
                  << "|" << std::setw(19) << "="
                  << "|" << std::setw(10) << "="
                  << std::setfill(' ') << std::endl;

        auto print = [](const opendnp3::Indexed<opendnp3::BinaryCommandEvent>& pair) {
            PrintBinaryCommandEvent(pair.value, pair.index);
        };

        values.ForeachItem(print);
        std::cout << std::endl;
    }

    void Process(const opendnp3::HeaderInfo& info,
                 const opendnp3::ICollection<opendnp3::Indexed<opendnp3::AnalogCommandEvent>>& values) override
    {
        PrintGroupVar(info);

        std::cout << std::left << std::setw(8) << "index" << "| " 
                  << std::left << std::setw(18) << "status" << "| "
                  << std::left << "value" 
                  << std::endl;

        std::cout << std::setw(8) << std::setfill('=') << "="
                  << "|" << std::setw(19) << "="
                  << "|" << std::setw(10) << "="
                  << std::setfill(' ') << std::endl;

        auto print = [](const opendnp3::Indexed<opendnp3::AnalogCommandEvent>& pair) {
            PrintAnalogCommandEvent(pair.value, pair.index);
        };

        values.ForeachItem(print);
        std::cout << std::endl;
    }

    void Process(const opendnp3::HeaderInfo& info, const opendnp3::ICollection<opendnp3::DNPTime>& values) override
    {
        PrintGroupVar(info);
        std::cout << "time" << std::endl << "====" << std::endl;


        auto print = [](const opendnp3::DNPTime& value) {
            auto time = ValueToString(value);
            std::cout << time << std::endl;
        };

        values.ForeachItem(print);
        std::cout << std::endl;
    }
};

class StdErrLogger final : public opendnp3::ILogHandler, public opendnp3::IChannelListener
{
public:
    explicit StdErrLogger(bool printLocation) : printLocation(printLocation) {}

    void log(opendnp3::ModuleId module_id,
             const char* id,
             opendnp3::LogLevel level,
             char const* location,
             char const* message) override
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
        std::cerr << oss.str() << std::endl;
    }

    static std::shared_ptr<StdErrLogger> Create(bool printLocation = false)
    {
        return std::make_shared<StdErrLogger>(printLocation);
    };

    void OnStateChange(opendnp3::ChannelState state) override
    {
        using namespace date;

        auto now = std::chrono::system_clock::now();
        std::cerr << now
                  << "Z [ INFO   ] (channel) state change: " << opendnp3::ChannelStateSpec::to_human_string(state)
                  << std::endl;
    }

private:
    bool printLocation;
    std::mutex mutex;
};

class ResultCallback : public opendnp3::StaticOnly
{

public:
    static opendnp3::CommandResultCallbackT Get()
    {
        return [](const opendnp3::ICommandTaskResult& result) -> void {
            std::cout << "Result Summary: " << opendnp3::TaskCompletionSpec::to_human_string(result.summary)
                      << std::endl;
            auto print = [](const opendnp3::CommandPointResult& res) {
                std::cout << "Header: " << res.headerIndex;
                std::cout << " Index: " << res.index;
                std::cout << " State: " << opendnp3::CommandPointStateSpec::to_human_string(res.state);
                std::cout << " Status: " << opendnp3::CommandStatusSpec::to_human_string(res.status) << std::endl;
            };
            result.ForeachItem(print);
        };
    }
};

class RestartCallback : public opendnp3::StaticOnly
{

public:
    static opendnp3::RestartOperationCallbackT Get()
    {
        return [](const opendnp3::RestartOperationResult& result) {
            if (result.summary == opendnp3::TaskCompletion::SUCCESS)
            {
                std::cout << "Restart succeeded, time to restart: " << result.restartTime.ToString() << std::endl;
            }
            else
            {
                std::cout << "Restart failed: " << opendnp3::TaskCompletionSpec::to_string(result.summary) << std::endl;
            }
        };
    }
};

#endif // CALDERADNP3_OUTSTATION_LOGGER_HPP