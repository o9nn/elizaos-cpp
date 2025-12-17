#pragma once
#include <any>
#include <functional>
#include <future>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#include "elizaos/core.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use

/**
 * CapSolver integration for solving various CAPTCHA types
 */

struct CapSolverConfig {
    std::string apiKey;
    std::optional<std::string> apiUrl;
    std::optional<double> retryAttempts;
    std::optional<double> pollingInterval;
};

struct CaptchaTask {
    std::string type;
    std::string websiteURL;
    std::string websiteKey;
    std::optional<std::string> proxy;
    std::optional<std::string> userAgent;
};

class CapSolverService {
public:
    CapSolverService(CapSolverConfig config);
    std::future<std::string> createTask(CaptchaTask task);
    std::future<std::any> getTaskResult(const std::string& taskId);
    std::future<std::string> solveTurnstile(const std::string& websiteURL, const std::string& websiteKey, std::optional<std::string> proxy, std::optional<std::string> userAgent);
    std::future<std::string> solveRecaptchaV2(const std::string& websiteURL, const std::string& websiteKey, auto isInvisible = false, std::optional<std::string> proxy);
    std::future<std::string> solveRecaptchaV3(const std::string& websiteURL, const std::string& websiteKey, const std::string& pageAction, auto minScore = 0.7, std::optional<std::string> proxy);
    std::future<std::string> solveHCaptcha(const std::string& websiteURL, const std::string& websiteKey, std::optional<std::string> proxy);

private:
    Required<CapSolverConfig> config_;
};

/**
 * Helper to detect CAPTCHA type on a page
 */
    // Check for Cloudflare Turnstile

    // Check for reCAPTCHA
        // Check if it's v3 by looking for grecaptcha.execute

    // Check for hCaptcha

/**
 * Inject CAPTCHA solution into the page
 */
std::future<void> injectCaptchaSolution(const std::any& page, const std::string& captchaType, const std::string& solution);

} // namespace elizaos
