#include "/home/runner/work/elizaos-cpp/elizaos-cpp/eliza-3d-hyperfy-starter/src/plugin-hyperfy/managers/emote-manager.h"

EmoteManager::EmoteManager(any runtime) {
    this->runtime = runtime;
    this->emoteHashMap = std::make_shared<Map>();
    this->currentEmoteTimeout = nullptr;
}

void EmoteManager::uploadEmotes()
{
    auto& __array706_2170 = EMOTES_LIST;
    for (auto __indx706_2170 = 0_N; __indx706_2170 < __array706_2170->get_length(); __indx706_2170++)
    {
        auto& emote = const_(__array706_2170)[__indx706_2170];
        {
            try
            {
                auto moduleDirPath = getModuleDirectory();
                auto emoteBuffer = std::async([=]() { fs->readFile(moduleDirPath + emote["path"]); });
                auto emoteMimeType = std::string("model/gltf-binary");
                auto emoteHash = std::async([=]() { hashFileBuffer(emoteBuffer); });
                auto emoteExt = OR((emote["path"]->split(std::string("."))->pop()->toLowerCase()), (std::string("glb")));
                auto emoteFullName = string_empty + emoteHash + std::string(".") + emoteExt + string_empty;
                auto emoteUrl = std::string("asset://") + emoteFullName + string_empty;
                console->info(std::string("[Appearance] Uploading emote '") + emote["name"] + std::string("' as ") + emoteFullName + std::string(" (") + (emoteBuffer->length / 1024)->toFixed(2) + std::string(" KB)"));
                auto emoteFile = std::make_shared<File>(array<any>{ emoteBuffer }, path->basename(emote["path"]), object{
                    object::pair{std::string("type"), emoteMimeType}
                });
                auto service = this->getService();
                auto world = service->getWorld();
                auto emoteUploadPromise = world->network->upload(emoteFile);
                auto emoteTimeout = std::make_shared<Promise>([=](auto _resolve, auto reject) mutable
                {
                    return setTimeout([=]() mutable
                    {
                        return reject(std::make_shared<Error>(std::string("Upload timed out")));
                    }
                    , 30000);
                }
                );
                std::async([=]() { Promise->race(array<any>{ emoteUploadPromise, emoteTimeout }); });
                this->emoteHashMap->set(emote["name"], emoteFullName);
                console->info(std::string("[Appearance] Emote '") + emote["name"] + std::string("' uploaded: ") + emoteUrl + string_empty);
            }
            catch (const any& err)
            {
                console->error(std::string("[Appearance] Failed to upload emote '") + emote["name"] + std::string("': ") + err["message"] + string_empty, err["stack"]);
            }
        }
    }
}

void EmoteManager::playEmote(string name)
{
    auto fallback = const_((as<Record<string, string>>(Emotes)))[name];
    auto hashName = OR((this->emoteHashMap->get(name)), (fallback));
    auto service = this->getService();
    auto world = service->getWorld();
    if (!hashName) {
        console->warn(std::string("[Emote] Emote '") + name + std::string("' not found."));
        return;
    }
    shared agentPlayer = world->entities->player;
    if (!agentPlayer) {
        console->warn(std::string("[Emote] Player entity not found."));
        return;
    }
    shared emoteUrl = (hashName->startsWith(std::string("asset://"))) ? hashName : std::string("asset://") + hashName + string_empty;
    agentPlayer->data->effect = OR((agentPlayer->data->effect), (object{}));
    agentPlayer->data->effect->emote = emoteUrl;
    console->info(std::string("[Emote] Playing '") + name + std::string("' → ") + emoteUrl + string_empty);
    this->clearTimers();
    auto emoteMeta = EMOTES_LIST->find([=](auto e) mutable
    {
        return e["name"] == name;
    }
    );
    shared duration = OR((emoteMeta["duration"]), (1.5));
    this->movementCheckInterval = setInterval([=]() mutable
    {
        if (agentPlayer->moving) {
            logger->info(std::string("[EmoteManager] '") + name + std::string("' cancelled early due to movement"));
            this->clearEmote(agentPlayer);
        }
    }
    , 100);
    this->currentEmoteTimeout = setTimeout([=]() mutable
    {
        if (agentPlayer->data->effect->emote == emoteUrl) {
            logger->info(std::string("[EmoteManager] '") + name + std::string("' finished after ") + duration + std::string("s"));
            this->clearEmote(agentPlayer);
        }
    }
    , duration * 1000);
}

void EmoteManager::clearEmote(any player)
{
    if (player["data"]["effect"]) {
        player["data"]["effect"]["emote"] = nullptr;
    }
    this->clearTimers();
}

void EmoteManager::clearTimers()
{
    if (this->currentEmoteTimeout) {
        clearTimeout(this->currentEmoteTimeout);
        this->currentEmoteTimeout = nullptr;
    }
    if (this->movementCheckInterval) {
        clearInterval(this->movementCheckInterval);
        this->movementCheckInterval = nullptr;
    }
}

any EmoteManager::getService()
{
    return this->runtime->getService<std::shared_ptr<HyperfyService>>(HyperfyService::serviceType);
}

