#include "index.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::future<void> initializeClients(Character character, IAgentRuntime runtime) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto clients = [];
    const auto clientTypes = character.clients.map((str) => str.toLowerCase()) || [];

    if (clientTypes.includes("auto")) {
        const auto autoClient = AutoClientInterface.start(runtime);
        if (autoClient) clients.push(autoClient);
    }

    if (clientTypes.includes("discord")) {
        clients.push(DiscordClientInterface.start(runtime));
    }

    if (clientTypes.includes("telegram")) {
        const auto telegramClient = TelegramClientInterface.start(runtime);
        if (telegramClient) clients.push(telegramClient);
    }

    if (clientTypes.includes("twitter")) {
        const auto twitterClients = TwitterClientInterface.start(runtime);
        clients.push(twitterClients);
    }

    if (character.plugins.length > 0) {
        for (const auto& plugin : character.plugins)
            if (plugin.clients) {
                for (const auto& client : plugin.clients)
                    clients.push(client.start(runtime));
                }
            }
        }
    }

    return clients;

}

} // namespace elizaos
