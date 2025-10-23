#include "actions.h"

/*********************
 * HELPER FUNCTIONS
 ********************/

std::shared_ptr<opendnp3::IMaster> init_client(std::shared_ptr<opendnp3::IChannel> channel,
                                               uint16_t local_link,
                                               uint16_t remote_link,
                                               const std::string& id)
{
    opendnp3::MasterStackConfig config;
    config.master.responseTimeout = opendnp3::TimeDuration::Seconds(120);
    config.master.disableUnsolOnStartup = false;
    config.master.ignoreRestartIIN = true;
    config.master.unsolClassMask = opendnp3::ClassField::None();
    config.master.startupIntegrityClassMask = opendnp3::ClassField::None();
    config.link.LocalAddr = local_link;
    config.link.RemoteAddr = remote_link;

    auto app = opendnp3::DefaultMasterApplication::Create();
    auto soe_handler = SOEHandler::Create();
    auto client = channel->AddMaster(id, soe_handler, app, config);
    return client;
}

std::shared_ptr<opendnp3::IMaster> init_client(std::shared_ptr<opendnp3::IChannel> channel,
                                               uint16_t local_link,
                                               uint16_t remote_link)
{
    return init_client(channel, local_link, remote_link, "client");
}

void parse_classes(const std::vector<uint8_t>& classes, std::vector<opendnp3::Header>* v)
{
    for (uint8_t i : classes)
    {
        opendnp3::PointClass point_class = opendnp3::PointClassSpec::from_type(1 << i);
        opendnp3::Header header = opendnp3::Header::From(point_class);
        v->push_back(header);
    }
}

/*********************
 * ACTION FUNCTIONS
 ********************/

void operate(std::shared_ptr<opendnp3::IMaster> client,
             const opendnp3::ControlRelayOutputBlock& crob,
             const std::vector<uint16_t>& indices,
             OperateMode mode)
{
    std::vector<opendnp3::Indexed<opendnp3::ControlRelayOutputBlock>> crobs;
    for (const auto& index : indices)
    {
        crobs.push_back(opendnp3::WithIndex(crob, index));
    }
    opendnp3::CommandSet commands;
    commands.Add(crobs);

    if (mode == OperateMode::DIRECT_OPERATE)
    {
        client->DirectOperate(std::move(commands), ResultCallback::Get(), opendnp3::TaskConfig::Default());
    }
    else if (mode == OperateMode::SELECT_BEFORE_OPERATE)
    {
        client->SelectAndOperate(std::move(commands), ResultCallback::Get(), opendnp3::TaskConfig::Default());
    }
}

void read(
    std::shared_ptr<opendnp3::IMaster> client, uint8_t group, uint8_t variation, int32_t start_range, int32_t end_range)
{
    auto soe_handler = SOEHandler::Create();
    auto g = opendnp3::GroupVariationID(group, variation);

    if (start_range > 0 && end_range > 0)
    {
        auto start = static_cast<uint16_t>(start_range);
        auto end = static_cast<uint16_t>(end_range);
        client->ScanRange(g, start, end, soe_handler);
    }
    else
    {
        client->ScanAllObjects(g, soe_handler);
    }
}

void integrity_poll(std::shared_ptr<opendnp3::IMaster> client)
{
    auto soe_handler = SOEHandler::Create();
    client->ScanClasses(opendnp3::ClassField::AllClasses(), soe_handler);
}

void restart_outstation(std::shared_ptr<opendnp3::IMaster> client, opendnp3::RestartType restart_type)
{
    client->Restart(restart_type, RestartCallback::Get());
}

void disable_unsolicited_messages(std::shared_ptr<opendnp3::IMaster> client, const std::vector<uint8_t>& classes)
{
    std::vector<opendnp3::Header> v;
    parse_classes(classes, &v);
    client->PerformFunction("disable unsolicited", opendnp3::FunctionCode::DISABLE_UNSOLICITED, v);
}

void enable_unsolicited_messages(std::shared_ptr<opendnp3::IMaster> client, const std::vector<uint8_t>& classes)
{
    std::vector<opendnp3::Header> v;
    parse_classes(classes, &v);
    client->PerformFunction("enable unsolicited", opendnp3::FunctionCode::ENABLE_UNSOLICITED, v);
}

void set_analog_outputs(std::shared_ptr<opendnp3::IMaster> client,
                        const std::vector<uint16_t>& indices,
                        const std::vector<double>& values,
                        AnalogOutputType output_type)
{
    if (indices.size() != values.size())
    {
        std::cout << "Error: Number of indices must match number of values" << std::endl;
        return;
    }

    opendnp3::CommandSet commands;
    switch (output_type)
    {
    case AnalogOutputType::INT16: {
        auto& header = commands.StartHeader<opendnp3::AnalogOutputInt16>();
        for (size_t i = 0; i < indices.size(); i++)
        {
            header.Add(opendnp3::AnalogOutputInt16(static_cast<int16_t>(values[i])), indices[i]);
        }
        break;
    }
    case AnalogOutputType::INT32: {
        auto& header = commands.StartHeader<opendnp3::AnalogOutputInt32>();
        for (size_t i = 0; i < indices.size(); i++)
        {
            header.Add(opendnp3::AnalogOutputInt32(static_cast<int32_t>(values[i])), indices[i]);
        }
        break;
    }
    case AnalogOutputType::FLOAT32: {
        auto& header = commands.StartHeader<opendnp3::AnalogOutputFloat32>();
        for (size_t i = 0; i < indices.size(); i++)
        {
            header.Add(opendnp3::AnalogOutputFloat32(static_cast<float>(values[i])), indices[i]);
        }
        break;
    }
    // Default to AnalogOutputDouble64
    default:
        auto& header = commands.StartHeader<opendnp3::AnalogOutputDouble64>();
        for (size_t i = 0; i < indices.size(); i++)
        {
            header.Add(opendnp3::AnalogOutputDouble64(values[i]), indices[i]);
        }
        break;
    }

    client->SelectAndOperate(std::move(commands), ResultCallback::Get());
}
