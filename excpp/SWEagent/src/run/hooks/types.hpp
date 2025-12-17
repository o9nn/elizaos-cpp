#pragma once
#include <any>
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>
#include "..agent/problem-statement.hpp"
#include "..environment/swe-env.hpp"
#include "..types.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use

/**
 * Type definitions for run hooks
 */

/**
 * Hook structure for the web server or other addons to interface with
 */
struct RunHook {
    double index;
    SWEEnv env;
    std::variant<ProblemStatement, ProblemStatementConfig> problemStatement;
};

/**
 * Abstract base class for run hooks
 */
    void onInit(const std::unordered_map<std::string, unknown>& _run);
    void onStart();
    void onEnd();
    void onInstanceStart(const std::any& _params);
    void onInstanceSkipped();
    void onInstanceCompleted(const std::any& _params);

/**
 * Combined run hooks manager
 */
class CombinedRunHooks {
public:
    void addHook(RunHook hook);
    std::vector<RunHook> hooks() const;
    void onInit(const std::unordered_map<std::string, unknown>& run);
    void onStart();
    void onEnd();
    void onInstanceStart(const std::any& params);
    void onInstanceSkipped();
    void onInstanceCompleted(const std::any& params);
};


} // namespace elizaos
