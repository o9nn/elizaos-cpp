#ifndef _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_AUTO_FUN_PACKAGES_SERVER_SRC_ROUTES_VALIDATORS_TOKENQUERY_H
#define _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_AUTO_FUN_PACKAGES_SERVER_SRC_ROUTES_VALIDATORS_TOKENQUERY_H
#include "core.hpp"
#include "./global.h"

std::shared_ptr<TokenQueryParams> parseTokensQuery(std::shared_ptr<RawTokenQuery> raw);

#endif
