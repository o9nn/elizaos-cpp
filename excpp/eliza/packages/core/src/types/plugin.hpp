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

import type { Character } from './agent';
import type { Action, Evaluator, Provider } from './components';
import type { IDatabaseAdapter } from './database';
import type { EventHandler, EventPayloadMap } from './events';
import type { IAgentRuntime } from './runtime';
import type { Service } from './service';
import type { TestSuite } from './testing';

using Route = std::variant<{
  type: 'GET', 'POST', 'PUT', 'DELETE', 'STATIC'>;
  path: string;
  filePath?: string;
  public?: boolean;
  name?: string extends { public: true } ? string : string | undefined;
  handler?: (req: any, res: any, runtime: IAgentRuntime) => Promise<void>;
  isMultipart?: boolean; // Indicates if the route expects multipart/form-data (file uploads)
};

/**
 * Plugin for extending agent functionality
 */

using PluginEvents = std::vector<{
  [K in keyof EventPayloadMap]?: EventHandler<K>>;
} & {
  [key: string]: ((params: any) => Promise<any>)[];
};

struct Plugin {
    std::string name;
    std::string description;
    std::optional<(config: Record<string, string>, runtime: IAgentRuntime) => Promise<void>> init;
    std::optional<{ [key: string]: any }> config;
    std::optional<std::vector<(typeof Service)>> services;
    std::optional<{> componentTypes;
    std::string name;
    std::unordered_map<std::string, unknown> schema;
    std::optional<(data: any) => boolean> validator;
    std::optional<std::vector<Action>> actions;
    std::optional<std::vector<Provider>> providers;
    std::optional<std::vector<Evaluator>> evaluators;
    std::optional<IDatabaseAdapter> adapter;
    std::optional<{> models;
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
    std::optional<(runtime: IAgentRuntime) => Promise<void>> init;
    std::optional<std::vector<Plugin>> plugins;
    std::optional<std::vector<TestSuite | TestSuite>> tests;
};


struct Project {
    std::vector<ProjectAgent> agents;
};


} // namespace elizaos
