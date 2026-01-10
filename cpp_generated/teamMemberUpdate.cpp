#include "/home/runner/work/elizaos-cpp/elizaos-cpp/the-org/src/projectManager/plugins/team-coordinator/actions/teamMemberUpdate.h"

std::shared_ptr<Promise<boolean>> postUpdateToDiscordChannel(std::shared_ptr<IAgentRuntime> runtime, std::shared_ptr<TeamMemberUpdate> update)
{
    try
    {
        logger->info(std::string("=== POST TEAM MEMBER UPDATE TO DISCORD START ==="));
        auto discordService = as<any>(runtime->getService(std::string("discord")));
        if (!discordService) {
            logger->error(std::string("Discord service not available"));
            return false;
        }
        logger->info(std::string("Discord service retrieved successfully"));
        auto roomId = createUniqueUuid(runtime, std::string("report-channel-config"));
        logger->info(std::string("Generated roomId for config:"), roomId);
        auto memories = std::async([=]() { runtime->getMemories(object{
            object::pair{std::string("roomId"), roomId}, 
            object::pair{std::string("tableName"), std::string("messages")}
        }); });
        logger->info(std::string("Retrieved report channel configs:"), object{
            object::pair{std::string("count"), memories->length}, 
            object::pair{std::string("configs"), memories->map([=](auto m) mutable
            {
                return (object{
                    object::pair{std::string("type"), m["content"]->type}, 
                    object::pair{std::string("channelId"), (m["content"]->config) ? any((as<std::shared_ptr<ReportChannelConfig>>(m["content"]->config))->channelId) : any(undefined)}
                });
            }
            )}
        });
        auto guilds = discordService->client["guilds"]["cache"];
        logger->info(std::string("Found ") + guilds->size + std::string(" Discord servers"));
        shared targetGuild = nullptr;
        for (auto& guild : guilds->values())
        {
            logger->info(std::string("Checking guild: ") + guild["name"] + std::string(" against update server name: ") + update->serverName + string_empty);
            if (guild["name"] == update->serverName) {
                targetGuild = guild;
                logger->info(std::string("Found matching guild: ") + guild["name"] + std::string(" with ID: ") + guild["id"] + string_empty);
                break;
            }
        }
        if (!targetGuild) {
            logger->warn(std::string("Could not find Discord server matching exact name: ") + update->serverName + string_empty);
            return false;
        }
        shared config = memories->find([=](auto memory) mutable
        {
            auto serverMatch = (targetGuild) ? any(targetGuild["id"]) : any(undefined);
            auto configData = as<any>(memory["content"]["config"]);
            logger->info(std::string("Checking config:"), object{
                object::pair{std::string("configType"), memory["content"]["type"]}, 
                object::pair{std::string("configServerId"), configData->serverId}, 
                object::pair{std::string("targetGuildId"), (targetGuild) ? any(targetGuild["id"]) : any(undefined)}, 
                object::pair{std::string("matches"), serverMatch}
            });
            return AND((memory["content"]["type"] == std::string("report-channel-config")), (serverMatch));
        }
        );
        if (!config) {
            logger->warn(std::string("No report channel config found for server: ") + targetGuild["name"] + std::string(" (") + targetGuild["id"] + std::string(")"));
            return false;
        }
        auto configData = as<std::shared_ptr<ReportChannelConfig>>(config->content->config);
        logger->info(std::string("Found report channel config:"), object{
            object::pair{std::string("configId"), config->id}, 
            object::pair{std::string("configType"), config->content->type}, 
            object::pair{std::string("configServerId"), targetGuild["id"]}, 
            object::pair{std::string("configChannelId"), configData->channelId}
        });
        shared channelId = configData->channelId;
        if (!channelId) {
            logger->warn(std::string("No channel ID in config"));
            return false;
        }
        auto formattedDate = ((std::make_shared<Date>(update->timestamp)))->toLocaleString();
        logger->info(std::string("Formatting update message with timestamp:"), object{
            object::pair{std::string("timestamp"), update->timestamp}, 
            object::pair{std::string("formatted"), formattedDate}
        });
        auto updateMessage = std::string("## Team Member Update\
**Team Member**: ") + (OR((update->teamMemberName), (std::string("Unknown")))) + std::string(" (") + update->teamMemberId + std::string(")\
**Server-name**: ") + update->serverName + std::string("\
**Check-in Type**: ") + update->checkInType + std::string("\
**Timestamp**: ") + formattedDate + string_empty;
        try
        {
            auto answers = JSON->parse(OR((update->answers), (std::string("{}"))));
            if (Object->keys(answers)->get_length() > 0) {
                updateMessage += std::string("\
\
**Update Details**:");
                for (auto& [question, answer] : Object->entries(answers))
                {
                    updateMessage += std::string("\
**") + question + std::string("**: ") + answer + string_empty;
                }
            }
        }
        catch (const any& error)
        {
            logger->error(std::string("Error parsing answers JSON:"), error);
            updateMessage += std::string("\
\
**Update Details**: Error parsing update details");
        }
        logger->info(std::string("Formatted update message:"), object{
            object::pair{std::string("messageLength"), updateMessage->get_length()}
        });
        auto client = discordService->client;
        if (!client) {
            logger->error(std::string("Discord client not available"));
            return false;
        }
        logger->info(std::string("Searching for channel ") + channelId + std::string(" in server: ") + targetGuild["name"] + string_empty);
        auto channels = std::async([=]() { targetGuild["channels"]["fetch"](); });
        auto targetChannel = channels["find"]([=](auto ch) mutable
        {
            return AND((AND((AND((ch), (type_of(ch) == std::string("object")))), (in(std::string("id"), ch)))), (ch["id"] == channelId));
        }
        );
        if (!targetChannel) {
            logger->warn(std::string("Could not find Discord channel with ID ") + channelId + std::string(" in server ") + targetGuild["name"] + string_empty);
            return false;
        }
        logger->info(std::string("Attempting to send update to Discord channel"));
        std::async([=]() { (as<any>(targetChannel))->send(updateMessage); });
        logger->info(std::string("Successfully sent team member update to Discord"));
        logger->info(std::string("=== POST TEAM MEMBER UPDATE TO DISCORD END ==="));
        return true;
    }
    catch (const any& error)
    {
        auto err = as<std::shared_ptr<Error>>(error);
        logger->error(std::string("=== POST TEAM MEMBER UPDATE TO DISCORD ERROR ==="));
        logger->error(std::string("Error details:"), object{
            object::pair{std::string("name"), OR((err->name), (std::string("Unknown")))}, 
            object::pair{std::string("message"), OR((err->message), (std::string("No message")))}, 
            object::pair{std::string("stack"), OR((err->stack), (std::string("No stack trace")))}
        });
        return false;
    }
};


