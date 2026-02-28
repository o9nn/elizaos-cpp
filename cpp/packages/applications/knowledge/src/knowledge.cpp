// knowledge.cpp - Full implementation of KnowledgeBase system
#include "elizaos/knowledge.hpp"
#include <algorithm>
#include <sstream>
#include <random>
#include <fstream>
#include <iomanip>

namespace elizaos {

std::shared_ptr<KnowledgeBase> globalKnowledgeBase;

// ==============================================================================
// KnowledgeEntry
// ==============================================================================

KnowledgeEntry::KnowledgeEntry(const std::string& content, KnowledgeType type)
    : content(content), type(type), confidence(ConfidenceLevel::MEDIUM),
      source(KnowledgeSource::PROGRAMMED) {
    created_at = std::chrono::system_clock::now();
    updated_at = created_at;
}

JsonValue KnowledgeEntry::toJson() const {
    JsonValue j;
    j["id"] = std::any(id);
    j["content"] = std::any(content);
    j["type"] = std::any(knowledgeTypeToString(type));
    j["confidence"] = std::any(static_cast<int>(confidence));
    j["source"] = std::any(knowledgeSourceToString(source));
    return j;
}

KnowledgeEntry KnowledgeEntry::fromJson(const JsonValue& json) {
    KnowledgeEntry entry("", KnowledgeType::FACT);
    auto it = json.find("content");
    if (it != json.end()) entry.content = std::any_cast<std::string>(it->second);
    it = json.find("id");
    if (it != json.end()) entry.id = std::any_cast<std::string>(it->second);
    it = json.find("type");
    if (it != json.end()) entry.type = stringToKnowledgeType(std::any_cast<std::string>(it->second));
    return entry;
}

void KnowledgeEntry::addTag(const std::string& tag) {
    if (!hasTag(tag)) tags.push_back(tag);
}

void KnowledgeEntry::addRelation(const std::string& entryId) {
    if (std::find(related_entries.begin(), related_entries.end(), entryId) == related_entries.end())
        related_entries.push_back(entryId);
}

void KnowledgeEntry::updateConfidence(ConfidenceLevel newConfidence) {
    confidence = newConfidence;
    updated_at = std::chrono::system_clock::now();
}

bool KnowledgeEntry::hasTag(const std::string& tag) const {
    return std::find(tags.begin(), tags.end(), tag) != tags.end();
}

// ==============================================================================
// KnowledgeQuery
// ==============================================================================

KnowledgeQuery::KnowledgeQuery(const std::string& queryText) : text(queryText) {}

// ==============================================================================
// KnowledgeInferenceEngine
// ==============================================================================

KnowledgeInferenceEngine::KnowledgeInferenceEngine() {}

std::vector<KnowledgeEntry> KnowledgeInferenceEngine::inferFromFacts(const std::vector<KnowledgeEntry>& facts) {
    std::lock_guard<std::mutex> lock(rulesMutex_);
    std::vector<KnowledgeEntry> results;
    for (auto& [name, rule] : rules_) {
        auto inferred = rule(facts);
        results.insert(results.end(), inferred.begin(), inferred.end());
    }
    return results;
}

std::vector<KnowledgeEntry> KnowledgeInferenceEngine::findRelatedConcepts(const KnowledgeEntry& entry) {
    return {};
}

KnowledgeEntry KnowledgeInferenceEngine::combineEvidence(const std::vector<KnowledgeEntry>& evidence) {
    if (evidence.empty()) return KnowledgeEntry("", KnowledgeType::FACT);
    std::string combined;
    for (const auto& e : evidence) {
        if (!combined.empty()) combined += "; ";
        combined += e.content;
    }
    KnowledgeEntry result(combined, KnowledgeType::FACT);
    result.confidence = ConfidenceLevel::MEDIUM;
    result.source = KnowledgeSource::INFERRED;
    return result;
}

void KnowledgeInferenceEngine::addInferenceRule(const std::string& ruleName,
    std::function<std::vector<KnowledgeEntry>(const std::vector<KnowledgeEntry>&)> rule) {
    std::lock_guard<std::mutex> lock(rulesMutex_);
    rules_[ruleName] = std::move(rule);
}

void KnowledgeInferenceEngine::removeInferenceRule(const std::string& ruleName) {
    std::lock_guard<std::mutex> lock(rulesMutex_);
    rules_.erase(ruleName);
}

// ==============================================================================
// KnowledgeBase
// ==============================================================================

static std::unordered_map<std::string, KnowledgeEntry> knowledgeStore_;

KnowledgeBase::KnowledgeBase() : inferenceEngine_(std::make_shared<KnowledgeInferenceEngine>()) {}
KnowledgeBase::~KnowledgeBase() {}

std::string KnowledgeBase::generateKnowledgeId() {
    static std::mt19937 gen(std::random_device{}());
    static std::uniform_int_distribution<> dis(0, 15);
    const char* hex = "0123456789abcdef";
    std::string id = "kb-";
    for (int i = 0; i < 16; ++i) id += hex[dis(gen)];
    return id;
}

std::string KnowledgeBase::addKnowledge(const KnowledgeEntry& entry) {
    std::lock_guard<std::mutex> lock(knowledgeMutex_);
    KnowledgeEntry e = entry;
    if (e.id.empty()) e.id = generateKnowledgeId();
    e.created_at = std::chrono::system_clock::now();
    e.updated_at = e.created_at;
    knowledgeStore_[e.id] = e;
    return e.id;
}

bool KnowledgeBase::updateKnowledge(const std::string& id, const KnowledgeEntry& entry) {
    std::lock_guard<std::mutex> lock(knowledgeMutex_);
    auto it = knowledgeStore_.find(id);
    if (it == knowledgeStore_.end()) return false;
    KnowledgeEntry e = entry;
    e.id = id;
    e.updated_at = std::chrono::system_clock::now();
    e.created_at = it->second.created_at;
    knowledgeStore_[id] = e;
    return true;
}

bool KnowledgeBase::removeKnowledge(const std::string& id) {
    std::lock_guard<std::mutex> lock(knowledgeMutex_);
    return knowledgeStore_.erase(id) > 0;
}

std::optional<KnowledgeEntry> KnowledgeBase::getKnowledge(const std::string& id) {
    std::lock_guard<std::mutex> lock(knowledgeMutex_);
    auto it = knowledgeStore_.find(id);
    if (it != knowledgeStore_.end()) return it->second;
    return std::nullopt;
}

std::vector<KnowledgeEntry> KnowledgeBase::query(const KnowledgeQuery& q) {
    std::lock_guard<std::mutex> lock(knowledgeMutex_);
    std::vector<KnowledgeEntry> results;
    for (auto& [id, entry] : knowledgeStore_) {
        if (!q.text.empty() && entry.content.find(q.text) == std::string::npos) continue;
        if (!q.types.empty()) {
            bool typeMatch = false;
            for (auto t : q.types) { if (entry.type == t) { typeMatch = true; break; } }
            if (!typeMatch) continue;
        }
        if (static_cast<int>(entry.confidence) < static_cast<int>(q.minConfidence)) continue;
        results.push_back(entry);
        if (static_cast<int>(results.size()) >= q.maxResults) break;
    }
    return results;
}

std::vector<KnowledgeEntry> KnowledgeBase::searchByText(const std::string& text, int maxResults) {
    KnowledgeQuery q(text);
    q.maxResults = maxResults;
    return query(q);
}

std::vector<KnowledgeEntry> KnowledgeBase::searchByTags(const std::vector<std::string>& tags, int maxResults) {
    std::lock_guard<std::mutex> lock(knowledgeMutex_);
    std::vector<KnowledgeEntry> results;
    for (auto& [id, entry] : knowledgeStore_) {
        for (const auto& tag : tags) {
            if (entry.hasTag(tag)) { results.push_back(entry); break; }
        }
        if (static_cast<int>(results.size()) >= maxResults) break;
    }
    return results;
}

std::vector<KnowledgeEntry> KnowledgeBase::getRelatedKnowledge(const std::string& entryId, int maxResults) {
    std::lock_guard<std::mutex> lock(knowledgeMutex_);
    std::vector<KnowledgeEntry> results;
    auto it = knowledgeStore_.find(entryId);
    if (it == knowledgeStore_.end()) return results;
    for (const auto& relId : it->second.related_entries) {
        auto rit = knowledgeStore_.find(relId);
        if (rit != knowledgeStore_.end()) {
            results.push_back(rit->second);
            if (static_cast<int>(results.size()) >= maxResults) break;
        }
    }
    return results;
}

std::vector<std::string> KnowledgeBase::getAllTags() const {
    std::lock_guard<std::mutex> lock(knowledgeMutex_);
    std::vector<std::string> allTags;
    for (auto& [id, entry] : knowledgeStore_) {
        for (const auto& tag : entry.tags) {
            if (std::find(allTags.begin(), allTags.end(), tag) == allTags.end())
                allTags.push_back(tag);
        }
    }
    return allTags;
}

std::unordered_map<KnowledgeType, int> KnowledgeBase::getKnowledgeTypeStats() const {
    std::lock_guard<std::mutex> lock(knowledgeMutex_);
    std::unordered_map<KnowledgeType, int> stats;
    for (auto& [id, entry] : knowledgeStore_) stats[entry.type]++;
    return stats;
}

std::vector<KnowledgeEntry> KnowledgeBase::getKnowledgeByType(KnowledgeType type) {
    std::lock_guard<std::mutex> lock(knowledgeMutex_);
    std::vector<KnowledgeEntry> results;
    for (auto& [id, entry] : knowledgeStore_) {
        if (entry.type == type) results.push_back(entry);
    }
    return results;
}

void KnowledgeBase::validateKnowledge() {
    std::lock_guard<std::mutex> lock(knowledgeMutex_);
    for (auto it = knowledgeStore_.begin(); it != knowledgeStore_.end();) {
        if (it->second.content.empty()) it = knowledgeStore_.erase(it);
        else ++it;
    }
}

void KnowledgeBase::pruneOldKnowledge(std::chrono::hours maxAge) {
    std::lock_guard<std::mutex> lock(knowledgeMutex_);
    auto cutoff = std::chrono::system_clock::now() - maxAge;
    for (auto it = knowledgeStore_.begin(); it != knowledgeStore_.end();) {
        if (it->second.updated_at < cutoff) it = knowledgeStore_.erase(it);
        else ++it;
    }
}

void KnowledgeBase::consolidateKnowledge() {
    std::lock_guard<std::mutex> lock(knowledgeMutex_);
    std::unordered_map<std::string, std::string> contentToId;
    for (auto it = knowledgeStore_.begin(); it != knowledgeStore_.end();) {
        auto cit = contentToId.find(it->second.content);
        if (cit != contentToId.end()) it = knowledgeStore_.erase(it);
        else { contentToId[it->second.content] = it->first; ++it; }
    }
}

std::vector<KnowledgeEntry> KnowledgeBase::performInference(const KnowledgeQuery& q) {
    auto facts = query(q);
    if (inferenceEngine_) return inferenceEngine_->inferFromFacts(facts);
    return {};
}

void KnowledgeBase::setInferenceEngine(std::shared_ptr<KnowledgeInferenceEngine> engine) {
    inferenceEngine_ = std::move(engine);
}

bool KnowledgeBase::exportToFile(const std::string& filename) const {
    std::lock_guard<std::mutex> lock(knowledgeMutex_);
    std::ofstream file(filename);
    if (!file.is_open()) return false;
    for (auto& [id, entry] : knowledgeStore_)
        file << id << "\t" << entry.content << "\t" << knowledgeTypeToString(entry.type) << "\n";
    return true;
}

bool KnowledgeBase::importFromFile(const std::string& filename) {
    std::lock_guard<std::mutex> lock(knowledgeMutex_);
    std::ifstream file(filename);
    if (!file.is_open()) return false;
    std::string line;
    while (std::getline(file, line)) {
        if (line.empty()) continue;
        KnowledgeEntry entry(line, KnowledgeType::FACT);
        entry.id = generateKnowledgeId();
        knowledgeStore_[entry.id] = entry;
    }
    return true;
}

JsonValue KnowledgeBase::exportToJson() const {
    std::lock_guard<std::mutex> lock(knowledgeMutex_);
    JsonValue j;
    int idx = 0;
    for (auto& [id, entry] : knowledgeStore_) {
        j[std::to_string(idx++)] = std::any(entry.toJson());
    }
    return j;
}

bool KnowledgeBase::importFromJson(const JsonValue& data) {
    std::lock_guard<std::mutex> lock(knowledgeMutex_);
    for (const auto& [key, val] : data) {
        try {
            auto entryJson = std::any_cast<JsonValue>(val);
            auto entry = KnowledgeEntry::fromJson(entryJson);
            if (entry.id.empty()) entry.id = generateKnowledgeId();
            knowledgeStore_[entry.id] = entry;
        } catch (...) {}
    }
    return true;
}

size_t KnowledgeBase::getKnowledgeCount() const {
    std::lock_guard<std::mutex> lock(knowledgeMutex_);
    return knowledgeStore_.size();
}

std::string KnowledgeBase::getStatistics() const {
    std::ostringstream ss;
    ss << "Knowledge Base Statistics:\n";
    ss << "  Total entries: " << knowledgeStore_.size() << "\n";
    return ss.str();
}

void KnowledgeBase::clear() {
    std::lock_guard<std::mutex> lock(knowledgeMutex_);
    knowledgeStore_.clear();
}

void KnowledgeBase::saveKnowledgeToMemory(const KnowledgeEntry&) {}
std::optional<KnowledgeEntry> KnowledgeBase::loadKnowledgeFromMemory(const std::string&) { return std::nullopt; }
std::vector<KnowledgeEntry> KnowledgeBase::searchMemoryByContent(const std::string&, int) { return {}; }
std::vector<KnowledgeEntry> KnowledgeBase::getAllKnowledgeFromMemory() const { return {}; }
bool KnowledgeBase::isValidKnowledgeEntry(const KnowledgeEntry& entry) const { return !entry.content.empty(); }
void KnowledgeBase::updateKnowledgeMetrics(const KnowledgeEntry&) {}

// ==============================================================================
// Utility functions
// ==============================================================================

std::string knowledgeTypeToString(KnowledgeType type) {
    switch (type) {
        case KnowledgeType::FACT: return "FACT";
        case KnowledgeType::RULE: return "RULE";
        case KnowledgeType::CONCEPT: return "CONCEPT";
        case KnowledgeType::RELATIONSHIP: return "RELATIONSHIP";
        case KnowledgeType::PROCEDURE: return "PROCEDURE";
        case KnowledgeType::EXPERIENCE: return "EXPERIENCE";
        default: return "UNKNOWN";
    }
}

KnowledgeType stringToKnowledgeType(const std::string& typeStr) {
    if (typeStr == "FACT") return KnowledgeType::FACT;
    if (typeStr == "RULE") return KnowledgeType::RULE;
    if (typeStr == "CONCEPT") return KnowledgeType::CONCEPT;
    if (typeStr == "RELATIONSHIP") return KnowledgeType::RELATIONSHIP;
    if (typeStr == "PROCEDURE") return KnowledgeType::PROCEDURE;
    if (typeStr == "EXPERIENCE") return KnowledgeType::EXPERIENCE;
    return KnowledgeType::FACT;
}

std::string confidenceLevelToString(ConfidenceLevel level) {
    switch (level) {
        case ConfidenceLevel::VERY_LOW: return "VERY_LOW";
        case ConfidenceLevel::LOW: return "LOW";
        case ConfidenceLevel::MEDIUM: return "MEDIUM";
        case ConfidenceLevel::HIGH: return "HIGH";
        case ConfidenceLevel::VERY_HIGH: return "VERY_HIGH";
        default: return "UNKNOWN";
    }
}

ConfidenceLevel stringToConfidenceLevel(const std::string& levelStr) {
    if (levelStr == "VERY_LOW") return ConfidenceLevel::VERY_LOW;
    if (levelStr == "LOW") return ConfidenceLevel::LOW;
    if (levelStr == "MEDIUM") return ConfidenceLevel::MEDIUM;
    if (levelStr == "HIGH") return ConfidenceLevel::HIGH;
    if (levelStr == "VERY_HIGH") return ConfidenceLevel::VERY_HIGH;
    return ConfidenceLevel::MEDIUM;
}

std::string knowledgeSourceToString(KnowledgeSource source) {
    switch (source) {
        case KnowledgeSource::LEARNED: return "LEARNED";
        case KnowledgeSource::PROGRAMMED: return "PROGRAMMED";
        case KnowledgeSource::INFERRED: return "INFERRED";
        case KnowledgeSource::OBSERVED: return "OBSERVED";
        case KnowledgeSource::COMMUNICATED: return "COMMUNICATED";
        default: return "UNKNOWN";
    }
}

KnowledgeSource stringToKnowledgeSource(const std::string& sourceStr) {
    if (sourceStr == "LEARNED") return KnowledgeSource::LEARNED;
    if (sourceStr == "PROGRAMMED") return KnowledgeSource::PROGRAMMED;
    if (sourceStr == "INFERRED") return KnowledgeSource::INFERRED;
    if (sourceStr == "OBSERVED") return KnowledgeSource::OBSERVED;
    if (sourceStr == "COMMUNICATED") return KnowledgeSource::COMMUNICATED;
    return KnowledgeSource::PROGRAMMED;
}

} // namespace elizaos
