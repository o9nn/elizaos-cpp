#include "/home/runner/work/elizaos-cpp/elizaos-cpp/eliza-3d-hyperfy-starter/src/plugin-hyperfy/managers/behavior-manager.h"

BehaviorManager::BehaviorManager(std::shared_ptr<IAgentRuntime> runtime) {
    this->runtime = runtime;
}

void BehaviorManager::start()
{
    if (this->isRunning) {
        logger->warn(std::string("[BehaviorManager] Already running"));
        return;
    }
    this->isRunning = true;
    logger->info(std::string("[BehaviorManager] Starting behavior loop for player"));
    this->runLoop()->_catch([=](auto err) mutable
    {
        return logger->error(std::string("[BehaviorManager] Fatal error in run loop:"), err);
    }
    );
}

void BehaviorManager::stop()
{
    if (!this->isRunning) {
        logger->warn(std::string("[BehaviorManager] Not running"));
        return;
    }
    this->isRunning = false;
    logger->info(std::string("[BehaviorManager] Stopped behavior loop"));
}

std::shared_ptr<Promise<void>> BehaviorManager::runLoop()
{
    while (this->isRunning)
    {
        try
        {
            std::async([=]() { this->executeBehavior(); });
        }
        catch (const any& error)
        {
            logger->error(std::string("[BehaviorManager] Error in behavior:"), error);
        }
        shared delay = TIME_INTERVAL_MIN + Math->floor(Math->random() * (TIME_INTERVAL_MAX - TIME_INTERVAL_MIN));
        std::async([=]() { std::make_shared<Promise>([=](auto resolve) mutable
        {
            return setTimeout(resolve, delay);
        }
        ); });
    }
    return std::shared_ptr<Promise<void>>();
}

any BehaviorManager::getService()
{
    return this->runtime->getService<std::shared_ptr<HyperfyService>>(HyperfyService::serviceType);
}

std::shared_ptr<Promise<void>> BehaviorManager::executeBehavior()
{
    shared service = this->getService();
    if (!service) {
        logger->error(std::string("[BehaviorManager] Cannot start — service not available"));
        return std::shared_ptr<Promise<void>>();
    }
    auto world = service->getWorld();
    if (!world) {
        logger->error(std::string("[BehaviorManager] Cannot start — world not found"));
        return std::shared_ptr<Promise<void>>();
    }
    auto player = world->entities->player;
    if (!player) {
        logger->error(std::string("[BehaviorManager] Cannot start — player entity not found"));
        return std::shared_ptr<Promise<void>>();
    }
    if (agentActivityLock->isActive()) {
        logger->info(std::string("[BehaviorManager] Skipping behavior — message activity in progress"));
        return std::shared_ptr<Promise<void>>();
    }
    auto _currentWorldId = service->currentWorldId;
    shared elizaRoomId = createUniqueUuid(this->runtime, OR((_currentWorldId), (std::string("hyperfy-unknown-world"))));
    shared entityId = createUniqueUuid(this->runtime, this->runtime->agentId);
    auto newMessage = object{
        object::pair{std::string("id"), createUniqueUuid(this->runtime, Date->now()->toString())}, 
        object::pair{std::string("content"), object{
            object::pair{std::string("text"), string_empty}, 
            object::pair{std::string("type"), std::string("text")}
        }}, 
        object::pair{std::string("roomId"), elizaRoomId}, 
        object::pair{std::string("worldId"), _currentWorldId}, 
        object::pair{std::string("entityId"), std::string("entityId")}
    };
    auto state = std::async([=]() { this->runtime->composeState(newMessage); });
    auto actionsData = std::async([=]() { getHyperfyActions(this->runtime, newMessage, state, array<string>{ std::string("HYPERFY_GOTO_ENTITY"), std::string("HYPERFY_WALK_RANDOMLY"), std::string("HYPERFY_USE_ITEM"), std::string("HYPERFY_UNUSE_ITEM"), std::string("HYPERFY_AMBIENT_SPEECH"), std::string("REPLY"), std::string("IGNORE") }); });
    auto actionsText = (actionsData->get_length() > 0) ? any(formatActions(actionsData)) : any(string_empty);
    auto responsePrompt = composePromptFromState(object{
        object::pair{std::string("state"), std::string("state")}, 
        object::pair{std::string("template"), autoTemplate(actionsText)}
    });
    auto response = std::async([=]() { this->runtime->useModel(ModelType->TEXT_LARGE, object{
        object::pair{std::string("prompt"), responsePrompt}
    }); });
    auto parsedXml = parseKeyValueXml(response);
    console->log(std::string("****** response\
"), parsedXml);
    auto responseMemory = object{
        object::pair{std::string("content"), object{
            object::pair{std::string("thought"), parsedXml->thought}, 
            object::pair{std::string("text"), parsedXml->text}, 
            object::pair{std::string("actions"), parsedXml->actions}, 
            object::pair{std::string("providers"), parsedXml->providers}, 
            object::pair{std::string("emote"), parsedXml->emote}
        }}, 
        object::pair{std::string("entityId"), createUniqueUuid(this->runtime, this->runtime->agentId)}, 
        object::pair{std::string("roomId"), elizaRoomId}
    };
    auto name = world->entities->player->data->name;
    std::async([=]() { this->runtime->ensureConnection(object{
        object::pair{std::string("entityId"), entityId}, 
        object::pair{std::string("roomId"), elizaRoomId}, 
        object::pair{std::string("userName"), name}, 
        object::pair{std::string("name"), std::string("name")}, 
        object::pair{std::string("source"), std::string("hyperfy")}, 
        object::pair{std::string("channelId"), _currentWorldId}, 
        object::pair{std::string("serverId"), std::string("hyperfy")}, 
        object::pair{std::string("type"), ChannelType->WORLD}, 
        object::pair{std::string("worldId"), _currentWorldId}, 
        object::pair{std::string("userId"), world->entities->player->data->id}
    }); });
    auto callback = [=](auto responseContent) mutable
    {
        console->info(std::string("[Hyperfy Auto Callback] Received response: ") + JSON->stringify(responseContent) + string_empty);
        auto emote = as<string>(responseContent->emote);
        auto callbackMemory = object{
            object::pair{std::string("id"), createUniqueUuid(this->runtime, Date->now()->toString())}, 
            object::pair{std::string("entityId"), this->runtime->agentId}, 
            object::pair{std::string("agentId"), this->runtime->agentId}, 
            object::pair{std::string("content"), utils::assign(object{
                , 
                object::pair{std::string("channelType"), ChannelType->WORLD}, 
                object::pair{std::string("emote"), std::string("emote")}
            }, responseContent)}, 
            object::pair{std::string("roomId"), elizaRoomId}, 
            object::pair{std::string("createdAt"), Date->now()}
        };
        std::async([=]() { this->runtime->createMemory(callbackMemory, std::string("messages")); });
        if (emote) {
            auto emoteManager = service->getEmoteManager();
            emoteManager->playEmote(emote);
        }
        if (responseContent->text) {
            auto messageManager = service->getMessageManager();
            messageManager->sendMessage(responseContent->text);
        }
        return array<any>();
    };
    std::async([=]() { this->runtime->processActions(newMessage, array<object>{ responseMemory }, state, callback); });
    std::async([=]() { this->runtime->evaluate(newMessage, state, true, callback, array<object>{ responseMemory }); });
}

double TIME_INTERVAL_MIN = 15000;
double TIME_INTERVAL_MAX = 30000;

void Main(void)
{
}

MAIN
