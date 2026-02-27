#include "/home/runner/work/elizaos-cpp/elizaos-cpp/the-org/src/projectManager/plugins/team-coordinator/actions/checkInList.h"

std::shared_ptr<Promise<array<std::shared_ptr<CheckInSchedule>>>> fetchCheckInSchedules(std::shared_ptr<IAgentRuntime> runtime)
{
    try
    {
        logger->info(std:("== FETCH CHECK-IN SCHEDULES START =="));
        auto checkInSchedulesRoomId = createUniqueUuid(runtime, std:("check-in-schedules"));
        logger->info(std:("Generated check-in schedules room ID: ") + checkInSchedulesRoomId + string_empty);
        logger->info(std:("Attempting to fetch memories from room..."));
        auto memories = std::async([=]() { runtime->getMemories(object{
            object::pair{std:("roomId"), checkInSchedulesRoomId}, 
            object::pair{std:("tableName"), std:("messages")}
        }); });
        logger->info(std:("Found ") + memories->length + std:(" total memories in check-in schedules room"));
        memories->slice(0, 3)->forEach([=](auto memory, auto index) mutable
        {
            logger->info(std:("Memory ") + index + std:(" content:"), object{
                object::pair{std:("id"), memory["id"]}, 
                object::pair{std:("type"), memory["content"]["type"]}, 
                object::pair{std:("hasSchedule"), !!memory["content"]["schedule"]}, 
                object::pair{std:("contentKeys"), Object->keys(OR((memory["content"]), (object{})))}
            });
        }
        );
        auto schedules = memories->filter([=](auto memory) mutable
        {
            auto isValidType = memory["content"]["type"] == std:("team-member-checkin-schedule");
            auto hasSchedule = !!memory["content"]["schedule"];
            logger->info(std:("Memory ") + memory["id"] + std:(" validation:"), object{
                object::pair{std:("isValidType"), std:("isValidType")}, 
                object::pair{std:("hasSchedule"), std:("hasSchedule")}, 
                object::pair{std:("contentType"), memory["content"]["type"]}
            });
            return AND((isValidType), (hasSchedule));
        }
        )->map([=](auto memory) mutable
        {
            auto schedule = as<std::shared_ptr<CheckInSchedule>>(memory["content"]["schedule"]);
            logger->info(std:("Processing schedule from memory ") + memory["id"] + std:(":"), object{
                object::pair{std:("scheduleId"), schedule->scheduleId}, 
                object::pair{std:("frequency"), schedule->frequency}
            });
            return schedule;
        }
        )->filter([=](auto schedule) mutable
        {
            auto isValid = schedule != undefined;
            if (!isValid) {
                logger->warn(std:("Found invalid schedule:"), schedule);
            }
            return isValid;
        }
        );
        logger->info(std:("Successfully extracted ") + schedules->length + std:(" valid schedules"));
        logger->info(std:("== FETCH CHECK-IN SCHEDULES END =="));
        logger->info(std:("== DETAILED SCHEDULES LOG =="));
        logger->info(std:("All schedules:"), JSON->stringify(schedules, nullptr, 2));
        logger->info(std:("== END DETAILED SCHEDULES LOG =="));
        return schedules;
    }
    catch (const any& error)
    {
        auto err = as<std::shared_ptr<Error>>(error);
        logger->error(std:("== FETCH CHECK-IN SCHEDULES ERROR =="));
        logger->error(std:("Error details:"), object{
            object::pair{std:("name"), OR((err->name), (std:("Unknown error")))}, 
            object::pair{std:("message"), OR((err->message), (std:("No error message")))}, 
            object::pair{std:("stack"), OR((err->stack), (std:("No stack trace")))}
        });
        throw any(error);
    }
};


string formatSchedule(std::shared_ptr<CheckInSchedule> schedule)
{
    logger->info(std:("Formatting schedule:"), object{
        object::pair{std:("scheduleId"), schedule->scheduleId}, 
        object::pair{std:("teamMemberName"), OR((schedule->teamMemberUserName), (schedule->teamMemberName))}, 
        object::pair{std:("checkInType"), schedule->checkInType}, 
        object::pair{std:("frequency"), schedule->frequency}, 
        object::pair{std:("checkInTime"), schedule->checkInTime}
    });
    auto formatted = std:("\
📅 Schedule ID: ") + schedule->scheduleId + std:("\
📝 Type: ") + schedule->checkInType + std:("\
📺 Channel ID: ") + schedule->channelId + std:("\
⏰ Time: ") + schedule->checkInTime + std:("\
🔄 Frequency: ") + schedule->frequency + std:("\
📋 Created: ") + ((std::make_shared<Date>(schedule->createdAt)))->toLocaleString() + std:("\
");
    logger->info(std:("Successfully formatted schedule"));
    return formatted;
};


