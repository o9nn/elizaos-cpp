#include "checkInForm.hpp"

std::shared_ptr<Promise<void>> sendCheckInReportForm(std::shared_ptr<HandlerCallback> callback, array<object> channels, object serverInfo)
{
    logger->info(std:("Sending check-in report form to Discord..."));
    logger->info(std:("Server context:"), object{
        object::pair{std:("serverId"), serverInfo["serverId"]}, 
        object::pair{std:("serverName"), serverInfo["serverName"]}
    });
    logger->debug(std:("Received ") + (OR((channels->get_length()), (0))) + std:(" channels for the form"));
    if (AND((channels), (channels->get_length() > 0))) {
        logger->debug(std:("Channels received:"));
        for (auto& channel : channels)
        {
            logger->debug(std:("- ") + channel["name"] + std:(" (") + channel["id"] + std:("), type: ") + channel["type"] + string_empty);
        }
    }
    auto channelOptions = OR((channels->map([=](auto channel) mutable
    {
        return (object{
            object::pair{std:("label"), channel["name"]}, 
            object::pair{std:("value"), channel["id"]}, 
            object::pair{std:("description"), std:("Text channel: #") + channel["name"] + string_empty}
        });
    }
    )), (array<any>()));
    if (channelOptions->get_length() > 0) {
        logger->debug(std:("Channel options prepared for form: ") + channelOptions->get_length() + std:(" options"));
    } else {
        logger->warn(std:("No channel options available for the form"));
    }
    auto formComponents = array<std::shared_ptr<DiscordActionRow>>();
    logger->debug(std:("Building form components..."));
    formComponents->push(object{
        object::pair{std:("type"), 1}, 
        object::pair{std:("components"), array<std::shared_ptr<DiscordTextInput>>{ as<std::shared_ptr<DiscordTextInput>>(object{
            object::pair{std:("type"), 4}, 
            object::pair{std:("custom_id"), std:("server_info")}, 
            object::pair{std:("value"), JSON->stringify(object{
                object::pair{std:("serverId"), serverInfo["serverId"]}
            })}, 
            object::pair{std:("style"), 2}
        }) }}
    });
    if (channelOptions->get_length() > 0) {
        formComponents->push(object{
            object::pair{std:("type"), 1}, 
            object::pair{std:("components"), array<std::shared_ptr<DiscordSelectMenu>>{ as<std::shared_ptr<DiscordSelectMenu>>(object{
                object::pair{std:("type"), 3}, 
                object::pair{std:("custom_id"), std:("report_channel")}, 
                object::pair{std:("placeholder"), std:("Select channel to send check-in updates")}, 
                object::pair{std:("options"), channelOptions}, 
                object::pair{std:("required"), true}
            }) }}
        });
        logger->debug(std:("Added channel selector for check-in updates"));
    }
    formComponents->push(object{
        object::pair{std:("type"), 1}, 
        object::pair{std:("components"), array<std::shared_ptr<DiscordButton>>{ as<std::shared_ptr<DiscordButton>>(object{
            object::pair{std:("type"), 2}, 
            object::pair{std:("style"), 1}, 
            object::pair{std:("custom_id"), std:("submit_report_channel")}, 
            object::pair{std:("label"), std:("Confirm Channel")}
        }), as<std::shared_ptr<DiscordButton>>(object{
            object::pair{std:("type"), 2}, 
            object::pair{std:("style"), 2}, 
            object::pair{std:("custom_id"), std:("cancel_report_setup")}, 
            object::pair{std:("label"), std:("Cancel")}
        }) }}
    });
    logger->debug(std:("Added submit/cancel buttons"));
    auto content = object{
        object::pair{std:("text"), std:("Select a channel where check-in updates should be sent when users submit their responses after this you can create a check in schedule:")}, 
        object::pair{std:("source"), std:("discord")}, 
        object::pair{std:("components"), formComponents}
    };
    try
    {
        logger->info(std:("Sending check-in report channel selection form to Discord..."));
        logger->debug(std:("Server info being sent:"), serverInfo);
        logger->debug(std:("Form components:"), JSON->stringify(formComponents, nullptr, 2));
        logger->debug(std:("Components count: ") + formComponents->get_length() + string_empty);
        if (formComponents->get_length() > 5) {
            logger->error(std:("ERROR: Trying to send ") + formComponents->get_length() + std:(" components, but Discord only allows 5"));
            content->components = formComponents->slice(0, 5);
            logger->warn(std:("Components trimmed to 5 to avoid Discord API error"));
        }
        std::async([=]() { callback(content, array<any>()); });
        logger->info(std:("Successfully sent check-in report form"));
    }
    catch (const any& error)
    {
        auto err = as<std::shared_ptr<Error>>(error);
        logger->error(std:("Error sending check-in report form: ") + err + string_empty);
        logger->error(std:("Error stack:"), err->stack);
        throw any(error);
    }
    return std::shared_ptr<Promise<void>>();
};


