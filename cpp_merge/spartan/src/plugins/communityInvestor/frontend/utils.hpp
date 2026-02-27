#ifndef _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_SPARTAN_SRC_PLUGINS_COMMUNITYINVESTOR_FRONTEND_UTILS_H
#define _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_SPARTAN_SRC_PLUGINS_COMMUNITYINVESTOR_FRONTEND_UTILS_H
#include "core.hpp"
#include "clsx.hpp"
#include "tailwind-merge.h"

template <typename /* spread: Args */>
any cn(Args... inputs_);

template <typename /* spread: Args */>
any cn(Args... inputs_)
{
    array<std::shared_ptr<ClassValue>> inputs = array<std::shared_ptr<ClassValue>>{inputs_...};
    return twMerge(clsx(inputs));
};


#endif
