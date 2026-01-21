#include "/home/runner/work/elizaos-cpp/elizaos-cpp/classified/packages/game/src/hooks/useTauriChat.h"

std::shared_ptr<UseTauriChatReturn> useTauriChat()
{
    auto [isConnected, setIsConnected] = useState(false);
    auto [messages, setMessages] = useState<array<std::shared_ptr<TauriMessage>>>(array<any>());
    auto [error, setError] = useState<any>(nullptr);
    auto [isLoading, setIsLoading] = useState(false);
    useEffect([=]() mutable
    {
        shared initialize = [=]() mutable
        {
            setIsLoading(true);
            std::async([=]() { std::make_shared<Promise>([=](auto resolve) mutable
            {
                return setTimeout(resolve, 150);
            }
            ); });
            std::async([=]() { TauriService->initialize(); });
            if (TauriService->isRunningInTauri()) {
                setIsConnected(true);
                auto initialMessages = std::async([=]() { TauriService->fetchMemories(); });
                setMessages(initialMessages);
                setTimeout([=]() mutable
                {
                    try
                    {
                        console->log(std::string("[AUTO-GREETING] Checking if agent and Ollama are ready..."));
                        auto agentHealth = std::async([=]() { TauriService->checkAgentHealth(); });
                        console->log(std::string("[AUTO-GREETING] Agent health:"), agentHealth);
                        if (AND((agentHealth), ((OR((agentHealth->status == std::string("healthy")), (agentHealth->success)))))) {
                            console->log(std::string("[AUTO-GREETING] Sending hello message to Eliza..."));
                            std::async([=]() { TauriService->sendMessage(std::string("hello eliza")); });
                            console->log(std::string("[AUTO-GREETING] Greeting sent successfully"));
                        } else {
                            console->log(std::string("[AUTO-GREETING] Agent not ready, skipping auto-greeting"));
                        }
                    }
                    catch (const any& error)
                    {
                        console->warn(std::string("[AUTO-GREETING] Failed to send auto-greeting:"), error);
                    }
                }
                , 2000);
            } else {
                setError(std::string("Not running in Tauri environment"));
            }
            setIsLoading(false);
        };
        initialize();
        shared unsubscribe = TauriService->onMessage([=](auto message) mutable
        {
            setMessages([=](auto prev) mutable
            {
                return array<any>{ prev, message };
            }
            );
        }
        );
        return [=]() mutable
        {
            unsubscribe();
        };
    }
    , array<any>());
    shared sendMessage = [=](auto content) mutable
    {
        std::async([=]() { TauriService->sendMessage(content); });
    };
    return object{
        object::pair{std::string("isConnected"), std::string("isConnected")}, 
        object::pair{std::string("sendMessage"), std::string("sendMessage")}, 
        object::pair{std::string("messages"), std::string("messages")}, 
        object::pair{std::string("error"), std::string("error")}, 
        object::pair{std::string("isLoading"), std::string("isLoading")}
    };
};


