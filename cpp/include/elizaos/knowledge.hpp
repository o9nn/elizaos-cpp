#pragma once

/**
 * ElizaOS C++ - Knowledge Module
 *
 * Semantic knowledge base with type/tag/relation management,
 * inference engine, and JSON import/export.
 */

#include "elizaos.hpp"
#include "core.hpp"
#include <chrono>
#include <functional>
#include <memory>
#include <mutex>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

namespace elizaos {

// ============================================================================
// KnowledgeType / ConfidenceLevel / KnowledgeSource
// ============================================================================

enum class KnowledgeType {
    FACT,
    RULE,
    CONCEPT,
    PROCEDURE,
    BELIEF,
    EXPERIENCE,
    INFERENCE,
    UNKNOWN
};

enum class ConfidenceLevel {
    VERY_LOW,
    LOW,
    MEDIUM,
    HIGH,
    VERY_HIGH,
    CERTAIN
};

enum class KnowledgeSource {
    PROGRAMMED,
    LEARNED,
    INFERRED,
    USER_PROVIDED,
    EXTERNAL,
    INTERNAL
};

// ============================================================================
// KnowledgeEntry
// ============================================================================

struct KnowledgeEntry {
    std::string     id;
    std::string     content;
    KnowledgeType   type       = KnowledgeType::FACT;
    ConfidenceLevel confidence = ConfidenceLevel::MEDIUM;
    KnowledgeSource source     = KnowledgeSource::PROGRAMMED;
    std::vector<std::string> tags;
    std::vector<std::string> related_entries;
    std::chrono::system_clock::time_point createdAt;
    std::chrono::system_clock::time_point updatedAt;

    KnowledgeEntry() = default;
    KnowledgeEntry(const std::string& content, KnowledgeType type = KnowledgeType::FACT);

    void addTag(const std::string& tag);
    void addRelation(const std::string& entryId);
    void updateConfidence(ConfidenceLevel newConfidence);
    bool hasTag(const std::string& tag) const;

    JsonValue toJson() const;
    static KnowledgeEntry fromJson(const JsonValue& json);
};

// ============================================================================
// KnowledgeQuery
// ============================================================================

struct KnowledgeQuery {
    std::string              text;
    std::vector<std::string> tags;
    KnowledgeType            type   = KnowledgeType::UNKNOWN;
    int                      limit  = 10;
    ConfidenceLevel          minConfidence = ConfidenceLevel::VERY_LOW;

    explicit KnowledgeQuery(const std::string& queryText = "");
};

// ============================================================================
// KnowledgeInferenceEngine
// ============================================================================

using InferenceRule = std::function<std::optional<KnowledgeEntry>(const KnowledgeEntry&, const KnowledgeEntry&)>;

class KnowledgeInferenceEngine {
public:
    KnowledgeInferenceEngine();

    std::vector<KnowledgeEntry> inferFromFacts(const std::vector<KnowledgeEntry>& facts);
    std::vector<KnowledgeEntry> findRelatedConcepts(const KnowledgeEntry& entry);
    KnowledgeEntry combineEvidence(const std::vector<KnowledgeEntry>& evidence);

    void addInferenceRule(const std::string& ruleName, InferenceRule rule);
    void removeInferenceRule(const std::string& ruleName);

private:
    std::unordered_map<std::string, InferenceRule> inferenceRules_;
};

// ============================================================================
// KnowledgeBase
// ============================================================================

class KnowledgeBase {
public:
    KnowledgeBase();

    // CRUD
    std::string                  addKnowledge(const KnowledgeEntry& entry);
    bool                         updateKnowledge(const std::string& id, const KnowledgeEntry& entry);
    bool                         removeKnowledge(const std::string& id);
    std::optional<KnowledgeEntry> getKnowledge(const std::string& id);

    // Query
    std::vector<KnowledgeEntry>  query(const KnowledgeQuery& q);
    std::vector<KnowledgeEntry>  searchByText(const std::string& text, int maxResults = 10);
    std::vector<KnowledgeEntry>  searchByTags(const std::vector<std::string>& tags, int maxResults = 10);
    std::vector<KnowledgeEntry>  getRelatedKnowledge(const std::string& entryId, int maxResults = 10);
    std::vector<KnowledgeEntry>  getKnowledgeByType(KnowledgeType type);

    // Maintenance
    void validateKnowledge();
    void pruneOldKnowledge(std::chrono::hours maxAge = std::chrono::hours(24 * 30));
    void consolidateKnowledge();
    void clear();

    // Analytics
    std::vector<std::string>                      getAllTags() const;
    std::unordered_map<KnowledgeType, int>        getKnowledgeTypeStats() const;
    size_t                                         getKnowledgeCount() const;
    std::string                                    getStatistics() const;

    // Inference
    std::vector<KnowledgeEntry>  performInference(const KnowledgeQuery& q);
    void setInferenceEngine(std::shared_ptr<KnowledgeInferenceEngine> engine);

    // Persistence
    bool exportToFile(const std::string& filename) const;
    bool importFromFile(const std::string& filename);
    JsonValue exportToJson() const;
    bool importFromJson(const JsonValue& data);

private:
    std::string generateKnowledgeId();

    std::unordered_map<std::string, KnowledgeEntry> knowledgeStore_;
    std::shared_ptr<KnowledgeInferenceEngine>        inferenceEngine_;
    mutable std::mutex                               kbMutex_;
};

// ============================================================================
// Global instance
// ============================================================================

extern std::shared_ptr<KnowledgeBase> globalKnowledgeBase;

} // namespace elizaos
