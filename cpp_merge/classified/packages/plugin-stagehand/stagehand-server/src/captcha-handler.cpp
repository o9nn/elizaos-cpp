#include "captcha-handler.h"

std::shared_ptr<Promise<std::shared_ptr<CaptchaInfo>>> detectCaptchaType(std::shared_ptr<Page> page)
{
    try
    {
        auto turnstileElement = std::async([=]() { page->$(std:("[id*="turnstile"], [class*="cf-turnstile"]")); });
        if (turnstileElement) {
            auto siteKey = std::async([=]() { page->evaluate([=]() mutable
            {
                auto element = as<std::shared_ptr<HTMLElement>>(document->querySelector(std:("[data-sitekey]")));
                return OR((element->dataset->sitekey), (nullptr));
            }
            ); });
            return object{
                object::pair{std:("type"), std:("turnstile")}, 
                object::pair{std:("siteKey"), std:("siteKey")}
            };
        }
        auto recaptchaV2Element = std::async([=]() { page->$(std:(".g-recaptcha, [data-sitekey]")); });
        if (recaptchaV2Element) {
            auto siteKey = std::async([=]() { page->evaluate([=]() mutable
            {
                auto element = as<std::shared_ptr<HTMLElement>>(document->querySelector(std:("[data-sitekey]")));
                return OR((element->dataset->sitekey), (nullptr));
            }
            ); });
            return object{
                object::pair{std:("type"), std:("recaptcha-v2")}, 
                object::pair{std:("siteKey"), std:("siteKey")}
            };
        }
        auto hasRecaptchaV3 = std::async([=]() { page->evaluate([=]() mutable
        {
            return !!(as<any>(window))["grecaptcha"]["execute"];
        }
        ); });
        if (hasRecaptchaV3) {
            auto siteKey = std::async([=]() { page->evaluate([=]() mutable
            {
                auto scripts = Array->from(document->querySelectorAll(std:("script[src*="recaptcha"]")));
                for (auto& script : scripts)
                {
                    auto match = (as<std::shared_ptr<HTMLScriptElement>>(script))->src->match((new RegExp(std:("render=([^&]+"))));
                    if (match) return (*const_(match))[1];
                }
                return nullptr;
            }
            ); });
            return object{
                object::pair{std:("type"), std:("recaptcha-v3")}, 
                object::pair{std:("siteKey"), std:("siteKey")}
            };
        }
        auto hcaptchaElement = std::async([=]() { page->$(std:(".h-captcha, [data-hcaptcha-widget-id]")); });
        if (hcaptchaElement) {
            auto siteKey = std::async([=]() { page->evaluate([=]() mutable
            {
                auto element = as<std::shared_ptr<HTMLElement>>(document->querySelector(std:("[data-sitekey]")));
                return OR((element->dataset->sitekey), (nullptr));
            }
            ); });
            return object{
                object::pair{std:("type"), std:("hcaptcha")}, 
                object::pair{std:("siteKey"), std:("siteKey")}
            };
        }
        return object{
            object::pair{std:("type"), nullptr}, 
            object::pair{std:("siteKey"), nullptr}
        };
    }
    catch (const any& error)
    {
        console->error(std:("Error detecting captcha type:"), error);
        return object{
            object::pair{std:("type"), nullptr}, 
            object::pair{std:("siteKey"), nullptr}
        };
    }
};


std::shared_ptr<Promise<void>> injectCaptchaSolution(std::shared_ptr<Page> page, string captchaType, string solution)
{
    console->log(std:("Would inject ") + captchaType + std:(" solution:"), solution->substring(0, 20) + std:("..."));
    return std::shared_ptr<Promise<void>>();
};


