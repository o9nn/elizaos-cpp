#include "cron.hpp"
#include "db.hpp"
#include "logger.hpp"
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#pragma once

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use

;
;
;
;
;
;

std::future<number | null> getLastProcessedSlot();
   return null;
}
std::future<void> setLastProcessedSlot(double slot);).onConflictDoUpdate({
      target: [metadata.key],
      set: { value: slot.toString() },
   });
   logger.log(`Updated last processed slot to ${slot}`);
}

std::future<double> findSlotAtOrBeforeTime(Connection connection, double targetTs, double low, double high); else {
         // mid is too old or just right, keep it
         low = mid;
      }
   }
   return low;
}
async );
      if (!block) return logger.log(`Slot ${slot} empty, skipping`);

      for (const tx of block.transactions) {
         const logs = tx.meta?.logMessages;
         if (!logs) continue;
         if (logs.some((l) => l.includes(process.env.PROGRAM_ID!))) {
            const signature = tx.transaction.signatures[0];
            await processTransactionLogs(logs, signature);
         }
      }
   } catch (err) {
      logger.error(`Error processing slot ${slot}:`, err);
   }
}

// Scan blocks from the last processed slot up to the current slot
std::future<void> processMissedEvents(Connection connection); else {
            startSlot = Math.max(0, currentSlot - 500);
         }
         logger.log(`No lastProcessedSlot found. Falling back to slot ${startSlot}`);
      } else {
         logger.log(`Resuming from lastProcessedSlot = ${startSlot}`);
      }
      const slots = await connection.getBlocks(startSlot + 1, currentSlot);
      logger.log(
         `Processing ${slots.length} slots from ${startSlot + 1} to ${currentSlot}`
      );
      // 2) Now process every slot from startSlot to currentSlot
      logger.log(`Scanning events from slot ${startSlot + 1} to ${currentSlot}`);
      const queue = new PQueue({ concurrency: 20 });
      for (const slot of slots) {
         queue.add(() => processSlot(slot, connection));
      }
      await queue.onIdle();
      await setLastProcessedSlot(currentSlot);
      logger.log(`✅ Updated lastProcessedSlot → ${currentSlot}`);
   } catch (error) {
      logger.error("Error processing missed events:", error);
   }
}

} // namespace elizaos
