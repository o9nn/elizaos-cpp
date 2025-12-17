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
#include <variant>
#include <vector>

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use


using Route = std::variant<{

/**
 * Plugin for extending agent functionality
 */

using PluginEvents = std::vector<{

struct Plugin {
    std::string name;
    std::string description;
    std::string name;
    std::optional<std::vector<Action>> actions;
    std::optional<std::vector<Provider>> providers;
    std::optional<std::vector<Evaluator>> evaluators;
    std::optional<IDatabaseAdapter> adapter;
    std::optional<PluginEvents> events;
    std::optional<std::vector<Route>> routes;
    std::optional<std::vector<TestSuite>> tests;
    std::optional<std::vector<std::string>> dependencies;
    std::optional<std::vector<std::string>> testDependencies;
    std::optional<double> priority;
    std::optional<std::any> schema;
};

struct ProjectAgent {
    Character character;
    std::optional<std::vector<Plugin>> plugins;
    std::optional<std::variant<TestSuite, std::vector<TestSuite>>> tests;
};

struct Project {
    std::vector<ProjectAgent> agents;
};


} // namespace elizaos