std::shared_ptr<Action> listCheckInSchedules = object{
    object::pair{std:("name"), std:("LIST_CHECK_IN_SCHEDULES")}, 
    object::pair{std:("description"), std:("Lists all schedules for team members")}, 
    object::pair{std:("similes"), array<string>{ std:("SHOW_CHECK_INS"), std:("GET_CHECK_IN_SCHEDULES"), std:("VIEW_CHECK_IN_SCHEDULES") }}, 
    object::pair{std:("validate"), [=](auto runtime, auto message) mutable
    {
        logger->info(std:("Validating listCheckInSchedules action:"), object{
            object::pair{std:("messageId"), message->id}, 
            object::pair{std:("entityId"), message->entityId}, 
            object::pair{std:("contentType"), message->content->type}
        });
        return true;
    }
    }, 
    object::pair{std:("handler"), [=](auto runtime, auto message, auto state, auto options = object{}, auto callback = undefined) mutable
    {
        try
        {
            logger->info(std:("== LIST CHECK-IN SCHEDULES HANDLER START =="));
            logger->info(std:("Handler details:"), object{
                object::pair{std:("messageId"), message->id}, 
                object::pair{std:("entityId"), message->entityId}, 
                object::pair{std:("hasCallback"), !!callback}, 
                object::pair{std:("stateKeys"), (state) ? Object->keys(state) : array<any>()}, 
                object::pair{std:("optionsKeys"), Object->keys(options)}
            });
            if (!callback) {
                logger->warn(std:("No callback function provided"));
                return false;
            }
            logger->info(std:("Fetching check-in schedules..."));
            auto schedules = std::async([=]() { fetchCheckInSchedules(runtime); });
            logger->info(std:("Retrieved ") + schedules->get_length() + std:(" schedules"));
            if (schedules->get_length() == 0) {
                logger->info(std:("No schedules found, sending empty response"));
                std::async([=]() { callback(object{
                    object::pair{std:("text"), std:("📝 No check-in schedules found. Use the check-in command to create a new schedule.")}, 
                    object::pair{std:("source"), std:("discord")}
                }, array<any>()); });
                return true;
            }
            logger->info(std:("Formatting schedules for display..."));
            auto formattedSchedules = schedules->map(formatSchedule)->join(std:("\
-------------------\
"));
            auto content = object{
                object::pair{std:("text"), std:("📋 Check-in Schedules (") + schedules->get_length() + std:(" total):\
") + formattedSchedules + string_empty}, 
                object::pair{std:("source"), std:("discord")}
            };
            logger->info(std:("Sending formatted schedules to callback"));
            std::async([=]() { callback(content, array<any>()); });
            logger->info(std:("Successfully sent check-in schedules list"));
            logger->info(std:("== LIST CHECK-IN SCHEDULES HANDLER END =="));
            return true;
        }
        catch (const any& error)
        {
            auto err = as<std::shared_ptr<Error>>(error);
            logger->error(std:("== LIST CHECK-IN SCHEDULES HANDLER ERROR =="));
            logger->error(std:("Error details:"), object{
                object::pair{std:("name"), OR((err->name), (std:("Unknown error")))}, 
                object::pair{std:("message"), OR((err->message), (std:("No error message")))}, 
                object::pair{std:("stack"), OR((err->stack), (std:("No stack trace")))}
            });
            if (callback) {
                std::async([=]() { callback(object{
                    object::pair{std:("text"), std:("❌ Error retrieving check-in schedules. Please try again.")}, 
                    object::pair{std:("source"), std:("discord")}
                }, array<any>()); });
            }
            return false;
        }
    }
    }, 
    object::pair{std:("examples"), array<array<any>>{ array<object>{ object{
        object::pair{std:("name"), std:("admin")}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), std:("Show me all check in schedules")}
        }}
    }, object{
        object::pair{std:("name"), std:("jimmy")}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), std:("Here are all the check-in schedules I've found")}, 
            object::pair{std:("actions"), array<string>{ std:("LIST_CHECK_IN_SCHEDULES") }}
        }}
    } }, array<object>{ object{
        object::pair{std:("name"), std:("admin")}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), std:("List team check-ins")}
        }}
    }, object{
        object::pair{std:("name"), std:("jimmy")}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), std:("I'll show you all active check-in schedules")}, 
            object::pair{std:("actions"), array<string>{ std:("LIST_CHECK_IN_SCHEDULES") }}
        }}
    } }, array<object>{ object{
        object::pair{std:("name"), std:("admin")}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), std:("list of checkins")}
        }}
    }, object{
        object::pair{std:("name"), std:("jimmy")}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), std:("I'll show you all active check-in schedules")}, 
            object::pair{std:("actions"), array<string>{ std:("LIST_CHECK_IN_SCHEDULES") }}
        }}
    } } }}
};

void Main(void)
{
}

MAIN
