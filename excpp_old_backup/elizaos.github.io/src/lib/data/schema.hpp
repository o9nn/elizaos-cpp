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



// User table - stores basic user information

// Wallet addresses table - stores user wallet addresses across different chains

// Repositories being tracked

// Raw GitHub data tables

// Processed data tables
    // Unique constraint for username, interval type, and date combination

// Repository summaries for storing monthly project analysis
    // Unique constraint for repoId, interval type, and date combination

// Labels table for storing unique labels

// Junction table for PR-Label relationships

// Junction table for Issue-Label relationships

// User daily scores for leaderboard and time-based analytics

// Now define all relations after all tables are defined

// Add relations for labels

// Add relations for repoSummaries

// Add relations for userDailyScores

// Add relations for tags

// New tables for reactions and closing issue references

// Reactions on PRs

// Reactions on PR comments

// Reactions on issues

// Reactions on issue comments

// Closing issue references for PRs

} // namespace elizaos
