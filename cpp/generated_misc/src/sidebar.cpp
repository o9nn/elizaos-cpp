#include "/home/runner/work/elizaos-cpp/elizaos-cpp/eliza/packages/docs/versioned_docs/version-0.25.9/rest/sidebar.h"

std::shared_ptr<SidebarsConfig> sidebar = object{
    object::pair{std::string("apisidebar"), array<object>{ object{
        object::pair{std::string("type"), std::string("doc")}, 
        object::pair{std::string("id"), std::string("rest/eliza-os-api")}
    }, object{
        object::pair{std::string("type"), std::string("category")}, 
        object::pair{std::string("label"), std::string("agents")}, 
        object::pair{std::string("items"), array<object>{ object{
            object::pair{std::string("type"), std::string("doc")}, 
            object::pair{std::string("id"), std::string("rest/list-agents")}, 
            object::pair{std::string("label"), std::string("List all agents")}, 
            object::pair{std::string("className"), std::string("api-method get")}
        }, object{
            object::pair{std::string("type"), std::string("doc")}, 
            object::pair{std::string("id"), std::string("rest/get-agent")}, 
            object::pair{std::string("label"), std::string("Get agent details")}, 
            object::pair{std::string("className"), std::string("api-method get")}
        }, object{
            object::pair{std::string("type"), std::string("doc")}, 
            object::pair{std::string("id"), std::string("rest/delete-agent")}, 
            object::pair{std::string("label"), std::string("Delete an agent")}, 
            object::pair{std::string("className"), std::string("api-method delete")}
        }, object{
            object::pair{std::string("type"), std::string("doc")}, 
            object::pair{std::string("id"), std::string("rest/set-agent")}, 
            object::pair{std::string("label"), std::string("Update or create an agent")}, 
            object::pair{std::string("className"), std::string("api-method post")}
        }, object{
            object::pair{std::string("type"), std::string("doc")}, 
            object::pair{std::string("id"), std::string("rest/start-agent")}, 
            object::pair{std::string("label"), std::string("Start a new agent")}, 
            object::pair{std::string("className"), std::string("api-method post")}
        }, object{
            object::pair{std::string("type"), std::string("doc")}, 
            object::pair{std::string("id"), std::string("rest/stop-agent")}, 
            object::pair{std::string("label"), std::string("Stop an agent")}, 
            object::pair{std::string("className"), std::string("api-method post")}
        } }}
    }, object{
        object::pair{std::string("type"), std::string("category")}, 
        object::pair{std::string("label"), std::string("memory")}, 
        object::pair{std::string("items"), array<object>{ object{
            object::pair{std::string("type"), std::string("doc")}, 
            object::pair{std::string("id"), std::string("rest/get-memories")}, 
            object::pair{std::string("label"), std::string("Get agent memories for a room")}, 
            object::pair{std::string("className"), std::string("api-method get")}
        } }}
    }, object{
        object::pair{std::string("type"), std::string("category")}, 
        object::pair{std::string("label"), std::string("utilities")}, 
        object::pair{std::string("items"), array<object>{ object{
            object::pair{std::string("type"), std::string("doc")}, 
            object::pair{std::string("id"), std::string("rest/get-api-root")}, 
            object::pair{std::string("label"), std::string("API root")}, 
            object::pair{std::string("className"), std::string("api-method get")}
        }, object{
            object::pair{std::string("type"), std::string("doc")}, 
            object::pair{std::string("id"), std::string("rest/get-hello")}, 
            object::pair{std::string("label"), std::string("Hello world endpoint")}, 
            object::pair{std::string("className"), std::string("api-method get")}
        }, object{
            object::pair{std::string("type"), std::string("doc")}, 
            object::pair{std::string("id"), std::string("rest/list-stored-characters")}, 
            object::pair{std::string("label"), std::string("List stored character files")}, 
            object::pair{std::string("className"), std::string("api-method get")}
        } }}
    } }}
};

void Main(void)
{
}

MAIN
