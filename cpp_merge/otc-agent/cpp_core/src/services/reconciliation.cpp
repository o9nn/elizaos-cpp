#include "reconciliation.hpp"
#include <future>
#include <iostream>
#include <stdexcept>

namespace elizaos {

ReconciliationService getReconciliationService() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    if (!reconciliationServiceInstance) {
        reconciliationServiceInstance = std::make_unique<ReconciliationService>();
    }
    return reconciliationServiceInstance;

}

std::future<void> runReconciliationTask() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    std::cout << "\n🔄 [Reconciliation Task] Starting...\n" << std::endl;

    const auto service = getReconciliationService();
    const auto health = service.healthCheck();
    console.log(
    "[Reconciliation] Block: " + health.blockNumber + ", Contract: " + health.contractAddress + "\n"
    );

    const auto result = service.reconcileAllActive();
    console.log(
    "\n✅ [Reconciliation] Complete: " + result.updated + "/" + result.total + " updated\n"
    );

}

} // namespace elizaos
