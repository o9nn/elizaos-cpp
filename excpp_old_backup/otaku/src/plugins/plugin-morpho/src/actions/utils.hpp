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



std::string fmtUSD(BigNumber.Value v, auto dp = 2);

std::string fmtPct(number | null | undefined v, auto dp = 2);

std::string fmtTok(BigNumber.Value v, const std::string& sym, auto dp = 6);

std::string fmtNum(BigNumber.Value v, auto dp = 2);

void shortHex(std::optional<std::string> id, auto prefix = 6, auto suffix = 6);

std::string fmtAbbrev(BigNumber.Value v, auto digits = 0);

std::string fmtTokCompact(BigNumber.Value v, const std::string& sym, auto digits = 0);

/**
 * Format data as a clean list for Discord display (no tables or emojis)
 */
std::string formatDataList(const std::string& title, Array<{ name: string; data: Record<string items, auto string> }>);

/**
 * Format a single item with key-value pairs in a clean format
 */
std::string formatItemDetails(const std::string& title, Record<string data, auto string>, std::optional<std::string> link);

} // namespace elizaos
