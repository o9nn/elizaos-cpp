#pragma once
#include <algorithm>
#include <any>
#include <cstdint>
#include <functional>
#include <future>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include "elizaos/core.hpp"
#include "execution/buyService.hpp"
#include "execution/sellService.hpp"
#include "execution/tradeExecutionService.hpp"
#include "types.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



class TaskService {
public:
    TaskService();
    std::future<void> registerTasks();
    std::future<void> stop();
    void registerSellTasks();
    void createSellTask(SellSignalMessage signal);
    void executeSellTask(const std::any& options);
};


} // namespace elizaos
