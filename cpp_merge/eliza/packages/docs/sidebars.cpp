#include "sidebars.hpp"

std::shared_ptr<SidebarsConfig> sidebars = object{
    object::pair{std:("tutorialSidebar"), array<object>{ object{
        object::pair{std:("type"), std:("doc")}, 
        object::pair{std:("id"), std:("intro")}, 
        object::pair{std:("label"), std:("🚀 Introduction")}
    }, object{
        object::pair{std:("type"), std:("category")}, 
        object::pair{std:("label"), std:("🏁 Getting Started")}, 
        object::pair{std:("items"), array<object>{ object{
            object::pair{std:("type"), std:("doc")}, 
            object::pair{std:("id"), std:("quickstart")}, 
            object::pair{std:("label"), std:("⭐ Quick Start")}
        }, object{
            object::pair{std:("type"), std:("doc")}, 
            object::pair{std:("id"), std:("faq")}, 
            object::pair{std:("label"), std:("❓ FAQ")}
        }, object{
            object::pair{std:("type"), std:("doc")}, 
            object::pair{std:("id"), std:("contributing")}, 
            object::pair{std:("label"), std:("👥 Contributing")}
        } }}, 
        object::pair{std:("collapsed"), false}
    }, object{
        object::pair{std:("type"), std:("category")}, 
        object::pair{std:("label"), std:("🧠 Core Concepts")}, 
        object::pair{std:("collapsed"), false}, 
        object::pair{std:("items"), array<object>{ object{
            object::pair{std:("type"), std:("doc")}, 
            object::pair{std:("id"), std:("core/overview")}, 
            object::pair{std:("label"), std:("Overview")}
        }, object{
            object::pair{std:("type"), std:("doc")}, 
            object::pair{std:("id"), std:("core/actions")}, 
            object::pair{std:("label"), std:("Actions")}
        }, object{
            object::pair{std:("type"), std:("doc")}, 
            object::pair{std:("id"), std:("core/agents")}, 
            object::pair{std:("label"), std:("Agent Runtime")}
        }, object{
            object::pair{std:("type"), std:("doc")}, 
            object::pair{std:("id"), std:("core/database")}, 
            object::pair{std:("label"), std:("Database Adapters")}
        }, object{
            object::pair{std:("type"), std:("doc")}, 
            object::pair{std:("id"), std:("core/entities")}, 
            object::pair{std:("label"), std:("Entities")}
        }, object{
            object::pair{std:("type"), std:("doc")}, 
            object::pair{std:("id"), std:("core/evaluators")}, 
            object::pair{std:("label"), std:("Evaluators")}
        }, object{
            object::pair{std:("type"), std:("doc")}, 
            object::pair{std:("id"), std:("core/characters")}, 
            object::pair{std:("label"), std:("Character Files")}
        }, object{
            object::pair{std:("type"), std:("doc")}, 
            object::pair{std:("id"), std:("core/knowledge")}, 
            object::pair{std:("label"), std:("Knowledge")}
        }, object{
            object::pair{std:("type"), std:("doc")}, 
            object::pair{std:("id"), std:("core/plugins")}, 
            object::pair{std:("label"), std:("Plugins")}
        }, object{
            object::pair{std:("type"), std:("doc")}, 
            object::pair{std:("id"), std:("core/project")}, 
            object::pair{std:("label"), std:("Project")}
        }, object{
            object::pair{std:("type"), std:("doc")}, 
            object::pair{std:("id"), std:("core/providers")}, 
            object::pair{std:("label"), std:("Providers")}
        }, object{
            object::pair{std:("type"), std:("doc")}, 
            object::pair{std:("id"), std:("core/rooms")}, 
            object::pair{std:("label"), std:("Rooms")}
        }, object{
            object::pair{std:("type"), std:("doc")}, 
            object::pair{std:("id"), std:("core/services")}, 
            object::pair{std:("label"), std:("Services")}
        }, object{
            object::pair{std:("type"), std:("doc")}, 
            object::pair{std:("id"), std:("core/tasks")}, 
            object::pair{std:("label"), std:("Tasks")}
        }, object{
            object::pair{std:("type"), std:("doc")}, 
            object::pair{std:("id"), std:("core/worlds")}, 
            object::pair{std:("label"), std:("Worlds")}
        }, object{
            object::pair{std:("type"), std:("doc")}, 
            object::pair{std:("id"), std:("core/testing")}, 
            object::pair{std:("label"), std:("Testing")}
        } }}
    }, object{
        object::pair{std:("type"), std:("category")}, 
        object::pair{std:("label"), std:("🖥️ CLI")}, 
        object::pair{std:("collapsed"), false}, 
        object::pair{std:("items"), array<object>{ object{
            object::pair{std:("type"), std:("doc")}, 
            object::pair{std:("id"), std:("cli/overview")}, 
            object::pair{std:("label"), std:("Overview")}
        }, object{
            object::pair{std:("type"), std:("doc")}, 
            object::pair{std:("id"), std:("cli/agent")}, 
            object::pair{std:("label"), std:("Agent")}
        }, object{
            object::pair{std:("type"), std:("doc")}, 
            object::pair{std:("id"), std:("cli/create")}, 
            object::pair{std:("label"), std:("Create")}
        }, object{
            object::pair{std:("type"), std:("doc")}, 
            object::pair{std:("id"), std:("cli/dev")}, 
            object::pair{std:("label"), std:("Dev")}
        }, object{
            object::pair{std:("type"), std:("doc")}, 
            object::pair{std:("id"), std:("cli/env")}, 
            object::pair{std:("label"), std:("Environment")}
        }, object{
            object::pair{std:("type"), std:("doc")}, 
            object::pair{std:("id"), std:("cli/publish")}, 
            object::pair{std:("label"), std:("Publish")}
        }, object{
            object::pair{std:("type"), std:("doc")}, 
            object::pair{std:("id"), std:("cli/start")}, 
            object::pair{std:("label"), std:("Start")}
        }, object{
            object::pair{std:("type"), std:("doc")}, 
            object::pair{std:("id"), std:("cli/test")}, 
            object::pair{std:("label"), std:("Test")}
        }, object{
            object::pair{std:("type"), std:("doc")}, 
            object::pair{std:("id"), std:("cli/update")}, 
            object::pair{std:("label"), std:("Update")}
        } }}
    }, object{
        object::pair{std:("type"), std:("category")}, 
        object::pair{std:("label"), std:("🌐 REST API")}, 
        object::pair{std:("items"), array<object>{ object{
            object::pair{std:("type"), std:("autogenerated")}, 
            object::pair{std:("dirName"), std:("rest")}
        } }}, 
        object::pair{std:("collapsed"), true}
    }, object{
        object::pair{std:("type"), std:("doc")}, 
        object::pair{std:("id"), std:("awesome-eliza")}, 
        object::pair{std:("label"), std:("Awesome elizaOS")}
    }, object{
        object::pair{std:("type"), std:("doc")}, 
        object::pair{std:("id"), std:("changelog")}, 
        object::pair{std:("label"), std:("CHANGELOG")}
    } }}
};

void Main(void)
{
}

MAIN
