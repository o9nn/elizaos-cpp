#include "sidebar.hpp"

std::shared_ptr<SidebarsConfig> sidebar = object{
    object::pair{std:("apisidebar"), array<object>{ object{
        object::pair{std:("type"), std:("doc")}, 
        object::pair{std:("id"), std:("rest/eliza-os-api")}
    }, object{
        object::pair{std:("type"), std:("category")}, 
        object::pair{std:("label"), std:("agents")}, 
        object::pair{std:("items"), array<object>{ object{
            object::pair{std:("type"), std:("doc")}, 
            object::pair{std:("id"), std:("rest/list-agents")}, 
            object::pair{std:("label"), std:("List all agents")}, 
            object::pair{std:("className"), std:("api-method get")}
        }, object{
            object::pair{std:("type"), std:("doc")}, 
            object::pair{std:("id"), std:("rest/get-agent")}, 
            object::pair{std:("label"), std:("Get agent details")}, 
            object::pair{std:("className"), std:("api-method get")}
        }, object{
            object::pair{std:("type"), std:("doc")}, 
            object::pair{std:("id"), std:("rest/delete-agent")}, 
            object::pair{std:("label"), std:("Delete an agent")}, 
            object::pair{std:("className"), std:("api-method delete")}
        }, object{
            object::pair{std:("type"), std:("doc")}, 
            object::pair{std:("id"), std:("rest/set-agent")}, 
            object::pair{std:("label"), std:("Update or create an agent")}, 
            object::pair{std:("className"), std:("api-method post")}
        }, object{
            object::pair{std:("type"), std:("doc")}, 
            object::pair{std:("id"), std:("rest/start-agent")}, 
            object::pair{std:("label"), std:("Start a new agent")}, 
            object::pair{std:("className"), std:("api-method post")}
        }, object{
            object::pair{std:("type"), std:("doc")}, 
            object::pair{std:("id"), std:("rest/stop-agent")}, 
            object::pair{std:("label"), std:("Stop an agent")}, 
            object::pair{std:("className"), std:("api-method post")}
        } }}
    }, object{
        object::pair{std:("type"), std:("category")}, 
        object::pair{std:("label"), std:("memory")}, 
        object::pair{std:("items"), array<object>{ object{
            object::pair{std:("type"), std:("doc")}, 
            object::pair{std:("id"), std:("rest/get-memories")}, 
            object::pair{std:("label"), std:("Get agent memories for a room")}, 
            object::pair{std:("className"), std:("api-method get")}
        } }}
    }, object{
        object::pair{std:("type"), std:("category")}, 
        object::pair{std:("label"), std:("utilities")}, 
        object::pair{std:("items"), array<object>{ object{
            object::pair{std:("type"), std:("doc")}, 
            object::pair{std:("id"), std:("rest/get-api-root")}, 
            object::pair{std:("label"), std:("API root")}, 
            object::pair{std:("className"), std:("api-method get")}
        }, object{
            object::pair{std:("type"), std:("doc")}, 
            object::pair{std:("id"), std:("rest/get-hello")}, 
            object::pair{std:("label"), std:("Hello world endpoint")}, 
            object::pair{std:("className"), std:("api-method get")}
        }, object{
            object::pair{std:("type"), std:("doc")}, 
            object::pair{std:("id"), std:("rest/list-stored-characters")}, 
            object::pair{std:("label"), std:("List stored character files")}, 
            object::pair{std:("className"), std:("api-method get")}
        } }}
    } }}
};

void Main(void)
{
}

MAIN
