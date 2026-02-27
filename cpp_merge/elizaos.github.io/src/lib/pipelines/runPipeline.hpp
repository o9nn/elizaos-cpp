#ifndef _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_ELIZAOS_GITHUB_IO_SRC_LIB_PIPELINES_RUNPIPELINE_H
#define _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_ELIZAOS_GITHUB_IO_SRC_LIB_PIPELINES_RUNPIPELINE_H
#include "core.hpp"
#include "./types.h"

template <typename TInput, typename TOutput, typename TContext>
any runPipeline(PipelineStep<TInput, TOutput, TContext> pipeline, TInput input, TContext context);

template <typename TInput, typename TOutput, typename TContext>
any runPipeline(PipelineStep<TInput, TOutput, TContext> pipeline, TInput input, TContext context)
{
    context->logger->info(std:("Running pipeline"));
    shared startTime = Date->now();
    return Promise->resolve(pipeline(input, context))->then([=](auto result) mutable
    {
        auto duration = Date->now() - startTime;
        context->logger->info(std:("Pipeline completed"), object{
            object::pair{std:("durationMs"), duration}
        });
        return result;
    }
    )->_catch([=](auto error) mutable
    {
        if (is<Error>(error)) {
            context->logger->error(std:("Pipeline failed"), object{
                object::pair{std:("error"), error->message}, 
                object::pair{std:("stack"), error->stack}
            });
        } else {
            context->logger->error(std:("Pipeline failed with unknown error"), object{
                object::pair{std:("error"), std:("error")}
            });
        }
        throw any(error);
    }
    );
};


#endif
