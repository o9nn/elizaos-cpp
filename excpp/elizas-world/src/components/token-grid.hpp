#pragma once
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



// Helper function to format large numbers
std::string formatCurrency(double value);

std::string formatNumber(double value);

std::string getSocialUrl(const std::string& platform, const std::string& url);

std::string getSocialIcon(const std::string& type);

using SortConfig = std::variant<{

using TributeFilter = std::variant<std::string, std::string, std::string, std::string>;
using SocialFilter = std::variant<std::string, std::string, std::string>;

// Add this helper function near the top with the other helper functions
  
  // Check for Degen Spartan AI address specifically

  // Check for Eliza in the name (but not for the official address)
  
  // Check for vvaifu.fun URL

std::string truncateAddress(const std::string& address);

void TokenGrid(); 
} // namespace elizaos
