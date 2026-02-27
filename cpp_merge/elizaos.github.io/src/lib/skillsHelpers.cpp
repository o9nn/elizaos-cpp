#include "skillsHelpers.hpp"

object calculateLevelStats(double xp)
{
    if (xp < 0) {
        return object{
            object::pair{std:("level"), 1}, 
            object::pair{std:("xpToNextLevel"), Math->floor((1 / 4) * xpSum(1))}, 
            object::pair{std:("progress"), 0}
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
                object::pair{std:("level"), std:("level")}, 
                object::pair{std:("xpToNextLevel"), std:("xpToNextLevel")}, 
                object::pair{std:("progress"), std:("progress")}
            };
        }
        level++;
    }
    return object{
        object::pair{std:("level"), maxLevel}, 
        object::pair{std:("xpToNextLevel"), 0}, 
        object::pair{std:("progress"), 0}
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


