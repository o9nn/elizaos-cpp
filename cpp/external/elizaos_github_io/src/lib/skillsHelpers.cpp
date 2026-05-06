#include "skillsHelpers.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

void calculateLevelStats(double xp) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    level: number;
    xpToNextLevel: number;
    progress: number;

}

double xpSum(double level) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    auto total = 0;
    for (int i = 1; i <= level; i++) {
        total += Math.floor(i + 150 * Math.pow(2, i / 10));
    }
    return total;

}

double xpForLevel(double level) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    if (level <= 1) return 0;
    if (level > 99) level = 99;

    return Math.floor((1 / 4) * xpSum(level - 1));

}

} // namespace elizaos
