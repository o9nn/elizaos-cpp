#include "service.hpp"
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



struct BrowserNavigationOptions {
    std::optional<double> timeout;
    std::optional<'load' | 'domcontentloaded' | 'networkidle0' | 'networkidle2'> waitUntil;
    std::optional<{> viewport;
    double width;
    double height;
    std::optional<std::string> userAgent;
    std::optional<std::unordered_map<std::string, std::string>> headers;
};

struct ScreenshotOptions {
    std::optional<bool> fullPage;
    std::optional<{> clip;
    double x;
    double y;
    double width;
    double height;
    std::optional<'png' | 'jpeg' | 'webp'> format;
    std::optional<double> quality;
    std::optional<bool> omitBackground;
};

struct ElementSelector {
    std::string selector;
    std::optional<std::string> text;
    std::optional<double> timeout;
};

struct ExtractedContent {
    std::string text;
    std::string html;
    Array<{ links;
    std::string url;
    std::string text;
    Array<{ images;
    std::string src;
    std::optional<std::string> alt;
    std::optional<std::string> title;
    std::optional<std::unordered_map<std::string, std::string>> metadata;
};

struct ClickOptions {
    std::optional<double> timeout;
    std::optional<bool> force;
    std::optional<bool> waitForNavigation;
};

struct TypeOptions {
    std::optional<double> delay;
    std::optional<double> timeout;
    std::optional<bool> clear;
};

/**
 * Interface for browser automation services
 */

  /**
   * Navigate to a URL
   * @param url - URL to navigate to
   * @param options - Navigation options
   * @returns Promise resolving when navigation completes
   */

  /**
   * Take a screenshot of the current page
   * @param options - Screenshot options
   * @returns Promise resolving to screenshot buffer
   */

  /**
   * Extract text and content from the current page
   * @param selector - Optional CSS selector to extract from specific element
   * @returns Promise resolving to extracted content
   */

  /**
   * Click on an element
   * @param selector - CSS selector or element selector
   * @param options - Click options
   * @returns Promise resolving when click completes
   */

  /**
   * Type text into an input field
   * @param selector - CSS selector for input field
   * @param text - Text to type
   * @param options - Typing options
   * @returns Promise resolving when typing completes
   */

  /**
   * Wait for an element to appear
   * @param selector - CSS selector or element selector
   * @returns Promise resolving when element is found
   */

  /**
   * Evaluate JavaScript in the browser context
   * @param script - JavaScript code to evaluate
   * @param args - Arguments to pass to the script
   * @returns Promise resolving to evaluation result
   */

  /**
   * Get the current page URL
   * @returns Promise resolving to current URL
   */

  /**
   * Go back in browser history
   * @returns Promise resolving when navigation completes
   */

  /**
   * Go forward in browser history
   * @returns Promise resolving when navigation completes
   */

  /**
   * Refresh the current page
   * @returns Promise resolving when refresh completes
   */

} // namespace elizaos