std::shared_ptr<Promise<boolean>> storeTeamMemberUpdate(std::shared_ptr<IAgentRuntime> runtime, std::shared_ptr<TeamMemberUpdate> update)
{
    try
    {
        logger->info(std::string("=== STORE TEAM MEMBER UPDATE START ==="));
        logger->info(std::string("Storing update for team member:"), object{
            object::pair{std::string("teamMemberId"), update->teamMemberId}, 
            object::pair{std::string("updateId"), update->updateId}, 
            object::pair{std::string("timestamp"), update->timestamp}
        });
        auto roomId = update->channelId;
        logger->info(std::string("Using existing room ID: ") + roomId + string_empty);
        if (!roomId) {
            logger->error(std::string("No room ID available for storing the update"));
            return false;
        }
        logger->info(std::string("Attempting to store update in memory..."));
        auto memory = object{
            object::pair{std::string("id"), createUniqueUuid(runtime, std::string("team-update-") + Date->now() + string_empty)}, 
            object::pair{std::string("agentId"), runtime->agentId}, 
            object::pair{std::string("roomId"), roomId}, 
            object::pair{std::string("entityId"), asUUID(update->teamMemberId)}, 
            object::pair{std::string("content"), object{
                object::pair{std::string("type"), std::string("team-member-update")}, 
                object::pair{std::string("update"), std::string("update")}
            }}, 
            object::pair{std::string("metadata"), object{
                object::pair{std::string("type"), MemoryType->CUSTOM}, 
                object::pair{std::string("timestamp"), Date->now()}
            }}
        };
        std::async([=]() { runtime->createMemory(memory, std::string("messages")); });
        logger->info(std::string("Successfully stored team member update"));
        logger->info(std::string("=== STORE TEAM MEMBER UPDATE END ==="));
        return true;
    }
    catch (const any& error)
    {
        auto err = as<std::shared_ptr<Error>>(error);
        logger->error(std::string("=== STORE TEAM MEMBER UPDATE ERROR ==="));
        logger->error(std::string("Error details:"), object{
            object::pair{std::string("name"), OR((err->name), (std::string("Unknown")))}, 
            object::pair{std::string("message"), OR((err->message), (std::string("No message")))}, 
            object::pair{std::string("stack"), OR((err->stack), (std::string("No stack trace")))}
        });
        return false;
    }
};


