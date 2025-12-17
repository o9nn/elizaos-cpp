#pragma once
#include <algorithm>
#include <any>
#include <cstdint>
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include "agent/problem-statement.hpp"
#include "environment/repo.hpp"
#include "environment/swe-env.hpp"
#include "types.hpp"
#include "utils/log.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use

/**
 * Hook for saving and applying patches
 */

/**
 * Check if patch is promising (likely to solve the issue)
 */
bool isPromisingPatch(AgentInfo info);

/**
 * Print patch message to console
 */
void printPatchMessage(const std::string& patchOutputFile);

/**
 * This hook saves patches to a separate directory and optionally applies them to a local repository
 */
class SaveApplyPatchHook {
public:
    SaveApplyPatchHook(bool applyPatchLocally = false, bool showSuccessMessage = true);
    void onInit(std::optional<std::any> run);
    void onInstanceStart(const std::any& params);
    void onInstanceCompleted(const std::any& params);
    std::string savePatch(const std::string& instanceId, AgentInfo info);
    void applyPatch(const std::string& patchFile, const std::string& localDir);

private:
    bool applyPatchLocally_;
    bool showSuccessMessage_;
};


} // namespace elizaos
