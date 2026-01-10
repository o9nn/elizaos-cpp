#include "/home/runner/work/elizaos-cpp/elizaos-cpp/the-org/src/projectManager/plugins/team-coordinator/forms/checkInForm.h"

std::shared_ptr<Promise<void>> sendCheckInReportForm(std::shared_ptr<HandlerCallback> callback, array<object> channels, object serverInfo)
{
    logger->info(std::string("Sending check-in report form to Discord..."));
    logger->info(std::string("Server context:"), object{
        object::pair{std::string("serverId"), serverInfo["serverId"]}, 
        object::pair{std::string("serverName"), serverInfo["serverName"]}
    });
    logger->debug(std::string("Received ") + (OR((channels->get_length()), (0))) + std::string(" channels for the form"));
    if (AND((channels), (channels->get_length() > 0))) {
        logger->debug(std::string("Channels received:"));
        for (auto& channel : channels)
        {
            logger->debug(std::string("- ") + channel["name"] + std::string(" (") + channel["id"] + std::string("), type: ") + channel["type"] + string_empty);
        }
    }
    auto channelOptions = OR((channels->map([=](auto channel) mutable
    {
        return (object{
            object::pair{std::string("label"), channel["name"]}, 
            object::pair{std::string("value"), channel["id"]}, 
            object::pair{std::string("description"), std::string("Text channel: #") + channel["name"] + string_empty}
        });
    }
    )), (array<any>()));
    if (channelOptions->get_length() > 0) {
        logger->debug(std::string("Channel options prepared for form: ") + channelOptions->get_length() + std::string(" options"));
    } else {
        logger->warn(std::string("No channel options available for the form"));
    }
    auto formComponents = array<std::shared_ptr<DiscordActionRow>>();
    logger->debug(std::string("Building form components..."));
    formComponents->push(object{
        object::pair{std::string("type"), 1}, 
        object::pair{std::string("components"), array<std::shared_ptr<DiscordTextInput>>{ as<std::shared_ptr<DiscordTextInput>>(object{
            object::pair{std::string("type"), 4}, 
            object::pair{std::string("custom_id"), std::string("server_info")}, 
            object::pair{std::string("value"), JSON->stringify(object{
                object::pair{std::string("serverId"), serverInfo["serverId"]}
            })}, 
            object::pair{std::string("style"), 2}
        }) }}
    });
    if (channelOptions->get_length() > 0) {
        formComponents->push(object{
            object::pair{std::string("type"), 1}, 
            object::pair{std::string("components"), array<std::shared_ptr<DiscordSelectMenu>>{ as<std::shared_ptr<DiscordSelectMenu>>(object{
                object::pair{std::string("type"), 3}, 
                object::pair{std::string("custom_id"), std::string("report_channel")}, 
                object::pair{std::string("placeholder"), std::string("Select channel to send check-in updates")}, 
                object::pair{std::string("options"), channelOptions}, 
                object::pair{std::string("required"), true}
            }) }}
        });
        logger->debug(std::string("Added channel selector for check-in updates"));
    }
    formComponents->push(object{
        object::pair{std::string("type"), 1}, 
        object::pair{std::string("components"), array<std::shared_ptr<DiscordButton>>{ as<std::shared_ptr<DiscordButton>>(object{
            object::pair{std::string("type"), 2}, 
            object::pair{std::string("style"), 1}, 
            object::pair{std::string("custom_id"), std::string("submit_report_channel")}, 
            object::pair{std::string("label"), std::string("Confirm Channel")}
        }), as<std::shared_ptr<DiscordButton>>(object{
            object::pair{std::string("type"), 2}, 
            object::pair{std::string("style"), 2}, 
            object::pair{std::string("custom_id"), std::string("cancel_report_setup")}, 
            object::pair{std::string("label"), std::string("Cancel")}
        }) }}
    });
    logger->debug(std::string("Added submit/cancel buttons"));
    auto content = object{
        object::pair{std::string("text"), std::string("Select a channel where check-in updates should be sent when users submit their responses after this you can create a check in schedule:")}, 
        object::pair{std::string("source"), std::string("discord")}, 
        object::pair{std::string("components"), formComponents}
    };
    try
    {
        logger->info(std::string("Sending check-in report channel selection form to Discord..."));
        logger->debug(std::string("Server info being sent:"), serverInfo);
        logger->debug(std::string("Form components:"), JSON->stringify(formComponents, nullptr, 2));
        logger->debug(std::string("Components count: ") + formComponents->get_length() + string_empty);
        if (formComponents->get_length() > 5) {
            logger->error(std::string("ERROR: Trying to send ") + formComponents->get_length() + std::string(" components, but Discord only allows 5"));
            content->components = formComponents->slice(0, 5);
            logger->warn(std::string("Components trimmed to 5 to avoid Discord API error"));
        }
        std::async([=]() { callback(content, array<any>()); });
        logger->info(std::string("Successfully sent check-in report form"));
    }
    catch (const any& error)
    {
        auto err = as<std::shared_ptr<Error>>(error);
        logger->error(std::string("Error sending check-in report form: ") + err + string_empty);
        logger->error(std::string("Error stack:"), err->stack);
        throw any(error);
    }
    return std::shared_ptr<Promise<void>>();
};


