#include "emote-manager.h"

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
                auto emoteMimeType = std:("model/gltf-binary");
                auto emoteHash = std::async([=]() { hashFileBuffer(emoteBuffer); });
                auto emoteExt = OR((emote["path"]->split(std:("."))->pop()->toLowerCase()), (std:("glb")));
                auto emoteFullName = string_empty + emoteHash + std:(".") + emoteExt + string_empty;
                auto emoteUrl = std:("asset://") + emoteFullName + string_empty;
                console->info(std:("[Appearance] Uploading emote '") + emote["name"] + std:("' as ") + emoteFullName + std:(" (") + (emoteBuffer->length / 1024)->toFixed(2) + std:(" KB)"));
                auto emoteFile = std::make_shared<File>(array<any>{ emoteBuffer }, path->basename(emote["path"]), object{
                    object::pair{std:("type"), emoteMimeType}
                });
                auto service = this->getService();
                auto world = service->getWorld();
                auto emoteUploadPromise = world->network->upload(emoteFile);
                auto emoteTimeout = std::make_shared<Promise>([=](auto _resolve, auto reject) mutable
                {
                    return setTimeout([=]() mutable
                    {
                        return reject(std::make_shared<Error>(std:("Upload timed out")));
                    }
                    , 30000);
                }
                );
                std::async([=]() { Promise->race(array<any>{ emoteUploadPromise, emoteTimeout }); });
                this->emoteHashMap->set(emote["name"], emoteFullName);
                console->info(std:("[Appearance] Emote '") + emote["name"] + std:("' uploaded: ") + emoteUrl + string_empty);
            }
            catch (const any& err)
            {
                console->error(std:("[Appearance] Failed to upload emote '") + emote["name"] + std:("': ") + err["message"] + string_empty, err["stack"]);
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
        console->warn(std:("[Emote] Emote '") + name + std:("' not found."));
        return;
    }
    shared agentPlayer = world->entities->player;
    if (!agentPlayer) {
        console->warn(std:("[Emote] Player entity not found."));
        return;
    }
    shared emoteUrl = (hashName->startsWith(std:("asset://"))) ? hashName : std:("asset://") + hashName + string_empty;
    agentPlayer->data->effect = OR((agentPlayer->data->effect), (object{}));
    agentPlayer->data->effect->emote = emoteUrl;
    console->info(std:("[Emote] Playing '") + name + std:("' → ") + emoteUrl + string_empty);
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
            logger->info(std:("[EmoteManager] '") + name + std:("' cancelled early due to movement"));
            this->clearEmote(agentPlayer);
        }
    }
    , 100);
    this->currentEmoteTimeout = setTimeout([=]() mutable
    {
        if (agentPlayer->data->effect->emote == emoteUrl) {
            logger->info(std:("[EmoteManager] '") + name + std:("' finished after ") + duration + std:("s"));
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

