#ifndef _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_CLASSIFIED_PACKAGES_PLUGIN_VISION_SRC_CONFIG_H
#define _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_CLASSIFIED_PACKAGES_PLUGIN_VISION_SRC_CONFIG_H
#include "core.hpp"
#include "zod.hpp"
// External dependency removed
#include "./types.h"

typedef z::input<VisionConfigSchema> VisionConfigInput;

typedef z::output<VisionConfigSchema> VisionConfigOutput;

class ConfigurationManager;

extern std::shared_ptr<VisionConfig> defaultVisionConfig;
extern any VisionConfigSchema;
class ConfigurationManager : public object, public std::enable_shared_from_this<ConfigurationManager> {
public:
    using std::enable_shared_from_this<ConfigurationManager>::shared_from_this;
    VisionConfigOutput config;

    any runtime;

    ConfigurationManager(any runtime);
    virtual VisionConfigOutput loadConfiguration();
    virtual any getSetting(string key);
    virtual boolean getBooleanSetting(string key, boolean defaultValue);
    virtual double getNumberSetting(string key, double defaultValue);
    virtual VisionConfigOutput get();
    virtual void update(Partial<VisionConfigInput> updates);
    virtual std::shared_ptr<VisionConfig> toLegacyFormat();
    static Partial<VisionConfigInput> getPreset(string name);
};

#endif
