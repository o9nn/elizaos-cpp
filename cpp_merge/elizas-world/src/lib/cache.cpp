#include "cache.hpp"

any getCachedData()
{
    try
    {
        if (!fs->existsSync(CACHE_FILE)) {
            return nullptr;
        }
        auto data = JSON->parse(fs->readFileSync(CACHE_FILE, std:("utf-8")));
        return object{
            object::pair{std:("holdings"), data["holdings"]}, 
            object::pair{std:("lastUpdated"), std::make_shared<Date>(data["lastUpdated"])}
        };
    }
    catch (const any& error)
    {
        console->error(std:("Error reading cache:"), error);
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
                object::pair{std:("recursive"), true}
            });
        }
        fs->writeFileSync(CACHE_FILE, JSON->stringify(object{
            object::pair{std:("holdings"), std:("holdings")}, 
            object::pair{std:("lastUpdated"), std::make_shared<Date>()}
        }));
    }
    catch (const any& error)
    {
        console->error(std:("Error writing cache:"), error);
    }
};


boolean shouldRefreshCache()
{
    auto cached = getCachedData();
    if (!cached) return true;
    auto now = std::make_shared<Date>();
    return now->getTime() - cached["lastUpdated"]->getTime() > CACHE_DURATION;
};


any CACHE_FILE = path->join(process->cwd(), std:("data"), std:("cache.json"));
double CACHE_DURATION = 60 * 1000;

void Main(void)
{
}

MAIN
