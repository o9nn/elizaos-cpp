// knowledge.cpp - Full implementation of KnowledgeBase system
#include "elizaos/knowledge.hpp"
#include <algorithm>
#include <sstream>
#include <random>
#include <fstream>
#include <iomanip>
#include <atomic>
#include <cctype>
#include <functional>
#include <unordered_set>
#include <queue>

namespace elizaos {

namespace {
std::string toLower(std::string value) {
    std::transform(value.begin(), value.end(), value.begin(), [](unsigned char c) {
        return static_cast<char>(std::tolower(c));
    });
    return value;
}

std::string makeKnowledgeEntryId() {
    static std::atomic<unsigned long long> counter{0};
    const auto now = std::chrono::duration_cast<std::chrono::nanoseconds>(
        std::chrono::system_clock::now().time_since_epoch()).count();
    std::ostringstream ss;
    ss << "ke-" << std::hex << now << "-" << counter.fetch_add(1, std::memory_order_relaxed);
    return ss.str();
}

bool containsCaseInsensitive(const std::string& haystack, const std::string& needle) {
    if (needle.empty()) return true;
    return toLower(haystack).find(toLower(needle)) != std::string::npos;
}
}

std::shared_ptr<KnowledgeBase> globalKnowledgeBase = std::make_shared<KnowledgeBase>();

// ==============================================================================
// KnowledgeEntry
// ==============================================================================

KnowledgeEntry::KnowledgeEntry(const std::string& content, KnowledgeType type)
    : id(makeKnowledgeEntryId()), content(content), type(type), confidence(ConfidenceLevel::MEDIUM),
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
    j["tags"] = std::any(tags);
    j["metadata"] = std::any(metadata);
    j["related_entries"] = std::any(related_entries);
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
    it = json.find("confidence");
    if (it != json.end()) {
        if (it->second.type() == typeid(int)) entry.confidence = static_cast<ConfidenceLevel>(std::any_cast<int>(it->second));
        else entry.confidence = stringToConfidenceLevel(std::any_cast<std::string>(it->second));
    }
    it = json.find("source");
    if (it != json.end()) entry.source = stringToKnowledgeSource(std::any_cast<std::string>(it->second));
    it = json.find("tags");
    if (it != json.end()) entry.tags = std::any_cast<std::vector<std::string>>(it->second);
    it = json.find("metadata");
    if (it != json.end()) entry.metadata = std::any_cast<std::unordered_map<std::string, std::string>>(it->second);
    it = json.find("related_entries");
    if (it != json.end()) entry.related_entries = std::any_cast<std::vector<std::string>>(it->second);
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

KnowledgeInferenceEngine::KnowledgeInferenceEngine() {
    addInferenceRule("default_summary", [](const std::vector<KnowledgeEntry>& facts) {
        std::vector<KnowledgeEntry> results;
        if (facts.empty()) return results;
        KnowledgeEntry inferred("Inferred knowledge from " + std::to_string(facts.size()) + " fact(s)", KnowledgeType::FACT);
        inferred.source = KnowledgeSource::INFERRED;
        inferred.confidence = ConfidenceLevel::MEDIUM;
        inferred.addTag("inferred");
        for (const auto& fact : facts) {
            for (const auto& tag : fact.tags) inferred.addTag(tag);
        }
        results.push_back(inferred);
        return results;
    });
}

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
    std::vector<KnowledgeEntry> related;
    KnowledgeEntry conceptEntry("Related concept for: " + entry.content, KnowledgeType::CONCEPT);
    conceptEntry.source = KnowledgeSource::INFERRED;
    conceptEntry.confidence = ConfidenceLevel::MEDIUM;
    conceptEntry.addTag("related_concept");
    for (const auto& tag : entry.tags) conceptEntry.addTag(tag);
    related.push_back(conceptEntry);
    return related;
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
    result.addTag("combined_evidence");
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
    if (!isValidKnowledgeEntry(entry)) return "";
    std::lock_guard<std::mutex> lock(knowledgeMutex_);
    KnowledgeEntry e = entry;
    if (e.id.empty()) e.id = generateKnowledgeId();
    if (e.created_at.time_since_epoch().count() == 0) e.created_at = std::chrono::system_clock::now();
    e.updated_at = std::chrono::system_clock::now();
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
        bool textMatch = q.text.empty() || containsCaseInsensitive(entry.content, q.text);
        if (!textMatch) {
            for (const auto& tag : entry.tags) {
                if (containsCaseInsensitive(tag, q.text)) { textMatch = true; break; }
            }
        }
        if (!textMatch) continue;
        if (!q.types.empty()) {
            bool typeMatch = false;
            for (auto t : q.types) { if (entry.type == t) { typeMatch = true; break; } }
            if (!typeMatch) continue;
        }
        if (!q.tags.empty()) {
            bool tagMatch = false;
            for (const auto& tag : q.tags) {
                if (entry.hasTag(tag)) { tagMatch = true; break; }
            }
            if (!tagMatch) continue;
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
        if (it->second.updated_at < cutoff && static_cast<int>(it->second.confidence) <= static_cast<int>(ConfidenceLevel::LOW)) it = knowledgeStore_.erase(it);
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
    if (!inferenceEngine_) return {};
    auto inferred = inferenceEngine_->inferFromFacts(facts);
    for (const auto& entry : inferred) addKnowledge(entry);
    return inferred;
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
        std::istringstream ss(line);
        std::string id, content, type;
        if (std::getline(ss, id, '\t') && std::getline(ss, content, '\t') && std::getline(ss, type, '\t')) {
            KnowledgeEntry entry(content, stringToKnowledgeType(type));
            entry.id = id.empty() ? generateKnowledgeId() : id;
            knowledgeStore_[entry.id] = entry;
        } else {
            KnowledgeEntry entry(line, KnowledgeType::FACT);
            entry.id = generateKnowledgeId();
            knowledgeStore_[entry.id] = entry;
        }
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

// Knowledge -> Memory bridge implementations.
//
// These methods integrate the in-memory knowledgeStore_ with the optional
// AgentMemoryManager (memory_), so that knowledge entries are durable across
// the broader cognitive architecture (attention, recall, embeddings) when a
// manager is wired in. They never throw if memory_ is unset; instead the
// in-process knowledgeStore_ remains the source of truth.

static const std::string kKnowledgeTable = "knowledge";

static std::string knowledgeContentToMemoryString(const KnowledgeEntry& e) {
    std::ostringstream os;
    os << "[KB:" << knowledgeTypeToString(e.type) << "|c="
       << static_cast<int>(e.confidence) << "] " << e.content;
    if (!e.tags.empty()) {
        os << " #";
        for (size_t i = 0; i < e.tags.size(); ++i) {
            if (i) os << ",";
            os << e.tags[i];
        }
    }
    return os.str();
}

void KnowledgeBase::saveKnowledgeToMemory(const KnowledgeEntry& entry) {
    if (!memory_) return;
    // Use the entry id as the memory id so we can round-trip cleanly.
    auto mem = std::make_shared<Memory>(
        entry.id,
        knowledgeContentToMemoryString(entry),
        /*entityId=*/std::string("knowledge_base"),
        /*agentId=*/std::string("knowledge_base"));
    CustomMetadata md;
    md.type = MemoryType::CUSTOM;
    md.tags = entry.tags;
    md.source = std::string("knowledge");
    md.scope = MemoryScope::SHARED;
    md.timestamp = entry.created_at;
    md.customData["kb_type"] = knowledgeTypeToString(entry.type);
    md.customData["kb_source"] = knowledgeSourceToString(entry.source);
    md.customData["kb_confidence"] = std::to_string(static_cast<int>(entry.confidence));
    for (const auto& kv : entry.metadata) {
        md.customData["meta_" + kv.first] = kv.second;
    }
    mem->setMetadata(md);
    mem->setUnique(true);
    memory_->createMemory(mem, kKnowledgeTable, /*unique=*/true);
}

std::optional<KnowledgeEntry> KnowledgeBase::loadKnowledgeFromMemory(const std::string& id) {
    if (!memory_) return std::nullopt;
    auto mem = memory_->getMemoryById(id);
    if (!mem) return std::nullopt;
    KnowledgeEntry e(mem->getContent(), KnowledgeType::FACT);
    e.id = mem->getId();
    const auto& metaVar = mem->getMetadata();
    if (auto* cm = std::get_if<CustomMetadata>(&metaVar)) {
        e.tags = cm->tags;
        auto it = cm->customData.find("kb_type");
        if (it != cm->customData.end()) e.type = stringToKnowledgeType(it->second);
        it = cm->customData.find("kb_source");
        if (it != cm->customData.end()) e.source = stringToKnowledgeSource(it->second);
        it = cm->customData.find("kb_confidence");
        if (it != cm->customData.end()) {
            try {
                int c = std::stoi(it->second);
                if (c < 1) c = 1;
                if (c > 5) c = 5;
                e.confidence = static_cast<ConfidenceLevel>(c);
            } catch (...) { /* leave default */ }
        }
        for (const auto& kv : cm->customData) {
            if (kv.first.rfind("meta_", 0) == 0) {
                e.metadata[kv.first.substr(5)] = kv.second;
            }
        }
    }
    e.created_at = mem->getCreatedAt();
    e.updated_at = e.created_at;
    return e;
}

std::vector<KnowledgeEntry> KnowledgeBase::searchMemoryByContent(const std::string& content,
                                                                 int maxResults) {
    std::vector<KnowledgeEntry> hits;
    if (!memory_) return hits;
    MemorySearchParams params;
    params.tableName = kKnowledgeTable;
    params.count = maxResults > 0 ? maxResults * 4 : 40; // overfetch then filter
    auto memories = memory_->getMemories(params);
    const std::string needle = content;
    for (const auto& mem : memories) {
        if (needle.empty() || mem->getContent().find(needle) != std::string::npos) {
            auto loaded = loadKnowledgeFromMemory(mem->getId());
            if (loaded) hits.push_back(*loaded);
            if (maxResults > 0 && static_cast<int>(hits.size()) >= maxResults) break;
        }
    }
    return hits;
}

std::vector<KnowledgeEntry> KnowledgeBase::getAllKnowledgeFromMemory() const {
    std::vector<KnowledgeEntry> all;
    if (!memory_) return all;
    MemorySearchParams params;
    params.tableName = kKnowledgeTable;
    params.count = 100000; // effectively all
    auto memories = memory_->getMemories(params);
    for (const auto& mem : memories) {
        // Have to call non-const helper through const_cast: load is logically
        // const for the caller (no observable mutation).
        auto loaded = const_cast<KnowledgeBase*>(this)->loadKnowledgeFromMemory(mem->getId());
        if (loaded) all.push_back(*loaded);
    }
    return all;
}

bool KnowledgeBase::isValidKnowledgeEntry(const KnowledgeEntry& entry) const {
    // Validity rules:
    // 1. content must be non-empty and non-whitespace
    // 2. confidence must be a known enum value
    // 3. type must be a known enum value
    if (entry.content.empty()) return false;
    bool nonWhitespace = false;
    for (char ch : entry.content) {
        if (!std::isspace(static_cast<unsigned char>(ch))) { nonWhitespace = true; break; }
    }
    if (!nonWhitespace) return false;
    int conf = static_cast<int>(entry.confidence);
    if (conf < 1 || conf > 5) return false;
    switch (entry.type) {
        case KnowledgeType::FACT:
        case KnowledgeType::RULE:
        case KnowledgeType::CONCEPT:
        case KnowledgeType::RELATIONSHIP:
        case KnowledgeType::PROCEDURE:
        case KnowledgeType::EXPERIENCE:
            break;
        default:
            return false;
    }
    return true;
}

// Internal usage statistics tracking for the knowledge base.
static std::mutex g_kbMetricsMutex;
static std::unordered_map<std::string, std::uint64_t> g_kbAccessCount;
static std::unordered_map<std::string, std::chrono::system_clock::time_point> g_kbLastAccess;

void KnowledgeBase::updateKnowledgeMetrics(const KnowledgeEntry& entry) {
    if (entry.id.empty()) return;
    std::lock_guard<std::mutex> lock(g_kbMetricsMutex);
    g_kbAccessCount[entry.id] += 1;
    g_kbLastAccess[entry.id] = std::chrono::system_clock::now();
    // Surface to logger if available, but never block.
    if (logger_) {
        try {
            logger_->log(
                std::string("knowledge accessed: ") + entry.id +
                " count=" + std::to_string(g_kbAccessCount[entry.id]),
                /*source=*/"KnowledgeBase",
                /*title=*/"knowledge",
                LogLevel::INFO,
                LogColor::CYAN,
                /*expand=*/true,
                /*panel=*/false,
                /*shouldLog=*/true);
        } catch (...) {}
    }
}

// Public accessors for the metrics (added to support tests / observability).
namespace knowledge_metrics {
    std::uint64_t getAccessCount(const std::string& id) {
        std::lock_guard<std::mutex> lock(g_kbMetricsMutex);
        auto it = g_kbAccessCount.find(id);
        return it == g_kbAccessCount.end() ? 0 : it->second;
    }
    void resetAll() {
        std::lock_guard<std::mutex> lock(g_kbMetricsMutex);
        g_kbAccessCount.clear();
        g_kbLastAccess.clear();
    }
}

void KnowledgeBase::setMemoryManager(std::shared_ptr<AgentMemoryManager> mgr) { memory_ = std::move(mgr); }
std::shared_ptr<AgentMemoryManager> KnowledgeBase::getMemoryManager() const { return memory_; }
void KnowledgeBase::setLogger(std::shared_ptr<AgentLogger> logger) { logger_ = std::move(logger); }
std::shared_ptr<AgentLogger> KnowledgeBase::getLogger() const { return logger_; }
void KnowledgeBase::persistToMemory(const KnowledgeEntry& entry) { saveKnowledgeToMemory(entry); }
std::optional<KnowledgeEntry> KnowledgeBase::loadFromMemory(const std::string& id) { return loadKnowledgeFromMemory(id); }
std::vector<KnowledgeEntry> KnowledgeBase::searchMemory(const std::string& content, int maxResults) { return searchMemoryByContent(content, maxResults); }
std::vector<KnowledgeEntry> KnowledgeBase::dumpMemory() const { return getAllKnowledgeFromMemory(); }
void KnowledgeBase::recordAccess(const KnowledgeEntry& entry) { updateKnowledgeMetrics(entry); }
bool KnowledgeBase::isValid(const KnowledgeEntry& entry) const { return isValidKnowledgeEntry(entry); }

// ==============================================================================
// Utility functions
// ==============================================================================

std::string knowledgeTypeToString(KnowledgeType type) {
    switch (type) {
        case KnowledgeType::FACT: return "fact";
        case KnowledgeType::RULE: return "rule";
        case KnowledgeType::CONCEPT: return "concept";
        case KnowledgeType::RELATIONSHIP: return "relationship";
        case KnowledgeType::PROCEDURE: return "procedure";
        case KnowledgeType::EXPERIENCE: return "experience";
        default: return "unknown";
    }
}

KnowledgeType stringToKnowledgeType(const std::string& typeStr) {
    const auto value = toLower(typeStr);
    if (value == "fact") return KnowledgeType::FACT;
    if (value == "rule") return KnowledgeType::RULE;
    if (value == "concept") return KnowledgeType::CONCEPT;
    if (value == "relationship") return KnowledgeType::RELATIONSHIP;
    if (value == "procedure") return KnowledgeType::PROCEDURE;
    if (value == "experience") return KnowledgeType::EXPERIENCE;
    return KnowledgeType::FACT;
}

std::string confidenceLevelToString(ConfidenceLevel level) {
    switch (level) {
        case ConfidenceLevel::VERY_LOW: return "very_low";
        case ConfidenceLevel::LOW: return "low";
        case ConfidenceLevel::MEDIUM: return "medium";
        case ConfidenceLevel::HIGH: return "high";
        case ConfidenceLevel::VERY_HIGH: return "very_high";
        default: return "unknown";
    }
}

ConfidenceLevel stringToConfidenceLevel(const std::string& levelStr) {
    const auto value = toLower(levelStr);
    if (value == "very_low") return ConfidenceLevel::VERY_LOW;
    if (value == "low") return ConfidenceLevel::LOW;
    if (value == "medium") return ConfidenceLevel::MEDIUM;
    if (value == "high") return ConfidenceLevel::HIGH;
    if (value == "very_high") return ConfidenceLevel::VERY_HIGH;
    return ConfidenceLevel::MEDIUM;
}

std::string knowledgeSourceToString(KnowledgeSource source) {
    switch (source) {
        case KnowledgeSource::LEARNED: return "learned";
        case KnowledgeSource::PROGRAMMED: return "programmed";
        case KnowledgeSource::INFERRED: return "inferred";
        case KnowledgeSource::OBSERVED: return "observed";
        case KnowledgeSource::COMMUNICATED: return "communicated";
        default: return "unknown";
    }
}

KnowledgeSource stringToKnowledgeSource(const std::string& sourceStr) {
    const auto value = toLower(sourceStr);
    if (value == "learned") return KnowledgeSource::LEARNED;
    if (value == "programmed") return KnowledgeSource::PROGRAMMED;
    if (value == "inferred") return KnowledgeSource::INFERRED;
    if (value == "observed") return KnowledgeSource::OBSERVED;
    if (value == "communicated") return KnowledgeSource::COMMUNICATED;
    return KnowledgeSource::PROGRAMMED;
}


// ============================================================================
// Phase 2.1: Hyperedge Implementation
// ============================================================================

namespace {
std::string makeHyperedgeId() {
    static std::atomic<unsigned long long> counter{0};
    const auto now = std::chrono::duration_cast<std::chrono::nanoseconds>(
        std::chrono::system_clock::now().time_since_epoch()).count();
    std::ostringstream ss;
    ss << "he-" << std::hex << now << "-" << counter.fetch_add(1, std::memory_order_relaxed);
    return ss.str();
}
}

Hyperedge::Hyperedge(const std::string& relType, const std::vector<std::string>& nodes)
    : id(makeHyperedgeId())
    , relationshipType(relType)
    , nodeIds(nodes) {
    
    if (nodes.size() == 2) {
        edgeType = HyperedgeType::BINARY;
    } else if (nodes.size() == 3) {
        edgeType = HyperedgeType::TERNARY;
    } else {
        edgeType = HyperedgeType::NARY;
    }
}

bool Hyperedge::isValid(const Timestamp& at) const {
    if (validFrom && at < *validFrom) return false;
    if (validUntil && at > *validUntil) return false;
    return true;
}

bool Hyperedge::connects(const std::string& nodeId) const {
    return std::find(nodeIds.begin(), nodeIds.end(), nodeId) != nodeIds.end();
}

// ============================================================================
// Phase 2.1: KnowledgeHypergraph Implementation
// ============================================================================

KnowledgeHypergraph::KnowledgeHypergraph() {}

std::string KnowledgeHypergraph::generateEdgeId() const {
    return makeHyperedgeId();
}

void KnowledgeHypergraph::addNode(const KnowledgeEntry& entry) {
    std::lock_guard<std::mutex> lock(graphMutex_);
    nodes_[entry.id] = entry;
}

void KnowledgeHypergraph::removeNode(const std::string& nodeId) {
    std::lock_guard<std::mutex> lock(graphMutex_);
    
    // Remove all edges connected to this node
    auto edgeIdsIt = nodeToEdges_.find(nodeId);
    if (edgeIdsIt != nodeToEdges_.end()) {
        for (const auto& edgeId : edgeIdsIt->second) {
            edges_.erase(edgeId);
        }
        nodeToEdges_.erase(edgeIdsIt);
    }
    
    // Remove the node itself
    nodes_.erase(nodeId);
    
    // Clean up references in other nodes' edge lists
    for (auto& [id, edgeIds] : nodeToEdges_) {
        edgeIds.erase(
            std::remove_if(edgeIds.begin(), edgeIds.end(),
                [this, &nodeId](const std::string& eid) {
                    auto it = edges_.find(eid);
                    return it != edges_.end() && it->second.connects(nodeId);
                }),
            edgeIds.end());
    }
}

std::optional<KnowledgeEntry> KnowledgeHypergraph::getNode(const std::string& nodeId) const {
    std::lock_guard<std::mutex> lock(graphMutex_);
    auto it = nodes_.find(nodeId);
    if (it != nodes_.end()) {
        return it->second;
    }
    return std::nullopt;
}

std::string KnowledgeHypergraph::addEdge(const Hyperedge& edge) {
    std::lock_guard<std::mutex> lock(graphMutex_);
    
    Hyperedge e = edge;
    if (e.id.empty()) {
        e.id = generateEdgeId();
    }
    
    edges_[e.id] = e;
    
    // Index by connected nodes
    for (const auto& nodeId : e.nodeIds) {
        nodeToEdges_[nodeId].push_back(e.id);
    }
    
    return e.id;
}

void KnowledgeHypergraph::removeEdge(const std::string& edgeId) {
    std::lock_guard<std::mutex> lock(graphMutex_);
    
    auto it = edges_.find(edgeId);
    if (it != edges_.end()) {
        // Remove from node indices
        for (const auto& nodeId : it->second.nodeIds) {
            auto& edgeIds = nodeToEdges_[nodeId];
            edgeIds.erase(
                std::remove(edgeIds.begin(), edgeIds.end(), edgeId),
                edgeIds.end());
        }
        edges_.erase(it);
    }
}

std::optional<Hyperedge> KnowledgeHypergraph::getEdge(const std::string& edgeId) const {
    std::lock_guard<std::mutex> lock(graphMutex_);
    auto it = edges_.find(edgeId);
    if (it != edges_.end()) {
        return it->second;
    }
    return std::nullopt;
}

std::vector<Hyperedge> KnowledgeHypergraph::getEdgesConnectingLocked(const std::string& nodeId) const {
    // Precondition: graphMutex_ already held by caller. Used by matchPattern()
    // and other methods that must avoid re-locking the non-recursive mutex.
    std::vector<Hyperedge> result;
    auto it = nodeToEdges_.find(nodeId);
    if (it != nodeToEdges_.end()) {
        for (const auto& edgeId : it->second) {
            auto edgeIt = edges_.find(edgeId);
            if (edgeIt != edges_.end()) {
                result.push_back(edgeIt->second);
            }
        }
    }
    return result;
}

std::vector<Hyperedge> KnowledgeHypergraph::getEdgesConnecting(const std::string& nodeId) const {
    std::lock_guard<std::mutex> lock(graphMutex_);
    return getEdgesConnectingLocked(nodeId);
}

std::vector<Hyperedge> KnowledgeHypergraph::getEdgesByType(const std::string& relationshipType) const {
    std::lock_guard<std::mutex> lock(graphMutex_);
    
    std::vector<Hyperedge> result;
    for (const auto& [id, edge] : edges_) {
        if (edge.relationshipType == relationshipType) {
            result.push_back(edge);
        }
    }
    return result;
}

std::vector<std::string> KnowledgeHypergraph::getNeighbors(const std::string& nodeId, int maxDepth) const {
    std::lock_guard<std::mutex> lock(graphMutex_);
    
    std::unordered_set<std::string> visited;
    std::vector<std::string> toVisit = {nodeId};
    visited.insert(nodeId);
    
    for (int depth = 0; depth < maxDepth && !toVisit.empty(); depth++) {
        std::vector<std::string> nextLevel;
        
        for (const auto& currentId : toVisit) {
            auto edgesIt = nodeToEdges_.find(currentId);
            if (edgesIt != nodeToEdges_.end()) {
                for (const auto& edgeId : edgesIt->second) {
                    auto edgeIt = edges_.find(edgeId);
                    if (edgeIt != edges_.end()) {
                        for (const auto& neighborId : edgeIt->second.nodeIds) {
                            if (visited.find(neighborId) == visited.end()) {
                                visited.insert(neighborId);
                                nextLevel.push_back(neighborId);
                            }
                        }
                    }
                }
            }
        }
        
        toVisit = std::move(nextLevel);
    }
    
    visited.erase(nodeId);  // Don't include the start node
    return std::vector<std::string>(visited.begin(), visited.end());
}

std::vector<std::vector<std::string>> KnowledgeHypergraph::findPaths(
    const std::string& fromId,
    const std::string& toId,
    int maxDepth) const {
    
    std::lock_guard<std::mutex> lock(graphMutex_);
    
    std::vector<std::vector<std::string>> paths;
    std::vector<std::string> currentPath = {fromId};
    std::unordered_set<std::string> visited = {fromId};
    
    std::function<void(const std::string&, int)> dfs = [&](const std::string& current, int depth) {
        if (depth > maxDepth) return;
        
        if (current == toId) {
            paths.push_back(currentPath);
            return;
        }
        
        auto edgesIt = nodeToEdges_.find(current);
        if (edgesIt != nodeToEdges_.end()) {
            for (const auto& edgeId : edgesIt->second) {
                auto edgeIt = edges_.find(edgeId);
                if (edgeIt != edges_.end()) {
                    for (const auto& neighborId : edgeIt->second.nodeIds) {
                        if (visited.find(neighborId) == visited.end()) {
                            visited.insert(neighborId);
                            currentPath.push_back(neighborId);
                            dfs(neighborId, depth + 1);
                            currentPath.pop_back();
                            visited.erase(neighborId);
                        }
                    }
                }
            }
        }
    };
    
    dfs(fromId, 0);
    return paths;
}

std::vector<KnowledgeEntry> KnowledgeHypergraph::extractSubgraph(
    const std::string& centerId,
    int radius) const {
    
    auto neighborIds = getNeighbors(centerId, radius);
    neighborIds.push_back(centerId);
    
    std::lock_guard<std::mutex> lock(graphMutex_);
    
    std::vector<KnowledgeEntry> result;
    for (const auto& id : neighborIds) {
        auto it = nodes_.find(id);
        if (it != nodes_.end()) {
            result.push_back(it->second);
        }
    }
    return result;
}

std::vector<std::unordered_map<std::string, std::string>> KnowledgeHypergraph::matchPattern(
    const std::vector<std::string>& patternNodes,
    const std::vector<std::pair<int, int>>& patternEdges,
    const std::string& relationshipType) const {
    
    std::lock_guard<std::mutex> lock(graphMutex_);
    
    std::vector<std::unordered_map<std::string, std::string>> matches;
    
    if (patternNodes.empty()) return matches;
    
    // Proper subgraph pattern matching via backtracking constraint
    // satisfaction. Each pattern variable (index in patternNodes) is bound to
    // a concrete graph node id; an edge constraint (from,to) is satisfied iff
    // a hyperedge of the requested relationshipType connects the two bound
    // nodes. We enumerate candidate node ids once and recursively assign each
    // pattern variable, pruning partial assignments that already violate an
    // edge whose endpoints are both bound.
    std::vector<std::string> allNodeIds;
    allNodeIds.reserve(nodes_.size());
    for (const auto& [id, entry] : nodes_) {
        allNodeIds.push_back(id);
    }

    const int numVars = static_cast<int>(patternNodes.size());

    // Filter edge constraints to those fully within the pattern's variable range.
    std::vector<std::pair<int, int>> validEdges;
    validEdges.reserve(patternEdges.size());
    for (const auto& [from, to] : patternEdges) {
        if (from >= 0 && from < numVars && to >= 0 && to < numVars) {
            validEdges.emplace_back(from, to);
        }
    }

    // Returns true iff some hyperedge of the requested relationship type relates
    // fromNode -> toNode. Hyperedges are membership sets whose nodeIds order
    // encodes role/direction, so a directed pattern edge (from,to) is satisfied
    // only when a single hyperedge contains BOTH endpoints as DISTINCT members
    // and fromNode occupies an earlier slot than toNode (preserving the n-ary
    // ordering, e.g. cause before effect). Uses the no-lock accessor since
    // graphMutex_ is already held by this method.
    auto edgeSatisfied = [&](const std::string& fromNode,
                             const std::string& toNode) -> bool {
        if (fromNode == toNode) return false; // no self-loops in patterns
        const auto connecting = getEdgesConnectingLocked(fromNode);
        for (const auto& e : connecting) {
            if (!relationshipType.empty() && e.relationshipType != relationshipType) {
                continue;
            }
            // Find ordered positions of both endpoints within this hyperedge.
            int fromPos = -1, toPos = -1;
            for (int i = 0; i < static_cast<int>(e.nodeIds.size()); ++i) {
                if (e.nodeIds[i] == fromNode && fromPos == -1) fromPos = i;
                else if (e.nodeIds[i] == toNode && toPos == -1) toPos = i;
            }
            if (fromPos != -1 && toPos != -1 && fromPos < toPos) {
                return true;
            }
        }
        return false;
    };

    // Check every edge constraint whose endpoints are both bound in `assigned`.
    // `justAssigned` is the variable index that was bound most recently so we
    // only need to validate constraints that touch it (incremental pruning).
    auto partialConsistent = [&](const std::vector<int>& assignment,
                                 int justAssigned) -> bool {
        for (const auto& [from, to] : validEdges) {
            if (from != justAssigned && to != justAssigned) continue;
            if (assignment[from] < 0 || assignment[to] < 0) continue; // not both bound yet
            if (!edgeSatisfied(allNodeIds[assignment[from]], allNodeIds[assignment[to]])) {
                return false;
            }
        }
        return true;
    };

    std::vector<int> assignment(numVars, -1); // index into allNodeIds, -1 = unbound
    std::function<void(int)> backtrack = [&](int var) {
        if (var == numVars) {
            std::unordered_map<std::string, std::string> binding;
            binding.reserve(numVars);
            for (int v = 0; v < numVars; ++v) {
                binding[patternNodes[v]] = allNodeIds[assignment[v]];
            }
            matches.push_back(std::move(binding));
            return;
        }
        for (int candidate = 0; candidate < static_cast<int>(allNodeIds.size()); ++candidate) {
            assignment[var] = candidate;
            if (partialConsistent(assignment, var)) {
                backtrack(var + 1);
            }
            assignment[var] = -1;
        }
    };

    if (!allNodeIds.empty()) {
        backtrack(0);
    }

    return matches;
}

size_t KnowledgeHypergraph::nodeCount() const {
    std::lock_guard<std::mutex> lock(graphMutex_);
    return nodes_.size();
}

size_t KnowledgeHypergraph::edgeCount() const {
    std::lock_guard<std::mutex> lock(graphMutex_);
    return edges_.size();
}

double KnowledgeHypergraph::averageDegree() const {
    std::lock_guard<std::mutex> lock(graphMutex_);
    
    if (nodes_.empty()) return 0.0;
    
    size_t totalDegree = 0;
    for (const auto& [nodeId, edgeIds] : nodeToEdges_) {
        totalDegree += edgeIds.size();
    }
    
    return static_cast<double>(totalDegree) / nodes_.size();
}

std::vector<std::string> KnowledgeHypergraph::findHubs(int topN) const {
    std::lock_guard<std::mutex> lock(graphMutex_);
    
    std::vector<std::pair<std::string, size_t>> nodeDegrees;
    for (const auto& [nodeId, edgeIds] : nodeToEdges_) {
        nodeDegrees.emplace_back(nodeId, edgeIds.size());
    }
    
    std::sort(nodeDegrees.begin(), nodeDegrees.end(),
              [](const auto& a, const auto& b) { return a.second > b.second; });
    
    std::vector<std::string> hubs;
    for (int i = 0; i < std::min(topN, static_cast<int>(nodeDegrees.size())); i++) {
        hubs.push_back(nodeDegrees[i].first);
    }
    
    return hubs;
}

// ============================================================================
// Phase 2.1: ChainedInferenceEngine Implementation
// ============================================================================

ChainedInferenceEngine::ChainedInferenceEngine() {}

void ChainedInferenceEngine::addRule(const InferenceRule& rule) {
    std::lock_guard<std::mutex> lock(inferenceMutex_);
    
    // Remove existing rule with same name
    rules_.erase(
        std::remove_if(rules_.begin(), rules_.end(),
            [&rule](const InferenceRule& r) { return r.name == rule.name; }),
        rules_.end());
    
    rules_.push_back(rule);
}

void ChainedInferenceEngine::removeRule(const std::string& ruleName) {
    std::lock_guard<std::mutex> lock(inferenceMutex_);
    
    rules_.erase(
        std::remove_if(rules_.begin(), rules_.end(),
            [&ruleName](const InferenceRule& r) { return r.name == ruleName; }),
        rules_.end());
}

void ChainedInferenceEngine::enableRule(const std::string& ruleName, bool enable) {
    std::lock_guard<std::mutex> lock(inferenceMutex_);
    
    for (auto& rule : rules_) {
        if (rule.name == ruleName) {
            rule.enabled = enable;
            break;
        }
    }
}

std::vector<ChainedInferenceEngine::InferenceRule> ChainedInferenceEngine::getRules() const {
    std::lock_guard<std::mutex> lock(inferenceMutex_);
    return rules_;
}

bool ChainedInferenceEngine::matchCondition(
    const KnowledgeEntry& entry,
    const std::pair<std::string, std::string>& condition) const {
    
    const auto& [condType, condPattern] = condition;
    
    // Check type
    if (!condType.empty() && knowledgeTypeToString(entry.type) != condType) {
        return false;
    }
    
    // Check pattern (simple substring match)
    if (!condPattern.empty()) {
        std::string lowerContent = toLower(entry.content);
        std::string lowerPattern = toLower(condPattern);
        if (lowerContent.find(lowerPattern) == std::string::npos) {
            return false;
        }
    }
    
    return true;
}

KnowledgeEntry ChainedInferenceEngine::applyRule(
    const InferenceRule& rule,
    const std::vector<KnowledgeEntry>& matchedFacts) {
    
    KnowledgeEntry result;
    result.type = stringToKnowledgeType(rule.conclusion.first);
    result.content = rule.conclusion.second;
    result.source = KnowledgeSource::INFERRED;
    
    // Calculate confidence based on input facts
    double totalConfidence = 0.0;
    for (const auto& fact : matchedFacts) {
        totalConfidence += static_cast<int>(fact.confidence);
    }
    
    double avgConfidence = matchedFacts.empty() ? 3.0 : totalConfidence / matchedFacts.size();
    avgConfidence *= rule.confidenceModifier;
    
    result.confidence = static_cast<ConfidenceLevel>(
        std::max(1, std::min(5, static_cast<int>(avgConfidence))));
    
    // Add relations to source facts
    for (const auto& fact : matchedFacts) {
        result.related_entries.push_back(fact.id);
    }
    
    return result;
}

std::vector<KnowledgeEntry> ChainedInferenceEngine::forwardChain(
    const std::vector<KnowledgeEntry>& facts,
    int maxIterations) {
    
    std::lock_guard<std::mutex> lock(inferenceMutex_);
    
    lastTrace_.clear();
    std::vector<KnowledgeEntry> workingSet = facts;
    std::unordered_set<std::string> derivedContents;
    
    for (const auto& f : facts) {
        derivedContents.insert(toLower(f.content));
    }
    
    for (int iteration = 0; iteration < maxIterations; iteration++) {
        bool newDerivations = false;
        
        for (const auto& rule : rules_) {
            if (!rule.enabled) continue;
            
            // Find all facts matching conditions
            std::vector<KnowledgeEntry> matchedFacts;
            bool allConditionsMet = true;
            
            for (const auto& condition : rule.conditions) {
                bool conditionMet = false;
                for (const auto& fact : workingSet) {
                    if (matchCondition(fact, condition)) {
                        matchedFacts.push_back(fact);
                        conditionMet = true;
                        break;
                    }
                }
                if (!conditionMet) {
                    allConditionsMet = false;
                    break;
                }
            }
            
            if (allConditionsMet && !matchedFacts.empty()) {
                KnowledgeEntry derived = applyRule(rule, matchedFacts);
                
                // Check if this is a new derivation
                if (derivedContents.find(toLower(derived.content)) == derivedContents.end()) {
                    if (static_cast<int>(derived.confidence) >= 
                        static_cast<int>(minConfidence_ * 5)) {
                        
                        derivedContents.insert(toLower(derived.content));
                        workingSet.push_back(derived);
                        newDerivations = true;
                        
                        // Record trace
                        InferenceStep step;
                        step.ruleApplied = rule.name;
                        for (const auto& f : matchedFacts) {
                            step.inputEntryIds.push_back(f.id);
                        }
                        step.outputEntryId = derived.id;
                        step.confidence = static_cast<double>(derived.confidence) / 5.0;
                        step.explanation = "Applied rule '" + rule.name + "' to derive: " + derived.content;
                        lastTrace_.push_back(step);
                    }
                }
            }
        }
        
        if (!newDerivations) break;
    }
    
    // Return only derived facts (not the original ones)
    std::vector<KnowledgeEntry> derived;
    for (size_t i = facts.size(); i < workingSet.size(); i++) {
        derived.push_back(workingSet[i]);
    }
    
    return derived;
}

std::vector<KnowledgeEntry> ChainedInferenceEngine::backwardChain(
    const KnowledgeEntry& goal,
    const std::vector<KnowledgeEntry>& knowledgeBase) {
    
    std::lock_guard<std::mutex> lock(inferenceMutex_);
    
    lastTrace_.clear();
    std::vector<KnowledgeEntry> supportingFacts;
    
    // Find rules that could produce the goal
    for (const auto& rule : rules_) {
        if (!rule.enabled) continue;
        
        // Check if rule conclusion matches goal
        if (rule.conclusion.first == knowledgeTypeToString(goal.type) ||
            containsCaseInsensitive(goal.content, rule.conclusion.second)) {
            
            // Find facts that satisfy the conditions
            std::vector<KnowledgeEntry> matchedFacts;
            bool allConditionsMet = true;
            
            for (const auto& condition : rule.conditions) {
                bool conditionMet = false;
                for (const auto& fact : knowledgeBase) {
                    if (matchCondition(fact, condition)) {
                        matchedFacts.push_back(fact);
                        conditionMet = true;
                        break;
                    }
                }
                if (!conditionMet) {
                    allConditionsMet = false;
                    break;
                }
            }
            
            if (allConditionsMet) {
                for (const auto& f : matchedFacts) {
                    // Avoid duplicates
                    bool found = false;
                    for (const auto& existing : supportingFacts) {
                        if (existing.id == f.id) {
                            found = true;
                            break;
                        }
                    }
                    if (!found) {
                        supportingFacts.push_back(f);
                    }
                }
                
                InferenceStep step;
                step.ruleApplied = rule.name;
                for (const auto& f : matchedFacts) {
                    step.inputEntryIds.push_back(f.id);
                }
                step.outputEntryId = goal.id;
                step.explanation = "Found supporting facts for goal via rule '" + rule.name + "'";
                lastTrace_.push_back(step);
            }
        }
    }
    
    return supportingFacts;
}

std::vector<KnowledgeEntry> ChainedInferenceEngine::abductiveReason(
    const KnowledgeEntry& observation,
    const std::vector<KnowledgeEntry>& knowledgeBase) {
    
    std::lock_guard<std::mutex> lock(inferenceMutex_);
    
    lastTrace_.clear();
    std::vector<KnowledgeEntry> explanations;
    
    // Find rules where the observation could be the conclusion
    // and look for the conditions as potential explanations
    for (const auto& rule : rules_) {
        if (!rule.enabled) continue;
        
        // Check if rule conclusion matches observation
        if (containsCaseInsensitive(observation.content, rule.conclusion.second)) {
            
            // The conditions of this rule could explain the observation
            for (const auto& condition : rule.conditions) {
                // Find facts that match this condition
                for (const auto& fact : knowledgeBase) {
                    if (matchCondition(fact, condition)) {
                        bool found = false;
                        for (const auto& e : explanations) {
                            if (e.id == fact.id) {
                                found = true;
                                break;
                            }
                        }
                        if (!found) {
                            explanations.push_back(fact);
                        }
                    }
                }
            }
            
            if (!explanations.empty()) {
                InferenceStep step;
                step.ruleApplied = rule.name;
                step.outputEntryId = observation.id;
                step.explanation = "Abductive explanation via rule '" + rule.name + "'";
                lastTrace_.push_back(step);
            }
        }
    }
    
    return explanations;
}

std::vector<InferenceStep> ChainedInferenceEngine::getLastInferenceTrace() const {
    std::lock_guard<std::mutex> lock(inferenceMutex_);
    return lastTrace_;
}

void ChainedInferenceEngine::setMaxChainDepth(int depth) {
    maxChainDepth_ = depth;
}

int ChainedInferenceEngine::getMaxChainDepth() const {
    return maxChainDepth_;
}

// ============================================================================
// Phase 2.1: KnowledgeFusionEngine Implementation
// ============================================================================

KnowledgeFusionEngine::KnowledgeFusionEngine() {}

double KnowledgeFusionEngine::calculateSimilarity(
    const KnowledgeEntry& a, 
    const KnowledgeEntry& b) const {
    
    // Simple Jaccard-like similarity based on word overlap
    std::unordered_set<std::string> wordsA, wordsB;
    
    std::istringstream streamA(toLower(a.content));
    std::istringstream streamB(toLower(b.content));
    std::string word;
    
    while (streamA >> word) {
        if (word.size() > 2) wordsA.insert(word);
    }
    while (streamB >> word) {
        if (word.size() > 2) wordsB.insert(word);
    }
    
    if (wordsA.empty() || wordsB.empty()) return 0.0;
    
    size_t intersection = 0;
    for (const auto& w : wordsA) {
        if (wordsB.find(w) != wordsB.end()) {
            intersection++;
        }
    }
    
    size_t unionSize = wordsA.size() + wordsB.size() - intersection;
    return static_cast<double>(intersection) / unionSize;
}

KnowledgeEntry KnowledgeFusionEngine::mergeEntries(
    const std::vector<KnowledgeEntry>& entries) const {
    
    if (entries.empty()) return KnowledgeEntry();
    if (entries.size() == 1) return entries[0];
    
    KnowledgeEntry merged = entries[0];
    
    // Combine content (take longest or most detailed)
    for (size_t i = 1; i < entries.size(); i++) {
        if (entries[i].content.size() > merged.content.size()) {
            merged.content = entries[i].content;
        }
    }
    
    // Take highest confidence
    for (const auto& e : entries) {
        if (static_cast<int>(e.confidence) > static_cast<int>(merged.confidence)) {
            merged.confidence = e.confidence;
        }
    }
    
    // Merge tags
    for (const auto& e : entries) {
        for (const auto& tag : e.tags) {
            merged.addTag(tag);
        }
    }
    
    // Merge related entries
    for (const auto& e : entries) {
        for (const auto& rel : e.related_entries) {
            merged.addRelation(rel);
        }
    }
    
    // Update timestamp
    merged.updated_at = std::chrono::system_clock::now();
    
    return merged;
}

std::vector<KnowledgeEntry> KnowledgeFusionEngine::fuseKnowledge(
    const std::vector<std::vector<KnowledgeEntry>>& sources,
    FusionStrategy strategy) {
    
    std::lock_guard<std::mutex> lock(fusionMutex_);
    
    // Flatten all entries
    std::vector<KnowledgeEntry> allEntries;
    for (const auto& source : sources) {
        for (const auto& entry : source) {
            allEntries.push_back(entry);
        }
    }
    
    // Detect conflicts
    auto conflicts = detectConflicts(allEntries);
    
    // Resolve conflicts
    std::vector<KnowledgeEntry> resolved;
    std::unordered_set<std::string> processedIds;
    
    for (auto& conflict : conflicts) {
        auto resolution = resolveConflict(conflict, strategy);
        resolved.push_back(resolution);
        
        for (const auto& v : conflict.conflictingEntries) {
            processedIds.insert(v.id);
        }
    }
    
    // Add non-conflicting entries
    for (const auto& entry : allEntries) {
        if (processedIds.find(entry.id) == processedIds.end()) {
            resolved.push_back(entry);
            processedIds.insert(entry.id);
        }
    }
    
    return resolved;
}

std::vector<KnowledgeConflict> KnowledgeFusionEngine::detectConflicts(
    const std::vector<KnowledgeEntry>& entries) {
    
    std::vector<KnowledgeConflict> conflicts;
    
    // Group similar entries
    std::vector<std::vector<KnowledgeEntry>> similarGroups;
    std::vector<bool> processed(entries.size(), false);
    
    for (size_t i = 0; i < entries.size(); i++) {
        if (processed[i]) continue;
        
        std::vector<KnowledgeEntry> group = {entries[i]};
        processed[i] = true;
        
        for (size_t j = i + 1; j < entries.size(); j++) {
            if (processed[j]) continue;
            
            double sim = calculateSimilarity(entries[i], entries[j]);
            if (sim > 0.5) {  // Similar enough to be potential conflict
                group.push_back(entries[j]);
                processed[j] = true;
            }
        }
        
        if (group.size() > 1) {
            similarGroups.push_back(group);
        }
    }
    
    // Create conflicts from groups
    for (const auto& group : similarGroups) {
        KnowledgeConflict conflict;
        // conflict has no entryId field;
        conflict.conflictingEntries = group;
        
        // Determine conflict type
        bool typesDiffer = false;
        bool confidencesDiffer = false;
        
        for (size_t i = 1; i < group.size(); i++) {
            if (group[i].type != group[0].type) typesDiffer = true;
            if (group[i].confidence != group[0].confidence) confidencesDiffer = true;
        }
        
        if (typesDiffer) {
            conflict.conflictType = "type";
        } else if (confidencesDiffer) {
            conflict.conflictType = "confidence";
        } else {
            conflict.conflictType = "value";
        }
        
        conflicts.push_back(conflict);
    }
    
    unresolvedConflicts_ = conflicts;
    return conflicts;
}

KnowledgeEntry KnowledgeFusionEngine::resolveConflict(
    const KnowledgeConflict& conflict,
    FusionStrategy strategy) {
    
    const auto& versions = conflict.conflictingEntries;
    
    if (versions.empty()) return KnowledgeEntry();
    if (versions.size() == 1) return versions[0];
    
    switch (strategy) {
        case FusionStrategy::HIGHEST_CONFIDENCE: {
            KnowledgeEntry best = versions[0];
            for (const auto& v : versions) {
                if (static_cast<int>(v.confidence) > static_cast<int>(best.confidence)) {
                    best = v;
                }
            }
            return best;
        }
        
        case FusionStrategy::MOST_RECENT: {
            KnowledgeEntry best = versions[0];
            for (const auto& v : versions) {
                if (v.updated_at > best.updated_at) {
                    best = v;
                }
            }
            return best;
        }
        
        case FusionStrategy::CONSENSUS: {
            // Take the version that appears most often (by content similarity)
            std::vector<int> votes(versions.size(), 0);
            for (size_t i = 0; i < versions.size(); i++) {
                for (size_t j = i + 1; j < versions.size(); j++) {
                    double sim = calculateSimilarity(versions[i], versions[j]);
                    if (sim > 0.8) {
                        votes[i]++;
                        votes[j]++;
                    }
                }
            }
            
            int bestIdx = 0;
            for (size_t i = 1; i < votes.size(); i++) {
                if (votes[i] > votes[bestIdx]) {
                    bestIdx = i;
                }
            }
            return versions[bestIdx];
        }
        
        case FusionStrategy::WEIGHTED_AVERAGE:
        case FusionStrategy::MANUAL:
        default:
            // Fall back to merging
            return mergeEntries(versions);
    }
}

void KnowledgeFusionEngine::recordVersion(
    const KnowledgeEntry& entry,
    const std::string& reason,
    const std::string& changedBy) {
    
    std::lock_guard<std::mutex> lock(fusionMutex_);
    
    KnowledgeVersion version;
    version.versionId = makeKnowledgeEntryId();
    // entryId stored in entry itself;
    version.entry = entry;
    version.timestamp = std::chrono::system_clock::now();
    version.reason = reason;
    version.changedBy = changedBy;
    
    versionHistory_[entry.id].push_back(version);
}

std::vector<KnowledgeVersion> KnowledgeFusionEngine::getVersionHistory(
    const std::string& entryId) const {
    
    std::lock_guard<std::mutex> lock(fusionMutex_);
    
    auto it = versionHistory_.find(entryId);
    if (it != versionHistory_.end()) {
        return it->second;
    }
    return {};
}

std::optional<KnowledgeEntry> KnowledgeFusionEngine::getVersionAtLocked(
    const std::string& entryId,
    const Timestamp& at) const {
    // Precondition: fusionMutex_ already held by caller.
    auto it = versionHistory_.find(entryId);
    if (it == versionHistory_.end()) return std::nullopt;

    // Find the most recent version at or before 'at'
    const KnowledgeVersion* best = nullptr;
    for (const auto& v : it->second) {
        if (v.timestamp <= at) {
            if (!best || v.timestamp > best->timestamp) {
                best = &v;
            }
        }
    }

    if (best) {
        return best->entry;
    }
    return std::nullopt;
}

std::optional<KnowledgeEntry> KnowledgeFusionEngine::getVersionAt(
    const std::string& entryId,
    const Timestamp& at) const {
    std::lock_guard<std::mutex> lock(fusionMutex_);
    return getVersionAtLocked(entryId, at);
}

bool KnowledgeFusionEngine::isValidAtLocked(
    const std::string& entryId,
    const Timestamp& at) const {
    // Precondition: fusionMutex_ already held by caller.
    auto it = validityWindows_.find(entryId);
    if (it == validityWindows_.end()) {
        // No explicit window => entry is unconstrained (always temporally valid).
        return true;
    }
    const auto& [validFrom, validUntil] = it->second;
    if (validFrom && at < *validFrom) return false;
    if (validUntil && at >= *validUntil) return false;
    return true;
}

std::optional<KnowledgeEntry> KnowledgeFusionEngine::rollbackToVersion(
    const std::string& versionId) {
    
    std::lock_guard<std::mutex> lock(fusionMutex_);
    
    for (const auto& [entryId, versions] : versionHistory_) {
        for (const auto& v : versions) {
            if (v.versionId == versionId) {
                return v.entry;
            }
        }
    }
    return std::nullopt;
}

void KnowledgeFusionEngine::setTemporalValidity(
    const std::string& entryId,
    std::optional<Timestamp> validFrom,
    std::optional<Timestamp> validUntil) {

    std::lock_guard<std::mutex> lock(fusionMutex_);

    // Record the explicit validity window for this entry. This is consulted by
    // getValidKnowledge() (via isValidAtLocked) so that temporally-bounded
    // knowledge is included only while within its window. Passing two unset
    // optionals clears any prior constraint (entry becomes always-valid).
    if (!validFrom && !validUntil) {
        validityWindows_.erase(entryId);
    } else {
        validityWindows_[entryId] = {validFrom, validUntil};
    }

    // Also reflect the window onto the most recent recorded version's entry
    // metadata so callers reading version history observe the validity bounds.
    // KnowledgeEntry has no dedicated validity fields, so we encode the bounds
    // as epoch-nanosecond strings under reserved metadata keys (absent key =>
    // unbounded on that side).
    auto it = versionHistory_.find(entryId);
    if (it != versionHistory_.end() && !it->second.empty()) {
        KnowledgeVersion* latest = &it->second.front();
        for (auto& v : it->second) {
            if (v.timestamp > latest->timestamp) latest = &v;
        }
        auto toNanos = [](const Timestamp& t) {
            return std::to_string(std::chrono::duration_cast<std::chrono::nanoseconds>(
                t.time_since_epoch()).count());
        };
        latest->entry.metadata.erase("_validFrom");
        latest->entry.metadata.erase("_validUntil");
        if (validFrom) latest->entry.metadata["_validFrom"] = toNanos(*validFrom);
        if (validUntil) latest->entry.metadata["_validUntil"] = toNanos(*validUntil);
    }
}

std::vector<KnowledgeEntry> KnowledgeFusionEngine::getValidKnowledge(
    const Timestamp& at) const {

    std::lock_guard<std::mutex> lock(fusionMutex_);

    std::vector<KnowledgeEntry> valid;
    for (const auto& [entryId, versions] : versionHistory_) {
        // Honor explicit temporal-validity windows.
        if (!isValidAtLocked(entryId, at)) {
            continue;
        }
        auto entry = getVersionAtLocked(entryId, at);
        if (entry) {
            valid.push_back(*entry);
        }
    }
    return valid;
}

// ============================================================================
// Phase 2.1: EnhancedKnowledgeBase Implementation
// ============================================================================

EnhancedKnowledgeBase::EnhancedKnowledgeBase()
    : KnowledgeBase()
    , inferenceEngine_(std::make_unique<ChainedInferenceEngine>())
    , fusionEngine_(std::make_unique<KnowledgeFusionEngine>()) {}

void EnhancedKnowledgeBase::enableHypergraph(bool enable) {
    hypergraphEnabled_ = enable;
}

std::string EnhancedKnowledgeBase::addRelationship(
    const std::string& relationshipType,
    const std::vector<std::string>& nodeIds,
    double weight) {
    
    if (!hypergraphEnabled_) {
        enableHypergraph(true);
    }
    
    // Ensure all nodes exist in hypergraph
    for (const auto& nodeId : nodeIds) {
        auto entry = getKnowledge(nodeId);
        if (entry) {
            hypergraph_.addNode(*entry);
        }
    }
    
    Hyperedge edge(relationshipType, nodeIds);
    edge.weight = weight;
    
    return hypergraph_.addEdge(edge);
}

std::vector<KnowledgeEntry> EnhancedKnowledgeBase::queryByRelationship(
    const std::string& relationshipType,
    const std::string& involvedNodeId) {
    
    if (!hypergraphEnabled_) return {};
    
    std::vector<KnowledgeEntry> results;
    
    auto edges = hypergraph_.getEdgesByType(relationshipType);
    
    for (const auto& edge : edges) {
        if (involvedNodeId.empty() || edge.connects(involvedNodeId)) {
            for (const auto& nodeId : edge.nodeIds) {
                auto node = hypergraph_.getNode(nodeId);
                if (node) {
                    // Avoid duplicates
                    bool found = false;
                    for (const auto& r : results) {
                        if (r.id == node->id) {
                            found = true;
                            break;
                        }
                    }
                    if (!found) {
                        results.push_back(*node);
                    }
                }
            }
        }
    }
    
    return results;
}

std::vector<KnowledgeEntry> EnhancedKnowledgeBase::infer(const KnowledgeQuery& query) {
    // Get relevant facts
    auto facts = this->query(query);
    
    // Run forward chaining
    auto inferred = inferenceEngine_->forwardChain(facts);
    
    return inferred;
}

void EnhancedKnowledgeBase::enableAutoInference(bool enable) {
    autoInferenceEnabled_ = enable;
}

void EnhancedKnowledgeBase::importAndFuse(
    const std::vector<KnowledgeEntry>& newKnowledge,
    KnowledgeFusionEngine::FusionStrategy strategy) {
    
    // Get existing knowledge
    std::vector<KnowledgeEntry> existing;
    auto allTags = getAllTags();  // Get some existing entries
    for (const auto& tag : allTags) {
        auto entries = searchByTags({tag});
        for (const auto& e : entries) {
            existing.push_back(e);
        }
    }
    
    // Fuse with new knowledge
    std::vector<std::vector<KnowledgeEntry>> sources = {existing, newKnowledge};
    auto fused = fusionEngine_->fuseKnowledge(sources, strategy);
    
    // Update knowledge base with fused entries
    for (const auto& entry : fused) {
        auto existingEntry = getKnowledge(entry.id);
        if (existingEntry) {
            updateKnowledge(entry.id, entry);
        } else {
            addKnowledge(entry);
        }
        
        // Record version
        fusionEngine_->recordVersion(entry, "Fused from multiple sources");
    }
}

std::vector<KnowledgeEntry> EnhancedKnowledgeBase::semanticSearch(
    const std::string& query,
    int maxResults,
    double minSimilarity) {
    
    // Get all entries and filter by similarity
    auto allEntries = searchByText(query, maxResults * 3);  // Get more for filtering
    
    std::vector<std::pair<KnowledgeEntry, double>> scored;
    
    KnowledgeEntry queryEntry;
    queryEntry.content = query;
    
    for (const auto& entry : allEntries) {
        // Calculate simple word overlap similarity
        double sim = 0.5;
        if (fusionEngine_) {
            sim = fusionEngine_->calculateSimilarity(queryEntry, entry);
        }
        
        if (sim >= minSimilarity) {
            scored.emplace_back(entry, sim);
        }
    }
    
    // Sort by similarity
    std::sort(scored.begin(), scored.end(),
              [](const auto& a, const auto& b) { return a.second > b.second; });
    
    // Return top results
    std::vector<KnowledgeEntry> results;
    for (size_t i = 0; i < std::min(static_cast<size_t>(maxResults), scored.size()); i++) {
        results.push_back(scored[i].first);
    }
    
    return results;
}

std::vector<std::vector<KnowledgeEntry>> EnhancedKnowledgeBase::findConnectionPaths(
    const std::string& fromId,
    const std::string& toId,
    int maxDepth) {
    
    if (!hypergraphEnabled_) return {};
    
    auto paths = hypergraph_.findPaths(fromId, toId, maxDepth);
    
    std::vector<std::vector<KnowledgeEntry>> entryPaths;
    
    for (const auto& path : paths) {
        std::vector<KnowledgeEntry> entryPath;
        for (const auto& nodeId : path) {
            auto node = hypergraph_.getNode(nodeId);
            if (node) {
                entryPath.push_back(*node);
            }
        }
        if (!entryPath.empty()) {
            entryPaths.push_back(entryPath);
        }
    }
    
    return entryPaths;
}

std::vector<std::vector<std::string>> EnhancedKnowledgeBase::clusterKnowledge(int numClusters) {
    // Simple clustering based on content similarity
    std::vector<std::vector<std::string>> clusters(numClusters);
    
    // Get all entries
    std::vector<KnowledgeEntry> allEntries;
    auto types = {KnowledgeType::FACT, KnowledgeType::RULE, KnowledgeType::CONCEPT,
                  KnowledgeType::RELATIONSHIP, KnowledgeType::PROCEDURE, KnowledgeType::EXPERIENCE};
    
    for (auto type : types) {
        auto entries = getKnowledgeByType(type);
        for (const auto& e : entries) {
            allEntries.push_back(e);
        }
    }
    
    if (allEntries.empty()) return clusters;
    
    // Simple round-robin assignment (a real implementation would use k-means)
    for (size_t i = 0; i < allEntries.size(); i++) {
        clusters[i % numClusters].push_back(allEntries[i].id);
    }
    
    return clusters;
}

EnhancedKnowledgeBase::EnhancedStats EnhancedKnowledgeBase::getEnhancedStats() const {
    std::lock_guard<std::mutex> lock(enhancedMutex_);
    
    EnhancedStats stats;
    stats.totalEntries = getKnowledgeCount();
    stats.totalRelationships = hypergraphEnabled_ ? hypergraph_.edgeCount() : 0;
    
    // Count inferred entries
    stats.inferredEntries = 0;
    auto types = {KnowledgeType::FACT, KnowledgeType::RULE, KnowledgeType::CONCEPT};
    for (auto type : types) {
        auto entries = const_cast<EnhancedKnowledgeBase*>(this)->getKnowledgeByType(type);
        for (const auto& e : entries) {
            if (e.source == KnowledgeSource::INFERRED) {
                stats.inferredEntries++;
            }
        }
    }
    
    // Conflicts pending would need access to fusion engine's internal state
    stats.conflictsPending = 0;
    
    stats.graphDensity = hypergraphEnabled_ && hypergraph_.nodeCount() > 0 ?
        static_cast<double>(hypergraph_.edgeCount()) / hypergraph_.nodeCount() : 0.0;
    
    stats.averagePathLength = 0;  // Would require path analysis
    
    return stats;
}


} // namespace elizaos
