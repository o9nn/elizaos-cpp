#pragma once
#include <algorithm>
#include <cstdint>
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <variant>
#include <vector>
#include "sentry/instrument.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



// Local utility function to avoid circular dependency
bool parseBooleanFromText(const std::string& value);

/**
 * Interface representing a log entry.
 * @property {number} [time] - The timestamp of the log entry.
 * @property {unknown} [key] - Additional properties that can be added to the log entry.
 */
/**
 * Interface representing a log entry.
 * @typedef {Object} LogEntry
 * @property {number} [time] - The time the log entry was created.
 * @property {string} key - The key for the log entry.
 * @property {unknown} value - The value associated with the key in the log entry.
 */
struct LogEntry {
    std::optional<double> time;
};

// Custom destination that maintains recent logs in memory
/**
 * Class representing an in-memory destination stream for logging.
 * Implements DestinationStream interface.
 */
class InMemoryDestination {
public:
    InMemoryDestination(const std::optional<DestinationStream>& stream);
    void write(const std::variant<std::string, LogEntry>& data);
    std::vector<LogEntry> recentLogs();
    void clear();

private:
    std::optional<DestinationStream> stream_;
};

// Set default log level to info to allow regular logs, but still filter service logs

// Create a function to generate the pretty configuration

    // Add a custom prettifier for error messages
      // Replace "ERROR (TypeError):" pattern with just "ERROR:"

// Create options with appropriate level

            return formatError(arg);

// allow runtime logger to inherent options set here

// Create basic logger initially
// Add type for logger with clear method

// Enhance logger with custom destination in Node.js environment
  // Create the destination with in-memory logging
  // Instead of async initialization, initialize synchronously to avoid race conditions

    // If we're in a Node.js environment where require is available, use require for pino-pretty
    // This will ensure synchronous loading
      // Fall back to async loading if synchronous loading fails

        // Add clear method to logger

  // If stream was created synchronously, use it now

    // Add clear method to logger

// for backward compatibility


} // namespace elizaos
