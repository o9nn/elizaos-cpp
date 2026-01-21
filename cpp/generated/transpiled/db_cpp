#include "/home/runner/work/elizaos-cpp/elizaos-cpp/elizaos.github.io/src/lib/data/db.h"

std::shared_ptr<Database::Database> sqlite;
any db = drizzle(sqlite, object{
    object::pair{std::string("schema"), std::string("schema")}
});

void Main(void)
{
    try
    {
        sqlite = std::make_shared<Database>(path->join(process->cwd(), std::string("data/db.sqlite")), object{});
        sqlite->pragma(std::string("journal_mode = WAL"));
    }
    catch (const any& error)
    {
        console->error(std::string("Failed to initialize database:"), error);
        process->exit(1);
    }
    process->on(std::string("exit"), [=]() mutable
    {
        sqlite->close();
    }
    );
}

MAIN
