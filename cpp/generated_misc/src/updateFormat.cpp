#include "/home/runner/work/elizaos-cpp/elizaos-cpp/the-org/src/projectManager/plugins/team-coordinator/actions/updateFormat.h"

std::shared_ptr<UUID> getStorageRoomId(std::shared_ptr<IAgentRuntime> runtime, string serverId)
{
    auto serverHash = serverId->replace((new RegExp(std::string("[^a-zA-Z0-9]"))), string_empty);
    return createUniqueUuid(runtime, std::string("store-team-members-") + serverHash + string_empty);
};


std::shared_ptr<Action> updatesFormatAction = object{
    object::pair{std::string("name"), std::string("UPDATES_FORMAT")}, 
    object::pair{std::string("description"), std::string("Show the updates format for a specific team member.")}, 
    object::pair{std::string("similes"), array<string>{ std::string("UPDATES_FORMAT"), std::string("SHOW_FORMAT"), std::string("GET_FORMAT"), std::string("MY_FORMAT"), std::string("VIEW_FORMAT") }}, 
    object::pair{std::string("validate"), [=](auto runtime, auto message, auto state) mutable
    {
        try
        {
            if (!state) return false;
            state["data"]["userId"] = message->entityId;
            logger->info(std::string("Valid request to get updates format for user ") + message->entityId + string_empty);
            return true;
        }
        catch (const any& error)
        {
            auto err = as<std::shared_ptr<Error>>(error);
            logger->error(std::string("Error in updatesFormatAction validation:"), err);
            logger->error(std::string("Error stack: ") + (OR((err->stack), (std::string("No stack trace available")))) + string_empty);
            return false;
        }
    }
    }, 
    object::pair{std::string("handler"), [=](auto runtime, auto message, auto state, auto options = object{}, auto callback = undefined) mutable
    {
        try
        {
            logger->info(std::string("=== UPDATES-FORMAT HANDLER START ==="));
            if (!state) return false;
            if (!callback) {
                logger->warn(std::string("No callback function provided"));
                return false;
            }
            auto entityById = std::async([=]() { runtime->getEntityById(message->entityId); });
            shared username = OR((entityById->metadata->discord->userName), (entityById->metadata->telegram->userName));
            logger->info(std::string("Entity metadata - Telegram:"), JSON->stringify(OR((entityById->metadata->telegram), (std::string("Not available")))));
            logger->info(std::string("Entity metadata - Discord:"), JSON->stringify(OR((entityById->metadata->discord), (std::string("Not available")))));
            if (!username) {
                logger->error(std::string("No username found in state"));
                std::async([=]() { callback(object{
                    object::pair{std::string("text"), std::string("❌ Failed to identify your username. Please try again.")}
                }, array<any>()); });
                return false;
            }
            logger->info(std::string("Looking for updates format for user: ") + username + string_empty);
            auto allMemories = std::async([=]() { runtime->getMemories(object{
                object::pair{std::string("tableName"), std::string("messages")}, 
                object::pair{std::string("agentId"), runtime->agentId}
            }); });
            logger->info(std::string("Found ") + allMemories->length + std::string(" total memories"));
            auto teamMemberConfigs = allMemories->filter([=](auto memory) mutable
            {
                return memory["content"]->type == std::string("store-team-members-memory");
            }
            );
            logger->info(std::string("Found ") + teamMemberConfigs->length + std::string(" team member config memories"));
            shared allTeamMembers = array<std::shared_ptr<TeamMember>>();
            teamMemberConfigs->forEach([=](auto config) mutable
            {
                if (config["content"]["config"]) {
                    auto configData = as<object>(config["content"]["config"]);
                    auto teamMembers = OR((configData["teamMembers"]), (array<any>()));
                    logger->info(std::string("Found ") + teamMembers->get_length() + std::string(" team members in config"));
                    allTeamMembers = array<std::shared_ptr<TeamMember>>{ allTeamMembers, teamMembers };
                }
            }
            );
            logger->info(std::string("Total of ") + allTeamMembers->get_length() + std::string(" team members found across all servers"));
            logger->info(std::string("Looking for username: ") + username + std::string(" among team members:"), allTeamMembers->map([=](auto member) mutable
            {
                return (object{
                    object::pair{std::string("tgName"), member->tgName}, 
                    object::pair{std::string("discordName"), member->discordName}
                });
            }
            ));
            auto teamMember = allTeamMembers->find([=](auto member) mutable
            {
                return OR(((AND((member->tgName), ((OR((member->tgName == username), (member->tgName->replace(std::string("@"), string_empty) == username["replace"](std::string("@"), string_empty)))))))), ((AND((member->discordName), (member->discordName->replace(std::string("@"), string_empty) == username["replace"](std::string("@"), string_empty))))));
            }
            );
            if (!teamMember) {
                logger->info(std::string("No team member found with username ") + username + string_empty);
                std::async([=]() { callback(object{
                    object::pair{std::string("text"), std::string("❌ No team member found with username ") + username + std::string(". Please make sure you are registered as a team member.")}
                }, array<any>()); });
                return true;
            }
            logger->info(std::string("Found team member: ") + JSON->stringify(teamMember) + string_empty);
            shared responseText = std::string("📋 **Updates Format for ") + username + std::string("**\
\
");
            responseText += std::string("Section: ") + (OR((teamMember->section), (std::string("Unassigned")))) + std::string("\
");
            if (AND((teamMember->updatesFormat), (teamMember->updatesFormat->get_length() > 0))) {
                responseText += std::string("\
Your updates should include the following fields:\
");
                teamMember->updatesFormat->forEach([=](auto field, auto index) mutable
                {
                    responseText += string_empty + (index + 1) + std::string(". ") + field + std::string("\
");
                }
                );
                responseText += std::string("\
Important: End your message with "sending my personal updates" so it can be properly tracked.");
            } else {
                responseText += std::string("\
You don't have any specific update format fields defined. Please use the standard format for your updates:\
\
") + std::string("- **Main Priority for next week**\
") + std::string("    - Text\
") + std::string("- **What did you get done this week?**\
") + std::string("    - Text\
") + std::string("- **Blockers**\
") + std::string("    - Text\
\
") + std::string("Important: End your message with "sending my personal updates" so it can be properly tracked.");
            }
            std::async([=]() { callback(object{
                object::pair{std::string("text"), responseText}
            }, array<any>()); });
            logger->info(std::string("=== UPDATES-FORMAT HANDLER END ==="));
            return true;
        }
        catch (const any& error)
        {
            auto err = as<std::shared_ptr<Error>>(error);
            logger->error(std::string("=== UPDATES-FORMAT HANDLER ERROR ==="));
            logger->error(std::string("Error processing updates format request: ") + err + string_empty);
            logger->error(std::string("Error stack: ") + (OR((err->stack), (std::string("No stack trace available")))) + string_empty);
            if (callback) {
                std::async([=]() { callback(object{
                    object::pair{std::string("text"), std::string("❌ An unexpected error occurred while fetching your updates format. Please try again later.")}
                }, array<any>()); });
            }
            return false;
        }
    }
    }, 
    object::pair{std::string("examples"), array<array<any>>{ array<object>{ object{
        object::pair{std::string("name"), std::string("{{name1}}")}, 
        object::pair{std::string("content"), object{
            object::pair{std::string("text"), std::string("What is my updates format?")}
        }}
    }, object{
        object::pair{std::string("name"), std::string("{{botName}}")}, 
        object::pair{std::string("content"), object{
            object::pair{std::string("text"), std::string("I'll show you your updates format")}, 
            object::pair{std::string("actions"), array<string>{ std::string("UPDATES_FORMAT") }}
        }}
    } }, array<object>{ object{
        object::pair{std::string("name"), std::string("{{name1}}")}, 
        object::pair{std::string("content"), object{
            object::pair{std::string("text"), std::string("Show me my updates format")}
        }}
    }, object{
        object::pair{std::string("name"), std::string("{{botName}}")}, 
        object::pair{std::string("content"), object{
            object::pair{std::string("text"), std::string("Let me fetch your updates format")}, 
            object::pair{std::string("actions"), array<string>{ std::string("UPDATES_FORMAT") }}
        }}
    } }, array<object>{ object{
        object::pair{std::string("name"), std::string("{{name1}}")}, 
        object::pair{std::string("content"), object{
            object::pair{std::string("text"), std::string("How should I format my updates?")}
        }}
    }, object{
        object::pair{std::string("name"), std::string("{{botName}}")}, 
        object::pair{std::string("content"), object{
            object::pair{std::string("text"), std::string("I'll get the format for your updates")}, 
            object::pair{std::string("actions"), array<string>{ std::string("UPDATES_FORMAT") }}
        }}
    } } }}
};

void Main(void)
{
}

MAIN
