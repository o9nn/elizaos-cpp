#ifndef _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_CLASSIFIED_PACKAGES_PLUGIN_STAGEHAND_STAGEHAND_SERVER_SRC_CAPTCHA_HANDLER_H
#define _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_CLASSIFIED_PACKAGES_PLUGIN_STAGEHAND_STAGEHAND_SERVER_SRC_CAPTCHA_HANDLER_H
#include "core.hpp"
#include "playwright.hpp"

class CaptchaInfo;

class CaptchaInfo : public object, public std::enable_shared_from_this<CaptchaInfo> {
public:
    using std::enable_shared_from_this<CaptchaInfo>::shared_from_this;
    any type;

    any siteKey;
};

std::shared_ptr<Promise<std::shared_ptr<CaptchaInfo>>> detectCaptchaType(std::shared_ptr<Page> page);

std::shared_ptr<Promise<void>> injectCaptchaSolution(std::shared_ptr<Page> page, string captchaType, string solution);

#endif
