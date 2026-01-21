// classified.cpp - Game and challenge system for ElizaOS
// Provides gamification, challenges, achievements, and leaderboards

#include "elizaos/core.hpp"
#include <string>
#include <vector>
#include <unordered_map>
#include <mutex>
#include <chrono>
#include <algorithm>
#include <ctime>

namespace elizaos {
namespace classified {

// ==============================================================================
// DATA STRUCTURES
// ==============================================================================

struct Challenge {
    std::string id;
    std::string title;
    std::string description;
    std::string category;
    int points;
    int difficulty;  // 1-5
    bool isActive;
    std::chrono::system_clock::time_point createdAt;
    std::chrono::system_clock::time_point expiresAt;
};

struct Achievement {
    std::string id;
    std::string title;
    std::string description;
    std::string icon;
    int points;
    bool isUnlocked;
    std::chrono::system_clock::time_point unlockedAt;
};

struct PlayerStats {
    std::string playerId;
    int totalPoints;
    int level;
    int challengesCompleted;
    int achievementsUnlocked;
    std::chrono::system_clock::time_point joinedAt;
    std::chrono::system_clock::time_point lastActiveAt;
};

struct LeaderboardEntry {
    std::string playerId;
    std::string playerName;
    int points;
    int rank;
};

// ==============================================================================
// CHALLENGE MANAGER
// ==============================================================================

class ChallengeManager {
private:
    std::unordered_map<std::string, Challenge> challenges_;
    std::unordered_map<std::string, std::vector<std::string>> playerChallenges_;
    std::mutex mutex_;
    
public:
    ChallengeManager() {
        initializeDefaultChallenges();
    }
    
    void initializeDefaultChallenges() {
        std::lock_guard<std::mutex> lock(mutex_);
        
        auto now = std::chrono::system_clock::now();
        auto oneWeek = std::chrono::hours(24 * 7);
        
        challenges_["daily_login"] = {
            "daily_login",
            "Daily Login",
            "Log in to ElizaOS every day for 7 days",
            "engagement",
            100,
            1,
            true,
            now,
            now + oneWeek
        };
        
        challenges_["first_agent"] = {
            "first_agent",
            "First Agent",
            "Create your first AI agent",
            "onboarding",
            50,
            1,
            true,
            now,
            now + oneWeek
        };
        
        challenges_["agent_master"] = {
            "agent_master",
            "Agent Master",
            "Create 10 different AI agents",
            "creation",
            500,
            3,
            true,
            now,
            now + oneWeek
        };
        
        challenges_["conversation_starter"] = {
            "conversation_starter",
            "Conversation Starter",
            "Have 100 conversations with your agents",
            "interaction",
            200,
            2,
            true,
            now,
            now + oneWeek
        };
        
        challenges_["memory_keeper"] = {
            "memory_keeper",
            "Memory Keeper",
            "Store 1000 memories in agent memory",
            "data",
            300,
            3,
            true,
            now,
            now + oneWeek
        };
    }
    
    std::vector<Challenge> getActiveChallenges() {
        std::lock_guard<std::mutex> lock(mutex_);
        std::vector<Challenge> active;
        
        auto now = std::chrono::system_clock::now();
        for (const auto& pair : challenges_) {
            if (pair.second.isActive && pair.second.expiresAt > now) {
                active.push_back(pair.second);
            }
        }
        
        return active;
    }
    
    Challenge getChallenge(const std::string& challengeId) {
        std::lock_guard<std::mutex> lock(mutex_);
        auto it = challenges_.find(challengeId);
        return (it != challenges_.end()) ? it->second : Challenge{};
    }
    
    bool completeChallenge(const std::string& playerId, const std::string& challengeId) {
        std::lock_guard<std::mutex> lock(mutex_);
        
        auto it = challenges_.find(challengeId);
        if (it == challenges_.end() || !it->second.isActive) {
            return false;
        }
        
        playerChallenges_[playerId].push_back(challengeId);
        return true;
    }
    
