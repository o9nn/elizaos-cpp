#include "reportGenerate.hpp"

std::shared_ptr<Promise<string>> generateTeamReport(std::shared_ptr<IAgentRuntime> runtime, string standupType, string roomId)
{
    try
    {
        logger->info(std:("== GENERATE TEAM REPORT START =="));
        logger->info(std:("Generating report for standup type: ") + standupType + string_empty);
        auto roomIdLocal = createUniqueUuid(runtime, std:("report-channel-config"));
        auto memories = std::async([=]() { runtime->getMemories(object{
            object::pair{std:("tableName"), std:("messages")}, 
            object::pair{std:("agentId"), runtime->agentId}
        }); });
        logger->info(std:("Retrieved ") + memories->length + std:(" total messages from room"));
        auto updates = memories->filter([=](auto memory) mutable
        {
            auto content = as<object>(memory["content"]);
            auto contentType = content["type"];
            auto requestedType = standupType->toLowerCase();
            auto checkInType = content["update"]->checkInType;
            return contentType == std:("team-member-update");
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
        logger->info(std:("Found ") + updates->length + std:(" updates matching standup type: ") + standupType + string_empty);
        auto report = std:("📊 **Team Progress Report - ") + standupType + std:(" Standups**\
\
");
        if (updates->length == 0) {
            report += std:("No updates found for "") + standupType + std:("" standups in this room.\
");
            return report;
        }
        auto updatesByMember = object{};
        for (auto& update : updates)
        {
            logger->info(std:("Processing update for team member: ") + (OR((update->teamMemberName), (std:("Unknown")))) + std:(" (") + update->teamMemberId + std:(")"));
            if (!const_(updatesByMember)[update->teamMemberId]) {
                updatesByMember[update->teamMemberId] = array<any>();
            }
            const_(updatesByMember)[update->teamMemberId]->push(update);
        }
        for (auto& [teamMemberId, memberUpdates] : Object->entries(updatesByMember))
        {
            shared teamMemberName = OR((const_(memberUpdates)[0]->teamMemberName), (std:("Unknown")));
            logger->info(std:("Generating report section for: ") + teamMemberName + std:(" (") + teamMemberId + std:(")"));
            report += std:("👤 **") + teamMemberName + std:("** (ID: ") + teamMemberId + std:(")\
\
");
            auto processedUpdates = memberUpdates->map([=](auto update) mutable
            {
                try
                {
                    auto answers = (update->answers) ? any(JSON->parse(update->answers)) (object{});
                    return object{
                        object::pair{std:("teamMemberId"), update->teamMemberId}, 
                        object::pair{std:("teamMemberName"), update->teamMemberName}, 
                        object::pair{std:("serverName"), update->serverName}, 
                        object::pair{std:("checkInType"), update->checkInType}, 
                        object::pair{std:("timestamp"), update->timestamp}, 
                        object::pair{std:("answers"), std:("answers")}
                    };
                }
                catch (const any& error)
                {
                    logger->error(std:("Error parsing answers JSON:"), error);
                    return update;
                }
            }
            );
            auto prompt = std:("Analyze these team member updates and provide a detailed productivity report.\
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
            logger->info(std:("Generating productivity analysis for team member:"), teamMemberName);
            try
            {
                auto analysis = std::async([=]() { runtime->useModel(ModelType->TEXT_LARGE, object{
                    object::pair{std:("prompt"), std:("prompt")}, 
                    object::pair{std:("stopSequences"), array<any>()}
                }); });
                report += std:("📋 **Productivity Analysis**:\
") + analysis + std:("\
\
");
                report += std:("📅 **Recent Updates**:\
");
                auto recentUpdates = memberUpdates->slice(0, 3);
                for (auto& update : recentUpdates)
                {
                    report += std:("\
🕒 ") + ((std::make_shared<Date>(update->timestamp)))->toLocaleString() + std:("\
");
                    try
                    {
                        auto answers = (update->answers) ? any(JSON->parse(update->answers)) (object{});
                        for (auto& [question, answer] : Object->entries(answers))
                        {
                            report += std:("▫️ **") + question + std:("**: ") + answer + std:("\
");
                        }
                    }
                    catch (const any& error)
                    {
                        logger->error(std:("Error parsing answers JSON for display:"), error);
                        report += std:("▫️ Error parsing update details\
");
                    }
                }
            }
            catch (const any& error)
            {
                logger->error(std:("Error generating analysis:"), error);
                report += std:("❌ Error generating analysis. Showing raw updates:\
\
");
                for (auto& update : memberUpdates)
                {
                    report += std:("Update from ") + ((std::make_shared<Date>(update->timestamp)))->toLocaleString() + std:(":\
");
                    try
                    {
                        auto answers = (update->answers) ? any(JSON->parse(update->answers)) (object{});
                        for (auto& [question, answer] : Object->entries(answers))
                        {
                            report += std:("▫️ **") + question + std:("**: ") + answer + std:("\
");
                        }
                    }
                    catch (const any& error)
                    {
                        logger->error(std:("Error parsing answers JSON for display:"), error);
                        report += std:("▫️ Error parsing update details\
");
                    }
                }
            }
            report += std:("\
-------------------\
\
");
        }
        logger->info(std:("Successfully generated team report"));
        logger->info(std:("== GENERATE TEAM REPORT END =="));
        return report;
    }
    catch (const any& error)
    {
        logger->error(std:("Error generating team report:"), error);
        throw any(error);
    }
};


std::shared_ptr<Action> generateReport = object{
    object::pair{std:("name"), std:("GENERATE_REPORT")}, 
    object::pair{std:("description"), std:("Generates a comprehensive report of team member updates and productivity analysis")}, 
    object::pair{std:("similes"), array<string>{ std:("CREATE_REPORT"), std:("TEAM_REPORT"), std:("GET_TEAM_REPORT"), std:("SHOW_TEAM_REPORT"), std:("PRODUCE_TEAM_ANALYSIS") }}, 
    object::pair{std:("validate"), [=](auto runtime, auto message) mutable
    {
        logger->info(std:("Validating generateReport action"));
        return true;
    }
    }, 
    object::pair{std:("handler"), [=](auto runtime, auto message, auto state, auto options = object{}, auto callback = undefined) mutable
    {
        try
        {
            logger->info(std:("== GENERATE REPORT HANDLER START =="));
            if (!state) return false;
            if (!callback) {
                logger->warn(std:("No callback function provided"));
                return false;
            }
            auto text = as<string>(message->content->text);
            if (!text) {
                logger->warn(std:("No text content found in message"));
                return false;
            }
            shared<string> standupType;
            try
            {
                auto prompt = std:("Extract the standup type from this text. Try to understand the sentence and its context.\
        Return one of these values: STANDUP, SPRINT, MENTAL_HEALTH, PROJECT_STATUS, RETRO.\
        If you can't determine a specific type, use STANDUP as default.\
        \
        Text: "") + text + std:(""");
                auto parsedType = std::async([=]() { runtime->useModel(ModelType->TEXT_LARGE, object{
                    object::pair{std:("prompt"), std:("prompt")}, 
                    object::pair{std:("stopSequences"), array<any>()}
                }); });
                logger->info(std:("AI parsed standup type:"), parsedType);
                if (AND((!state["standupType"]), (!parsedType))) {
                    logger->info(std:("Asking for standup type"));
                    auto template = std:("Please select a check-in type:\
          - Daily Standup (STANDUP)\
          - Sprint Check-in (SPRINT) \
          - Mental Health Check-in (MENTAL_HEALTH)\
          - Project Status Update (PROJECT_STATUS)\
          - Team Retrospective (RETRO)");
                    auto promptContent = object{
                        object::pair{std:("text"), template}, 
                        object::pair{std:("source"), std:("discord")}
                    };
                    std::async([=]() { callback(promptContent, array<any>()); });
                    return true;
                }
                standupType = (OR(((as<string>(state["standupType"]))), (parsedType)))["toLowerCase"]()["trim"]();
                logger->info(std:("Generating report with parameters:"), object{
                    object::pair{std:("standupType"), std:("standupType")}, 
                    object::pair{std:("roomId"), message->roomId}
                });
                auto validTypes = array<string>{ std:("standup"), std:("sprint"), std:("mental_health"), std:("project_status"), std:("retro") };
                auto isValidType = validTypes->some([=](auto type) mutable
                {
                    return standupType == type;
                }
                );
                if (!isValidType) {
                    std::async([=]() { callback(object{
                        object::pair{std:("text"), std:("Invalid check-in type. Please select one of: Daily Standup, Sprint Check-in, Mental Health Check-in, Project Status Update, or Team Retrospective")}, 
                        object::pair{std:("source"), std:("discord")}
                    }, array<any>()); });
                    return false;
                }
            }
            catch (const any& aiError)
            {
                logger->error(std:("Error using AI to parse input:"), aiError);
                std::async([=]() { callback(object{
                    object::pair{std:("text"), std:("I couldn't understand the check-in type. Please try again with a valid type.")}, 
                    object::pair{std:("source"), std:("discord")}
                }, array<any>()); });
                return false;
            }
            auto report = std::async([=]() { generateTeamReport(runtime, standupType, message->roomId); });
            auto content = object{
                object::pair{std:("text"), report}, 
                object::pair{std:("source"), std:("discord")}
            };
            std::async([=]() { callback(content, array<any>()); });
            logger->info(std:("== GENERATE REPORT HANDLER END =="));
            return true;
        }
        catch (const any& error)
        {
            auto err = as<std::shared_ptr<Error>>(error);
            logger->error(std:("== GENERATE REPORT HANDLER ERROR =="));
            logger->error(std:("Error details:"), object{
                object::pair{std:("name"), OR((err->name), (std:("Unknown error")))}, 
                object::pair{std:("message"), OR((err->message), (std:("No error message")))}, 
                object::pair{std:("stack"), OR((err->stack), (std:("No stack trace")))}
            });
            if (callback) {
                auto errorContent = object{
                    object::pair{std:("text"), std:("❌ An error occurred while generating the report. Please try again.")}, 
                    object::pair{std:("source"), std:("discord")}
                };
                std::async([=]() { callback(errorContent, array<any>()); });
            }
            return false;
        }
    }
    }, 
    object::pair{std:("examples"), array<array<any>>{ array<object>{ object{
        object::pair{std:("name"), std:("{{name1}}")}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), std:("Generate a daily standup report")}
        }}
    }, object{
        object::pair{std:("name"), std:("{{botName}}")}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), std:("I'll generate a daily standup report for you")}, 
            object::pair{std:("actions"), array<string>{ std:("GENERATE_REPORT") }}
        }}
    } }, array<object>{ object{
        object::pair{std:("name"), std:("{{name1}}")}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), std:("Can I see the sprint progress report?")}
        }}
    }, object{
        object::pair{std:("name"), std:("{{botName}}")}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), std:("I'll create a sprint check-in report for you")}, 
            object::pair{std:("actions"), array<string>{ std:("GENERATE_REPORT") }}
        }}
    } }, array<object>{ object{
        object::pair{std:("name"), std:("{{name1}}")}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), std:("How is the team doing with the project?")}
        }}
    }, object{
        object::pair{std:("name"), std:("{{botName}}")}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), std:("I'll generate a project status report to show you how the team is progressing")}, 
            object::pair{std:("actions"), array<string>{ std:("GENERATE_REPORT") }}
        }}
    } } }}
};

void Main(void)
{
}

MAIN
