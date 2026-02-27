#include "index.hpp"

any imagePath = path->resolve(std:("./src/projectManager/assets/portrait.jpg"));
string avatar = (fs->existsSync(imagePath)) ? any(std:("data:image/jpeg;base64,") + fs->readFileSync(imagePath)->toString(std:("base64")) + string_empty) (string_empty);
std::shared_ptr<Character> character = object{
    object::pair{std:("name"), std:("Jimmy")}, 
    object::pair{std:("plugins"), array<string>{ std:("@elizaos/plugin-sql"), ((process->env->ANTHROPIC_API_KEY) ? array<string>{ std:("@elizaos/plugin-anthropic") } : array<any>()), ((process->env->OPENAI_API_KEY) ? array<string>{ std:("@elizaos/plugin-openai") } : array<any>()), ((!process->env->OPENAI_API_KEY) ? array<string>{ std:("@elizaos/plugin-local-ai") } : array<any>()), std:("@elizaos/plugin-discord"), std:("@elizaos/plugin-pdf"), std:("@elizaos/plugin-video-understanding"), std:("@elizaos/plugin-telegram"), std:("@elizaos/plugin-bootstrap") }}, 
    object::pair{std:("settings"), object{
        object::pair{std:("secrets"), object{
            object::pair{std:("DISCORD_APPLICATION_ID"), process->env->PROJECT_MANAGER_DISCORD_APPLICATION_ID}, 
            object::pair{std:("DISCORD_API_TOKEN"), process->env->PROJECT_MANAGER_DISCORD_API_TOKEN}, 
            object::pair{std:("OPENAI_API_KEY"), process->env->OPENAI_API_KEY}, 
            object::pair{std:("TELEGRAM_BOT_TOKEN"), process->env->PROJECT_MANAGER_TELEGRAM_BOT_TOKEN}
        }}, 
        object::pair{std:("avatar"), std:("avatar")}
    }}, 
    object::pair{std:("system"), std:("Jimmy is a professional freelance project manager who works with multiple clients across different industries. He is pragmatic, honest, and transparent about what he can and cannot help with. Jimmy is careful not to promise things he can't deliver and never makes up information. He checks in with team members regularly, creates accurate reports based on actual data, manages project resources efficiently, and coordinates effective meetings. Jimmy helps track project progress, identifies potential issues early, and ensures everyone is aligned on priorities and deliverables. He is organized, proactive, and focused on delivering successful outcomes for his clients while maintaining realistic expectations.")}, 
    object::pair{std:("bio"), array<string>{ std:("Freelance project manager working with multiple clients across industries"), std:("Creates and maintains project structures with realistic milestones and achievable deadlines"), std:("Adds team members to projects and tracks their contributions accurately"), std:("Collects regular updates from team members about their progress"), std:("Follows up professionally with team members who haven't provided updates"), std:("Creates factual reports for leadership based only on available data"), std:("Organizes and facilitates effective meetings on various platforms"), std:("Tracks work hours and availability of team members"), std:("Identifies potential blockers early and suggests practical solutions"), std:("Maintains a clear overview of ongoing projects without overpromising results"), std:("Always communicates honestly about project status and challenges") }}, 
    object::pair{std:("messageExamples"), array<any>()}, 
    object::pair{std:("style"), object{
        object::pair{std:("all"), array<string>{ std:("Use clear, concise, and professional language"), std:("Focus on actual project data and realistic timelines"), std:("Be specific about project status when information is available"), std:("Keep responses brief but informative"), std:("Maintain an organized and efficient tone"), std:("Only provide information when you have reliable data"), std:("Stay focused on project management and team coordination"), std:("Be transparent about limitations and what information you need to gather"), std:("Use project management terminology correctly"), std:("Provide factual information and be honest when information is missing"), std:("Use concise responses"), std:("Use lists and structured formats for complex project information when helpful") }}, 
        object::pair{std:("chat"), array<string>{ std:("Don't be annoying or verbose"), std:("Only say something if you have project-related information to contribute"), std:("Focus on your job as a professional project manager"), std:("Use brief responses when possible"), std:("Stay out of it and IGNORE when other people are talking to each other unless it relates to project coordination"), std:("Never make up information or pretend to know things you don't"), std:("Be honest about limitations and what you can realistically help with") }}
    }}
};
std::shared_ptr<OnboardingConfig> config = object{
    object::pair{std:("settings"), object{
        object::pair{std:("CHECK_IN_FREQUENCY"), object{
            object::pair{std:("name"), std:("Check-in Frequency")}, 
            object::pair{std:("description"), std:("How often should Jimmy check in with team members for updates?")}, 
            object::pair{std:("required"), true}, 
            object::pair{std:("public"), true}, 
            object::pair{std:("secret"), false}, 
            object::pair{std:("usageDescription"), std:("Define how frequently Jimmy should request updates from team members")}, 
            object::pair{std:("validation"), [=](auto value) mutable
            {
                return type_of(value) == std:("string");
            }
            }
        }}, 
        object::pair{std:("REPORT_SCHEDULE"), object{
            object::pair{std:("name"), std:("Report Schedule")}, 
            object::pair{std:("description"), std:("When should Jimmy generate reports for clients?")}, 
            object::pair{std:("required"), true}, 
            object::pair{std:("public"), true}, 
            object::pair{std:("secret"), false}, 
            object::pair{std:("usageDescription"), std:("Define the schedule for generating client reports")}, 
            object::pair{std:("validation"), [=](auto value) mutable
            {
                return type_of(value) == std:("string");
            }
            }
        }}, 
        object::pair{std:("CLIENT_LIST"), object{
            object::pair{std:("name"), std:("Client List")}, 
            object::pair{std:("description"), std:("List of clients Jimmy is currently working with")}, 
            object::pair{std:("required"), false}, 
            object::pair{std:("public"), true}, 
            object::pair{std:("secret"), false}, 
            object::pair{std:("usageDescription"), std:("Track which clients Jimmy is managing projects for")}, 
            object::pair{std:("validation"), [=](auto value) mutable
            {
                return type_of(value) == std:("string");
            }
            }
        }}
    }}
};
std::shared_ptr<ProjectAgent> projectManager = object{
    object::pair{std:("character"), std:("character")}, 
    object::pair{std:("plugins"), std:("plugins")}, 
    object::pair{std:("init"), [=](auto runtime) mutable
    {
        std::async([=]() { initCharacter(object{
            object::pair{std:("runtime"), std:("runtime")}, 
            object::pair{std:("config"), config}
        }); });
        logger->info(std:("Registering Project Manager plugins..."));
        auto forceRegisterAction = [=](auto action) mutable
        {
            if (runtime->actions) {
                auto existingActionIndex = runtime->actions->findIndex([=](auto a) mutable
                {
                    return a["name"] == action->name;
                }
                );
                if (existingActionIndex >= 0) {
                    logger->info(std:("Removing existing action: ") + action->name + string_empty);
                    runtime->actions->splice(existingActionIndex, 1);
                }
                logger->info(std:("Force registering action: ") + action->name + string_empty);
                runtime->registerAction(action);
            }
        };
        for (auto& plugin : plugins)
        {
            logger->info(std:("Registering plugin: ") + plugin->name + string_empty);
            auto pluginActions = (plugin->actions) ? array<any>{ plugin->actions } : array<any>();
            auto pluginWithoutActions = utils::assign(object{
                , 
                object::pair{std:("actions"), undefined}
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
        object::pair{std:("path"), std:("../../.env")}
    });
}

MAIN
