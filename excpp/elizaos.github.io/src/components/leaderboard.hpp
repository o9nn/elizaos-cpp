#pragma once
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>
#include "leaderboard-card.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



void LeaderboardFallback();

using LeaderboardPeriod = std::variant<"all", "weekly", "monthly">;

struct LeaderboardUser {
    std::string username;
    double points;
    double totalXp;
    double totalLevel;
    std::optional<std::optional<std::string>> avatarUrl;
    std::optional<std::string> category;
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

void Leaderboard(auto { tabs, LeaderboardProps tags });

struct PaginationControlsProps {
    double currentPage;
    double totalPages;
};


} // namespace elizaos
