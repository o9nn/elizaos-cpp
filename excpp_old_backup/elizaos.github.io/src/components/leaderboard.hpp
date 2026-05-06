#include "leaderboard-card.hpp"
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



void LeaderboardFallback();

using LeaderboardPeriod = std::variant<"all", "weekly", "monthly">;

struct LeaderboardUser {
    std::string username;
    double points;
    double totalXp;
    double totalLevel;
    std::optional<string | null> avatarUrl;
    std::vector<{ tagName: string; category: string | null; score: number }> allTags;

struct LeaderboardTab {
    LeaderboardPeriod id;
    std::string title;
    std::optional<std::string> startDate;
    std::optional<std::string> endDate;
    std::vector<LeaderboardUser> users;
};

struct LeaderboardProps {
    std::vector<LeaderboardTab> tabs;
    std::vector<{ name: string; category: string }> tags;

void Leaderboard(auto { tabs, LeaderboardProps tags });

struct PaginationControlsProps {
    double currentPage;
    double totalPages;
    (page: number) => void onPageChange;
};


} // namespace elizaos
