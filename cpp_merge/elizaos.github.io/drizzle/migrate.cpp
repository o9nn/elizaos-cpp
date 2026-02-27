#include "/home/runner/work/elizaos-cpp/elizaos-cpp/elizaos.github.io/drizzle/migrate.h"

std::function<void(any, any, any)> customMigrate = [=](auto db, auto config, auto maxMigrationNumber) mutable
{
    auto journalPath = path->join(config["migrationsFolder"], std:("meta/_journal.json"));
    if (!fs->existsSync(journalPath)) {
        throw any(std::make_shared<Error>(std:("Can't find meta/_journal.json file at ") + journalPath + string_empty));
    }
    auto journalAsString = fs->readFileSync(journalPath)->toString();
    auto journal = JSON->parse(journalAsString);
    auto journalEntries = journal["entries"];
    if (maxMigrationNumber != undefined) {
        console->log(std:("Filtering migrations up to number: ") + maxMigrationNumber + string_empty);
        journalEntries = journal["entries"]["filter"]([=](auto entry) mutable
        {
            return entry["idx"] <= maxMigrationNumber;
        }
        );
    }
    console->log(std:("Found ") + journalEntries["length"] + std:(" migrations to apply."));
    if (journalEntries["length"] == 0) {
        console->log(std:("No migrations to apply."));
        return;
    }
    auto migrationQueries = array<std::shared_ptr<MigrationMeta>>();
    for (auto& journalEntry : journalEntries)
    {
        auto migrationPath = path->join(config["migrationsFolder"], string_empty + journalEntry["tag"] + std:(".sql"));
        try
        {
            auto query = fs->readFileSync(migrationPath)->toString();
            auto result = query->split(std:("--> statement-breakpoint"))->map([=](auto it) mutable
            {
                return it->trim();
            }
            )->filter([=](auto it) mutable
            {
                return it->length > 0;
            }
            );
            migrationQueries->push(object{
                object::pair{std:("sql"), result}, 
                object::pair{std:("bps"), journalEntry["breakpoints"]}, 
                object::pair{std:("folderMillis"), journalEntry["when"]}, 
                object::pair{std:("hash"), crypto->createHash(std:("sha256"))->update(query)->digest(std:("hex"))}
            });
        }
        catch (const any& e)
        {
            throw any(std::make_shared<Error>(std:("Error reading migration file ") + migrationPath + std:(": ") + e["message"] + string_empty));
        }
    }
    db["dialect"]["migrate"](migrationQueries, db["session"], config);
};
string dbPath = OR((process->env->DB_PATH), (std:("./data/db.sqlite")));
any dbDir = path->dirname(dbPath);
any sqlite = std::make_shared<Database>(dbPath, object{
    object::pair{std:("create"), true}
});
any db = drizzle(sqlite);
string maxMigrationArg = const_(process->argv)[2];
any maxMigration;

void Main(void)
{
    console->log(std:("Using database at: ") + dbPath + string_empty);
    if (!fs->existsSync(dbDir)) {
        fs->mkdirSync(dbDir, object{
            object::pair{std:("recursive"), true}
        });
    }
    if (maxMigrationArg) {
        double parsed = parseInt(maxMigrationArg, 10);
        if (isNaN(parsed)) {
            console->error(std:("Error: Invalid migration number provided: "") + maxMigrationArg + std:("". Must be an integer."));
            process->exit(1);
        }
        maxMigration = parsed;
    }
    try
    {
        customMigrate(db, object{
            object::pair{std:("migrationsFolder"), std:("./drizzle")}
        }, maxMigration);
        console->log(std:("Migration complete"));
    }
    catch (const any& error)
    {
        console->error(std:("Migration failed:"), error);
        process->exit(1);
    }
}

MAIN
