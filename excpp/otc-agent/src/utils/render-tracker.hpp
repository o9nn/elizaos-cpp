#pragma once
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use

/**
 * Development-only render tracking utility.
 * Tracks component renders and throws if a component renders too many times
 * in a short time window, indicating a render loop or performance issue.
 */

using RenderRecord = {

// Configuration
  /** Maximum renders allowed in the time window before throwing */
  /** Maximum renders allowed during initial mount period (higher to allow for StrictMode + library init) */
  /** Time window in milliseconds to track renders */
  /** Initial mount grace period in milliseconds (allow more renders during startup) */
  /** Enable verbose logging of every render */
  /** Components to ignore (some are expected to render frequently) */
    // Providers re-render during initialization due to multiple useEffects + StrictMode
    // TokenGroupLoader renders per token during grid load - expected behavior

/**
 * Get a serializable summary of a value for debugging
 */
std::string summarizeValue(unknown value);

/**
 * Get a props summary for debugging
 */
std::unordered_map<std::string, std::string> getPropsSnapshot(Record<string props, auto unknown>);

/**
 * Find what changed between two snapshots
 */
std::vector<std::string> findChanges(Record<string prev, auto string> | undefined, Record<string curr, auto string>);

/**
 * Track a component render. Call this at the top of your component.
 * In development, throws an error if the component renders excessively.
 *
 * @param componentName - Unique identifier for the component
 * @param props - Component props (optional, for debugging)
 * @param state - Component state snapshot (optional, for debugging)
 *
 * @example
 * ```tsx
 * void MyComponent(MyProps props);
 * ```
 */
void trackRender(const std::string& componentName, std::optional<Record<string> props, auto unknown>, std::optional<Record<string> state, auto unknown>);

/**
 * React hook version of trackRender for easier integration.
 *
 * @example
 * ```tsx
 * void MyComponent(MyProps props);
 * ```
 */
void useRenderTracker(const std::string& componentName, std::optional<Record<string> props, auto unknown>, std::optional<Record<string> state, auto unknown>);

} // namespace elizaos
