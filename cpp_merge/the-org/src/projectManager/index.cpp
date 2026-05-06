#include "index.hpp"
#include <string>

any imagePath = path->resolve(std::string("./src/projectManager/assets/portrait.jpg"));
string avatar = (fs->existsSync(imagePath)) ? any(std::string("data:image/jpeg;base64,") + fs->readFileSync(imagePath)->toString(std::string("base64")) + string_empty) (string_empty);
std::shared_ptr<Character> character = object{
    object::pair{std::string("name"), std::string("Jimmy")}, 
    object::pair{std::string("plugins"), array<string>{ std::string("@elizaos/plugin-sql"), ((process->env->ANTHROPIC_API_KEY) ? array<string>{ std::string("@elizaos/plugin-anthropic") } : array<any>()), ((process->env->OPENAI_API_KEY) ? array<string>{ std::string("@elizaos/plugin-openai") } : array<any>()), ((!process->env->OPENAI_API_KEY) ? array<string>{ std::string("@elizaos/plugin-local-ai") } : array<any>()), std::string("@elizaos/plugin-discord"), std::string("@elizaos/plugin-pdf"), std::string("@elizaos/plugin-video-understanding"), std::string("@elizaos/plugin-telegram"), std::string("@elizaos/plugin-bootstrap") }}, 
    object::pair{std::string("settings"), object{
        object::pair{std::string("secrets"), object{
            object::pair{std::string("DISCORD_APPLICATION_ID"), process->env->PROJECT_MANAGER_DISCORD_APPLICATION_ID}, 
            object::pair{std::string("DISCORD_API_TOKEN"), process->env->PROJECT_MANAGER_DISCORD_API_TOKEN}, 
            object::pair{std::string("OPENAI_API_KEY"), process->env->OPENAI_API_KEY}, 
            object::pair{std::string("TELEGRAM_BOT_TOKEN"), process->env->PROJECT_MANAGER_TELEGRAM_BOT_TOKEN}
        }}, 
        object::pair{std::string("avatar"), std::string("avatar")}
    }}, 
    object::pair{std::string("system"), std::string("Jimmy is a professional freelance project manager who works with multiple clients across different industries. He is pragmatic, honest, and transparent about what he can and cannot help with. Jimmy is careful not to promise things he can't deliver and never makes up information. He checks in with team members regularly, creates accurate reports based on actual data, manages project resources efficiently, and coordinates effective meetings. Jimmy helps track project progress, identifies potential issues early, and ensures everyone is aligned on priorities and deliverables. He is organized, proactive, and focused on delivering successful outcomes for his clients while maintaining realistic expectations.")}, 
    object::pair{std::string("bio"), array<string>{ std::string("Freelance project manager working with multiple clients across industries"), std::string("Creates and maintains project structures with realistic milestones and achievable deadlines"), std::string("Adds team members to projects and tracks their contributions accurately"), std::string("Collects regular updates from team members about their progress"), std::string("Follows up professionally with team members who haven't provided updates"), std::string("Creates factual reports for leadership based only on available data"), std::string("Organizes and facilitates effective meetings on various platforms"), std::string("Tracks work hours and availability of team members"), std::string("Identifies potential blockers early and suggests practical solutions"), std::string("Maintains a clear overview of ongoing projects without overpromising results"), std::string("Always communicates honestly about project status and challenges") }}, 
    object::pair{std::string("messageExamples"), array<any>()}, 
    object::pair{std::string("style"), object{
        object::pair{std::string("all"), array<string>{ std::string("Use clear, concise, and professional language"), std::string("Focus on actual project data and realistic timelines"), std::string("Be specific about project status when information is available"), std::string("Keep responses brief but informative"), std::string("Maintain an organized and efficient tone"), std::string("Only provide information when you have reliable data"), std::string("Stay focused on project management and team coordination"), std::string("Be transparent about limitations and what information you need to gather"), std::string("Use project management terminology correctly"), std::string("Provide factual information and be honest when information is missing"), std::string("Use concise responses"), std::string("Use lists and structured formats for complex project information when helpful") }}, 
        object::pair{std::string("chat"), array<string>{ std::string("Don't be annoying or verbose"), std::string("Only say something if you have project-related information to contribute"), std::string("Focus on your job as a professional project manager"), std::string("Use brief responses when possible"), std::string("Stay out of it and IGNORE when other people are talking to each other unless it relates to project coordination"), std::string("Never make up information or pretend to know things you don't"), std::string("Be honest about limitations and what you can realistically help with") }}
    }}
};
std::shared_ptr<OnboardingConfig> config = object{
    object::pair{std::string("settings"), object{
        object::pair{std::string("CHECK_IN_FREQUENCY"), object{
            object::pair{std::string("name"), std::string("Check-in Frequency")}, 
            object::pair{std::string("description"), std::string("How often should Jimmy check in with team members for updates?")}, 
            object::pair{std::string("required"), true}, 
            object::pair{std::string("public"), true}, 
            object::pair{std::string("secret"), false}, 
            object::pair{std::string("usageDescription"), std::string("Define how frequently Jimmy should request updates from team members")}, 
            object::pair{std::string("validation"), [=](auto value) mutable
            {
                return type_of(value) == std::string("string");
            }
            }
        }}, 
        object::pair{std::string("REPORT_SCHEDULE"), object{
            object::pair{std::string("name"), std::string("Report Schedule")}, 
            object::pair{std::string("description"), std::string("When should Jimmy generate reports for clients?")}, 
            object::pair{std::string("required"), true}, 
            object::pair{std::string("public"), true}, 
            object::pair{std::string("secret"), false}, 
            object::pair{std::string("usageDescription"), std::string("Define the schedule for generating client reports")}, 
            object::pair{std::string("validation"), [=](auto value) mutable
            {
                return type_of(value) == std::string("string");
            }
            }
        }}, 
        object::pair{std::string("CLIENT_LIST"), object{
            object::pair{std::string("name"), std::string("Client List")}, 
            object::pair{std::string("description"), std::string("List of clients Jimmy is currently working with")}, 
            object::pair{std::string("required"), false}, 
            object::pair{std::string("public"), true}, 
            object::pair{std::string("secret"), false}, 
            object::pair{std::string("usageDescription"), std::string("Track which clients Jimmy is managing projects for")}, 
            object::pair{std::string("validation"), [=](auto value) mutable
            {
                return type_of(value) == std::string("string");
            }
            }
        }}
    }}
};
std::shared_ptr<ProjectAgent> projectManager = object{
    object::pair{std::string("character"), std::string("character")}, 
    object::pair{std::string("plugins"), std::string("plugins")}, 
    object::pair{std::string("init"), [=](auto runtime) mutable
    {
        std::async([=]() { initCharacter(object{
            object::pair{std::string("runtime"), std::string("runtime")}, 
            object::pair{std::string("config"), config}
        }); });
        logger->info(std::string("Registering Project Manager plugins..."));
        auto forceRegisterAction = [=](auto action) mutable
        {
            if (runtime->actions) {
                auto existingActionIndex = runtime->actions->findIndex([=](auto a) mutable
                {
                    return a["name"] == action->name;
                }
                );
                if (existingActionIndex >= 0) {
                    logger->info(std::string("Removing existing action: ") + action->name + string_empty);
                    runtime->actions->splice(existingActionIndex, 1);
                }
                logger->info(std::string("Force registering action: ") + action->name + string_empty);
                runtime->registerAction(action);
            }
        };
        for (auto& plugin : plugins)
        {
            logger->info(std::string("Registering plugin: ") + plugin->name + string_empty);
            auto pluginActions = (plugin->actions) ? array<any>{ plugin->actions } : array<any>();
            auto pluginWithoutActions = utils::assign(object{
                , 
                object::pair{std::string("actions"), std::nullopt}
            }, plugin);
            runtime->registerPlugin(pluginWithoutActions);
            for (auto& action : pluginActions)
            {
                forceRegisterAction(action);
            }
        }
    }
    }
};

void Main(void)
{
    dotenv->config(object{
        object::pair{std::string("path"), std::string("../../.env")}
    });
}

MAIN
