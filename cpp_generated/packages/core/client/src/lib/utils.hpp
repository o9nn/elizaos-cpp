#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_ELIZA_PACKAGES_CLIENT_SRC_LIB_UTILS_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_ELIZA_PACKAGES_CLIENT_SRC_LIB_UTILS_H
#include "core.h"
#include "@/constants.h"
#include "@elizaos/core.h"
#include "clsx.h"
#include "dayjs.h"
using dayjs = _default;
#include "dayjs/plugin/localizedFormat.h"
using localizedFormat = _default;
#include "tailwind-merge.h"
#include "@elizaos/core.h"
using CoreUUID = UUID;
#include "@/types.h"
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
