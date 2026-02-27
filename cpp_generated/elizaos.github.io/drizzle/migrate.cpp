#include "/home/runner/work/elizaos-cpp/elizaos-cpp/elizaos.github.io/drizzle/migrate.h"

std::function<void(any, any, any)> customMigrate = [=](auto db, auto config, auto maxMigrationNumber) mutable
{
    auto journalPath = path->join(config["migrationsFolder"], std::string("meta/_journal.json"));
    if (!fs->existsSync(journalPath)) {
        throw any(std::make_shared<Error>(std::string("Can't find meta/_journal.json file at ") + journalPath + string_empty));
    }
    auto journalAsString = fs->readFileSync(journalPath)->toString();
    auto journal = JSON->parse(journalAsString);
    auto journalEntries = journal["entries"];
    if (maxMigrationNumber != undefined) {
        console->log(std::string("Filtering migrations up to number: ") + maxMigrationNumber + string_empty);
        journalEntries = journal["entries"]["filter"]([=](auto entry) mutable
        {
            return entry["idx"] <= maxMigrationNumber;
        }
        );
    }
    console->log(std::string("Found ") + journalEntries["length"] + std::string(" migrations to apply."));
    if (journalEntries["length"] == 0) {
        console->log(std::string("No migrations to apply."));
        return;
    }
    auto migrationQueries = array<std::shared_ptr<MigrationMeta>>();
    for (auto& journalEntry : journalEntries)
    {
        auto migrationPath = path->join(config["migrationsFolder"], string_empty + journalEntry["tag"] + std::string(".sql"));
        try
        {
            auto query = fs->readFileSync(migrationPath)->toString();
            auto result = query->split(std::string("--> statement-breakpoint"))->map([=](auto it) mutable
            {
                return it->trim();
            }
            )->filter([=](auto it) mutable
            {
                return it->length > 0;
            }
            );
            migrationQueries->push(object{
                object::pair{std::string("sql"), result}, 
                object::pair{std::string("bps"), journalEntry["breakpoints"]}, 
                object::pair{std::string("folderMillis"), journalEntry["when"]}, 
                object::pair{std::string("hash"), crypto->createHash(std::string("sha256"))->update(query)->digest(std::string("hex"))}
            });
        }
        catch (const any& e)
        {
            throw any(std::make_shared<Error>(std::string("Error reading migration file ") + migrationPath + std::string(": ") + e["message"] + string_empty));
        }
    }
    db["dialect"]["migrate"](migrationQueries, db["session"], config);
};
string dbPath = OR((process->env->DB_PATH), (std::string("./data/db.sqlite")));
any dbDir = path->dirname(dbPath);
any sqlite = std::make_shared<Database>(dbPath, object{
    object::pair{std::string("create"), true}
});
any db = drizzle(sqlite);
string maxMigrationArg = const_(process->argv)[2];
any maxMigration;

void Main(void)
{
    console->log(std::string("Using database at: ") + dbPath + string_empty);
    if (!fs->existsSync(dbDir)) {
        fs->mkdirSync(dbDir, object{
            object::pair{std::string("recursive"), true}
        });
    }
    if (maxMigrationArg) {
        double parsed = parseInt(maxMigrationArg, 10);
        if (isNaN(parsed)) {
            console->error(std::string("Error: Invalid migration number provided: "") + maxMigrationArg + std::string("". Must be an integer."));
            process->exit(1);
        }
        maxMigration = parsed;
    }
    try
    {
        customMigrate(db, object{
            object::pair{std::string("migrationsFolder"), std::string("./drizzle")}
        }, maxMigration);
        console->log(std::string("Migration complete"));
    }
    catch (const any& error)
    {
        console->error(std::string("Migration failed:"), error);
        process->exit(1);
    }
}

MAIN
