#ifndef _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_DISCRUB_EXT_SRC_SERVICES_CHROME_SERVICE_H
#define _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_DISCRUB_EXT_SRC_SERVICES_CHROME_SERVICE_H
#include "core.hpp"
#include "../enum/discrub-setting.h"
#include "../enum/resolution-type.h"
#include "../enum/sort-direction.h"
#include "../features/app/app-types.h"

typedef std::function<any(string)> ChromeCallback;


extern std::function<void(string, std::shared_ptr<ChromeCallback>)> sendChromeMessage;
extern array<object> defaultSettings;
extern std::function<std::shared_ptr<Promise<std::shared_ptr<Record<DiscrubSetting, string>>>>()> initializeSettings;
extern std::function<std::shared_ptr<Promise<std::shared_ptr<Record<DiscrubSetting, string>>>>()> getSettings;
extern std::function<std::shared_ptr<Promise<std::shared_ptr<Record<DiscrubSetting, string>>>>(string, string)> setSetting;
#endif
