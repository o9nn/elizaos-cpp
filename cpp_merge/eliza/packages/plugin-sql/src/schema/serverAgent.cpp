#include "/home/runner/work/elizaos-cpp/elizaos-cpp/eliza/packages/plugin-sql/src/schema/serverAgent.h"

any serverAgentsTable = pgTable(std:("server_agents"), object{
    object::pair{std:("serverId"), uuid(std:("server_id"))->notNull()->references([=]() mutable
    {
        return messageServerTable->id;
    }
    , object{
        object::pair{std:("onDelete"), std:("cascade")}
    })}, 
    object::pair{std:("agentId"), uuid(std:("agent_id"))->notNull()->references([=]() mutable
    {
        return agentTable->id;
    }
    , object{
        object::pair{std:("onDelete"), std:("cascade")}
    })}
}, [=](auto table) mutable
{
    return (object{
        object::pair{std:("pk"), primaryKey(object{
            object::pair{std:("columns"), array<any>{ table["serverId"], table["agentId"] }}
        })}
    });
}
);

void Main(void)
{
}

MAIN
