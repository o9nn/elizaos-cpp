#include "/home/runner/work/elizaos-cpp/elizaos-cpp/the-org/src/projectManager/plugins/team-coordinator/actions/reportGenerate.h"

std::shared_ptr<Promise<string>> generateTeamReport(std::shared_ptr<IAgentRuntime> runtime, string standupType, string roomId)
{
    try
    {
        logger->info(std::string("=== GENERATE TEAM REPORT START ==="));
        logger->info(std::string("Generating report for standup type: ") + standupType + string_empty);
        auto roomIdLocal = createUniqueUuid(runtime, std::string("report-channel-config"));
        auto memories = std::async([=]() { runtime->getMemories(object{
            object::pair{std::string("tableName"), std::string("messages")}, 
            object::pair{std::string("agentId"), runtime->agentId}
        }); });
        logger->info(std::string("Retrieved ") + memories->length + std::string(" total messages from room"));
        auto updates = memories->filter([=](auto memory) mutable
        {
            auto content = as<object>(memory["content"]);
            auto contentType = content["type"];
            auto requestedType = standupType->toLowerCase();
            auto checkInType = content["update"]->checkInType;
            return contentType == std::string("team-member-update");
        }
        )->map([=](auto memory) mutable
        {
            return (as<object>(memory["content"]))["update"];
        }
        )->filter([=](auto update) mutable
        {
            return !!update;
        }
        )->sort([=](auto a, auto b) mutable
        {
            return ((std::make_shared<Date>(b["timestamp"])))->getTime() - ((std::make_shared<Date>(a["timestamp"])))->getTime();
        }
        );
        logger->info(std::string("Found ") + updates->length + std::string(" updates matching standup type: ") + standupType + string_empty);
        auto report = std::string("📊 **Team Progress Report - ") + standupType + std::string(" Standups**\
\
");
        if (updates->length == 0) {
            report += std::string("No updates found for "") + standupType + std::string("" standups in this room.\
");
            return report;
        }
        auto updatesByMember = object{};
        for (auto& update : updates)
        {
            logger->info(std::string("Processing update for team member: ") + (OR((update->teamMemberName), (std::string("Unknown")))) + std::string(" (") + update->teamMemberId + std::string(")"));
            if (!const_(updatesByMember)[update->teamMemberId]) {
                updatesByMember[update->teamMemberId] = array<any>();
            }
            const_(updatesByMember)[update->teamMemberId]->push(update);
        }
        for (auto& [teamMemberId, memberUpdates] : Object->entries(updatesByMember))
        {
            shared teamMemberName = OR((const_(memberUpdates)[0]->teamMemberName), (std::string("Unknown")));
            logger->info(std::string("Generating report section for: ") + teamMemberName + std::string(" (") + teamMemberId + std::string(")"));
            report += std::string("👤 **") + teamMemberName + std::string("** (ID: ") + teamMemberId + std::string(")\
\
");
            auto processedUpdates = memberUpdates->map([=](auto update) mutable
            {
                try
                {
                    auto answers = (update->answers) ? any(JSON->parse(update->answers)) : any(object{});
                    return object{
                        object::pair{std::string("teamMemberId"), update->teamMemberId}, 
                        object::pair{std::string("teamMemberName"), update->teamMemberName}, 
                        object::pair{std::string("serverName"), update->serverName}, 
                        object::pair{std::string("checkInType"), update->checkInType}, 
                        object::pair{std::string("timestamp"), update->timestamp}, 
                        object::pair{std::string("answers"), std::string("answers")}
                    };
                }
                catch (const any& error)
                {
                    logger->error(std::string("Error parsing answers JSON:"), error);
                    return update;
                }
            }
            );
            auto prompt = std::string("Analyze these team member updates and provide a detailed productivity report.\
      \
      The "answers" field contains all the update information in a question-answer format.\
      \
      Highlight the following in your analysis:\
      1. Overall Progress: What major tasks/milestones were completed?\
      2. Current Focus: What are they actively working on?\
      3. Productivity Analysis: Are they meeting deadlines? Any patterns in their work?\
      4. Blockers Impact: How are blockers affecting their progress?\
      5. Recommendations: What could improve their productivity?\
\
      Updates data: ") + JSON->stringify(processedUpdates, nullptr, 2) + string_empty;
            logger->info(std::string("Generating productivity analysis for team member:"), teamMemberName);
            try
            {
                auto analysis = std::async([=]() { runtime->useModel(ModelType->TEXT_LARGE, object{
                    object::pair{std::string("prompt"), std::string("prompt")}, 
                    object::pair{std::string("stopSequences"), array<any>()}
                }); });
                report += std::string("📋 **Productivity Analysis**:\
") + analysis + std::string("\
\
");
                report += std::string("📅 **Recent Updates**:\
");
                auto recentUpdates = memberUpdates->slice(0, 3);
                for (auto& update : recentUpdates)
                {
                    report += std::string("\
🕒 ") + ((std::make_shared<Date>(update->timestamp)))->toLocaleString() + std::string("\
");
                    try
                    {
                        auto answers = (update->answers) ? any(JSON->parse(update->answers)) : any(object{});
                        for (auto& [question, answer] : Object->entries(answers))
                        {
                            report += std::string("▫️ **") + question + std::string("**: ") + answer + std::string("\
");
                        }
                    }
                    catch (const any& error)
                    {
                        logger->error(std::string("Error parsing answers JSON for display:"), error);
                        report += std::string("▫️ Error parsing update details\
");
                    }
                }
            }
            catch (const any& error)
            {
                logger->error(std::string("Error generating analysis:"), error);
                report += std::string("❌ Error generating analysis. Showing raw updates:\
\
");
                for (auto& update : memberUpdates)
                {
                    report += std::string("Update from ") + ((std::make_shared<Date>(update->timestamp)))->toLocaleString() + std::string(":\
");
                    try
                    {
                        auto answers = (update->answers) ? any(JSON->parse(update->answers)) : any(object{});
                        for (auto& [question, answer] : Object->entries(answers))
                        {
                            report += std::string("▫️ **") + question + std::string("**: ") + answer + std::string("\
");
                        }
                    }
                    catch (const any& error)
                    {
                        logger->error(std::string("Error parsing answers JSON for display:"), error);
                        report += std::string("▫️ Error parsing update details\
");
                    }
                }
            }
            report += std::string("\
-------------------\
\
");
        }
        logger->info(std::string("Successfully generated team report"));
        logger->info(std::string("=== GENERATE TEAM REPORT END ==="));
        return report;
    }
    catch (const any& error)
    {
        logger->error(std::string("Error generating team report:"), error);
        throw any(error);
    }
};


