#pragma once
#include <algorithm>
#include <cstdint>
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include "service.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



struct BrowserNavigationOptions {
    std::optional<double> timeout;
    std::optional<std::string> waitUntil;
    double width;
    double height;
    std::optional<std::string> userAgent;
};

struct ScreenshotOptions {
    std::optional<bool> fullPage;
    double x;
    double y;
    double width;
    double height;
    std::optional<std::string> format;
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
    std::string url;
    std::string text;
    std::string src;
    std::optional<std::string> alt;
    std::optional<std::string> title;
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


} // namespace elizaos
