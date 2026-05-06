#ifndef _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_CLASSIFIED_PACKAGES_CORE_SRC_TYPES_COMPONENTS_H
#define _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_CLASSIFIED_PACKAGES_CORE_SRC_TYPES_COMPONENTS_H
#include "core.hpp"
#include "./memory.h"
#include "./primitives.h"
#include "./runtime.h"
#include "./state.h"

typedef std::function<std::shared_ptr<Promise<array<std::shared_ptr<Memory>>>>(std::shared_ptr<Content>, any)> HandlerCallback;

typedef std::function<std::shared_ptr<Promise<any>>(std::shared_ptr<IAgentRuntime>, std::shared_ptr<Memory>, std::shared_ptr<State>, object, HandlerCallback, array<std::shared_ptr<Memory>>)> Handler;

typedef std::function<std::shared_ptr<Promise<boolean>>(std::shared_ptr<IAgentRuntime>, std::shared_ptr<Memory>, std::shared_ptr<State>)> Validator;

class ActionExample;
class Action;
class EvaluationExample;
class Evaluator;
class ProviderResult;
class Provider;
class ActionResult;
class ActionContext;

class ActionExample : public object, public std::enable_shared_from_this<ActionExample> {
public:
    using std::enable_shared_from_this<ActionExample>::shared_from_this;
    string name;

    std::shared_ptr<Content> content;
};

class Action : public object, public std::enable_shared_from_this<Action> {
public:
    using std::enable_shared_from_this<Action>::shared_from_this;
    array<string> similes;

    string description;

    array<array<std::shared_ptr<ActionExample>>> examples;

    Handler handler;

    string name;

    Validator validate;
};

class EvaluationExample : public object, public std::enable_shared_from_this<EvaluationExample> {
public:
    using std::enable_shared_from_this<EvaluationExample>::shared_from_this;
    string prompt;

    array<std::shared_ptr<ActionExample>> messages;

    string outcome;
};

class Evaluator : public object, public std::enable_shared_from_this<Evaluator> {
public:
    using std::enable_shared_from_this<Evaluator>::shared_from_this;
    boolean alwaysRun;

    string description;

    array<string> similes;

    array<std::shared_ptr<EvaluationExample>> examples;

    Handler handler;

    string name;

    Validator validate;
};

class ProviderResult : public object, public std::enable_shared_from_this<ProviderResult> {
public:
    using std::enable_shared_from_this<ProviderResult>::shared_from_this;
    object values;

    object data;

    string text;
};

class Provider : public object, public std::enable_shared_from_this<Provider> {
public:
    using std::enable_shared_from_this<Provider>::shared_from_this;
    string name;

    string description;

    boolean dynamic;

    double position;

    boolean private;

    std::function<std::shared_ptr<Promise<std::shared_ptr<ProviderResult>>>(std::shared_ptr<IAgentRuntime>, std::shared_ptr<Memory>, std::shared_ptr<State>)> get;
};

class ActionResult : public object, public std::enable_shared_from_this<ActionResult> {
public:
    using std::enable_shared_from_this<ActionResult>::shared_from_this;
    string text;

    Record<string, any> values;

    Record<string, any> data;

    boolean success;

    any error;
};

class ActionContext : public object, public std::enable_shared_from_this<ActionContext> {
public:
    using std::enable_shared_from_this<ActionContext>::shared_from_this;
    array<std::shared_ptr<ActionResult>> previousResults;

    std::function<any(string)> getPreviousResult;
};

std::shared_ptr<ActionResult> createActionResult(Partial<std::shared_ptr<ActionResult>> partial = object{});

#endif
