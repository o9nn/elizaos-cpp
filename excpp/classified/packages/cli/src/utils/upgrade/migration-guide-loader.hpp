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
#include "elizaos/core.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



struct MigrationGuide {
    std::string name;
    std::string path;
    std::string content;
    std::string category;
    std::vector<std::string> keywords;
};

struct GuideSearchResult {
    MigrationGuide guide;
    double relevanceScore;
    std::vector<std::string> matchedKeywords;
};

class MigrationGuideLoader {
public:
    MigrationGuideLoader(std::optional<std::string> projectRoot);
    std::string findGuidesDirectory(std::optional<std::string> projectRoot);
    void loadGuides();
    void createEmbeddedGuides(const std::vector<std::any>& guideConfigs);
    std::string getEmbeddedMigrationGuide();
    std::string getEmbeddedTestingGuide();
    std::string getEmbeddedCompletionGuide();
    std::vector<GuideSearchResult> searchGuides(const std::string& query, double limit = 3);
    std::optional<MigrationGuide> getGuide(const std::string& name);
    std::vector<MigrationGuide> getGuidesByCategory();
    std::vector<GuideSearchResult> getRelevantGuidesForIssue(const std::string& issue);
    std::string generateMigrationContext();
    std::string getAllGuidesContent();

private:
    std::string guidesDir_;
};

/**
 * Helper function to create a guide loader instance
 */
MigrationGuideLoader createMigrationGuideLoader(std::optional<std::string> projectRoot);

/**
 * Helper function to get migration context for common issues
 */
std::string getMigrationHelpForIssue(const std::string& issue, std::optional<std::string> projectRoot);

} // namespace elizaos
