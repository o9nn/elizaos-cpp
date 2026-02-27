#include "/home/runner/work/elizaos-cpp/elizaos-cpp/the-org/src/projectManager/plugins/team-coordinator/actions/teamMemberUpdate.h"

std::shared_ptr<Promise<boolean>> postUpdateToDiscordChannel(std::shared_ptr<IAgentRuntime> runtime, std::shared_ptr<TeamMemberUpdate> update)
{
    try
    {
        logger->info(std:("== POST TEAM MEMBER UPDATE TO DISCORD START =="));
        auto discordService = as<any>(runtime->getService(std:("discord")));
        if (!discordService) {
            logger->error(std:("Discord service not available"));
            return false;
        }
        logger->info(std:("Discord service retrieved successfully"));
        auto roomId = createUniqueUuid(runtime, std:("report-channel-config"));
        logger->info(std:("Generated roomId for config:"), roomId);
        auto memories = std::async([=]() { runtime->getMemories(object{
            object::pair{std:("roomId"), roomId}, 
            object::pair{std:("tableName"), std:("messages")}
        }); });
        logger->info(std:("Retrieved report channel configs:"), object{
            object::pair{std:("count"), memories->length}, 
            object::pair{std:("configs"), memories->map([=](auto m) mutable
            {
                return (object{
                    object::pair{std:("type"), m["content"]->type}, 
                    object::pair{std:("channelId"), (m["content"]->config) ? any((as<std::shared_ptr<ReportChannelConfig>>(m["content"]->config))->channelId) (undefined)}
                });
            }
            )}
        });
        auto guilds = discordService->client["guilds"]["cache"];
        logger->info(std:("Found ") + guilds->size + std:(" Discord servers"));
        shared targetGuild = nullptr;
        for (auto& guild : guilds->values())
        {
            logger->info(std:("Checking guild: ") + guild["name"] + std:(" against update server name: ") + update->serverName + string_empty);
            if (guild["name"] == update->serverName) {
                targetGuild = guild;
                logger->info(std:("Found matching guild: ") + guild["name"] + std:(" with ID: ") + guild["id"] + string_empty);
                break;
            }
        }
        if (!targetGuild) {
            logger->warn(std:("Could not find Discord server matching exact name: ") + update->serverName + string_empty);
            return false;
        }
        shared config = memories->find([=](auto memory) mutable
        {
            auto serverMatch = (targetGuild) ? any(targetGuild["id"]) (undefined);
            auto configData = as<any>(memory["content"]["config"]);
            logger->info(std:("Checking config:"), object{
                object::pair{std:("configType"), memory["content"]["type"]}, 
                object::pair{std:("configServerId"), configData->serverId}, 
                object::pair{std:("targetGuildId"), (targetGuild) ? any(targetGuild["id"]) (undefined)}, 
                object::pair{std:("matches"), serverMatch}
            });
            return AND((memory["content"]["type"] == std:("report-channel-config")), (serverMatch));
        }
        );
        if (!config) {
            logger->warn(std:("No report channel config found for server: ") + targetGuild["name"] + std:(" (") + targetGuild["id"] + std:(")"));
            return false;
        }
        auto configData = as<std::shared_ptr<ReportChannelConfig>>(config->content->config);
        logger->info(std:("Found report channel config:"), object{
            object::pair{std:("configId"), config->id}, 
            object::pair{std:("configType"), config->content->type}, 
            object::pair{std:("configServerId"), targetGuild["id"]}, 
            object::pair{std:("configChannelId"), configData->channelId}
        });
        shared channelId = configData->channelId;
        if (!channelId) {
            logger->warn(std:("No channel ID in config"));
            return false;
        }
        auto formattedDate = ((std::make_shared<Date>(update->timestamp)))->toLocaleString();
        logger->info(std:("Formatting update message with timestamp:"), object{
            object::pair{std:("timestamp"), update->timestamp}, 
            object::pair{std:("formatted"), formattedDate}
        });
        auto updateMessage = std:("## Team Member Update\
**Team Member**: ") + (OR((update->teamMemberName), (std:("Unknown")))) + std:(" (") + update->teamMemberId + std:(")\
**Server-name**: ") + update->serverName + std:("\
**Check-in Type**: ") + update->checkInType + std:("\
**Timestamp**: ") + formattedDate + string_empty;
        try
        {
            auto answers = JSON->parse(OR((update->answers), (std:("{}"))));
            if (Object->keys(answers)->get_length() > 0) {
                updateMessage += std:("\
\
**Update Details**:");
                for (auto& [question, answer] : Object->entries(answers))
                {
                    updateMessage += std:("\
**") + question + std:("**: ") + answer + string_empty;
                }
            }
        }
        catch (const any& error)
        {
            logger->error(std:("Error parsing answers JSON:"), error);
            updateMessage += std:("\
\
**Update Details**: Error parsing update details");
        }
        logger->info(std:("Formatted update message:"), object{
            object::pair{std:("messageLength"), updateMessage->get_length()}
        });
        auto client = discordService->client;
        if (!client) {
            logger->error(std:("Discord client not available"));
            return false;
        }
        logger->info(std:("Searching for channel ") + channelId + std:(" in server: ") + targetGuild["name"] + string_empty);
        auto channels = std::async([=]() { targetGuild["channels"]["fetch"](); });
        auto targetChannel = channels["find"]([=](auto ch) mutable
        {
            return AND((AND((AND((ch), (type_of(ch) == std:("object")))), (in(std:("id"), ch)))), (ch["id"] == channelId));
        }
        );
        if (!targetChannel) {
            logger->warn(std:("Could not find Discord channel with ID ") + channelId + std:(" in server ") + targetGuild["name"] + string_empty);
            return false;
        }
        logger->info(std:("Attempting to send update to Discord channel"));
        std::async([=]() { (as<any>(targetChannel))->send(updateMessage); });
        logger->info(std:("Successfully sent team member update to Discord"));
        logger->info(std:("== POST TEAM MEMBER UPDATE TO DISCORD END =="));
        return true;
    }
    catch (const any& error)
    {
        auto err = as<std::shared_ptr<Error>>(error);
        logger->error(std:("== POST TEAM MEMBER UPDATE TO DISCORD ERROR =="));
        logger->error(std:("Error details:"), object{
            object::pair{std:("name"), OR((err->name), (std:("Unknown")))}, 
            object::pair{std:("message"), OR((err->message), (std:("No message")))}, 
            object::pair{std:("stack"), OR((err->stack), (std:("No stack trace")))}
        });
        return false;
    }
};


