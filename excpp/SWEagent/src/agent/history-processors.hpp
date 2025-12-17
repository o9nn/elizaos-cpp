#pragma once
#include <any>
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#include ".types.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



/**
 * Configuration for history processors
 */

/**
 * Abstract interface for history processors
 */
struct AbstractHistoryProcessor {
    std::optional<std::string> type;
};

/**
 * Utility function to get text content
 */
std::string getContentText(HistoryItem entry);

/**
 * Add cache control to an entry
 */
void addCacheControlToEntry(HistoryItem entry);

/**
 * Helper to make a class callable as AbstractHistoryProcessor
 */

/**
 * Default history processor that returns history unchanged
 */
class DefaultHistoryProcessor {
public:
    History process(History history);
};

/**
 * Elide all but the last n observations
 */
class LastNObservations {
public:
    LastNObservations(std::optional<std::any> config);
    History process(History history);

private:
    double n_;
};

/**
 * Tag tool call observations for better formatting
 */
class TagToolCallObservations {
public:
    TagToolCallObservations(std::optional<std::any> config);
    History process(History history);
    bool shouldTag(const std::string& action);

private:
    std::unordered_set<std::string> tags_;
    std::unordered_set<std::string> functionNames_;
};

/**
 * Apply closed window processing to history
 */
class ClosedWindowHistoryProcessor {
public:
    ClosedWindowHistoryProcessor(std::optional<std::any> config);
    History process(History history);

private:
    double windowSize_;
};

/**
 * Process history with cache control for Anthropic models
 */
class CacheControlHistoryProcessor {
public:
    CacheControlHistoryProcessor(std::optional<std::any> config);
    History process(History history);

private:
    double cacheLastN_;
};

/**
 * Remove content matching regex patterns
 */
class RemoveRegex {
public:
    RemoveRegex(std::optional<std::any> config);
    History process(History history);

private:
    std::vector<RegExp> patterns_;
};

/**
 * Parse images in history content
 */
class ImageParsingHistoryProcessor {
public:
    ImageParsingHistoryProcessor(std::optional<std::any> config);
    History process(History history);
    Array< parseImageContent(const std::string& content);

private:
    RegExp pattern_;
    std::unordered_set<std::string> allowedMimeTypes_;
};

/**
 * Create a history processor from configuration
 */
AbstractHistoryProcessor createHistoryProcessor(const std::any& config);

/**
 * Chain multiple history processors
 */
AbstractHistoryProcessor chainHistoryProcessors(const std::vector<AbstractHistoryProcessor>& processors);

} // namespace elizaos
