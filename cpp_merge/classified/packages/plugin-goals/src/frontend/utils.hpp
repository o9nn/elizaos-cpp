#ifndef _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_CLASSIFIED_PACKAGES_PLUGIN_GOALS_SRC_FRONTEND_UTILS_H
#define _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_CLASSIFIED_PACKAGES_PLUGIN_GOALS_SRC_FRONTEND_UTILS_H
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
