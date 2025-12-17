#include "points.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

double calculatePoints(PointEvent evt) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    switch (evt.type) {
        // case "wallet_connected":
        return 50;
        // case "creator_token_bonds":
        return 50;
        // case "prebond_buy":
        return Math.min(evt.usdVolume * 0.6);
        // case "postbond_buy":
        return Math.min(evt.usdVolume * 0.02);
        // case "prebond_sell":
        return Math.min(evt.usdVolume * 0.1);
        // case "postbond_sell":
        return Math.min(evt.usdVolume * 0.01);
        // case "trade_volume_bonus":
        if (evt.usdVolume > 100) return 500;
        if (evt.usdVolume > 10) return 10;
        return 0;
        // case "successful_bond":
        return 500;
        // case "daily_holding":
        return Math.floor(evt.usdHeld / 100) * 1;
        // case "graduation_holding":
        return Math.floor(evt.heldAtGraduation / 100) * 1.5;
        // case "graduating_tx":
        return 1000;
        // case "owner_graduation":
        return 500;
        // case "referral":
        return 100;
        // case "daily_trading_streak":
        return Math.min(evt.days * 10, 280);
        // case "first_buyer":
        return 200;
        // default:
        return 0;
    }

}

std::future<void> awardUserPoints(const std::string& userAddress, PointEvent event, auto description) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto db = getDB();
    const auto now = new Date();
    const auto rawPoints = calculatePoints(event);
    const auto pointsToAdd = Math.round(rawPoints);

    if (pointsToAdd <= 0) return;

    // 1) Upsert into user_points
    //   await db
    //     .insert(userPoints)
    //     .values({
    //       id: uuidv4(),
    //       userAddress,
    //       eventType: event.type,
    //       pointsAwarded: pointsToAdd,
    //       description,
    //       timestamp: now,
    //     })
    //     .onConflictDoUpdate({
    //       target: [userPoints.userAddress, userPoints.eventType],
    //       set: {
    //          pointsAwarded: sql`${userPoints.pointsAwarded} + ${pointsToAdd}`,
    //         description,
    //         timestamp: now,
    //       },
    //     })
    //     .execute();
    // this needs to be moved to a new database/system
    //so we can keep logs of each event where a user was rewarded points {/* Malibu To do */}

    // 2) Update running total in users table
    const auto existing = db;
    .select();
    .from(users);
    .where(eq(users.address, userAddress));
    .limit(1);
    .execute();

    if (existing.length) {
        db;
        .update(users);
        .set({
            "points: sql" + users.points + " + " + pointsToAdd
            });
            .where(eq(users.address, userAddress));
            .execute();
            } else {
                db;
                .insert(users);
                .values([;
                {
                    address: userAddress,
                    name: nullptr,
                    points: pointsToAdd,
                    rewardPoints: 0,
                    createdAt: now,
                    },
                    ]);
                    .execute();
                }

}

std::future<void> awardGraduationPoints(const std::string& mint) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto db = getDB();
    const auto redisCache = createRedisCache();

    // Last swap user
    std::string lastSwapUser = nullptr;
    try {
        const auto listKey = "swapsList:" + mint;
        const auto [lastSwapString] = redisCache.lrange(listKey, 0, 0); // Get the first item (most recent);

        if (lastSwapString) {
            const auto lastSwapData = /* JSON.parse */ lastSwapString;
            lastSwapUser = lastSwapData.user;
        }
        } catch (redisError) {
            // Use logger if available, otherwise console.error
            console.error(
            "Failed to get last swap user from Redis for " + mint + ":"
            redisError,
            );
            // Continue without awarding points for last swap if Redis fails
        }

        if (lastSwapUser) {
            awardUserPoints(;
            lastSwapUser, // Use user fetched from Redis;
            { type: "graduating_tx" },
            "Graduating transaction bonus",
            );
        }

        // Owner graduation
        const auto tokenRecord = getToken(mint);
        const auto creator = tokenRecord.creator;
        if (creator) {
            awardUserPoints(;
            creator,
            { type: "owner_graduation" },
            "Owner graduation bonus",
            );
        }

        // Holding through graduation
        std::vector<std::any> holders = [];
        const auto holdersListKey = "holders:" + mint;
        try {
            const auto holdersString = redisCache.get(holdersListKey);
            if (holdersString) {
                holders = /* JSON.parse */ holdersString;
                } else {
                    // Use logger if available
                    console.log(
                    "No holders found in Redis for " + mint + " during graduation point calculation."
                    );
                }
                } catch (redisError) {
                    console.error(
                    "Failed to get holders from Redis for graduation points (" + mint + "):"
                    redisError,
                    );
                    // Continue without awarding holder points if Redis fails
                }

                const auto [priceRow] = db;
                .select({ tokenPriceUSD: tokens.tokenPriceUSD })
                .from(tokens);
                .where(eq(tokens.mint, mint));
                .limit(1);
                .execute();

                const auto priceAtGraduation = priceRow.tokenPriceUSD || 0;

                for (const auto& h : holders)
                    const auto usdHeld = (h.amount || 0) * priceAtGraduation;
                    awardUserPoints(;
                    h.address,
                    { type: "graduation_holding", heldAtGraduation: usdHeld },
                    "Holding through graduation: $" + std::to_string(usdHeld.toFixed(2))
                    );
                }

}

std::future<> distributeWeeklyPoints(auto weeklyPool, auto capPercent) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    distributed: number; unassigned: number
}

} // namespace elizaos