std::shared_ptr<Promise<boolean>> storeTeamMemberUpdate(std::shared_ptr<IAgentRuntime> runtime, std::shared_ptr<TeamMemberUpdate> update)
{
    try
    {
        logger->info(std:("== STORE TEAM MEMBER UPDATE START =="));
        logger->info(std:("Storing update for team member:"), object{
            object::pair{std:("teamMemberId"), update->teamMemberId}, 
            object::pair{std:("updateId"), update->updateId}, 
            object::pair{std:("timestamp"), update->timestamp}
        });
        auto roomId = update->channelId;
        logger->info(std:("Using existing room ID: ") + roomId + string_empty);
        if (!roomId) {
            logger->error(std:("No room ID available for storing the update"));
            return false;
        }
        logger->info(std:("Attempting to store update in memory..."));
        auto memory = object{
            object::pair{std:("id"), createUniqueUuid(runtime, std:("team-update-") + Date->now() + string_empty)}, 
            object::pair{std:("agentId"), runtime->agentId}, 
            object::pair{std:("roomId"), roomId}, 
            object::pair{std:("entityId"), asUUID(update->teamMemberId)}, 
            object::pair{std:("content"), object{
                object::pair{std:("type"), std:("team-member-update")}, 
                object::pair{std:("update"), std:("update")}
            }}, 
            object::pair{std:("metadata"), object{
                object::pair{std:("type"), MemoryType->CUSTOM}, 
                object::pair{std:("timestamp"), Date->now()}
            }}
        };
        std::async([=]() { runtime->createMemory(memory, std:("messages")); });
        logger->info(std:("Successfully stored team member update"));
        logger->info(std:("== STORE TEAM MEMBER UPDATE END =="));
        return true;
    }
    catch (const any& error)
    {
        auto err = as<std::shared_ptr<Error>>(error);
        logger->error(std:("== STORE TEAM MEMBER UPDATE ERROR =="));
        logger->error(std:("Error details:"), object{
            object::pair{std:("name"), OR((err->name), (std:("Unknown")))}, 
            object::pair{std:("message"), OR((err->message), (std:("No message")))}, 
            object::pair{std:("stack"), OR((err->stack), (std:("No stack trace")))}
        });
        return false;
    }
};


