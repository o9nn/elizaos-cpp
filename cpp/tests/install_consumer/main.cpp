#include <elizaos/cognitive_bridge.hpp>
#include <elizaos/cognitive_curriculum.hpp>
#include <elizaos/llama_cpp.hpp>
#include <elizaos/persistence.hpp>

#include <iostream>
#include <stdexcept>

int main() {
    elizaos::CognitiveCurriculum curriculum;
    if (curriculum.propertyCount() != 15U) {
        throw std::runtime_error("cognitive curriculum contract failed");
    }
    if (elizaos::llama::utils::quantizationToString(
            elizaos::llama::QuantizationType::Q4_0).empty()) {
        throw std::runtime_error("llama integration linkage failed");
    }

    elizaos::CognitiveBridge bridge("installed-consumer");
    if (bridge.id() != "installed-consumer") {
        throw std::runtime_error("cognitive bridge linkage failed");
    }

    elizaos::SQLiteBackend database(elizaos::StorageConfig::inMemory());
    if (!database.connect()) {
        throw std::runtime_error("persistence connect failed");
    }
    const auto result = database.execute(
        "CREATE TABLE consumer_probe (id INTEGER PRIMARY KEY, value TEXT)");
    if (!result.success || !database.tableExists("consumer_probe")) {
        throw std::runtime_error("persistence execution failed");
    }
    if (!database.disconnect()) {
        throw std::runtime_error("persistence disconnect failed");
    }

    std::cout << "installed ElizaOS consumer passed\n";
    return 0;
}
