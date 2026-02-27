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



std: fmtUSD(BigNumber::Value v, auto dp);

std: fmtPct(const std::optional<double>& v, auto dp);

std: fmtTok(BigNumber::Value v, const std:& sym, auto dp);

std: fmtNum(BigNumber::Value v, auto dp);

void shortHex(std::optional<std:> id, auto prefix, auto suffix);

std: fmtAbbrev(BigNumber::Value v, auto digits);

std: fmtTokCompact(BigNumber::Value v, const std:& sym, auto digits);

/**
 * Format data as a clean list for Discord display (no tables or emojis)
 */
std: formatDataList(const std:& title, const std::vector<std::string>& items);

/**
 * Format a single item with key-value pairs in a clean format
 */
std: formatItemDetails(const std:& title, const std::unordered_map<std:, std:>& data, std::optional<std:> link);

} // namespace elizaos