    std::vector<std::string> getPlayerChallenges(const std::string& playerId) {
        std::lock_guard<std::mutex> lock(mutex_);
        auto it = playerChallenges_.find(playerId);
        return (it != playerChallenges_.end()) ? it->second : std::vector<std::string>{};
    }
};

// ==============================================================================
// ACHIEVEMENT MANAGER
// ==============================================================================

class AchievementManager {
private:
    std::unordered_map<std::string, Achievement> achievements_;
    std::unordered_map<std::string, std::vector<std::string>> playerAchievements_;
    std::mutex mutex_;
    
public:
    AchievementManager() {
        initializeDefaultAchievements();
    }
    
    void initializeDefaultAchievements() {
        std::lock_guard<std::mutex> lock(mutex_);
        
        achievements_["welcome"] = {
            "welcome",
            "Welcome to ElizaOS",
            "Complete your first action",
            "🎉",
            10,
            false,
            std::chrono::system_clock::now()
        };
        
        achievements_["agent_creator"] = {
            "agent_creator",
            "Agent Creator",
            "Create your first AI agent",
            "🤖",
            50,
            false,
            std::chrono::system_clock::now()
        };
        
        achievements_["conversation_expert"] = {
            "conversation_expert",
            "Conversation Expert",
            "Have 1000 conversations",
            "💬",
            200,
            false,
            std::chrono::system_clock::now()
        };
        
        achievements_["memory_master"] = {
            "memory_master",
            "Memory Master",
            "Store 10000 memories",
            "🧠",
            500,
            false,
            std::chrono::system_clock::now()
        };
        
        achievements_["elite_agent"] = {
            "elite_agent",
            "Elite Agent",
            "Reach level 50",
            "⭐",
            1000,
            false,
            std::chrono::system_clock::now()
        };
    }
    
    std::vector<Achievement> getAllAchievements() {
        std::lock_guard<std::mutex> lock(mutex_);
        std::vector<Achievement> all;
        
        for (const auto& pair : achievements_) {
            all.push_back(pair.second);
        }
        
        return all;
    }
    
    bool unlockAchievement(const std::string& playerId, const std::string& achievementId) {
        std::lock_guard<std::mutex> lock(mutex_);
        
        auto it = achievements_.find(achievementId);
        if (it == achievements_.end()) {
            return false;
        }
        
        // Check if already unlocked
        auto& playerAchs = playerAchievements_[playerId];
        if (std::find(playerAchs.begin(), playerAchs.end(), achievementId) != playerAchs.end()) {
            return false;
        }
        
        playerAchs.push_back(achievementId);
        it->second.isUnlocked = true;
        it->second.unlockedAt = std::chrono::system_clock::now();
        
        return true;
    }
    
    std::vector<Achievement> getPlayerAchievements(const std::string& playerId) {
        std::lock_guard<std::mutex> lock(mutex_);
        std::vector<Achievement> playerAchs;
        
        auto it = playerAchievements_.find(playerId);
        if (it != playerAchievements_.end()) {
            for (const auto& achId : it->second) {
                auto achIt = achievements_.find(achId);
                if (achIt != achievements_.end()) {
                    playerAchs.push_back(achIt->second);
                }
            }
        }
        
        return playerAchs;
    }
};

// ==============================================================================
// LEADERBOARD MANAGER
// ==============================================================================

class LeaderboardManager {
private:
    std::unordered_map<std::string, PlayerStats> playerStats_;
    std::mutex mutex_;
    
public:
    void updatePlayerStats(const std::string& playerId, int pointsToAdd) {
        std::lock_guard<std::mutex> lock(mutex_);
        
        auto& stats = playerStats_[playerId];
        stats.playerId = playerId;
        stats.totalPoints += pointsToAdd;
        stats.level = calculateLevel(stats.totalPoints);
        stats.lastActiveAt = std::chrono::system_clock::now();
        
        if (stats.joinedAt == std::chrono::system_clock::time_point{}) {
            stats.joinedAt = std::chrono::system_clock::now();
        }
    }
    
    PlayerStats getPlayerStats(const std::string& playerId) {
        std::lock_guard<std::mutex> lock(mutex_);
        auto it = playerStats_.find(playerId);
        return (it != playerStats_.end()) ? it->second : PlayerStats{};
    }
    
