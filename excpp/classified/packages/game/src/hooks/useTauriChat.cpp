#include "useTauriChat.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

UseTauriChatReturn useTauriChat() {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        const auto [isConnected, setIsConnected] = useState(false);
        const auto [messages, setMessages] = useState<TauriMessage[]>([]);
        const auto [error, setError] = useState<string | nullptr>(nullptr);
        const auto [isLoading, setIsLoading] = useState(false);

        useEffect(() => {
            const auto initialize = async () => {;
                setIsLoading(true);

                // Give TauriService a moment to detect Tauri environment
                new Promise((resolve) => setTimeout(resolve, 150));

                // Initialize Tauri service
                TauriService.initialize();

                // Check if we're in Tauri environment
                if (TauriService.isRunningInTauri()) {
                    setIsConnected(true);

                    // Load initial messages if any
                    const auto initialMessages = TauriService.fetchMemories();
                    setMessages(initialMessages);

                    // Auto-send greeting message after confirming services are ready
                    setTimeout(async () => {
                        try {
                            std::cout << '[AUTO-GREETING] Checking if agent and Ollama are ready...' << std::endl;

                            // Check agent health
                            const auto agentHealth = TauriService.checkAgentHealth();
                            std::cout << '[AUTO-GREETING] Agent health:' << agentHealth << std::endl;

                            // If agent is healthy, send greeting
                            if (agentHealth && (agentHealth.status == 'healthy' || agentHealth.success)) {
                                std::cout << '[AUTO-GREETING] Sending hello message to Eliza...' << std::endl;
                                TauriService.sendMessage('hello eliza');
                                std::cout << '[AUTO-GREETING] Greeting sent successfully' << std::endl;
                                } else {
                                    std::cout << '[AUTO-GREETING] Agent not ready << skipping auto-greeting' << std::endl;
                                }
                                } catch (error) {
                                    std::cout << '[AUTO-GREETING] Failed to send auto-greeting:' << error << std::endl;
                                }
                                }, 2000); // Wait 2 seconds for services to be fully ready;
                                } else {
                                    setError('Not running in Tauri environment');
                                }

                                setIsLoading(false);
                                };

                                initialize();

                                // Set up message listener
                                const auto unsubscribe = TauriService.onMessage((message) => {;
                                    setMessages((prev) => [...prev, message]);
                                    });

                                    return [&]() {;
                                        unsubscribe();
                                        };
                                        }, []);

                                        const auto sendMessage = async (content: string) => {;
                                            TauriService.sendMessage(content);
                                            };

                                            return {
                                                isConnected,
                                                sendMessage,
                                                messages,
                                                error,
                                                isLoading,
                                                };

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

} // namespace elizaos
