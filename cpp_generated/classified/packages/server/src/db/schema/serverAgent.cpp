#include "/home/runner/work/elizaos-cpp/elizaos-cpp/classified/packages/server/src/db/schema/serverAgent.h"

any serverAgentsTable = pgTable(std::string("server_agents"), object{
    object::pair{std::string("serverId"), uuid(std::string("server_id"))->notNull()->references([=]() mutable
    {
        return messageServerTable->id;
    }
    , object{
        object::pair{std::string("onDelete"), std::string("cascade")}
    })}, 
    object::pair{std::string("agentId"), uuid(std::string("agent_id"))->notNull()}
}, [=](auto table) mutable
{
    return (object{
        object::pair{std::string("pk"), primaryKey(object{
            object::pair{std::string("columns"), array<any>{ table["serverId"], table["agentId"] }}
        })}
    });
}
);

void Main(void)
{
}

MAIN
