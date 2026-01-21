#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_ELIZA_PACKAGES_CLIENT_SRC_CONFIG_VOICE-MODELS_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_ELIZA_PACKAGES_CLIENT_SRC_CONFIG_VOICE-MODELS_H
#include "core.h"

class VoiceModel;

class VoiceModel : public object, public std::enable_shared_from_this<VoiceModel> {
public:
    using std::enable_shared_from_this<VoiceModel>::shared_from_this;
    string value;

    string label;

    any provider;

    any gender;

    string language;

    array<string> features;
};

extern Record<string, string> providerPluginMap;
extern array<std::shared_ptr<VoiceModel>> noVoiceModel;
extern array<std::shared_ptr<VoiceModel>> localVoiceModels;
extern array<std::shared_ptr<VoiceModel>> elevenLabsVoiceModels;
extern array<std::shared_ptr<VoiceModel>> openAIVoiceModels;
extern std::function<array<std::shared_ptr<VoiceModel>>()> getAllVoiceModels;
extern std::function<array<std::shared_ptr<VoiceModel>>(any)> getVoiceModelsByProvider;
extern std::function<std::shared_ptr<VoiceModel>(string)> getVoiceModelByValue;
extern std::function<string(string)> getRequiredPluginForProvider;
extern std::function<array<string>()> getAllRequiredPlugins;
#endif
