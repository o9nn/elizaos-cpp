#include "index.hpp"

any initializeClients(std::shared_ptr<Character> character, std::shared_ptr<IAgentRuntime> runtime)
{
    auto clients = array<any>();
    auto clientTypes = OR((character->clients->map([=](auto str) mutable
    {
        return str["toLowerCase"]();
    }
    )), (array<any>()));
    if (clientTypes["includes"](std:("auto"))) {
        auto autoClient = std::async([=]() { AutoClientInterface->start(runtime); });
        if (autoClient) clients->push(autoClient);
    }
    if (clientTypes["includes"](std:("discord"))) {
        clients->push(std::async([=]() { DiscordClientInterface->start(runtime); }));
    }
    if (clientTypes["includes"](std:("telegram"))) {
        auto telegramClient = std::async([=]() { TelegramClientInterface->start(runtime); });
        if (telegramClient) clients->push(telegramClient);
    }
    if (clientTypes["includes"](std:("twitter"))) {
        auto twitterClients = std::async([=]() { TwitterClientInterface->start(runtime); });
        clients->push(twitterClients);
    }
    if (character->plugins->length > 0) {
        for (auto& plugin : character->plugins)
        {
            if (plugin->clients) {
                for (auto& client : plugin->clients)
                {
                    clients->push(std::async([=]() { client->start(runtime); }));
                }
            }
        }
    }
    return clients;
};


