#include "/home/runner/work/elizaos-cpp/elizaos-cpp/the-org/src/projectManager/plugins/team-coordinator/actions/checkInList.h"

std::shared_ptr<Promise<array<std::shared_ptr<CheckInSchedule>>>> fetchCheckInSchedules(std::shared_ptr<IAgentRuntime> runtime)
{
    try
    {
        logger->info(std::string("=== FETCH CHECK-IN SCHEDULES START ==="));
        auto checkInSchedulesRoomId = createUniqueUuid(runtime, std::string("check-in-schedules"));
        logger->info(std::string("Generated check-in schedules room ID: ") + checkInSchedulesRoomId + string_empty);
        logger->info(std::string("Attempting to fetch memories from room..."));
        auto memories = std::async([=]() { runtime->getMemories(object{
            object::pair{std::string("roomId"), checkInSchedulesRoomId}, 
            object::pair{std::string("tableName"), std::string("messages")}
        }); });
        logger->info(std::string("Found ") + memories->length + std::string(" total memories in check-in schedules room"));
        memories->slice(0, 3)->forEach([=](auto memory, auto index) mutable
        {
            logger->info(std::string("Memory ") + index + std::string(" content:"), object{
                object::pair{std::string("id"), memory["id"]}, 
                object::pair{std::string("type"), memory["content"]["type"]}, 
                object::pair{std::string("hasSchedule"), !!memory["content"]["schedule"]}, 
                object::pair{std::string("contentKeys"), Object->keys(OR((memory["content"]), (object{})))}
            });
        }
        );
        auto schedules = memories->filter([=](auto memory) mutable
        {
            auto isValidType = memory["content"]["type"] == std::string("team-member-checkin-schedule");
            auto hasSchedule = !!memory["content"]["schedule"];
            logger->info(std::string("Memory ") + memory["id"] + std::string(" validation:"), object{
                object::pair{std::string("isValidType"), std::string("isValidType")}, 
                object::pair{std::string("hasSchedule"), std::string("hasSchedule")}, 
                object::pair{std::string("contentType"), memory["content"]["type"]}
            });
            return AND((isValidType), (hasSchedule));
        }
        )->map([=](auto memory) mutable
        {
            auto schedule = as<std::shared_ptr<CheckInSchedule>>(memory["content"]["schedule"]);
            logger->info(std::string("Processing schedule from memory ") + memory["id"] + std::string(":"), object{
                object::pair{std::string("scheduleId"), schedule->scheduleId}, 
                object::pair{std::string("frequency"), schedule->frequency}
            });
            return schedule;
        }
        )->filter([=](auto schedule) mutable
        {
            auto isValid = schedule != undefined;
            if (!isValid) {
                logger->warn(std::string("Found invalid schedule:"), schedule);
            }
            return isValid;
        }
        );
        logger->info(std::string("Successfully extracted ") + schedules->length + std::string(" valid schedules"));
        logger->info(std::string("=== FETCH CHECK-IN SCHEDULES END ==="));
        logger->info(std::string("=== DETAILED SCHEDULES LOG ==="));
        logger->info(std::string("All schedules:"), JSON->stringify(schedules, nullptr, 2));
        logger->info(std::string("=== END DETAILED SCHEDULES LOG ==="));
        return schedules;
    }
    catch (const any& error)
    {
        auto err = as<std::shared_ptr<Error>>(error);
        logger->error(std::string("=== FETCH CHECK-IN SCHEDULES ERROR ==="));
        logger->error(std::string("Error details:"), object{
            object::pair{std::string("name"), OR((err->name), (std::string("Unknown error")))}, 
            object::pair{std::string("message"), OR((err->message), (std::string("No error message")))}, 
            object::pair{std::string("stack"), OR((err->stack), (std::string("No stack trace")))}
        });
        throw any(error);
    }
};


string formatSchedule(std::shared_ptr<CheckInSchedule> schedule)
{
    logger->info(std::string("Formatting schedule:"), object{
        object::pair{std::string("scheduleId"), schedule->scheduleId}, 
        object::pair{std::string("teamMemberName"), OR((schedule->teamMemberUserName), (schedule->teamMemberName))}, 
        object::pair{std::string("checkInType"), schedule->checkInType}, 
        object::pair{std::string("frequency"), schedule->frequency}, 
        object::pair{std::string("checkInTime"), schedule->checkInTime}
    });
    auto formatted = std::string("\
📅 Schedule ID: ") + schedule->scheduleId + std::string("\
📝 Type: ") + schedule->checkInType + std::string("\
📺 Channel ID: ") + schedule->channelId + std::string("\
⏰ Time: ") + schedule->checkInTime + std::string("\
🔄 Frequency: ") + schedule->frequency + std::string("\
📋 Created: ") + ((std::make_shared<Date>(schedule->createdAt)))->toLocaleString() + std::string("\
");
    logger->info(std::string("Successfully formatted schedule"));
    return formatted;
};