std::shared_ptr<Promise<any>> parseTeamMemberUpdate(std::shared_ptr<IAgentRuntime> runtime, std::shared_ptr<Memory> message)
{
    try
    {
        logger->info(std:("== PARSE TEAM MEMBER UPDATE START =="));
        logger->info(std:("Parsing update from message:"), object{
            object::pair{std:("messageId"), message->id}, 
            object::pair{std:("entityId"), message->entityId}
        });
        auto text = as<string>(message->content->text);
        if (!text) {
            logger->warn(std:("No text content found in message"));
            return nullptr;
        }
        auto prompt = std:("Extract information from this team member update. The update will likely end with "sending my updates".\
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
    Text to parse: "") + text + std:(""");
        logger->info(std:("Sending text to AI for parsing"));
        logger->info(std:("Prompt:"), prompt);
        auto parsedResponse = std::async([=]() { runtime->useModel(ModelType->TEXT_LARGE, object{
            object::pair{std:("prompt"), std:("prompt")}, 
            object::pair{std:("stopSequences"), array<any>()}
        }); });
        logger->info(std:("Raw AI response:"), parsedResponse);
        any parsedData;
        try
        {
            auto cleanedResponse = parsedResponse->replace((new RegExp(std:(""""json\n?|\n?""""))), string_empty)->trim();
            parsedData = JSON->parse(cleanedResponse);
            logger->info(std:("Successfully parsed fields from AI response:"), parsedData);
        }
        catch (const any& error)
        {
            logger->error(std:("Failed to parse AI response as JSON:"), error);
            logger->error(std:("Raw response that failed parsing:"), parsedResponse);
            throw any(std::make_shared<Error>(std:("PARSING_ERROR: AI response was not valid JSON")));
        }
        if (OR((!parsedData["serverName"]), (!parsedData["checkInType"]))) {
            logger->warn(std:("Missing required basic fields:"), object{
                object::pair{std:("hasServerName"), !!parsedData["serverName"]}, 
                object::pair{std:("hasCheckInType"), !!parsedData["checkInType"]}
            });
            throw any(std::make_shared<Error>(std:("MISSING_FIELDS:serverName,checkInType")));
        }
        if (OR((!parsedData["answers"]), (Object->keys(parsedData["answers"])->get_length() == 0))) {
            logger->warn(std:("No answers were parsed from the update"));
            throw any(std::make_shared<Error>(std:("MISSING_FIELDS:answers")));
        }
        auto entityById = std::async([=]() { runtime->getEntityById(message->entityId); });
        auto userName = OR((entityById->metadata->discord->userName), (entityById->metadata->telegram->name));
        auto update = object{
            object::pair{std:("type"), std:("team-member-update")}, 
            object::pair{std:("updateId"), createUniqueUuid(runtime, std:("team-update"))}, 
            object::pair{std:("teamMemberId"), OR((message->entityId), (asUUID(std:("unknown"))))}, 
            object::pair{std:("teamMemberName"), userName}, 
            object::pair{std:("serverName"), parsedData["serverName"]}, 
            object::pair{std:("checkInType"), parsedData["checkInType"]}, 
            object::pair{std:("timestamp"), ((std::make_shared<Date>()))->toISOString()}, 
            object::pair{std:("channelId"), message->roomId}, 
            object::pair{std:("answers"), JSON->stringify(parsedData["answers"])}
        };
        logger->info(std:("Successfully parsed team member update:"), update);
        logger->info(std:("== PARSE TEAM MEMBER UPDATE END =="));
        return update;
    }
    catch (const any& error)
    {
        auto err = as<std::shared_ptr<Error>>(error);
        logger->error(std:("== PARSE TEAM MEMBER UPDATE ERROR =="));
        logger->error(std:("Error details:"), object{
            object::pair{std:("name"), OR((err->name), (std:("Unknown")))}, 
            object::pair{std:("message"), OR((err->message), (std:("No message")))}, 
            object::pair{std:("stack"), OR((err->stack), (std:("No stack trace")))}
        });
        throw any(error);
    }
};


std::shared_ptr<Action> teamMemberUpdatesAction = object{
    object::pair{std:("name"), std:("TEAM_MEMBER_UPDATES")}, 
    object::pair{std:("description"), std:("Records individual status updates from team members including progress details, blockers, and estimated completion times")}, 
    object::pair{std:("similes"), array<string>{ std:("SEND_PERSONAL_UPDATE"), std:("SUBMIT_UPDATE"), std:("SHARE_PROGRESS"), std:("REPORT_STATUS"), std:("LOG_TEAM_UPDATE") }}, 
    object::pair{std:("validate"), [=](auto runtime, auto message) mutable
    {
        return true;
    }
    }, 
    object::pair{std:("handler"), [=](auto runtime, auto message, auto state, auto options = object{}, auto callback = undefined) mutable
    {
        try
        {
            logger->info(std:("== RECORD TEAM MEMBER UPDATES HANDLER START =="));
            logger->info(std:("Handler details:"), object{
                object::pair{std:("messageId"), message->id}, 
                object::pair{std:("entityId"), message->entityId}, 
                object::pair{std:("hasCallback"), !!callback}, 
                object::pair{std:("stateKeys"), (state) ? Object->keys(state) : array<any>()}, 
                object::pair{std:("optionsKeys"), Object->keys(options)}
            });
            logger->info(std:("Processing message:"), object{
                object::pair{std:("id"), message->id}, 
                object::pair{std:("content"), JSON->stringify(message->content)}, 
                object::pair{std:("entityId"), message->entityId}, 
                object::pair{std:("roomId"), message->roomId}, 
                object::pair{std:("timestamp"), message->metadata->timestamp}, 
                object::pair{std:("text"), message->content->text}, 
                object::pair{std:("type"), message->content->type}, 
                object::pair{std:("metadata"), JSON->stringify(message->metadata)}, 
                object::pair{std:("fullMessage"), JSON->stringify(message)}
            });
            if (!callback) {
                logger->warn(std:("No callback function provided"));
                return false;
            }
            try
            {
                auto update = std::async([=]() { parseTeamMemberUpdate(runtime, message); });
                if (!update) {
                    auto template = std:("Please provide your update using the following format:\
\
Server-name: [server name]\
Check-in Type: [daily/weekly/sprint]\
Current Progress: [what you've completed]\
Next Steps: [upcoming tasks]\
Blockers: [any blockers or "none"]\
\
End your message with "sending my updates"");
                    std::async([=]() { callback(object{
                        object::pair{std:("text"), std:("❌ I was unable to process your update. ") + template + string_empty}, 
                        object::pair{std:("source"), std:("discord")}
                    }, array<any>()); });
                    return false;
                }
                auto stored = std::async([=]() { storeTeamMemberUpdate(runtime, update); });
                if (!stored) {
                    logger->error(std:("Failed to store team member update"));
                    std::async([=]() { callback(object{
                        object::pair{std:("text"), std:("❌ There was an error storing your update. Please try again later.")}, 
                        object::pair{std:("source"), std:("discord")}
                    }, array<any>()); });
                    return false;
                }
                auto posted = std::async([=]() { postUpdateToDiscordChannel(runtime, update); });
                if (posted) {
                    logger->info(std:("Successfully posted team member update to Discord"));
                } else {
                    logger->warn(std:("Could not post update to Discord, but continuing with normal flow"));
                }
                auto content = object{
                    object::pair{std:("text"), std:("✅ Thanks for your update! I've recorded your progress details.") + (posted) ? std:(" Your update has been posted to the configured channel.") : string_empty + string_empty}, 
                    object::pair{std:("source"), std:("discord")}
                };
                logger->info(std:("Sending confirmation to user"));
                std::async([=]() { callback(content, array<any>()); });
                logger->info(std:("Successfully recorded team member update"));
                logger->info(std:("== RECORD TEAM MEMBER UPDATES HANDLER END =="));
                return true;
            }
            catch (const any& error)
            {
                auto err = as<std::shared_ptr<Error>>(error);
                if (AND((err->message), (err->message->startsWith(std:("MISSING_FIELDS:"))))) {
                    auto missingFields = const_(err->message->split(std:(":")))[1]->split(std:(","));
                    auto missingFieldsList = missingFields->join(std:(", "));
                    std::async([=]() { callback(object{
                        object::pair{std:("text"), std:("❌ Your update is missing the following required fields: ") + missingFieldsList + std:("\
\
Please include all required fields and try again:\
• Server-name\
• Check-in Type\
• Current Progress\
• Next Steps\
• Blockers\
\
Remember to end your message with "sending my updates"")}, 
                        object::pair{std:("source"), std:("discord")}
                    }, array<any>()); });
                    return false;
                }
                logger->error(std:("Unexpected error:"), error);
                std::async([=]() { callback(object{
                    object::pair{std:("text"), std:("❌ An error occurred while processing your update. Please try again.")}, 
                    object::pair{std:("source"), std:("discord")}
                }, array<any>()); });
                return false;
            }
        }
        catch (const any& error)
        {
            auto err = as<std::shared_ptr<Error>>(error);
            logger->error(std:("== RECORD TEAM MEMBER UPDATES HANDLER ERROR =="));
            logger->error(std:("Error details:"), object{
                object::pair{std:("name"), OR((err->name), (std:("Unknown")))}, 
                object::pair{std:("message"), OR((err->message), (std:("No message")))}, 
                object::pair{std:("stack"), OR((err->stack), (std:("No stack trace")))}
            });
            if (callback) {
                std::async([=]() { callback(object{
                    object::pair{std:("text"), std:("❌ An error occurred while processing your update. Please try again.")}, 
                    object::pair{std:("source"), std:("discord")}
                }, array<any>()); });
            }
            return false;
        }
    }
    }, 
    object::pair{std:("examples"), array<array<any>>{ array<object>{ object{
        object::pair{std:("name"), std:("{{name1}}")}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), std:("Server-name: Development Server\
Check-in Type: Daily\
What did you get done this week? Completed the API integration and fixed authentication bugs\
Main Priority for next week: Deploy to staging environment and start beta testing\
Blockers: None at the moment\
Team morale: High, everyone is collaborating well\
\
sending my updates")}
        }}
    }, object{
        object::pair{std:("name"), std:("{{botName}}")}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), std:("✅ Thank you for your update! I've recorded your progress details and will share them with the team.")}, 
            object::pair{std:("actions"), array<string>{ std:("TEAM_MEMBER_UPDATES") }}
        }}
    } }, array<object>{ object{
        object::pair{std:("name"), std:("{{name1}}")}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), std:("Server-name: Project X Server\
Check-in Type: SPRINT\
Progress: Fixed 3 critical bugs in the frontend\
Next task: Code review and documentation\
Roadblocks: Waiting on design assets from the design team\
Estimated completion: End of next week\
\
sending my updates")}
        }}
    }, object{
        object::pair{std:("name"), std:("{{botName}}")}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), std:("✅ Your status has been logged successfully. I will make sure the team is aware of your progress and blockers.")}, 
            object::pair{std:("actions"), array<string>{ std:("TEAM_MEMBER_UPDATES") }}
        }}
    } }, array<object>{ object{
        object::pair{std:("name"), std:("{{name1}}")}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), std:("Server-name: Engineering Team\
Check-in Type: STANDUP\
Weekly achievements: Implemented user authentication API and completed unit tests\
Focus areas: Begin frontend integration\
Technical challenges: Dependency issues with auth library\
Questions for team: Do we need to support SSO for the initial release?\
\
sending my updates")}
        }}
    }, object{
        object::pair{std:("name"), std:("{{botName}}")}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), std:("✅ Status recorded! Your individual work update has been saved and will be included in the next team report.")}, 
            object::pair{std:("actions"), array<string>{ std:("TEAM_MEMBER_UPDATES") }}
        }}
    } }, array<object>{ object{
        object::pair{std:("name"), std:("{{name1}}")}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), std:("Server-name: Product Development\
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
        object::pair{std:("name"), std:("{{botName}}")}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), std:("✅ Your individual status has been logged. I've noted your blockers regarding resource allocation approval.")}, 
            object::pair{std:("actions"), array<string>{ std:("TEAM_MEMBER_UPDATES") }}
        }}
    } }, array<object>{ object{
        object::pair{std:("name"), std:("{{name1}}")}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), std:("Server-name: Marketing Team\
Check-in Type: WEEKLY\
This week: Completed social media campaign designs\
Next week: Start A/B testing different ad copy\
Customer feedback: Very positive on new landing page\
Conversion rate: Up 15% from last week\
\
sending my updates")}
        }}
    }, object{
        object::pair{std:("name"), std:("{{botName}}")}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), std:("✅ Thanks for your detailed update! I've recorded all the information you provided.")}, 
            object::pair{std:("actions"), array<string>{ std:("TEAM_MEMBER_UPDATES") }}
        }}
    } } }}
};

void Main(void)
{
}

MAIN
