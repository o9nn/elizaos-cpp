// knowledge.cpp - Full implementation of KnowledgeBase system
#include "elizaos/knowledge.hpp"
#include <algorithm>
#include <sstream>
#include <random>
#include <fstream>
#include <iomanip>
#include <atomic>
#include <cctype>

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
    KnowledgeEntry related_concept_entry("Related concept for: " + entry.content, KnowledgeType::CONCEPT);
    related_concept_entry.source = KnowledgeSource::INFERRED;
    related_concept_entry.confidence = ConfidenceLevel::MEDIUM;
    related_concept_entry.addTag("related_concept");
    for (const auto& tag : entry.tags) related_concept_entry.addTag(tag);
    related.push_back(related_concept_entry);
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

} // namespace elizaos
