#pragma once
#include <any>
#include <functional>
#include <future>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#include ".types.hpp"
#include "elizaos/core.hpp"
#include "execution/buyService.hpp"
#include "execution/sellService.hpp"
#include "execution/tradeExecutionService.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



class TaskService {
public:
    TaskService(IAgentRuntime protected override runtime, BuyService private buyService, SellService private sellService);
    std::future<void> registerTasks();
    std::future<void> stop();
    void registerSellTasks();
    void createSellTask(SellSignalMessage signal);
    void executeSellTask(const std::any& options);
};


} // namespace elizaos
