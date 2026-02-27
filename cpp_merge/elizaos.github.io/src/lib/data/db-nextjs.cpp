#include "db-nextjs.h"

std::shared_ptr<Database::Database> sqlite;
any db = drizzle(sqlite, object{
    object::pair{std:("schema"), std:("schema")}
});

void Main(void)
{
    try
    {
        sqlite = std::make_shared<Database>(path->join(process->cwd(), std:("data/db.sqlite")), object{
            object::pair{std:("fileMustExist"), false}
        });
        sqlite->pragma(std:("journal_mode = WAL"));
    }
    catch (const any& error)
    {
        console->error(std:("Failed to initialize database:"), error);
        process->exit(1);
    }
    process->on(std:("exit"), [=]() mutable
    {
        sqlite->close();
    }
    );
}

MAIN
