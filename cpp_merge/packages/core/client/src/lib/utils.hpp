#ifndef _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_ELIZA_PACKAGES_CLIENT_SRC_LIB_UTILS_H
#define _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_ELIZA_PACKAGES_CLIENT_SRC_LIB_UTILS_H
#include "core.hpp"
// External dependency removed
// External dependency removed
#include "clsx.hpp"
#include "dayjs.hpp"
// Using alias removed (invalid transpilation)
#include "dayjs/plugin/localizedFormat.h"
// Using alias removed (invalid transpilation)
#include "tailwind-merge.h"
// External dependency removed
using CoreUUID = UUID;
// External dependency removed
using ClientMessageChannel = MessageChannel;

template <typename /* spread: Args */>
any cn(Args... inputs_);

extern any moment;
extern std::function<string(string)> formatAgentName;
string characterNameToUrl(string name);

string urlToCharacterName(string urlName);

std::shared_ptr<UUID> randomUUID();

std::shared_ptr<UUID> getEntityId();

extern std::function<std::shared_ptr<Promise<string>>(std::shared_ptr<File>, any, double)> compressImage;
extern array<string> AGENT_AVATAR_PLACEHOLDERS;
extern std::function<string(object)> getAgentAvatar;
extern std::function<string(any, array<any>, any)> generateGroupName;
template <typename /* spread: Args */>
any cn(Args... inputs_)
{
    array<std::shared_ptr<ClassValue>> inputs = array<std::shared_ptr<ClassValue>>{inputs_...};
    return twMerge(clsx(inputs));
};


#endif