std::shared_ptr<Promise<any>> parseTeamMemberUpdate(std::shared_ptr<IAgentRuntime> runtime, std::shared_ptr<Memory> message)
{
    try
    {
        logger->info(std::string("=== PARSE TEAM MEMBER UPDATE START ==="));
        logger->info(std::string("Parsing update from message:"), object{
            object::pair{std::string("messageId"), message->id}, 
            object::pair{std::string("entityId"), message->entityId}
        });
        auto text = as<string>(message->content->text);
        if (!text) {
            logger->warn(std::string("No text content found in message"));
            return nullptr;
        }
        auto prompt = std::string("Extract information from this team member update. The update will likely end with "sending my updates".\
\
    Parse the text and return a JSON object with these fields:\
    {\
      "serverName": "value", // Name of the server\
      "checkInType": "value", // Type of check-in (could be STANDUP, SPRINT, MENTAL_HEALTH, PROJECT_STATUS, RETRO, or something else)\
      "answers": { // Dynamic field with key-value pairs for all questions and answers found\
        "questionText1": "answerText1",\
        "questionText2": "answerText2"\
        // Any other key-value pairs detected in the format\
      }\
    }\
\
    For the "answers" field, extract any key-value pairs that look like questions and answers in the text.\
    Include ALL information from the update in the answers object.\
\
    Text to parse: "") + text + std::string(""");
        logger->info(std::string("Sending text to AI for parsing"));
        logger->info(std::string("Prompt:"), prompt);
        auto parsedResponse = std::async([=]() { runtime->useModel(ModelType->TEXT_LARGE, object{
            object::pair{std::string("prompt"), std::string("prompt")}, 
            object::pair{std::string("stopSequences"), array<any>()}
        }); });
        logger->info(std::string("Raw AI response:"), parsedResponse);
        any parsedData;
        try
        {
            auto cleanedResponse = parsedResponse->replace((new RegExp(std::string("```json\n?|\n?```"))), string_empty)->trim();
            parsedData = JSON->parse(cleanedResponse);
            logger->info(std::string("Successfully parsed fields from AI response:"), parsedData);
        }
        catch (const any& error)
        {
            logger->error(std::string("Failed to parse AI response as JSON:"), error);
            logger->error(std::string("Raw response that failed parsing:"), parsedResponse);
            throw any(std::make_shared<Error>(std::string("PARSING_ERROR: AI response was not valid JSON")));
        }
        if (OR((!parsedData["serverName"]), (!parsedData["checkInType"]))) {
            logger->warn(std::string("Missing required basic fields:"), object{
                object::pair{std::string("hasServerName"), !!parsedData["serverName"]}, 
                object::pair{std::string("hasCheckInType"), !!parsedData["checkInType"]}
            });
            throw any(std::make_shared<Error>(std::string("MISSING_FIELDS:serverName,checkInType")));
        }
        if (OR((!parsedData["answers"]), (Object->keys(parsedData["answers"])->get_length() == 0))) {
            logger->warn(std::string("No answers were parsed from the update"));
            throw any(std::make_shared<Error>(std::string("MISSING_FIELDS:answers")));
        }
        auto entityById = std::async([=]() { runtime->getEntityById(message->entityId); });
        auto userName = OR((entityById->metadata->discord->userName), (entityById->metadata->telegram->name));
        auto update = object{
            object::pair{std::string("type"), std::string("team-member-update")}, 
            object::pair{std::string("updateId"), createUniqueUuid(runtime, std::string("team-update"))}, 
            object::pair{std::string("teamMemberId"), OR((message->entityId), (asUUID(std::string("unknown"))))}, 
            object::pair{std::string("teamMemberName"), userName}, 
            object::pair{std::string("serverName"), parsedData["serverName"]}, 
            object::pair{std::string("checkInType"), parsedData["checkInType"]}, 
            object::pair{std::string("timestamp"), ((std::make_shared<Date>()))->toISOString()}, 
            object::pair{std::string("channelId"), message->roomId}, 
            object::pair{std::string("answers"), JSON->stringify(parsedData["answers"])}
        };
        logger->info(std::string("Successfully parsed team member update:"), update);
        logger->info(std::string("=== PARSE TEAM MEMBER UPDATE END ==="));
        return update;
    }
    catch (const any& error)
    {
        auto err = as<std::shared_ptr<Error>>(error);
        logger->error(std::string("=== PARSE TEAM MEMBER UPDATE ERROR ==="));
        logger->error(std::string("Error details:"), object{
            object::pair{std::string("name"), OR((err->name), (std::string("Unknown")))}, 
            object::pair{std::string("message"), OR((err->message), (std::string("No message")))}, 
            object::pair{std::string("stack"), OR((err->stack), (std::string("No stack trace")))}
        });
        throw any(error);
    }
};


