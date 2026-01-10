#include "/home/runner/work/elizaos-cpp/elizaos-cpp/the-org/src/projectManager/plugins/team-coordinator/forms/scheduleForm.h"

std::shared_ptr<Promise<void>> sendCheckInScheduleForm(std::shared_ptr<HandlerCallback> callback, array<object> channels)
{
    logger->info(std::string("Sending check-in schedule form to Discord..."));
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
        object::pair{std::string("components"), array<std::shared_ptr<DiscordSelectMenu>>{ as<std::shared_ptr<DiscordSelectMenu>>(object{
            object::pair{std::string("type"), 3}, 
            object::pair{std::string("custom_id"), std::string("checkin_type")}, 
            object::pair{std::string("placeholder"), std::string("Select check-in type")}, 
            object::pair{std::string("options"), array<object>{ object{
                object::pair{std::string("label"), std::string("Daily Standup")}, 
                object::pair{std::string("value"), std::string("STANDUP")}, 
                object::pair{std::string("description"), std::string("Quick daily team sync-up meeting")}
            }, object{
                object::pair{std::string("label"), std::string("Sprint Check-in")}, 
                object::pair{std::string("value"), std::string("SPRINT")}, 
                object::pair{std::string("description"), std::string("Sprint progress and blockers discussion")}
            }, object{
                object::pair{std::string("label"), std::string("Mental Health Check-in")}, 
                object::pair{std::string("value"), std::string("MENTAL_HEALTH")}, 
                object::pair{std::string("description"), std::string("Team wellness and support check-in")}
            }, object{
                object::pair{std::string("label"), std::string("Project Status Update")}, 
                object::pair{std::string("value"), std::string("PROJECT_STATUS")}, 
                object::pair{std::string("description"), std::string("Detailed project progress review")}
            }, object{
                object::pair{std::string("label"), std::string("Team Retrospective")}, 
                object::pair{std::string("value"), std::string("RETRO")}, 
                object::pair{std::string("description"), std::string("Team reflection and improvement discussion")}
            } }}
        }) }}
    });
    logger->debug(std::string("Added check-in type selector (1/5 components)"));
    if (channelOptions->get_length() > 0) {
        formComponents->push(object{
            object::pair{std::string("type"), 1}, 
            object::pair{std::string("components"), array<std::shared_ptr<DiscordSelectMenu>>{ as<std::shared_ptr<DiscordSelectMenu>>(object{
                object::pair{std::string("type"), 3}, 
                object::pair{std::string("custom_id"), std::string("checkin_channel")}, 
                object::pair{std::string("placeholder"), std::string("Select channel for check-in")}, 
                object::pair{std::string("options"), channelOptions}
            }) }}
        });
        logger->debug(std::string("Added channel selector (2/5 components)"));
    }
    formComponents->push(object{
        object::pair{std::string("type"), 1}, 
        object::pair{std::string("components"), array<std::shared_ptr<DiscordSelectMenu>>{ as<std::shared_ptr<DiscordSelectMenu>>(object{
            object::pair{std::string("type"), 3}, 
            object::pair{std::string("custom_id"), std::string("checkin_frequency")}, 
            object::pair{std::string("placeholder"), std::string("Select check-in frequency")}, 
            object::pair{std::string("options"), array<object>{ object{
                object::pair{std::string("label"), std::string("Weekdays (Mon-Fri)")}, 
                object::pair{std::string("value"), std::string("WEEKDAYS")}, 
                object::pair{std::string("description"), std::string("Every Monday through Friday")}
            }, object{
                object::pair{std::string("label"), std::string("Daily (All days)")}, 
                object::pair{std::string("value"), std::string("DAILY")}, 
                object::pair{std::string("description"), std::string("Every day including weekends")}
            }, object{
                object::pair{std::string("label"), std::string("Weekly")}, 
                object::pair{std::string("value"), std::string("WEEKLY")}, 
                object::pair{std::string("description"), std::string("Once every week")}
            }, object{
                object::pair{std::string("label"), std::string("Bi-weekly")}, 
                object::pair{std::string("value"), std::string("BIWEEKLY")}, 
                object::pair{std::string("description"), std::string("Once every two weeks")}
            }, object{
                object::pair{std::string("label"), std::string("Monthly")}, 
                object::pair{std::string("value"), std::string("MONTHLY")}, 
                object::pair{std::string("description"), std::string("Once every month")}
            }, object{
                object::pair{std::string("label"), std::string("Custom")}, 
                object::pair{std::string("value"), std::string("CUSTOM")}, 
                object::pair{std::string("description"), std::string("Custom schedule")}
            } }}
        }) }}
    });
    logger->debug(std::string("Added frequency selector (3/5 components)"));
    formComponents->push(object{
        object::pair{std::string("type"), 1}, 
        object::pair{std::string("components"), array<std::shared_ptr<DiscordSelectMenu>>{ as<std::shared_ptr<DiscordSelectMenu>>(object{
            object::pair{std::string("type"), 3}, 
            object::pair{std::string("custom_id"), std::string("checkin_time")}, 
            object::pair{std::string("placeholder"), std::string("Select check-in time")}, 
            object::pair{std::string("options"), array<object>{ object{
                object::pair{std::string("label"), std::string("6:00 AM")}, 
                object::pair{std::string("value"), std::string("06:00")}
            }, object{
                object::pair{std::string("label"), std::string("6:30 AM")}, 
                object::pair{std::string("value"), std::string("06:30")}
            }, object{
                object::pair{std::string("label"), std::string("7:00 AM")}, 
                object::pair{std::string("value"), std::string("07:00")}
            }, object{
                object::pair{std::string("label"), std::string("7:30 AM")}, 
                object::pair{std::string("value"), std::string("07:30")}
            }, object{
                object::pair{std::string("label"), std::string("8:00 AM")}, 
                object::pair{std::string("value"), std::string("08:00")}
            }, object{
                object::pair{std::string("label"), std::string("8:30 AM")}, 
                object::pair{std::string("value"), std::string("08:30")}
            }, object{
                object::pair{std::string("label"), std::string("9:00 AM")}, 
                object::pair{std::string("value"), std::string("09:00")}
            }, object{
                object::pair{std::string("label"), std::string("9:30 AM")}, 
                object::pair{std::string("value"), std::string("09:30")}
            }, object{
                object::pair{std::string("label"), std::string("10:00 AM")}, 
                object::pair{std::string("value"), std::string("10:00")}
            }, object{
                object::pair{std::string("label"), std::string("10:30 AM")}, 
                object::pair{std::string("value"), std::string("10:30")}
            }, object{
                object::pair{std::string("label"), std::string("11:00 AM")}, 
                object::pair{std::string("value"), std::string("11:00")}
            }, object{
                object::pair{std::string("label"), std::string("11:30 AM")}, 
                object::pair{std::string("value"), std::string("11:30")}
            }, object{
                object::pair{std::string("label"), std::string("12:00 PM")}, 
                object::pair{std::string("value"), std::string("12:00")}
            }, object{
                object::pair{std::string("label"), std::string("1:00 PM")}, 
                object::pair{std::string("value"), std::string("13:00")}
            }, object{
                object::pair{std::string("label"), std::string("1:30 PM")}, 
                object::pair{std::string("value"), std::string("13:30")}
            }, object{
                object::pair{std::string("label"), std::string("2:00 PM")}, 
                object::pair{std::string("value"), std::string("14:00")}
            }, object{
                object::pair{std::string("label"), std::string("2:30 PM")}, 
                object::pair{std::string("value"), std::string("14:30")}
            }, object{
                object::pair{std::string("label"), std::string("3:00 PM")}, 
                object::pair{std::string("value"), std::string("15:00")}
            }, object{
                object::pair{std::string("label"), std::string("3:30 PM")}, 
                object::pair{std::string("value"), std::string("15:30")}
            }, object{
                object::pair{std::string("label"), std::string("4:00 PM")}, 
                object::pair{std::string("value"), std::string("16:00")}
            }, object{
                object::pair{std::string("label"), std::string("4:30 PM")}, 
                object::pair{std::string("value"), std::string("16:30")}
            }, object{
                object::pair{std::string("label"), std::string("5:00 PM")}, 
                object::pair{std::string("value"), std::string("17:00")}
            }, object{
                object::pair{std::string("label"), std::string("5:30 PM")}, 
                object::pair{std::string("value"), std::string("17:30")}
            }, object{
                object::pair{std::string("label"), std::string("6:00 PM")}, 
                object::pair{std::string("value"), std::string("18:00")}
            }, object{
                object::pair{std::string("label"), std::string("6:30 PM")}, 
                object::pair{std::string("value"), std::string("18:30")}
            } }}
        }) }}
    });
    logger->debug(std::string("Added time selector (4/5 components)"));
    formComponents->push(object{
        object::pair{std::string("type"), 1}, 
        object::pair{std::string("components"), array<std::shared_ptr<DiscordButton>>{ as<std::shared_ptr<DiscordButton>>(object{
            object::pair{std::string("type"), 2}, 
            object::pair{std::string("style"), 1}, 
            object::pair{std::string("custom_id"), std::string("submit_checkin_schedule")}, 
            object::pair{std::string("label"), std::string("Create Check-in Schedule")}
        }), as<std::shared_ptr<DiscordButton>>(object{
            object::pair{std::string("type"), 2}, 
            object::pair{std::string("style"), 2}, 
            object::pair{std::string("custom_id"), std::string("cancel_checkin_schedule")}, 
            object::pair{std::string("label"), std::string("Cancel")}
        }) }}
    });
    logger->debug(std::string("Added submit/cancel buttons (5/5 components)"));
    auto content = object{
        object::pair{std::string("text"), std::string("Set up a check-in schedule:")}, 
        object::pair{std::string("source"), std::string("discord")}, 
        object::pair{std::string("components"), formComponents}
    };
    try
    {
        logger->info(std::string("Sending check-in schedule form to Discord..."));
        logger->debug(std::string("Components count: ") + formComponents->get_length() + string_empty);
        if (formComponents->get_length() > 5) {
            logger->error(std::string("ERROR: Trying to send ") + formComponents->get_length() + std::string(" components, but Discord only allows 5"));
            content->components = formComponents->slice(0, 5);
            logger->warn(std::string("Components trimmed to 5 to avoid Discord API error"));
        }
        std::async([=]() { callback(content, array<any>()); });
        logger->info(std::string("Successfully sent check-in schedule form"));
    }
    catch (const any& error)
    {
        auto err = as<std::shared_ptr<Error>>(error);
        logger->error(std::string("Error sending check-in schedule form: ") + err + string_empty);
        logger->error(std::string("Error stack:"), err->stack);
        throw any(error);
    }
    return std::shared_ptr<Promise<void>>();
};


