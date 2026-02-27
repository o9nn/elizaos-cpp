#include "/home/runner/work/elizaos-cpp/elizaos-cpp/eliza/packages/docs/sidebars.h"

std::shared_ptr<SidebarsConfig> sidebars = object{
    object::pair{std::string("tutorialSidebar"), array<object>{ object{
        object::pair{std::string("type"), std::string("doc")}, 
        object::pair{std::string("id"), std::string("intro")}, 
        object::pair{std::string("label"), std::string("🚀 Introduction")}
    }, object{
        object::pair{std::string("type"), std::string("category")}, 
        object::pair{std::string("label"), std::string("🏁 Getting Started")}, 
        object::pair{std::string("items"), array<object>{ object{
            object::pair{std::string("type"), std::string("doc")}, 
            object::pair{std::string("id"), std::string("quickstart")}, 
            object::pair{std::string("label"), std::string("⭐ Quick Start")}
        }, object{
            object::pair{std::string("type"), std::string("doc")}, 
            object::pair{std::string("id"), std::string("faq")}, 
            object::pair{std::string("label"), std::string("❓ FAQ")}
        }, object{
            object::pair{std::string("type"), std::string("doc")}, 
            object::pair{std::string("id"), std::string("contributing")}, 
            object::pair{std::string("label"), std::string("👥 Contributing")}
        } }}, 
        object::pair{std::string("collapsed"), false}
    }, object{
        object::pair{std::string("type"), std::string("category")}, 
        object::pair{std::string("label"), std::string("🧠 Core Concepts")}, 
        object::pair{std::string("collapsed"), false}, 
        object::pair{std::string("items"), array<object>{ object{
            object::pair{std::string("type"), std::string("doc")}, 
            object::pair{std::string("id"), std::string("core/overview")}, 
            object::pair{std::string("label"), std::string("Overview")}
        }, object{
            object::pair{std::string("type"), std::string("doc")}, 
            object::pair{std::string("id"), std::string("core/actions")}, 
            object::pair{std::string("label"), std::string("Actions")}
        }, object{
            object::pair{std::string("type"), std::string("doc")}, 
            object::pair{std::string("id"), std::string("core/agents")}, 
            object::pair{std::string("label"), std::string("Agent Runtime")}
        }, object{
            object::pair{std::string("type"), std::string("doc")}, 
            object::pair{std::string("id"), std::string("core/database")}, 
            object::pair{std::string("label"), std::string("Database Adapters")}
        }, object{
            object::pair{std::string("type"), std::string("doc")}, 
            object::pair{std::string("id"), std::string("core/entities")}, 
            object::pair{std::string("label"), std::string("Entities")}
        }, object{
            object::pair{std::string("type"), std::string("doc")}, 
            object::pair{std::string("id"), std::string("core/evaluators")}, 
            object::pair{std::string("label"), std::string("Evaluators")}
        }, object{
            object::pair{std::string("type"), std::string("doc")}, 
            object::pair{std::string("id"), std::string("core/characters")}, 
            object::pair{std::string("label"), std::string("Character Files")}
        }, object{
            object::pair{std::string("type"), std::string("doc")}, 
            object::pair{std::string("id"), std::string("core/knowledge")}, 
            object::pair{std::string("label"), std::string("Knowledge")}
        }, object{
            object::pair{std::string("type"), std::string("doc")}, 
            object::pair{std::string("id"), std::string("core/plugins")}, 
            object::pair{std::string("label"), std::string("Plugins")}
        }, object{
            object::pair{std::string("type"), std::string("doc")}, 
            object::pair{std::string("id"), std::string("core/project")}, 
            object::pair{std::string("label"), std::string("Project")}
        }, object{
            object::pair{std::string("type"), std::string("doc")}, 
            object::pair{std::string("id"), std::string("core/providers")}, 
            object::pair{std::string("label"), std::string("Providers")}
        }, object{
            object::pair{std::string("type"), std::string("doc")}, 
            object::pair{std::string("id"), std::string("core/rooms")}, 
            object::pair{std::string("label"), std::string("Rooms")}
        }, object{
            object::pair{std::string("type"), std::string("doc")}, 
            object::pair{std::string("id"), std::string("core/services")}, 
            object::pair{std::string("label"), std::string("Services")}
        }, object{
            object::pair{std::string("type"), std::string("doc")}, 
            object::pair{std::string("id"), std::string("core/tasks")}, 
            object::pair{std::string("label"), std::string("Tasks")}
        }, object{
            object::pair{std::string("type"), std::string("doc")}, 
            object::pair{std::string("id"), std::string("core/worlds")}, 
            object::pair{std::string("label"), std::string("Worlds")}
        }, object{
            object::pair{std::string("type"), std::string("doc")}, 
            object::pair{std::string("id"), std::string("core/testing")}, 
            object::pair{std::string("label"), std::string("Testing")}
        } }}
    }, object{
        object::pair{std::string("type"), std::string("category")}, 
        object::pair{std::string("label"), std::string("🖥️ CLI")}, 
        object::pair{std::string("collapsed"), false}, 
        object::pair{std::string("items"), array<object>{ object{
            object::pair{std::string("type"), std::string("doc")}, 
            object::pair{std::string("id"), std::string("cli/overview")}, 
            object::pair{std::string("label"), std::string("Overview")}
        }, object{
            object::pair{std::string("type"), std::string("doc")}, 
            object::pair{std::string("id"), std::string("cli/agent")}, 
            object::pair{std::string("label"), std::string("Agent")}
        }, object{
            object::pair{std::string("type"), std::string("doc")}, 
            object::pair{std::string("id"), std::string("cli/create")}, 
            object::pair{std::string("label"), std::string("Create")}
        }, object{
            object::pair{std::string("type"), std::string("doc")}, 
            object::pair{std::string("id"), std::string("cli/dev")}, 
            object::pair{std::string("label"), std::string("Dev")}
        }, object{
            object::pair{std::string("type"), std::string("doc")}, 
            object::pair{std::string("id"), std::string("cli/env")}, 
            object::pair{std::string("label"), std::string("Environment")}
        }, object{
            object::pair{std::string("type"), std::string("doc")}, 
            object::pair{std::string("id"), std::string("cli/publish")}, 
            object::pair{std::string("label"), std::string("Publish")}
        }, object{
            object::pair{std::string("type"), std::string("doc")}, 
            object::pair{std::string("id"), std::string("cli/start")}, 
            object::pair{std::string("label"), std::string("Start")}
        }, object{
            object::pair{std::string("type"), std::string("doc")}, 
            object::pair{std::string("id"), std::string("cli/test")}, 
            object::pair{std::string("label"), std::string("Test")}
        }, object{
            object::pair{std::string("type"), std::string("doc")}, 
            object::pair{std::string("id"), std::string("cli/update")}, 
            object::pair{std::string("label"), std::string("Update")}
        } }}
    }, object{
        object::pair{std::string("type"), std::string("category")}, 
        object::pair{std::string("label"), std::string("🌐 REST API")}, 
        object::pair{std::string("items"), array<object>{ object{
            object::pair{std::string("type"), std::string("autogenerated")}, 
            object::pair{std::string("dirName"), std::string("rest")}
        } }}, 
        object::pair{std::string("collapsed"), true}
    }, object{
        object::pair{std::string("type"), std::string("doc")}, 
        object::pair{std::string("id"), std::string("awesome-eliza")}, 
        object::pair{std::string("label"), std::string("Awesome elizaOS")}
    }, object{
        object::pair{std::string("type"), std::string("doc")}, 
        object::pair{std::string("id"), std::string("changelog")}, 
        object::pair{std::string("label"), std::string("CHANGELOG")}
    } }}
};

void Main(void)
{
}

MAIN
