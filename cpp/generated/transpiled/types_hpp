#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_ELIZAOS_GITHUB_IO_SRC_LIB_PIPELINES_TYPES_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_ELIZAOS_GITHUB_IO_SRC_LIB_PIPELINES_TYPES_H
#include "core.h"
#include "./pipelineConfig.h"
#include "../logger.h"
#include "p-map.h"
using pMap = _default;

template <typename TInput, typename TOutput, typename TContext>
using PipelineStep = std::function<any(TInput, TContext)>;

template <typename T>
class PipelineResult;
class BasePipelineContext;
class RepoPipelineContext;

template <typename T>
class PipelineResult : public object, public std::enable_shared_from_this<PipelineResult<T>> {
public:
    using std::enable_shared_from_this<PipelineResult<T>>::shared_from_this;
    T data;
};

class BasePipelineContext : public object, public std::enable_shared_from_this<BasePipelineContext> {
public:
    using std::enable_shared_from_this<BasePipelineContext>::shared_from_this;
    std::shared_ptr<PipelineConfig> config;

    std::shared_ptr<Logger> logger;
};

class RepoPipelineContext : public BasePipelineContext, public std::enable_shared_from_this<RepoPipelineContext> {
public:
    using std::enable_shared_from_this<RepoPipelineContext>::shared_from_this;
    string repoId;

    object dateRange;
};

template <typename TContexttypename ...Args>
PipelineStep<any, any, TContext> pipe(Args... operations_);

template <typename TContexttypename ...Args>
PipelineStep<any, array<any>, TContext> parallel(Args... operations_);

template <typename TContexttypename ...Args>
PipelineStep<any, array<any>, TContext> sequence(Args... operations_);

template <typename TInput, typename TOutput, typename TContext>
PipelineStep<array<TInput>, array<TOutput>, TContext> mapStep(PipelineStep<TInput, TOutput, TContext> operation);

template <typename TInput, typename TOutput, typename TContext>
PipelineStep<TInput, TOutput, TContext> createStep(string name, std::function<any(TInput, TContext)> transform);

template <typename TContexttypename ...Args>
PipelineStep<any, any, TContext> pipe(Args... operations_)
{
    array<PipelineStep<any, any, TContext>> operations = array<PipelineStep<any, any, TContext>>{operations_...};
    return [=](auto input, auto context) mutable
    {
        auto lastResult = input;
        for (auto& operation : operations)
        {
            lastResult = std::async([=]() { operation(lastResult, context); });
        }
        return lastResult;
    };
};


template <typename TContexttypename ...Args>
PipelineStep<any, array<any>, TContext> parallel(Args... operations_)
{
    array<PipelineStep<any, any, TContext>> operations = array<PipelineStep<any, any, TContext>>{operations_...};
    return [=](auto input, auto context) mutable
    {
        return std::async([=]() { Promise->all(operations->map([=](auto operation) mutable
        {
            return operation(input, context);
        }
        )); });
    };
};


template <typename TContexttypename ...Args>
PipelineStep<any, array<any>, TContext> sequence(Args... operations_)
{
    array<PipelineStep<any, any, TContext>> operations = array<PipelineStep<any, any, TContext>>{operations_...};
    return [=](auto input, auto context) mutable
    {
        auto results = array<any>();
        for (auto& operation : operations)
        {
            results->push(std::async([=]() { operation(input, context); }));
        }
        return results;
    };
};


template <typename TInput, typename TOutput, typename TContext>
PipelineStep<array<TInput>, array<TOutput>, TContext> mapStep(PipelineStep<TInput, TOutput, TContext> operation)
{
    return [=](auto inputs, auto context) mutable
    {
        if (!Array->isArray(inputs)) {
            throw any(std::make_shared<Error>(std::string("mapStep requires an array input")));
        }
        auto results = std::async([=]() { pMap(inputs, [=](auto item) mutable
        {
            return operation(item, context);
        }
        , object{
            object::pair{std::string("concurrency"), 5}
        }); });
        return results;
    };
};


template <typename TInput, typename TOutput, typename TContext>
PipelineStep<TInput, TOutput, TContext> createStep(string name, std::function<any(TInput, TContext)> transform)
{
    return [=](auto input, auto context) mutable
    {
        context->logger->trace(std::string("Executing step: ") + name + string_empty);
        auto stepLogger = context->logger->child(name);
        auto output = std::async([=]() { transform(input, utils::assign(object{
            , 
            object::pair{std::string("logger"), stepLogger}
        }, context)); });
        context->logger->trace(std::string("Completed step: ") + name + string_empty);
        return output;
    };
};


#endif