std::shared_ptr<Action> listCheckInSchedules = object{
    object::pair{std::string("name"), std::string("LIST_CHECK_IN_SCHEDULES")}, 
    object::pair{std::string("description"), std::string("Lists all schedules for team members")}, 
    object::pair{std::string("similes"), array<string>{ std::string("SHOW_CHECK_INS"), std::string("GET_CHECK_IN_SCHEDULES"), std::string("VIEW_CHECK_IN_SCHEDULES") }}, 
    object::pair{std::string("validate"), [=](auto runtime, auto message) mutable
    {
        logger->info(std::string("Validating listCheckInSchedules action:"), object{
            object::pair{std::string("messageId"), message->id}, 
            object::pair{std::string("entityId"), message->entityId}, 
            object::pair{std::string("contentType"), message->content->type}
        });
        return true;
    }
    }, 
    object::pair{std::string("handler"), [=](auto runtime, auto message, auto state, auto options = object{}, auto callback = undefined) mutable
    {
        try
        {
            logger->info(std::string("=== LIST CHECK-IN SCHEDULES HANDLER START ==="));
            logger->info(std::string("Handler details:"), object{
                object::pair{std::string("messageId"), message->id}, 
                object::pair{std::string("entityId"), message->entityId}, 
                object::pair{std::string("hasCallback"), !!callback}, 
                object::pair{std::string("stateKeys"), (state) ? Object->keys(state) : array<any>()}, 
                object::pair{std::string("optionsKeys"), Object->keys(options)}
            });
            if (!callback) {
                logger->warn(std::string("No callback function provided"));
                return false;
            }
            logger->info(std::string("Fetching check-in schedules..."));
            auto schedules = std::async([=]() { fetchCheckInSchedules(runtime); });
            logger->info(std::string("Retrieved ") + schedules->get_length() + std::string(" schedules"));
            if (schedules->get_length() == 0) {
                logger->info(std::string("No schedules found, sending empty response"));
                std::async([=]() { callback(object{
                    object::pair{std::string("text"), std::string("📝 No check-in schedules found. Use the check-in command to create a new schedule.")}, 
                    object::pair{std::string("source"), std::string("discord")}
                }, array<any>()); });
                return true;
            }
            logger->info(std::string("Formatting schedules for display..."));
            auto formattedSchedules = schedules->map(formatSchedule)->join(std::string("\
-------------------\
"));
            auto content = object{
                object::pair{std::string("text"), std::string("📋 Check-in Schedules (") + schedules->get_length() + std::string(" total):\
") + formattedSchedules + string_empty}, 
                object::pair{std::string("source"), std::string("discord")}
            };
            logger->info(std::string("Sending formatted schedules to callback"));
            std::async([=]() { callback(content, array<any>()); });
            logger->info(std::string("Successfully sent check-in schedules list"));
            logger->info(std::string("=== LIST CHECK-IN SCHEDULES HANDLER END ==="));
            return true;
        }
        catch (const any& error)
        {
            auto err = as<std::shared_ptr<Error>>(error);
            logger->error(std::string("=== LIST CHECK-IN SCHEDULES HANDLER ERROR ==="));
            logger->error(std::string("Error details:"), object{
                object::pair{std::string("name"), OR((err->name), (std::string("Unknown error")))}, 
                object::pair{std::string("message"), OR((err->message), (std::string("No error message")))}, 
                object::pair{std::string("stack"), OR((err->stack), (std::string("No stack trace")))}
            });
            if (callback) {
                std::async([=]() { callback(object{
                    object::pair{std::string("text"), std::string("❌ Error retrieving check-in schedules. Please try again.")}, 
                    object::pair{std::string("source"), std::string("discord")}
                }, array<any>()); });
            }
            return false;
        }
    }
    }, 
    object::pair{std::string("examples"), array<array<any>>{ array<object>{ object{
        object::pair{std::string("name"), std::string("admin")}, 
        object::pair{std::string("content"), object{
            object::pair{std::string("text"), std::string("Show me all check in schedules")}
        }}
    }, object{
        object::pair{std::string("name"), std::string("jimmy")}, 
        object::pair{std::string("content"), object{
            object::pair{std::string("text"), std::string("Here are all the check-in schedules I've found")}, 
            object::pair{std::string("actions"), array<string>{ std::string("LIST_CHECK_IN_SCHEDULES") }}
        }}
    } }, array<object>{ object{
        object::pair{std::string("name"), std::string("admin")}, 
        object::pair{std::string("content"), object{
            object::pair{std::string("text"), std::string("List team check-ins")}
        }}
    }, object{
        object::pair{std::string("name"), std::string("jimmy")}, 
        object::pair{std::string("content"), object{
            object::pair{std::string("text"), std::string("I'll show you all active check-in schedules")}, 
            object::pair{std::string("actions"), array<string>{ std::string("LIST_CHECK_IN_SCHEDULES") }}
        }}
    } }, array<object>{ object{
        object::pair{std::string("name"), std::string("admin")}, 
        object::pair{std::string("content"), object{
            object::pair{std::string("text"), std::string("list of checkins")}
        }}
    }, object{
        object::pair{std::string("name"), std::string("jimmy")}, 
        object::pair{std::string("content"), object{
            object::pair{std::string("text"), std::string("I'll show you all active check-in schedules")}, 
            object::pair{std::string("actions"), array<string>{ std::string("LIST_CHECK_IN_SCHEDULES") }}
        }}
    } } }}
};

void Main(void)
{
}

MAIN
