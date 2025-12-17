#include "reconciliation.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

ReconciliationService getReconciliationService() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    if (!reconciliationServiceInstance) {
        reconciliationServiceInstance = new ReconciliationService();
    }
    return reconciliationServiceInstance;

}

std::future<void> runReconciliationTask() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    std::cout << "\n🔄 [Reconciliation Task] Starting...\n" << std::endl;

    const auto service = getReconciliationService();
    const auto health = service.healthCheck();
    console.log(
    "[Reconciliation] Block: " + std::to_string(health.blockNumber) + ", Contract: " + std::to_string(health.contractAddress) + "\n"
    );

    const auto result = service.reconcileAllActive();
    console.log(
    "\n✅ [Reconciliation] Complete: " + std::to_string(result.updated) + "/" + std::to_string(result.total) + " updated\n"
    );

}

} // namespace elizaos
