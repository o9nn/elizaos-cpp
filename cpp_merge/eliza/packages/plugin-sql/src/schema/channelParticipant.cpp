#include "/home/runner/work/elizaos-cpp/elizaos-cpp/eliza/packages/plugin-sql/src/schema/channelParticipant.h"

any channelParticipantsTable = pgTable(std:("channel_participants"), object{
    object::pair{std:("channelId"), text(std:("channel_id"))->notNull()->references([=]() mutable
    {
        return channelTable->id;
    }
    , object{
        object::pair{std:("onDelete"), std:("cascade")}
    })}, 
    object::pair{std:("userId"), text(std:("user_id"))->notNull()}
}, [=](auto table) mutable
{
    return (object{
        object::pair{std:("pk"), primaryKey(object{
            object::pair{std:("columns"), array<any>{ table["channelId"], table["userId"] }}
        })}
    });
}
);

void Main(void)
{
}

MAIN
