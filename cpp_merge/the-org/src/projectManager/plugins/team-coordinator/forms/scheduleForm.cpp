#include "scheduleForm.hpp"

std::shared_ptr<Promise<void>> sendCheckInScheduleForm(std::shared_ptr<HandlerCallback> callback, array<object> channels)
{
    logger->info(std:("Sending check-in schedule form to Discord..."));
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
        object::pair{std:("components"), array<std::shared_ptr<DiscordSelectMenu>>{ as<std::shared_ptr<DiscordSelectMenu>>(object{
            object::pair{std:("type"), 3}, 
            object::pair{std:("custom_id"), std:("checkin_type")}, 
            object::pair{std:("placeholder"), std:("Select check-in type")}, 
            object::pair{std:("options"), array<object>{ object{
                object::pair{std:("label"), std:("Daily Standup")}, 
                object::pair{std:("value"), std:("STANDUP")}, 
                object::pair{std:("description"), std:("Quick daily team sync-up meeting")}
            }, object{
                object::pair{std:("label"), std:("Sprint Check-in")}, 
                object::pair{std:("value"), std:("SPRINT")}, 
                object::pair{std:("description"), std:("Sprint progress and blockers discussion")}
            }, object{
                object::pair{std:("label"), std:("Mental Health Check-in")}, 
                object::pair{std:("value"), std:("MENTAL_HEALTH")}, 
                object::pair{std:("description"), std:("Team wellness and support check-in")}
            }, object{
                object::pair{std:("label"), std:("Project Status Update")}, 
                object::pair{std:("value"), std:("PROJECT_STATUS")}, 
                object::pair{std:("description"), std:("Detailed project progress review")}
            }, object{
                object::pair{std:("label"), std:("Team Retrospective")}, 
                object::pair{std:("value"), std:("RETRO")}, 
                object::pair{std:("description"), std:("Team reflection and improvement discussion")}
            } }}
        }) }}
    });
    logger->debug(std:("Added check-in type selector (1/5 components)"));
    if (channelOptions->get_length() > 0) {
        formComponents->push(object{
            object::pair{std:("type"), 1}, 
            object::pair{std:("components"), array<std::shared_ptr<DiscordSelectMenu>>{ as<std::shared_ptr<DiscordSelectMenu>>(object{
                object::pair{std:("type"), 3}, 
                object::pair{std:("custom_id"), std:("checkin_channel")}, 
                object::pair{std:("placeholder"), std:("Select channel for check-in")}, 
                object::pair{std:("options"), channelOptions}
            }) }}
        });
        logger->debug(std:("Added channel selector (2/5 components)"));
    }
    formComponents->push(object{
        object::pair{std:("type"), 1}, 
        object::pair{std:("components"), array<std::shared_ptr<DiscordSelectMenu>>{ as<std::shared_ptr<DiscordSelectMenu>>(object{
            object::pair{std:("type"), 3}, 
            object::pair{std:("custom_id"), std:("checkin_frequency")}, 
            object::pair{std:("placeholder"), std:("Select check-in frequency")}, 
            object::pair{std:("options"), array<object>{ object{
                object::pair{std:("label"), std:("Weekdays (Mon-Fri)")}, 
                object::pair{std:("value"), std:("WEEKDAYS")}, 
                object::pair{std:("description"), std:("Every Monday through Friday")}
            }, object{
                object::pair{std:("label"), std:("Daily (All days)")}, 
                object::pair{std:("value"), std:("DAILY")}, 
                object::pair{std:("description"), std:("Every day including weekends")}
            }, object{
                object::pair{std:("label"), std:("Weekly")}, 
                object::pair{std:("value"), std:("WEEKLY")}, 
                object::pair{std:("description"), std:("Once every week")}
            }, object{
                object::pair{std:("label"), std:("Bi-weekly")}, 
                object::pair{std:("value"), std:("BIWEEKLY")}, 
                object::pair{std:("description"), std:("Once every two weeks")}
            }, object{
                object::pair{std:("label"), std:("Monthly")}, 
                object::pair{std:("value"), std:("MONTHLY")}, 
                object::pair{std:("description"), std:("Once every month")}
            }, object{
                object::pair{std:("label"), std:("Custom")}, 
                object::pair{std:("value"), std:("CUSTOM")}, 
                object::pair{std:("description"), std:("Custom schedule")}
            } }}
        }) }}
    });
    logger->debug(std:("Added frequency selector (3/5 components)"));
    formComponents->push(object{
        object::pair{std:("type"), 1}, 
        object::pair{std:("components"), array<std::shared_ptr<DiscordSelectMenu>>{ as<std::shared_ptr<DiscordSelectMenu>>(object{
            object::pair{std:("type"), 3}, 
            object::pair{std:("custom_id"), std:("checkin_time")}, 
            object::pair{std:("placeholder"), std:("Select check-in time")}, 
            object::pair{std:("options"), array<object>{ object{
                object::pair{std:("label"), std:("6:00 AM")}, 
                object::pair{std:("value"), std:("06:00")}
            }, object{
                object::pair{std:("label"), std:("6:30 AM")}, 
                object::pair{std:("value"), std:("06:30")}
            }, object{
                object::pair{std:("label"), std:("7:00 AM")}, 
                object::pair{std:("value"), std:("07:00")}
            }, object{
                object::pair{std:("label"), std:("7:30 AM")}, 
                object::pair{std:("value"), std:("07:30")}
            }, object{
                object::pair{std:("label"), std:("8:00 AM")}, 
                object::pair{std:("value"), std:("08:00")}
            }, object{
                object::pair{std:("label"), std:("8:30 AM")}, 
                object::pair{std:("value"), std:("08:30")}
            }, object{
                object::pair{std:("label"), std:("9:00 AM")}, 
                object::pair{std:("value"), std:("09:00")}
            }, object{
                object::pair{std:("label"), std:("9:30 AM")}, 
                object::pair{std:("value"), std:("09:30")}
            }, object{
                object::pair{std:("label"), std:("10:00 AM")}, 
                object::pair{std:("value"), std:("10:00")}
            }, object{
                object::pair{std:("label"), std:("10:30 AM")}, 
                object::pair{std:("value"), std:("10:30")}
            }, object{
                object::pair{std:("label"), std:("11:00 AM")}, 
                object::pair{std:("value"), std:("11:00")}
            }, object{
                object::pair{std:("label"), std:("11:30 AM")}, 
                object::pair{std:("value"), std:("11:30")}
            }, object{
                object::pair{std:("label"), std:("12:00 PM")}, 
                object::pair{std:("value"), std:("12:00")}
            }, object{
                object::pair{std:("label"), std:("1:00 PM")}, 
                object::pair{std:("value"), std:("13:00")}
            }, object{
                object::pair{std:("label"), std:("1:30 PM")}, 
                object::pair{std:("value"), std:("13:30")}
            }, object{
                object::pair{std:("label"), std:("2:00 PM")}, 
                object::pair{std:("value"), std:("14:00")}
            }, object{
                object::pair{std:("label"), std:("2:30 PM")}, 
                object::pair{std:("value"), std:("14:30")}
            }, object{
                object::pair{std:("label"), std:("3:00 PM")}, 
                object::pair{std:("value"), std:("15:00")}
            }, object{
                object::pair{std:("label"), std:("3:30 PM")}, 
                object::pair{std:("value"), std:("15:30")}
            }, object{
                object::pair{std:("label"), std:("4:00 PM")}, 
                object::pair{std:("value"), std:("16:00")}
            }, object{
                object::pair{std:("label"), std:("4:30 PM")}, 
                object::pair{std:("value"), std:("16:30")}
            }, object{
                object::pair{std:("label"), std:("5:00 PM")}, 
                object::pair{std:("value"), std:("17:00")}
            }, object{
                object::pair{std:("label"), std:("5:30 PM")}, 
                object::pair{std:("value"), std:("17:30")}
            }, object{
                object::pair{std:("label"), std:("6:00 PM")}, 
                object::pair{std:("value"), std:("18:00")}
            }, object{
                object::pair{std:("label"), std:("6:30 PM")}, 
                object::pair{std:("value"), std:("18:30")}
            } }}
        }) }}
    });
    logger->debug(std:("Added time selector (4/5 components)"));
    formComponents->push(object{
        object::pair{std:("type"), 1}, 
        object::pair{std:("components"), array<std::shared_ptr<DiscordButton>>{ as<std::shared_ptr<DiscordButton>>(object{
            object::pair{std:("type"), 2}, 
            object::pair{std:("style"), 1}, 
            object::pair{std:("custom_id"), std:("submit_checkin_schedule")}, 
            object::pair{std:("label"), std:("Create Check-in Schedule")}
        }), as<std::shared_ptr<DiscordButton>>(object{
            object::pair{std:("type"), 2}, 
            object::pair{std:("style"), 2}, 
            object::pair{std:("custom_id"), std:("cancel_checkin_schedule")}, 
            object::pair{std:("label"), std:("Cancel")}
        }) }}
    });
    logger->debug(std:("Added submit/cancel buttons (5/5 components)"));
    auto content = object{
        object::pair{std:("text"), std:("Set up a check-in schedule:")}, 
        object::pair{std:("source"), std:("discord")}, 
        object::pair{std:("components"), formComponents}
    };
    try
    {
        logger->info(std:("Sending check-in schedule form to Discord..."));
        logger->debug(std:("Components count: ") + formComponents->get_length() + string_empty);
        if (formComponents->get_length() > 5) {
            logger->error(std:("ERROR: Trying to send ") + formComponents->get_length() + std:(" components, but Discord only allows 5"));
            content->components = formComponents->slice(0, 5);
            logger->warn(std:("Components trimmed to 5 to avoid Discord API error"));
        }
        std::async([=]() { callback(content, array<any>()); });
        logger->info(std:("Successfully sent check-in schedule form"));
    }
    catch (const any& error)
    {
        auto err = as<std::shared_ptr<Error>>(error);
        logger->error(std:("Error sending check-in schedule form: ") + err + string_empty);
        logger->error(std:("Error stack:"), err->stack);
        throw any(error);
    }
    return std::shared_ptr<Promise<void>>();
};