std::shared_ptr<Action> teamMemberUpdatesAction = object{
    object::pair{std::string("name"), std::string("TEAM_MEMBER_UPDATES")}, 
    object::pair{std::string("description"), std::string("Records individual status updates from team members including progress details, blockers, and estimated completion times")}, 
    object::pair{std::string("similes"), array<string>{ std::string("SEND_PERSONAL_UPDATE"), std::string("SUBMIT_UPDATE"), std::string("SHARE_PROGRESS"), std::string("REPORT_STATUS"), std::string("LOG_TEAM_UPDATE") }}, 
    object::pair{std::string("validate"), [=](auto runtime, auto message) mutable
    {
        return true;
    }
    }, 
    object::pair{std::string("handler"), [=](auto runtime, auto message, auto state, auto options = object{}, auto callback = undefined) mutable
    {
        try
        {
            logger->info(std::string("=== RECORD TEAM MEMBER UPDATES HANDLER START ==="));
            logger->info(std::string("Handler details:"), object{
                object::pair{std::string("messageId"), message->id}, 
                object::pair{std::string("entityId"), message->entityId}, 
                object::pair{std::string("hasCallback"), !!callback}, 
                object::pair{std::string("stateKeys"), (state) ? Object->keys(state) : array<any>()}, 
                object::pair{std::string("optionsKeys"), Object->keys(options)}
            });
            logger->info(std::string("Processing message:"), object{
                object::pair{std::string("id"), message->id}, 
                object::pair{std::string("content"), JSON->stringify(message->content)}, 
                object::pair{std::string("entityId"), message->entityId}, 
                object::pair{std::string("roomId"), message->roomId}, 
                object::pair{std::string("timestamp"), message->metadata->timestamp}, 
                object::pair{std::string("text"), message->content->text}, 
                object::pair{std::string("type"), message->content->type}, 
                object::pair{std::string("metadata"), JSON->stringify(message->metadata)}, 
                object::pair{std::string("fullMessage"), JSON->stringify(message)}
            });
            if (!callback) {
                logger->warn(std::string("No callback function provided"));
                return false;
            }
            try
            {
                auto update = std::async([=]() { parseTeamMemberUpdate(runtime, message); });
                if (!update) {
                    auto template = std::string("Please provide your update using the following format:\
\
Server-name: [server name]\
Check-in Type: [daily/weekly/sprint]\
Current Progress: [what you've completed]\
Next Steps: [upcoming tasks]\
Blockers: [any blockers or "none"]\
\
End your message with "sending my updates"");
                    std::async([=]() { callback(object{
                        object::pair{std::string("text"), std::string("❌ I was unable to process your update. ") + template + string_empty}, 
                        object::pair{std::string("source"), std::string("discord")}
                    }, array<any>()); });
                    return false;
                }
                auto stored = std::async([=]() { storeTeamMemberUpdate(runtime, update); });
                if (!stored) {
                    logger->error(std::string("Failed to store team member update"));
                    std::async([=]() { callback(object{
                        object::pair{std::string("text"), std::string("❌ There was an error storing your update. Please try again later.")}, 
                        object::pair{std::string("source"), std::string("discord")}
                    }, array<any>()); });
                    return false;
                }
                auto posted = std::async([=]() { postUpdateToDiscordChannel(runtime, update); });
                if (posted) {
                    logger->info(std::string("Successfully posted team member update to Discord"));
                } else {
                    logger->warn(std::string("Could not post update to Discord, but continuing with normal flow"));
                }
                auto content = object{
                    object::pair{std::string("text"), std::string("✅ Thanks for your update! I've recorded your progress details.") + (posted) ? std::string(" Your update has been posted to the configured channel.") : string_empty + string_empty}, 
                    object::pair{std::string("source"), std::string("discord")}
                };
                logger->info(std::string("Sending confirmation to user"));
                std::async([=]() { callback(content, array<any>()); });
                logger->info(std::string("Successfully recorded team member update"));
                logger->info(std::string("=== RECORD TEAM MEMBER UPDATES HANDLER END ==="));
                return true;
            }
            catch (const any& error)
            {
                auto err = as<std::shared_ptr<Error>>(error);
                if (AND((err->message), (err->message->startsWith(std::string("MISSING_FIELDS:"))))) {
                    auto missingFields = const_(err->message->split(std::string(":")))[1]->split(std::string(","));
                    auto missingFieldsList = missingFields->join(std::string(", "));
                    std::async([=]() { callback(object{
                        object::pair{std::string("text"), std::string("❌ Your update is missing the following required fields: ") + missingFieldsList + std::string("\
\
Please include all required fields and try again:\
• Server-name\
• Check-in Type\
• Current Progress\
• Next Steps\
• Blockers\
\
Remember to end your message with "sending my updates"")}, 
                        object::pair{std::string("source"), std::string("discord")}
                    }, array<any>()); });
                    return false;
                }
                logger->error(std::string("Unexpected error:"), error);
                std::async([=]() { callback(object{
                    object::pair{std::string("text"), std::string("❌ An error occurred while processing your update. Please try again.")}, 
                    object::pair{std::string("source"), std::string("discord")}
                }, array<any>()); });
                return false;
            }
        }
        catch (const any& error)
        {
            auto err = as<std::shared_ptr<Error>>(error);
            logger->error(std::string("=== RECORD TEAM MEMBER UPDATES HANDLER ERROR ==="));
            logger->error(std::string("Error details:"), object{
                object::pair{std::string("name"), OR((err->name), (std::string("Unknown")))}, 
                object::pair{std::string("message"), OR((err->message), (std::string("No message")))}, 
                object::pair{std::string("stack"), OR((err->stack), (std::string("No stack trace")))}
            });
            if (callback) {
                std::async([=]() { callback(object{
                    object::pair{std::string("text"), std::string("❌ An error occurred while processing your update. Please try again.")}, 
                    object::pair{std::string("source"), std::string("discord")}
                }, array<any>()); });
            }
            return false;
        }
    }
    }, 
    object::pair{std::string("examples"), array<array<any>>{ array<object>{ object{
        object::pair{std::string("name"), std::string("{{name1}}")}, 
        object::pair{std::string("content"), object{
            object::pair{std::string("text"), std::string("Server-name: Development Server\
Check-in Type: Daily\
What did you get done this week? Completed the API integration and fixed authentication bugs\
Main Priority for next week: Deploy to staging environment and start beta testing\
Blockers: None at the moment\
Team morale: High, everyone is collaborating well\
\
sending my updates")}
        }}
    }, object{
        object::pair{std::string("name"), std::string("{{botName}}")}, 
        object::pair{std::string("content"), object{
            object::pair{std::string("text"), std::string("✅ Thank you for your update! I've recorded your progress details and will share them with the team.")}, 
            object::pair{std::string("actions"), array<string>{ std::string("TEAM_MEMBER_UPDATES") }}
        }}
    } }, array<object>{ object{
        object::pair{std::string("name"), std::string("{{name1}}")}, 
        object::pair{std::string("content"), object{
            object::pair{std::string("text"), std::string("Server-name: Project X Server\
Check-in Type: SPRINT\
Progress: Fixed 3 critical bugs in the frontend\
Next task: Code review and documentation\
Roadblocks: Waiting on design assets from the design team\
Estimated completion: End of next week\
\
sending my updates")}
        }}
    }, object{
        object::pair{std::string("name"), std::string("{{botName}}")}, 
        object::pair{std::string("content"), object{
            object::pair{std::string("text"), std::string("✅ Your status has been logged successfully. I will make sure the team is aware of your progress and blockers.")}, 
            object::pair{std::string("actions"), array<string>{ std::string("TEAM_MEMBER_UPDATES") }}
        }}
    } }, array<object>{ object{
        object::pair{std::string("name"), std::string("{{name1}}")}, 
        object::pair{std::string("content"), object{
            object::pair{std::string("text"), std::string("Server-name: Engineering Team\
Check-in Type: STANDUP\
Weekly achievements: Implemented user authentication API and completed unit tests\
Focus areas: Begin frontend integration\
Technical challenges: Dependency issues with auth library\
Questions for team: Do we need to support SSO for the initial release?\
\
sending my updates")}
        }}
    }, object{
        object::pair{std::string("name"), std::string("{{botName}}")}, 
        object::pair{std::string("content"), object{
            object::pair{std::string("text"), std::string("✅ Status recorded! Your individual work update has been saved and will be included in the next team report.")}, 
            object::pair{std::string("actions"), array<string>{ std::string("TEAM_MEMBER_UPDATES") }}
        }}
    } }, array<object>{ object{
        object::pair{std::string("name"), std::string("{{name1}}")}, 
        object::pair{std::string("content"), object{
            object::pair{std::string("text"), std::string("Server-name: Product Development\
Check-in Type: PROJECT_STATUS\
Current state: Completed feature specification documents\
Next milestone: Schedule technical planning session\
Anticipated Launch Date: Q3 2023\
Blockers: Resource allocation pending approval\
Budget status: Within projections\
\
sending my updates")}
        }}
    }, object{
        object::pair{std::string("name"), std::string("{{botName}}")}, 
        object::pair{std::string("content"), object{
            object::pair{std::string("text"), std::string("✅ Your individual status has been logged. I've noted your blockers regarding resource allocation approval.")}, 
            object::pair{std::string("actions"), array<string>{ std::string("TEAM_MEMBER_UPDATES") }}
        }}
    } }, array<object>{ object{
        object::pair{std::string("name"), std::string("{{name1}}")}, 
        object::pair{std::string("content"), object{
            object::pair{std::string("text"), std::string("Server-name: Marketing Team\
Check-in Type: WEEKLY\
This week: Completed social media campaign designs\
Next week: Start A/B testing different ad copy\
Customer feedback: Very positive on new landing page\
Conversion rate: Up 15% from last week\
\
sending my updates")}
        }}
    }, object{
        object::pair{std::string("name"), std::string("{{botName}}")}, 
        object::pair{std::string("content"), object{
            object::pair{std::string("text"), std::string("✅ Thanks for your detailed update! I've recorded all the information you provided.")}, 
            object::pair{std::string("actions"), array<string>{ std::string("TEAM_MEMBER_UPDATES") }}
        }}
    } } }}
};

void Main(void)
{
}

MAIN
