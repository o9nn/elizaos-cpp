#include "getAllTokens.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::future<std::optional<double>> getLastProcessedSlot() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto db = getDB();

    const auto row = db.select();
    .from(metadata);
    .where(eq(metadata.key, "lastProcessedSlot"));
    .limit(1);

    if (row.length > 0) {
        return parseInt(row[0].value, 10);
    }
    return nullptr;

}

std::future<void> setLastProcessedSlot(double slot) {
    // NOTE: Auto-converted from TypeScript - may need refinement



    const auto db = getDB();
    db.insert(metadata).values({ key: "lastProcessedSlot", value: std::to_string(slot) }).onConflictDoUpdate({
        target: [metadata.key],
        set: { value: std::to_string(slot) },
        });
        std::cout << "Updated last processed slot to " + slot << std::endl;

}

std::future<double> findSlotAtOrBeforeTime(Connection connection, double targetTs, double low, double high) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    while (low < high) {
        const auto mid = Math.floor((low + high + 1) / 2);  // bias upwards;
        const auto t = connection.getBlockTime(mid);
        if (t == null || t > targetTs) {
            // Too new (or missing), search lower
            high = mid - 1;
            } else {
                // mid is too old or just right, keep it
                low = mid;
            }
        }
        return low;

}

std::future<void> processSlot(double slot, Connection connection) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    try {
        const auto block = connection.getBlock(slot, {;
            transactionDetails: "full",
            rewards: false,
            commitment: "confirmed",
            maxSupportedTransactionVersion: 0,
            });
            if (!block) return logger.log(`Slot ${slot} empty, skipping`);

            for (const auto& tx : block.transactions)
                const auto logs = tx.meta.logMessages;
                if (!logs) continue;
                if (logs.some((l) => l.includes(process.env.PROGRAM_ID!))) {
                    const auto signature = tx.transaction.signatures[0];
                    processTransactionLogs(logs, signature);
                }
            }
            } catch (err) {
                std::cerr << "Error processing slot " + slot + ":" << err << std::endl;
            }

}

std::future<void> processMissedEvents(Connection connection) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    try {
        const auto currentSlot = connection.getSlot("confirmed");
        const auto currentTime = connection.getBlockTime(currentSlot);
        auto startSlot = getLastProcessedSlot();

        if (startSlot == null) {
            // First time running: fall back to ~6 h ago
            const auto currentTime = connection.getBlockTime(currentSlot);
            if (currentTime != null) {
                const auto cutoffTs = currentTime - 6 * 3600; // 6 hours;
                startSlot = findSlotAtOrBeforeTime(;
                connection,
                cutoffTs,
                0,
                currentSlot;
                );
                } else {
                    startSlot = Math.max(0, currentSlot - 500);
                }
                std::cout << "No lastProcessedSlot found. Falling back to slot " + startSlot << std::endl;
                } else {
                    std::cout << "Resuming from lastProcessedSlot = " + startSlot << std::endl;
                }
                const auto slots = connection.getBlocks(startSlot + 1, currentSlot);
                logger.log(
                "Processing " + slots.size() + " slots from " + std::to_string(startSlot + 1) + " to " + currentSlot;
                );
                // 2) Now process every slot from startSlot to currentSlot
                std::cout << "Scanning events from slot " + std::to_string(startSlot + 1) + " to " + currentSlot << std::endl;
                const auto queue = new PQueue({ concurrency: 20 });
                for (const auto& slot : slots)
                    queue.add(() => processSlot(slot, connection));
                }
                queue.onIdle();
                setLastProcessedSlot(currentSlot);
                std::cout << "✅ Updated lastProcessedSlot → " + currentSlot << std::endl;
                } catch (error) {
                    std::cerr << "Error processing missed events:" << error << std::endl;
                }

}

} // namespace elizaos