std::shared_ptr<Action> generateReport = object{
    object::pair{std::string("name"), std::string("GENERATE_REPORT")}, 
    object::pair{std::string("description"), std::string("Generates a comprehensive report of team member updates and productivity analysis")}, 
    object::pair{std::string("similes"), array<string>{ std::string("CREATE_REPORT"), std::string("TEAM_REPORT"), std::string("GET_TEAM_REPORT"), std::string("SHOW_TEAM_REPORT"), std::string("PRODUCE_TEAM_ANALYSIS") }}, 
    object::pair{std::string("validate"), [=](auto runtime, auto message) mutable
    {
        logger->info(std::string("Validating generateReport action"));
        return true;
    }
    }, 
    object::pair{std::string("handler"), [=](auto runtime, auto message, auto state, auto options = object{}, auto callback = undefined) mutable
    {
        try
        {
            logger->info(std::string("=== GENERATE REPORT HANDLER START ==="));
            if (!state) return false;
            if (!callback) {
                logger->warn(std::string("No callback function provided"));
                return false;
            }
            auto text = as<string>(message->content->text);
            if (!text) {
                logger->warn(std::string("No text content found in message"));
                return false;
            }
            shared<string> standupType;
            try
            {
                auto prompt = std::string("Extract the standup type from this text. Try to understand the sentence and its context.\
        Return one of these values: STANDUP, SPRINT, MENTAL_HEALTH, PROJECT_STATUS, RETRO.\
        If you can't determine a specific type, use STANDUP as default.\
        \
        Text: "") + text + std::string(""");
                auto parsedType = std::async([=]() { runtime->useModel(ModelType->TEXT_LARGE, object{
                    object::pair{std::string("prompt"), std::string("prompt")}, 
                    object::pair{std::string("stopSequences"), array<any>()}
                }); });
                logger->info(std::string("AI parsed standup type:"), parsedType);
                if (AND((!state["standupType"]), (!parsedType))) {
                    logger->info(std::string("Asking for standup type"));
                    auto template = std::string("Please select a check-in type:\
          - Daily Standup (STANDUP)\
          - Sprint Check-in (SPRINT) \
          - Mental Health Check-in (MENTAL_HEALTH)\
          - Project Status Update (PROJECT_STATUS)\
          - Team Retrospective (RETRO)");
                    auto promptContent = object{
                        object::pair{std::string("text"), template}, 
                        object::pair{std::string("source"), std::string("discord")}
                    };
                    std::async([=]() { callback(promptContent, array<any>()); });
                    return true;
                }
                standupType = (OR(((as<string>(state["standupType"]))), (parsedType)))["toLowerCase"]()["trim"]();
                logger->info(std::string("Generating report with parameters:"), object{
                    object::pair{std::string("standupType"), std::string("standupType")}, 
                    object::pair{std::string("roomId"), message->roomId}
                });
                auto validTypes = array<string>{ std::string("standup"), std::string("sprint"), std::string("mental_health"), std::string("project_status"), std::string("retro") };
                auto isValidType = validTypes->some([=](auto type) mutable
                {
                    return standupType == type;
                }
                );
                if (!isValidType) {
                    std::async([=]() { callback(object{
                        object::pair{std::string("text"), std::string("Invalid check-in type. Please select one of: Daily Standup, Sprint Check-in, Mental Health Check-in, Project Status Update, or Team Retrospective")}, 
                        object::pair{std::string("source"), std::string("discord")}
                    }, array<any>()); });
                    return false;
                }
            }
            catch (const any& aiError)
            {
                logger->error(std::string("Error using AI to parse input:"), aiError);
                std::async([=]() { callback(object{
                    object::pair{std::string("text"), std::string("I couldn't understand the check-in type. Please try again with a valid type.")}, 
                    object::pair{std::string("source"), std::string("discord")}
                }, array<any>()); });
                return false;
            }
            auto report = std::async([=]() { generateTeamReport(runtime, standupType, message->roomId); });
            auto content = object{
                object::pair{std::string("text"), report}, 
                object::pair{std::string("source"), std::string("discord")}
            };
            std::async([=]() { callback(content, array<any>()); });
            logger->info(std::string("=== GENERATE REPORT HANDLER END ==="));
            return true;
        }
        catch (const any& error)
        {
            auto err = as<std::shared_ptr<Error>>(error);
            logger->error(std::string("=== GENERATE REPORT HANDLER ERROR ==="));
            logger->error(std::string("Error details:"), object{
                object::pair{std::string("name"), OR((err->name), (std::string("Unknown error")))}, 
                object::pair{std::string("message"), OR((err->message), (std::string("No error message")))}, 
                object::pair{std::string("stack"), OR((err->stack), (std::string("No stack trace")))}
            });
            if (callback) {
                auto errorContent = object{
                    object::pair{std::string("text"), std::string("❌ An error occurred while generating the report. Please try again.")}, 
                    object::pair{std::string("source"), std::string("discord")}
                };
                std::async([=]() { callback(errorContent, array<any>()); });
            }
            return false;
        }
    }
    }, 
    object::pair{std::string("examples"), array<array<any>>{ array<object>{ object{
        object::pair{std::string("name"), std::string("{{name1}}")}, 
        object::pair{std::string("content"), object{
            object::pair{std::string("text"), std::string("Generate a daily standup report")}
        }}
    }, object{
        object::pair{std::string("name"), std::string("{{botName}}")}, 
        object::pair{std::string("content"), object{
            object::pair{std::string("text"), std::string("I'll generate a daily standup report for you")}, 
            object::pair{std::string("actions"), array<string>{ std::string("GENERATE_REPORT") }}
        }}
    } }, array<object>{ object{
        object::pair{std::string("name"), std::string("{{name1}}")}, 
        object::pair{std::string("content"), object{
            object::pair{std::string("text"), std::string("Can I see the sprint progress report?")}
        }}
    }, object{
        object::pair{std::string("name"), std::string("{{botName}}")}, 
        object::pair{std::string("content"), object{
            object::pair{std::string("text"), std::string("I'll create a sprint check-in report for you")}, 
            object::pair{std::string("actions"), array<string>{ std::string("GENERATE_REPORT") }}
        }}
    } }, array<object>{ object{
        object::pair{std::string("name"), std::string("{{name1}}")}, 
        object::pair{std::string("content"), object{
            object::pair{std::string("text"), std::string("How is the team doing with the project?")}
        }}
    }, object{
        object::pair{std::string("name"), std::string("{{botName}}")}, 
        object::pair{std::string("content"), object{
            object::pair{std::string("text"), std::string("I'll generate a project status report to show you how the team is progressing")}, 
            object::pair{std::string("actions"), array<string>{ std::string("GENERATE_REPORT") }}
        }}
    } } }}
};

void Main(void)
{
}

MAIN
