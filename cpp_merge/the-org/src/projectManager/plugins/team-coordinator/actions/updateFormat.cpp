#include "/home/runner/work/elizaos-cpp/elizaos-cpp/the-org/src/projectManager/plugins/team-coordinator/actions/updateFormat.h"

std::shared_ptr<UUID> getStorageRoomId(std::shared_ptr<IAgentRuntime> runtime, string serverId)
{
    auto serverHash = serverId->replace((new RegExp(std:("[^a-zA-Z0-9]"))), string_empty);
    return createUniqueUuid(runtime, std:("store-team-members-") + serverHash + string_empty);
};


std::shared_ptr<Action> updatesFormatAction = object{
    object::pair{std:("name"), std:("UPDATES_FORMAT")}, 
    object::pair{std:("description"), std:("Show the updates format for a specific team member.")}, 
    object::pair{std:("similes"), array<string>{ std:("UPDATES_FORMAT"), std:("SHOW_FORMAT"), std:("GET_FORMAT"), std:("MY_FORMAT"), std:("VIEW_FORMAT") }}, 
    object::pair{std:("validate"), [=](auto runtime, auto message, auto state) mutable
    {
        try
        {
            if (!state) return false;
            state["data"]["userId"] = message->entityId;
            logger->info(std:("Valid request to get updates format for user ") + message->entityId + string_empty);
            return true;
        }
        catch (const any& error)
        {
            auto err = as<std::shared_ptr<Error>>(error);
            logger->error(std:("Error in updatesFormatAction validation:"), err);
            logger->error(std:("Error stack: ") + (OR((err->stack), (std:("No stack trace available")))) + string_empty);
            return false;
        }
    }
    }, 
    object::pair{std:("handler"), [=](auto runtime, auto message, auto state, auto options = object{}, auto callback = undefined) mutable
    {
        try
        {
            logger->info(std:("== UPDATES-FORMAT HANDLER START =="));
            if (!state) return false;
            if (!callback) {
                logger->warn(std:("No callback function provided"));
                return false;
            }
            auto entityById = std::async([=]() { runtime->getEntityById(message->entityId); });
            shared username = OR((entityById->metadata->discord->userName), (entityById->metadata->telegram->userName));
            logger->info(std:("Entity metadata - Telegram:"), JSON->stringify(OR((entityById->metadata->telegram), (std:("Not available")))));
            logger->info(std:("Entity metadata - Discord:"), JSON->stringify(OR((entityById->metadata->discord), (std:("Not available")))));
            if (!username) {
                logger->error(std:("No username found in state"));
                std::async([=]() { callback(object{
                    object::pair{std:("text"), std:("❌ Failed to identify your username. Please try again.")}
                }, array<any>()); });
                return false;
            }
            logger->info(std:("Looking for updates format for user: ") + username + string_empty);
            auto allMemories = std::async([=]() { runtime->getMemories(object{
                object::pair{std:("tableName"), std:("messages")}, 
                object::pair{std:("agentId"), runtime->agentId}
            }); });
            logger->info(std:("Found ") + allMemories->length + std:(" total memories"));
            auto teamMemberConfigs = allMemories->filter([=](auto memory) mutable
            {
                return memory["content"]->type == std:("store-team-members-memory");
            }
            );
            logger->info(std:("Found ") + teamMemberConfigs->length + std:(" team member config memories"));
            shared allTeamMembers = array<std::shared_ptr<TeamMember>>();
            teamMemberConfigs->forEach([=](auto config) mutable
            {
                if (config["content"]["config"]) {
                    auto configData = as<object>(config["content"]["config"]);
                    auto teamMembers = OR((configData["teamMembers"]), (array<any>()));
                    logger->info(std:("Found ") + teamMembers->get_length() + std:(" team members in config"));
                    allTeamMembers = array<std::shared_ptr<TeamMember>>{ allTeamMembers, teamMembers };
                }
            }
            );
            logger->info(std:("Total of ") + allTeamMembers->get_length() + std:(" team members found across all servers"));
            logger->info(std:("Looking for username: ") + username + std:(" among team members:"), allTeamMembers->map([=](auto member) mutable
            {
                return (object{
                    object::pair{std:("tgName"), member->tgName}, 
                    object::pair{std:("discordName"), member->discordName}
                });
            }
            ));
            auto teamMember = allTeamMembers->find([=](auto member) mutable
            {
                return OR(((AND((member->tgName), ((OR((member->tgName == username), (member->tgName->replace(std:("@"), string_empty) == username["replace"](std:("@"), string_empty)))))))), ((AND((member->discordName), (member->discordName->replace(std:("@"), string_empty) == username["replace"](std:("@"), string_empty))))));
            }
            );
            if (!teamMember) {
                logger->info(std:("No team member found with username ") + username + string_empty);
                std::async([=]() { callback(object{
                    object::pair{std:("text"), std:("❌ No team member found with username ") + username + std:(". Please make sure you are registered as a team member.")}
                }, array<any>()); });
                return true;
            }
            logger->info(std:("Found team member: ") + JSON->stringify(teamMember) + string_empty);
            shared responseText = std:("📋 **Updates Format for ") + username + std:("**\
\
");
            responseText += std:("Section: ") + (OR((teamMember->section), (std:("Unassigned")))) + std:("\
");
            if (AND((teamMember->updatesFormat), (teamMember->updatesFormat->get_length() > 0))) {
                responseText += std:("\
Your updates should include the following fields:\
");
                teamMember->updatesFormat->forEach([=](auto field, auto index) mutable
                {
                    responseText += string_empty + (index + 1) + std:(". ") + field + std:("\
");
                }
                );
                responseText += std:("\
Important: End your message with "sending my personal updates" so it can be properly tracked.");
            } else {
                responseText += std:("\
You don't have any specific update format fields defined. Please use the standard format for your updates:\
\
") + std:("- **Main Priority for next week**\
") + std:("    - Text\
") + std:("- **What did you get done this week?**\
") + std:("    - Text\
") + std:("- **Blockers**\
") + std:("    - Text\
\
") + std:("Important: End your message with "sending my personal updates" so it can be properly tracked.");
            }
            std::async([=]() { callback(object{
                object::pair{std:("text"), responseText}
            }, array<any>()); });
            logger->info(std:("== UPDATES-FORMAT HANDLER END =="));
            return true;
        }
        catch (const any& error)
        {
            auto err = as<std::shared_ptr<Error>>(error);
            logger->error(std:("== UPDATES-FORMAT HANDLER ERROR =="));
            logger->error(std:("Error processing updates format request: ") + err + string_empty);
            logger->error(std:("Error stack: ") + (OR((err->stack), (std:("No stack trace available")))) + string_empty);
            if (callback) {
                std::async([=]() { callback(object{
                    object::pair{std:("text"), std:("❌ An unexpected error occurred while fetching your updates format. Please try again later.")}
                }, array<any>()); });
            }
            return false;
        }
    }
    }, 
    object::pair{std:("examples"), array<array<any>>{ array<object>{ object{
        object::pair{std:("name"), std:("{{name1}}")}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), std:("What is my updates format?")}
        }}
    }, object{
        object::pair{std:("name"), std:("{{botName}}")}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), std:("I'll show you your updates format")}, 
            object::pair{std:("actions"), array<string>{ std:("UPDATES_FORMAT") }}
        }}
    } }, array<object>{ object{
        object::pair{std:("name"), std:("{{name1}}")}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), std:("Show me my updates format")}
        }}
    }, object{
        object::pair{std:("name"), std:("{{botName}}")}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), std:("Let me fetch your updates format")}, 
            object::pair{std:("actions"), array<string>{ std:("UPDATES_FORMAT") }}
        }}
    } }, array<object>{ object{
        object::pair{std:("name"), std:("{{name1}}")}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), std:("How should I format my updates?")}
        }}
    }, object{
        object::pair{std:("name"), std:("{{botName}}")}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), std:("I'll get the format for your updates")}, 
            object::pair{std:("actions"), array<string>{ std:("UPDATES_FORMAT") }}
        }}
    } } }}
};

void Main(void)
{
}

MAIN
