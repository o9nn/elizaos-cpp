#include "/home/runner/work/elizaos-cpp/elizaos-cpp/elizas-world/src/lib/cache.h"

any getCachedData()
{
    try
    {
        if (!fs->existsSync(CACHE_FILE)) {
            return nullptr;
        }
        auto data = JSON->parse(fs->readFileSync(CACHE_FILE, std::string("utf-8")));
        return object{
            object::pair{std::string("holdings"), data["holdings"]}, 
            object::pair{std::string("lastUpdated"), std::make_shared<Date>(data["lastUpdated"])}
        };
    }
    catch (const any& error)
    {
        console->error(std::string("Error reading cache:"), error);
        return nullptr;
    }
};


void setCachedData(array<std::shared_ptr<TokenHolding>> holdings)
{
    try
    {
        auto dir = path->dirname(CACHE_FILE);
        if (!fs->existsSync(dir)) {
            fs->mkdirSync(dir, object{
                object::pair{std::string("recursive"), true}
            });
        }
        fs->writeFileSync(CACHE_FILE, JSON->stringify(object{
            object::pair{std::string("holdings"), std::string("holdings")}, 
            object::pair{std::string("lastUpdated"), std::make_shared<Date>()}
        }));
    }
    catch (const any& error)
    {
        console->error(std::string("Error writing cache:"), error);
    }
};


boolean shouldRefreshCache()
{
    auto cached = getCachedData();
    if (!cached) return true;
    auto now = std::make_shared<Date>();
    return now->getTime() - cached["lastUpdated"]->getTime() > CACHE_DURATION;
};


any CACHE_FILE = path->join(process->cwd(), std::string("data"), std::string("cache.json"));
double CACHE_DURATION = 60 * 1000;

void Main(void)
{
}

MAIN
