#pragma once
#include <functional>
#include <future>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



/**
 * Checks if a given port is free.
 * @param port The port number to check.
 * @returns Promise<boolean> indicating if the port is free.
 */
std::future<bool> isPortFree(double port);

/**
 * Finds the next available port starting from the given port.
 * @param startPort The initial port to check.
 * @returns Promise<number> The next available port.
 */
std::future<double> findNextAvailablePort(double startPort);

} // namespace elizaos
