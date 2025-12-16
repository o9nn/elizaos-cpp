#include "elizaos/core.hpp"
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#pragma once

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
  private config: Required<CapSolverConfig>;

  constructor(config: CapSolverConfig) {
    this.config = {
      apiUrl: 'https://api.capsolver.com',
      retryAttempts: 60,
      pollingInterval: 2000,
      ...config,
    };
  }

  /**
   * Create a CAPTCHA solving task
   */

  /**
   * Get task result with polling
   */

        // Still processing

  /**
   * Solve Cloudflare Turnstile
   */

      // Use proxy version

  /**
   * Solve reCAPTCHA v2
   */

  /**
   * Solve reCAPTCHA v3
   */

  /**
   * Solve hCaptcha
   */

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
