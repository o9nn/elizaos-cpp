#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_THE-ORG_SRC_INDEX_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_THE-ORG_SRC_INDEX_H
#include "core.h"
#include "dotenv.h"
using dotenv = _default;
#include "@elizaos/core.h"
#include "./communityManager.h"
using communityManager = _default;
#include "./devRel.h"
using devRel = _default;
#include "./liaison.h"
using liaison = _default;
#include "./projectManager.h"
using projectManager = _default;
#include "./socialMediaManager.h"
using socialMediaManager = _default;

template <typename P0>
boolean hasRequiredEnvVars(P0 agent);

extern array<any> allAgents;
extern array<string> rawArgs;
extern array<any> enabledAgents;
extern array<string> potentialAgentFlags;
extern double doubleDashIndex;
extern array<any> availableAgents;
extern object project;
template <typename P0>
boolean hasRequiredEnvVars(P0 agent)
{
    auto usesDiscord = agent["character"]["plugins"]["includes"](std::string("@elizaos/plugin-discord"));
    auto usesTelegram = agent["character"]["plugins"]["includes"](std::string("@elizaos/plugin-telegram"));
    if (AND((!usesDiscord), (!usesTelegram))) return true;
    auto hasValidPlatform = false;
    if (usesDiscord) {
        auto discordId = agent["character"]["settings"]["secrets"]["DISCORD_APPLICATION_ID"];
        auto discordToken = agent["character"]["settings"]["secrets"]["DISCORD_API_TOKEN"];
        if (AND((discordId), (discordToken))) {
            hasValidPlatform = true;
            logger->debug(std::string("Agent "") + (OR((agent["character"]["name"]), (std::string("Unknown")))) + std::string("" has Discord configuration"));
        }
    }
    if (usesTelegram) {
        auto telegramToken = agent["character"]["settings"]["secrets"]["TELEGRAM_BOT_TOKEN"];
        if (telegramToken) {
            hasValidPlatform = true;
            logger->debug(std::string("Agent "") + (OR((agent["character"]["name"]), (std::string("Unknown")))) + std::string("" has Telegram configuration"));
        }
    }
    if (!hasValidPlatform) {
        logger->warn(std::string("Agent "") + (OR((agent["character"]["name"]), (std::string("Unknown")))) + std::string("" disabled - missing platform configuration"));
    }
    return hasValidPlatform;
};


#endif
