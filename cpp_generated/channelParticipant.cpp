#include "/home/runner/work/elizaos-cpp/elizaos-cpp/eliza/packages/plugin-sql/src/schema/channelParticipant.h"

any channelParticipantsTable = pgTable(std::string("channel_participants"), object{
    object::pair{std::string("channelId"), text(std::string("channel_id"))->notNull()->references([=]() mutable
    {
        return channelTable->id;
    }
    , object{
        object::pair{std::string("onDelete"), std::string("cascade")}
    })}, 
    object::pair{std::string("userId"), text(std::string("user_id"))->notNull()}
}, [=](auto table) mutable
{
    return (object{
        object::pair{std::string("pk"), primaryKey(object{
            object::pair{std::string("columns"), array<any>{ table["channelId"], table["userId"] }}
        })}
    });
}
);

void Main(void)
{
}

MAIN
