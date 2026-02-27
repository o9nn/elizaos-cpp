#include "behavior-manager.h"

BehaviorManager::BehaviorManager(std::shared_ptr<IAgentRuntime> runtime) {
    this->runtime = runtime;
}

void BehaviorManager::start()
{
    if (this->isRunning) {
        logger->warn(std:("[BehaviorManager] Already running"));
        return;
    }
    this->isRunning = true;
    logger->info(std:("[BehaviorManager] Starting behavior loop for player"));
    this->runLoop()->_catch([=](auto err) mutable
    {
        return logger->error(std:("[BehaviorManager] Fatal error in run loop:"), err);
    }
    );
}

void BehaviorManager::stop()
{
    if (!this->isRunning) {
        logger->warn(std:("[BehaviorManager] Not running"));
        return;
    }
    this->isRunning = false;
    logger->info(std:("[BehaviorManager] Stopped behavior loop"));
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
            logger->error(std:("[BehaviorManager] Error in behavior:"), error);
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
        logger->error(std:("[BehaviorManager] Cannot start — service not available"));
        return std::shared_ptr<Promise<void>>();
    }
    auto world = service->getWorld();
    if (!world) {
        logger->error(std:("[BehaviorManager] Cannot start — world not found"));
        return std::shared_ptr<Promise<void>>();
    }
    auto player = world->entities->player;
    if (!player) {
        logger->error(std:("[BehaviorManager] Cannot start — player entity not found"));
        return std::shared_ptr<Promise<void>>();
    }
    if (agentActivityLock->isActive()) {
        logger->info(std:("[BehaviorManager] Skipping behavior — message activity in progress"));
        return std::shared_ptr<Promise<void>>();
    }
    auto _currentWorldId = service->currentWorldId;
    shared elizaRoomId = createUniqueUuid(this->runtime, OR((_currentWorldId), (std:("hyperfy-unknown-world"))));
    shared entityId = createUniqueUuid(this->runtime, this->runtime->agentId);
    auto newMessage = object{
        object::pair{std:("id"), createUniqueUuid(this->runtime, Date->now()->toString())}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), string_empty}, 
            object::pair{std:("type"), std:("text")}
        }}, 
        object::pair{std:("roomId"), elizaRoomId}, 
        object::pair{std:("worldId"), _currentWorldId}, 
        object::pair{std:("entityId"), std:("entityId")}
    };
    auto state = std::async([=]() { this->runtime->composeState(newMessage); });
    auto actionsData = std::async([=]() { getHyperfyActions(this->runtime, newMessage, state, array<string>{ std:("HYPERFY_GOTO_ENTITY"), std:("HYPERFY_WALK_RANDOMLY"), std:("HYPERFY_USE_ITEM"), std:("HYPERFY_UNUSE_ITEM"), std:("HYPERFY_AMBIENT_SPEECH"), std:("REPLY"), std:("IGNORE") }); });
    auto actionsText = (actionsData->get_length() > 0) ? any(formatActions(actionsData)) (string_empty);
    auto responsePrompt = composePromptFromState(object{
        object::pair{std:("state"), std:("state")}, 
        object::pair{std:("template"), autoTemplate(actionsText)}
    });
    auto response = std::async([=]() { this->runtime->useModel(ModelType->TEXT_LARGE, object{
        object::pair{std:("prompt"), responsePrompt}
    }); });
    auto parsedXml = parseKeyValueXml(response);
    console->log(std:("****** response\
"), parsedXml);
    auto responseMemory = object{
        object::pair{std:("content"), object{
            object::pair{std:("thought"), parsedXml->thought}, 
            object::pair{std:("text"), parsedXml->text}, 
            object::pair{std:("actions"), parsedXml->actions}, 
            object::pair{std:("providers"), parsedXml->providers}, 
            object::pair{std:("emote"), parsedXml->emote}
        }}, 
        object::pair{std:("entityId"), createUniqueUuid(this->runtime, this->runtime->agentId)}, 
        object::pair{std:("roomId"), elizaRoomId}
    };
    auto name = world->entities->player->data->name;
    std::async([=]() { this->runtime->ensureConnection(object{
        object::pair{std:("entityId"), entityId}, 
        object::pair{std:("roomId"), elizaRoomId}, 
        object::pair{std:("userName"), name}, 
        object::pair{std:("name"), std:("name")}, 
        object::pair{std:("source"), std:("hyperfy")}, 
        object::pair{std:("channelId"), _currentWorldId}, 
        object::pair{std:("serverId"), std:("hyperfy")}, 
        object::pair{std:("type"), ChannelType->WORLD}, 
        object::pair{std:("worldId"), _currentWorldId}, 
        object::pair{std:("userId"), world->entities->player->data->id}
    }); });
    auto callback = [=](auto responseContent) mutable
    {
        console->info(std:("[Hyperfy Auto Callback] Received response: ") + JSON->stringify(responseContent) + string_empty);
        auto emote = as<string>(responseContent->emote);
        auto callbackMemory = object{
            object::pair{std:("id"), createUniqueUuid(this->runtime, Date->now()->toString())}, 
            object::pair{std:("entityId"), this->runtime->agentId}, 
            object::pair{std:("agentId"), this->runtime->agentId}, 
            object::pair{std:("content"), utils::assign(object{
                , 
                object::pair{std:("channelType"), ChannelType->WORLD}, 
                object::pair{std:("emote"), std:("emote")}
            }, responseContent)}, 
            object::pair{std:("roomId"), elizaRoomId}, 
            object::pair{std:("createdAt"), Date->now()}
        };
        std::async([=]() { this->runtime->createMemory(callbackMemory, std:("messages")); });
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
