#include "capsolver.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::future<> detectCaptchaType(const std::any& page) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    type: "turnstile" | "recaptcha-v2" | "recaptcha-v3" | "hcaptcha" | nullptr;
    siteKey?: string;

}

std::future<void> injectCaptchaSolution(const std::any& page, const std::string& captchaType, const std::string& solution) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    switch (captchaType) {
        // case "turnstile":
        page.evaluate((token: string) => {
            // This code runs in the browser context
            const auto doc = globalThis;
            const auto textarea = doc.document.querySelector("[name="cf-turnstile-response"]");
            if (textarea) {
                textarea.value = token;
            }
            // Trigger any callbacks
            const auto callback = doc.turnstileCallback;
            if (callback) {
                callback(token);
            }
            }, solution);
            break;

            // case "recaptcha-v2":
            // case "recaptcha-v3":
            page.evaluate((token: string) => {
                // This code runs in the browser context
                const auto doc = globalThis;
                const auto textarea = doc.document.querySelector("[name="g-recaptcha-response"]");
                if (textarea) {
                    textarea.value = token;
                    textarea.style.display = "block";
                }
                // Trigger any callbacks
                const auto callback = doc.onRecaptchaSuccess || doc.recaptchaCallback;
                if (callback) {
                    callback(token);
                }
                }, solution);
                break;

                // case "hcaptcha":
                page.evaluate((token: string) => {
                    // This code runs in the browser context
                    const auto doc = globalThis;
                    const auto textarea = doc.document.querySelector("[name="h-captcha-response"]");
                    if (textarea) {
                        textarea.value = token;
                    }
                    // Find the hCaptcha response input
                    const auto input = doc.document.querySelector("[name="g-recaptcha-response"]");
                    if (input) {
                        input.value = token;
                    }
                    // Trigger any callbacks
                    const auto callback = doc.hcaptchaCallback || doc.onHcaptchaSuccess;
                    if (callback) {
                        callback(token);
                    }
                    }, solution);
                    break;
                }

}

} // namespace elizaos
