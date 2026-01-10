#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_ELIZAOS_GITHUB_IO_SRC_LIB_UTILS_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_ELIZAOS_GITHUB_IO_SRC_LIB_UTILS_H
#include "core.h"
#include "clsx.h"
#include "tailwind-merge.h"

template <typename ...Args>
any cn(Args... inputs_);

template <typename ...Args>
any cn(Args... inputs_)
{
    array<std::shared_ptr<ClassValue>> inputs = array<std::shared_ptr<ClassValue>>{inputs_...};
    return twMerge(clsx(inputs));
};


#endif
