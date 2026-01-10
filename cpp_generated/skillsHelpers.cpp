#include "/home/runner/work/elizaos-cpp/elizaos-cpp/elizaos.github.io/src/lib/skillsHelpers.h"

object calculateLevelStats(double xp)
{
    if (xp < 0) {
        return object{
            object::pair{std::string("level"), 1}, 
            object::pair{std::string("xpToNextLevel"), Math->floor((1 / 4) * xpSum(1))}, 
            object::pair{std::string("progress"), 0}
        };
    }
    auto level = 1;
    auto maxLevel = 99;
    while (level < maxLevel)
    {
        auto nextLevelXP = Math->floor((1 / 4) * xpSum(level));
        if (xp < nextLevelXP) {
            auto xpToNextLevel = nextLevelXP - xp;
            auto currentLevelXP = Math->floor((1 / 4) * xpSum(level - 1));
            auto progress = (xp - currentLevelXP) / (nextLevelXP - currentLevelXP);
            return object{
                object::pair{std::string("level"), std::string("level")}, 
                object::pair{std::string("xpToNextLevel"), std::string("xpToNextLevel")}, 
                object::pair{std::string("progress"), std::string("progress")}
            };
        }
        level++;
    }
    return object{
        object::pair{std::string("level"), maxLevel}, 
        object::pair{std::string("xpToNextLevel"), 0}, 
        object::pair{std::string("progress"), 0}
    };
};


double xpSum(double level)
{
    auto total = 0;
    for (auto i = 1; i <= level; i++)
    {
        total += Math->floor(i + 150 * Math->pow(2, i / 10));
    }
    return total;
};


double xpForLevel(double level)
{
    if (level <= 1) return 0;
    if (level > 99) level = 99;
    return Math->floor((1 / 4) * xpSum(level - 1));
};


