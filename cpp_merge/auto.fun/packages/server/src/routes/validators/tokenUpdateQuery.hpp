#ifndef _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_AUTO_FUN_PACKAGES_SERVER_SRC_ROUTES_VALIDATORS_TOKENUPDATEQUERY_H
#define _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_AUTO_FUN_PACKAGES_SERVER_SRC_ROUTES_VALIDATORS_TOKENUPDATEQUERY_H
#include "core.hpp"
#include "./global.h"
#include "zod.hpp"

typedef z::infer<updateTokenBodySchema> UpdateTokenBody;


extern any updateTokenBodySchema;
object parseUpdateTokenRequest(object raw);

#endif
