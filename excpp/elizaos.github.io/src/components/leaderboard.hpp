#pragma once
#include <algorithm>
#include <cstdint>
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include "leaderboard-card.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



void LeaderboardFallback();

using LeaderboardPeriod = std::string;

struct LeaderboardUser {
    std::string username;
    double points;
    double totalXp;
    double totalLevel;
    std::optional<std::string> avatarUrl;
    std::string category;
    std::vector<LinkedWallet> linkedWallets;
};

struct LeaderboardTab {
    LeaderboardPeriod id;
    std::string title;
    std::optional<std::string> startDate;
    std::optional<std::string> endDate;
    std::vector<LeaderboardUser> users;
};

struct LeaderboardProps {
    std::vector<LeaderboardTab> tabs;
};

void Leaderboard();

struct PaginationControlsProps {
    double currentPage;
    double totalPages;
};

void PaginationControls(auto totalPages, auto onPageChange);

} // namespace elizaos
