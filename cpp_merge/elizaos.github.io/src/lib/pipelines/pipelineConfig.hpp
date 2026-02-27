#ifndef _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_ELIZAOS_GITHUB_IO_SRC_LIB_PIPELINES_PIPELINECONFIG_H
#define _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_ELIZAOS_GITHUB_IO_SRC_LIB_PIPELINES_PIPELINECONFIG_H
#include "core.hpp"
#include "zod.hpp"
#include "./summarize/config.h"

typedef z::infer<PipelineConfigSchema> PipelineConfig;

typedef z::infer<ScoringConfigSchema> ScoringConfig;

typedef z::infer<TagConfigSchema> TagConfig;

typedef z::infer<RepositoryConfigSchema> RepositoryConfig;

typedef ScoringConfig ScoringRules;


extern any ScoringConfigSchema;
extern any TagTypeSchema;
extern any TagConfigSchema;
extern any RepositoryConfigSchema;
extern any PipelineConfigSchema;
#endif