    std::vector<LeaderboardEntry> getTopPlayers(int limit = 10) {
        std::lock_guard<std::mutex> lock(mutex_);
        
        std::vector<LeaderboardEntry> entries;
        for (const auto& pair : playerStats_) {
            entries.push_back({
                pair.second.playerId,
                pair.second.playerId,  // In real system, fetch player name
                pair.second.totalPoints,
                0  // Will be set after sorting
            });
        }
        
        // Sort by points descending
        std::sort(entries.begin(), entries.end(), 
                 [](const LeaderboardEntry& a, const LeaderboardEntry& b) {
                     return a.points > b.points;
                 });
        
        // Assign ranks
        for (size_t i = 0; i < entries.size(); ++i) {
            entries[i].rank = static_cast<int>(i + 1);
        }
        
        // Return top N
        if (entries.size() > static_cast<size_t>(limit)) {
            entries.resize(limit);
        }
        
        return entries;
    }
    
private:
    int calculateLevel(int points) {
        // Simple level calculation: 100 points per level
        return (points / 100) + 1;
    }
};

// ==============================================================================
// CLASSIFIED GAME SYSTEM
// ==============================================================================

class ClassifiedGameSystem {
private:
    ChallengeManager challengeManager_;
    AchievementManager achievementManager_;
    LeaderboardManager leaderboardManager_;
    std::mutex mutex_;
    
    static ClassifiedGameSystem* instance_;
    
    ClassifiedGameSystem() = default;
    
public:
    static ClassifiedGameSystem& getInstance() {
        static std::mutex instanceMutex;
        std::lock_guard<std::mutex> lock(instanceMutex);
        
        if (!instance_) {
            instance_ = new ClassifiedGameSystem();
        }
        
        return *instance_;
    }
    
    // Challenge operations
    std::vector<Challenge> getActiveChallenges() {
        return challengeManager_.getActiveChallenges();
    }
    
    bool completeChallenge(const std::string& playerId, const std::string& challengeId) {
        bool success = challengeManager_.completeChallenge(playerId, challengeId);
        
        if (success) {
            Challenge challenge = challengeManager_.getChallenge(challengeId);
            leaderboardManager_.updatePlayerStats(playerId, challenge.points);
        }
        
        return success;
    }
    
    // Achievement operations
    std::vector<Achievement> getAllAchievements() {
        return achievementManager_.getAllAchievements();
    }
    
    bool unlockAchievement(const std::string& playerId, const std::string& achievementId) {
        return achievementManager_.unlockAchievement(playerId, achievementId);
    }
    
    std::vector<Achievement> getPlayerAchievements(const std::string& playerId) {
        return achievementManager_.getPlayerAchievements(playerId);
    }
    
    // Leaderboard operations
    std::vector<LeaderboardEntry> getLeaderboard(int limit = 10) {
        return leaderboardManager_.getTopPlayers(limit);
    }
    
    PlayerStats getPlayerStats(const std::string& playerId) {
        return leaderboardManager_.getPlayerStats(playerId);
    }
    
    void addPoints(const std::string& playerId, int points) {
        leaderboardManager_.updatePlayerStats(playerId, points);
    }
};

ClassifiedGameSystem* ClassifiedGameSystem::instance_ = nullptr;

// ==============================================================================
// EXPORTED API
// ==============================================================================

void initializeGameSystem() {
    ClassifiedGameSystem::getInstance();
}

std::vector<Challenge> getActiveChallenges() {
    return ClassifiedGameSystem::getInstance().getActiveChallenges();
}

bool completeChallenge(const std::string& playerId, const std::string& challengeId) {
    return ClassifiedGameSystem::getInstance().completeChallenge(playerId, challengeId);
}

std::vector<Achievement> getAllAchievements() {
    return ClassifiedGameSystem::getInstance().getAllAchievements();
}

bool unlockAchievement(const std::string& playerId, const std::string& achievementId) {
    return ClassifiedGameSystem::getInstance().unlockAchievement(playerId, achievementId);
}

std::vector<LeaderboardEntry> getLeaderboard(int limit) {
    return ClassifiedGameSystem::getInstance().getLeaderboard(limit);
}

PlayerStats getPlayerStats(const std::string& playerId) {
    return ClassifiedGameSystem::getInstance().getPlayerStats(playerId);
}

} // namespace classified
} // namespace elizaos
