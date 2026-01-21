#pragma once
#include <algorithm>
#include <any>
#include <cstdint>
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



std::string fmtUSD(BigNumber::Value v, auto dp);

std::string fmtPct(const std::optional<double>& v, auto dp);

std::string fmtTok(BigNumber::Value v, const std::string& sym, auto dp);

std::string fmtNum(BigNumber::Value v, auto dp);

void shortHex(std::optional<std::string> id, auto prefix, auto suffix);

std::string fmtAbbrev(BigNumber::Value v, auto digits);

std::string fmtTokCompact(BigNumber::Value v, const std::string& sym, auto digits);

/**
 * Format data as a clean list for Discord display (no tables or emojis)
 */
std::string formatDataList(const std::string& title, const std::vector<std::any>& items);

/**
 * Format a single item with key-value pairs in a clean format
 */
std::string formatItemDetails(const std::string& title, const std::unordered_map<std::string, std::string>& data, std::optional<std::string> link);

} // namespace elizaos
