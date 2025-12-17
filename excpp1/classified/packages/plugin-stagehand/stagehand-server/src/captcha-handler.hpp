#pragma once
#include <functional>
#include <future>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



struct CaptchaInfo {
    std::variant<'turnstile', 'recaptcha-v2', 'recaptcha-v3', 'hcaptcha'> type;
    std::optional<std::string> siteKey;
};

std::future<CaptchaInfo> detectCaptchaType(Page page);

std::future<void> injectCaptchaSolution(Page page, const std::string& captchaType, const std::string& solution);

} // namespace elizaos
