#pragma once
#include <functional>
#include <future>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#include "db.hpp"
#include "mcap.hpp"
#include "util.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



 // Import path for resolving asset path
 // Import fs for checking file existence

 // Assuming this is available and gives SOL price

// --- Helper Functions ---

// Simple fetch with timeout
std::future<Response> fetchWithTimeout(const std::string& resource, std::optional<RequestInit & { timeout: number } = {}> options);

// Format numbers nicely
std::string formatCurrency(const std::optional<double>& value, number = 2 decimals);

std::string formatMarketCap(const std::optional<double>& value);

// --- Main Generation Function ---

// Function to safely load the logo buffer - REMOVED as it's no longer used
/*
*/

std::future<Buffer> generateOgImage(const std::string& mint); 
} // namespace elizaos
