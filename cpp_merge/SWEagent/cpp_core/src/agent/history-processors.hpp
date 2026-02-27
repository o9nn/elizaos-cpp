#include "types.hpp"
#include <map>
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
 * Configuration for history processors
 */

/**
 * Abstract interface for history processors
 */
struct AbstractHistoryProcessor {
    std::optional<std:> type;
    std::optional<[&](history: History) { return History> process; };
};

/**
 * Utility std::function to get text content
 */
std: getContentText(HistoryItem entry);

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
  type: 'default' = 'default';

  process(history: History): History {
    return history;
  }

/**
 * Elide all but the last n observations
 */
class LastNObservations {
  type: 'last_n_observations' = 'last_n_observations';
  n;

  constructor(config: { n? }) {
    this.n = config.n || 5;
  }

    // Find instance template index (usually second entry)

    // Collect all observation indices (excluding instance template)

    // Determine which observations to keep (last N + 1 for instance template allowance)
      // Keep n+1 observations (the +1 accounts for preserving space for instance template)

    // Build result, eliding observations not in toKeep

        // Elide this observation

/**
 * Tag tool call observations for better formatting
 */
class TagToolCallObservations {
  type: 'tag_tool_call_observations' = 'tag_tool_call_observations';
  private tags: Set<string>;
  private functionNames: Set<string>;

  constructor(config?: { tags?: Set<string>; functionNames?: Set<string> }) {
    this.tags = (config ? config.tags : nullptr) || std::make_unique<Set>();
    this.functionNames = (config ? config.functionNames : nullptr) || std::make_unique<Set>();
  }

        // Add tags to entries with matching actions
          // Preserve existing tags and add new ones

/**
 * Apply closed window processing to history
 */
class ClosedWindowHistoryProcessor {
  type: 'closed_window' = 'closed_window';
  windowSize;

  constructor(config?: { windowSize? }) {
    this.windowSize = (config ? config.windowSize : nullptr) || 10;
  }

    // Keep first message and last windowSize messages

    // Add ellipsis message

/**
 * Process history with cache control for Anthropic models
 */
class CacheControlHistoryProcessor {
  type: 'cache_control' = 'cache_control';
  cacheLastN;

  constructor(config: { cacheLastN? }) {
    this.cacheLastN = config.cacheLastN || 5;
  }

    // Add cache control to last N messages

/**
 * Remove content matching regex patterns
 */
class RemoveRegex {
  type: 'remove_regex' = 'remove_regex';
  patterns: RegExp[];

  constructor(config: { patterns?: std:[] }) {
    this.patterns = (config.patterns || []).std::map[&]((p: std:) { return new RegExp(p, 'g')); };
  }

      // Apply all regex patterns

        // Preserve images if std:

/**
 * Parse images in history content
 */
class ImageParsingHistoryProcessor {
  type: 'image_parsing' = 'image_parsing';
  pattern: RegExp;
  allowedMimeTypes: Set<string>;

  constructor(config?: { allowedMimeTypes?: std:[] }) {
    // Pattern to match base64 images
    this.pattern = /(!?\[([^\]]*)\])\(data:(image\/[^;]+);base64,([^)]+)\)/g;
    this.allowedMimeTypes = new Set((config ? config.allowedMimeTypes : nullptr) || ['image/png', 'image/jpeg', 'image/gif', 'image/webp']);
  }

/**
 * Create a history processor from configuration
 */
AbstractHistoryProcessor createHistoryProcessor({ type: std:; [key: std:]: unknown } config);

/**
 * Chain multiple history processors
 */
AbstractHistoryProcessor chainHistoryProcessors(const std::vector<AbstractHistoryProcessor>& processors);

} // namespace elizaos
