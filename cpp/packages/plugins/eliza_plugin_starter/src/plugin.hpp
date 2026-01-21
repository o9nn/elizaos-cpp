#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_ELIZA_PACKAGES_CORE_SRC_TYPES_PLUGIN_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_ELIZA_PACKAGES_CORE_SRC_TYPES_PLUGIN_H
#include "core.h"
#include "./agent.h"
#include "./components.h"
#include "./database.h"
#include "./events.h"
#include "./runtime.h"
#include "./service.h"
#include "./testing.h"

typedef object Route;

typedef any PluginEvents;

class Plugin;
class ProjectAgent;
class Project;

class Plugin : public object, public std::enable_shared_from_this<Plugin> {
public:
    using std::enable_shared_from_this<Plugin>::shared_from_this;
    string name;

    string description;

    std::function<std::shared_ptr<Promise<void>>(Record<string, string>, std::shared_ptr<IAgentRuntime>)> init;

    object config;

    array<any> services;

    array<object> componentTypes;

    array<std::shared_ptr<Action>> actions;

    array<std::shared_ptr<Provider>> providers;

    array<std::shared_ptr<Evaluator>> evaluators;

    std::shared_ptr<IDatabaseAdapter> adapter;

    object models;

    PluginEvents events;

    array<Route> routes;

    array<std::shared_ptr<TestSuite>> tests;

    array<string> dependencies;

    array<string> testDependencies;

    double priority;

    any schema;
};

class ProjectAgent : public object, public std::enable_shared_from_this<ProjectAgent> {
public:
    using std::enable_shared_from_this<ProjectAgent>::shared_from_this;
    std::shared_ptr<Character> character;

    std::function<std::shared_ptr<Promise<void>>(std::shared_ptr<IAgentRuntime>)> init;

    array<std::shared_ptr<Plugin>> plugins;

    any tests;
};

class Project : public object, public std::enable_shared_from_this<Project> {
public:
    using std::enable_shared_from_this<Project>::shared_from_this;
    array<std::shared_ptr<ProjectAgent>> agents;
};

#endif
