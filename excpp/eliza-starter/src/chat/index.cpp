#include "index.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::future<void> handleUserInput(auto input, auto agentId) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    if (input.toLowerCase() == "exit") {
        rl.close();
        process.exit(0);
    }

    try {
        const auto serverPort = parseInt(settings.SERVER_PORT || "3000");

        const auto response = fetch(;
        "http://localhost:" + serverPort + "/" + agentId + "/message"
        {
            method: "POST",
            headers: { "Content-Type": "application/json" },
            body: JSON.stringify({
                text: input,
                userId: "user",
                userName: "User",
                }),
            }
            );

            const auto data = response.json();
            data.forEach((message) => console.log(`${"Agent"}: ${message.text}`));
            } catch (error) {
                std::cerr << "Error fetching response:" << error << std::endl;
            }

}

void startChat(auto characters) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    function chat() {
        const auto agentId = characters[0].name || "Agent";
        rl.question("You: ", async (input) => {
            handleUserInput(input, agentId);
            if (input.toLowerCase() != "exit") {
                chat(); // Loop back to ask another question;
            }
            });
        }

        return chat;

}

} // namespace elizaos
