/**
 * ElizaOS C++ — Enhanced CognitiveBridge implementation.
 *
 * Implements the Phase 3.2 Distributed Cognitive State System declared in
 * include/cognitive_bridge.hpp:
 *
 *   - CompressedCognitiveState : deterministic serialization + RLE compression
 *   - CognitiveSyncProtocol    : multi-agent state sync, conflict detection,
 *                                leader election, consensus computation
 *   - CognitiveStateVisualizer : timeline/graph/metrics export + streaming
 *   - EnhancedCognitiveBridge  : snapshots, replay, compression, cluster ops
 *
 * All behavior is in-process and deterministic so it is fully reproducible
 * in CI, mirroring the design of the base CognitiveBridge.
 */
#include "elizaos/cognitive_bridge.hpp"

#include <algorithm>
#include <cmath>
#include <cstring>
#include <map>
#include <iomanip>
#include <sstream>
#include <stdexcept>

namespace elizaos {

// ============================================================================
// Internal serialization helpers (deterministic, versioned, self-contained)
// ============================================================================

namespace {

constexpr char kSerializationVersion = 1;

void appendString(std::vector<uint8_t>& out, const std::string& s) {
    const uint32_t n = static_cast<uint32_t>(s.size());
    out.push_back(static_cast<uint8_t>(n & 0xFF));
    out.push_back(static_cast<uint8_t>((n >> 8) & 0xFF));
    out.push_back(static_cast<uint8_t>((n >> 16) & 0xFF));
    out.push_back(static_cast<uint8_t>((n >> 24) & 0xFF));
    out.insert(out.end(), s.begin(), s.end());
}

std::string readString(const std::vector<uint8_t>& in, size_t& pos) {
    if (pos + 4 > in.size()) return {};
    uint32_t n = static_cast<uint32_t>(in[pos]) |
                 (static_cast<uint32_t>(in[pos + 1]) << 8) |
                 (static_cast<uint32_t>(in[pos + 2]) << 16) |
                 (static_cast<uint32_t>(in[pos + 3]) << 24);
    pos += 4;
    if (pos + n > in.size()) return {};
    std::string s(in.begin() + static_cast<long>(pos),
                  in.begin() + static_cast<long>(pos + n));
    pos += n;
    return s;
}

void appendDouble(std::vector<uint8_t>& out, double v) {
    uint64_t bits = 0;
    static_assert(sizeof(bits) == sizeof(v), "double must be 64-bit");
    std::memcpy(&bits, &v, sizeof(bits));
    for (int i = 0; i < 8; ++i) {
        out.push_back(static_cast<uint8_t>((bits >> (8 * i)) & 0xFF));
    }
}

double readDouble(const std::vector<uint8_t>& in, size_t& pos) {
    if (pos + 8 > in.size()) return 0.0;
    uint64_t bits = 0;
    for (int i = 0; i < 8; ++i) {
        bits |= (static_cast<uint64_t>(in[pos + static_cast<size_t>(i)]) << (8 * i));
    }
    pos += 8;
    double v = 0.0;
    std::memcpy(&v, &bits, sizeof(v));
    return v;
}

void appendInt32(std::vector<uint8_t>& out, int32_t v) {
    const uint32_t u = static_cast<uint32_t>(v);
    out.push_back(static_cast<uint8_t>(u & 0xFF));
    out.push_back(static_cast<uint8_t>((u >> 8) & 0xFF));
    out.push_back(static_cast<uint8_t>((u >> 16) & 0xFF));
    out.push_back(static_cast<uint8_t>((u >> 24) & 0xFF));
}

int32_t readInt32(const std::vector<uint8_t>& in, size_t& pos) {
    if (pos + 4 > in.size()) return 0;
    uint32_t u = static_cast<uint32_t>(in[pos]) |
                 (static_cast<uint32_t>(in[pos + 1]) << 8) |
                 (static_cast<uint32_t>(in[pos + 2]) << 16) |
                 (static_cast<uint32_t>(in[pos + 3]) << 24);
    pos += 4;
    return static_cast<int32_t>(u);
}

std::vector<uint8_t> serializeState(const CognitiveState& state) {
    std::vector<uint8_t> out;
    out.push_back(static_cast<uint8_t>(kSerializationVersion));
    appendString(out, state.agentId);
    appendString(out, state.mood);
    appendString(out, state.focus);
    appendInt32(out, state.echobeatsStep);
    appendInt32(out, state.echobeatsPhase);
    appendDouble(out, state.valence);
    appendDouble(out, state.arousal);
    // Metadata is emitted in sorted key order so serialization is
    // deterministic regardless of unordered_map iteration order.
    std::vector<std::pair<std::string, std::string>> meta(state.metadata.begin(),
                                                          state.metadata.end());
    std::sort(meta.begin(), meta.end());
    appendInt32(out, static_cast<int32_t>(meta.size()));
    for (const auto& kv : meta) {
        appendString(out, kv.first);
        appendString(out, kv.second);
    }
    const auto epochNs = std::chrono::duration_cast<std::chrono::nanoseconds>(
                             state.timestamp.time_since_epoch())
                             .count();
    appendDouble(out, static_cast<double>(epochNs));
    return out;
}

CognitiveState deserializeState(const std::vector<uint8_t>& in) {
    CognitiveState state;
    size_t pos = 0;
    if (in.empty() || in[pos++] != static_cast<uint8_t>(kSerializationVersion)) {
        return state;
    }
    state.agentId = readString(in, pos);
    state.mood = readString(in, pos);
    state.focus = readString(in, pos);
    state.echobeatsStep = readInt32(in, pos);
    state.echobeatsPhase = readInt32(in, pos);
    state.valence = readDouble(in, pos);
    state.arousal = readDouble(in, pos);
    const int32_t metaCount = readInt32(in, pos);
    for (int32_t i = 0; i < metaCount; ++i) {
        std::string key = readString(in, pos);
        std::string value = readString(in, pos);
        state.metadata[key] = value;
    }
    const double epochNs = readDouble(in, pos);
    state.timestamp = std::chrono::system_clock::time_point(
        std::chrono::duration_cast<std::chrono::system_clock::duration>(
            std::chrono::nanoseconds(static_cast<long long>(epochNs))));
    return state;
}

// Byte-level run-length encoding: (count, byte) pairs. Deterministic and
// dependency-free; typical cognitive states compress well because of long
// runs of zero bytes in the fixed-width numeric fields.
std::vector<uint8_t> rleEncode(const std::vector<uint8_t>& in) {
    std::vector<uint8_t> out;
    size_t i = 0;
    while (i < in.size()) {
        uint8_t value = in[i];
        size_t run = 1;
        while (i + run < in.size() && in[i + run] == value && run < 255) {
            ++run;
        }
        out.push_back(static_cast<uint8_t>(run));
        out.push_back(value);
        i += run;
    }
    return out;
}

std::vector<uint8_t> rleDecode(const std::vector<uint8_t>& in) {
    std::vector<uint8_t> out;
    for (size_t i = 0; i + 1 < in.size(); i += 2) {
        const uint8_t run = in[i];
        const uint8_t value = in[i + 1];
        out.insert(out.end(), run, value);
    }
    return out;
}

// FNV-1a checksum rendered as hex — stable across platforms.
std::string fnv1aChecksum(const std::vector<uint8_t>& data) {
    uint64_t hash = 1469598103934665603ULL;
    for (uint8_t b : data) {
        hash ^= b;
        hash *= 1099511628211ULL;
    }
    std::ostringstream oss;
    oss << std::hex << std::setw(16) << std::setfill('0') << hash;
    return oss.str();
}

std::string escapeJson(const std::string& s) {
    std::ostringstream oss;
    for (char c : s) {
        switch (c) {
            case '"': oss << "\\\""; break;
            case '\\': oss << "\\\\"; break;
            case '\n': oss << "\\n"; break;
            case '\r': oss << "\\r"; break;
            case '\t': oss << "\\t"; break;
            default:
                if (static_cast<unsigned char>(c) < 0x20) {
                    oss << "\\u" << std::hex << std::setw(4) << std::setfill('0')
                        << static_cast<int>(c);
                } else {
                    oss << c;
                }
        }
    }
    return oss.str();
}

std::string stateToJson(const CognitiveState& state) {
    std::ostringstream oss;
    oss << "{\"agentId\":\"" << escapeJson(state.agentId) << "\","
        << "\"mood\":\"" << escapeJson(state.mood) << "\","
        << "\"focus\":\"" << escapeJson(state.focus) << "\","
        << "\"echobeatsStep\":" << state.echobeatsStep << ","
        << "\"echobeatsPhase\":" << state.echobeatsPhase << ","
        << "\"valence\":" << state.valence << ","
        << "\"arousal\":" << state.arousal << "}";
    return oss.str();
}

}  // namespace

// ============================================================================
// CompressedCognitiveState
// ============================================================================

CompressedCognitiveState CompressedCognitiveState::compress(
    const CognitiveState& state, const std::string& algorithm) {
    CompressedCognitiveState out;
    const std::vector<uint8_t> raw = serializeState(state);
    out.originalSize = raw.size();
    if (algorithm == "none") {
        out.data = raw;
        out.compressionAlgorithm = "none";
    } else {
        // "lz4" and "zstd" requests are honored with the built-in RLE codec so
        // the module carries no external dependencies. The label preserves the
        // caller's requested algorithm so peers negotiate consistently; every
        // non-"none" label decodes through the same RLE path.
        out.data = rleEncode(raw);
        out.compressionAlgorithm = algorithm;
    }
    out.checksum = fnv1aChecksum(raw);
    return out;
}

CognitiveState CompressedCognitiveState::decompress() const {
    std::vector<uint8_t> raw;
    if (compressionAlgorithm == "none") {
        raw = data;
    } else {
        raw = rleDecode(data);
    }
    // Integrity gate: a checksum mismatch must not propagate corrupted
    // cognition — fail loudly so callers can drop or re-request the state.
    if (!checksum.empty() && fnv1aChecksum(raw) != checksum) {
        throw std::runtime_error(
            "CompressedCognitiveState: checksum mismatch (corrupted payload)");
    }
    return deserializeState(raw);
}

// ============================================================================
// CognitiveSyncProtocol
// ============================================================================

CognitiveSyncProtocol::CognitiveSyncProtocol(const std::string& agentId,
                                             const SyncConfig& config)
    : localAgentId_(agentId), config_(config) {
    running_.store(true);
    // The local agent is always a member of its own cluster.
    CognitiveState self;
    self.agentId = localAgentId_;
    remoteStates_[localAgentId_] = self;
}

CognitiveSyncProtocol::~CognitiveSyncProtocol() {
    running_.store(false);
    if (syncThread_.joinable()) {
        syncThread_.join();
    }
}

void CognitiveSyncProtocol::registerAgent(const std::string& agentId) {
    std::lock_guard<std::mutex> lock(syncMutex_);
    if (remoteStates_.find(agentId) == remoteStates_.end()) {
        CognitiveState placeholderState;
        placeholderState.agentId = agentId;
        remoteStates_[agentId] = placeholderState;
    }
    // First registered agent becomes leader if none was elected yet
    // (LEADER_FOLLOWER bootstrap).
    if (config_.mode == SyncMode::LEADER_FOLLOWER && currentLeader_.empty()) {
        currentLeader_ = agentId;
    }
}

void CognitiveSyncProtocol::unregisterAgent(const std::string& agentId) {
    std::lock_guard<std::mutex> lock(syncMutex_);
    remoteStates_.erase(agentId);
    if (currentLeader_ == agentId) {
        // Deterministic re-election: lexicographically smallest remaining id.
        currentLeader_.clear();
        for (const auto& kv : remoteStates_) {
            if (currentLeader_.empty() || kv.first < currentLeader_) {
                currentLeader_ = kv.first;
            }
        }
    }
}

std::vector<std::string> CognitiveSyncProtocol::getRegisteredAgents() const {
    std::lock_guard<std::mutex> lock(syncMutex_);
    std::vector<std::string> ids;
    ids.reserve(remoteStates_.size());
    for (const auto& kv : remoteStates_) {
        ids.push_back(kv.first);
    }
    std::sort(ids.begin(), ids.end());
    return ids;
}

void CognitiveSyncProtocol::publishLocalState(const CognitiveState& state) {
    std::lock_guard<std::mutex> lock(syncMutex_);
    CognitiveState copy = state;
    if (copy.agentId.empty()) {
        copy.agentId = localAgentId_;
    }
    remoteStates_[copy.agentId] = copy;
}

std::vector<CognitiveState> CognitiveSyncProtocol::getRemoteStates() const {
    std::lock_guard<std::mutex> lock(syncMutex_);
    std::vector<CognitiveState> states;
    states.reserve(remoteStates_.size());
    // Deterministic order (sorted by agentId).
    std::vector<std::string> ids;
    for (const auto& kv : remoteStates_) ids.push_back(kv.first);
    std::sort(ids.begin(), ids.end());
    for (const auto& id : ids) {
        auto it = remoteStates_.find(id);
        if (it != remoteStates_.end() && it->first != localAgentId_) {
            states.push_back(it->second);
        }
    }
    return states;
}

CognitiveState CognitiveSyncProtocol::getConsensusState() const {
    std::lock_guard<std::mutex> lock(syncMutex_);
    CognitiveState consensus;
    consensus.agentId = "consensus";
    if (remoteStates_.empty()) {
        return consensus;
    }
    // LEADER_FOLLOWER: consensus is the leader's state verbatim.
    if (config_.mode == SyncMode::LEADER_FOLLOWER && !currentLeader_.empty()) {
        auto it = remoteStates_.find(currentLeader_);
        if (it != remoteStates_.end()) {
            consensus = it->second;
            consensus.agentId = "consensus";
            return consensus;
        }
    }
    // Other modes: numeric fields are averaged; categorical fields adopt the
    // most frequent value (ties broken lexicographically for determinism).
    double valenceSum = 0.0;
    double arousalSum = 0.0;
    std::map<std::string, int> moodVotes;
    std::map<std::string, int> focusVotes;
    int stepMax = 0;
    size_t contributors = 0;
    for (const auto& kv : remoteStates_) {
        // Placeholder entries (registered but never published: empty mood and
        // focus with zeroed affect) carry no cognitive evidence and must not
        // dilute consensus.
        const auto& s = kv.second;
        const bool placeholder = s.mood.empty() && s.focus.empty() &&
                                 s.valence == 0.0 && s.arousal == 0.0 &&
                                 s.echobeatsStep == 0;
        if (placeholder) continue;
        ++contributors;
        valenceSum += s.valence;
        arousalSum += s.arousal;
        if (!s.mood.empty()) moodVotes[s.mood]++;
        if (!s.focus.empty()) focusVotes[s.focus]++;
        stepMax = std::max(stepMax, s.echobeatsStep);
    }
    if (contributors == 0) {
        return consensus;
    }
    const double n = static_cast<double>(contributors);
    consensus.valence = valenceSum / n;
    consensus.arousal = arousalSum / n;
    consensus.echobeatsStep = stepMax;
    consensus.echobeatsPhase = CognitiveBridge::phaseForStep(stepMax);
    auto pickMajority = [](const std::map<std::string, int>& votes) {
        std::string winner;
        int best = 0;
        for (const auto& kv : votes) {
            if (kv.second > best) {
                best = kv.second;
                winner = kv.first;
            }
        }
        return winner;
    };
    consensus.mood = pickMajority(moodVotes);
    consensus.focus = pickMajority(focusVotes);
    return consensus;
}

std::vector<CognitiveSyncProtocol::ConflictInfo>
CognitiveSyncProtocol::detectConflicts() const {
    std::lock_guard<std::mutex> lock(syncMutex_);
    std::vector<ConflictInfo> conflicts;
    if (remoteStates_.size() < 2) {
        return conflicts;
    }
    // Field-level divergence detection across all registered agents.
    auto collect = [this](auto accessor) {
        std::vector<std::pair<std::string, std::string>> values;
        for (const auto& kv : remoteStates_) {
            values.emplace_back(kv.first, accessor(kv.second));
        }
        std::sort(values.begin(), values.end());
        return values;
    };
    auto diverges = [](const std::vector<std::pair<std::string, std::string>>& values) {
        for (size_t i = 1; i < values.size(); ++i) {
            if (values[i].second != values[0].second) return true;
        }
        return false;
    };
    struct FieldSpec {
        const char* name;
        std::function<std::string(const CognitiveState&)> accessor;
    };
    const std::vector<FieldSpec> fields = {
        {"mood", [](const CognitiveState& s) { return s.mood; }},
        {"focus", [](const CognitiveState& s) { return s.focus; }},
    };
    for (const auto& field : fields) {
        auto values = collect(field.accessor);
        if (diverges(values)) {
            ConflictInfo info;
            info.field = field.name;
            info.agentValues = values;
            // Auto-resolution: majority vote, ties broken lexicographically.
            std::map<std::string, int> votes;
            for (const auto& v : values) votes[v.second]++;
            int best = 0;
            for (const auto& kv : votes) {
                if (kv.second > best) {
                    best = kv.second;
                    info.resolvedValue = kv.first;
                }
            }
            info.autoResolved = true;
            conflicts.push_back(std::move(info));
        }
    }
    // Numeric divergence beyond the configured threshold is also a conflict.
    double minValence = 1.0, maxValence = -1.0;
    for (const auto& kv : remoteStates_) {
        minValence = std::min(minValence, kv.second.valence);
        maxValence = std::max(maxValence, kv.second.valence);
    }
    if (remoteStates_.size() >= 2 &&
        (maxValence - minValence) > config_.conflictThreshold) {
        ConflictInfo info;
        info.field = "valence";
        for (const auto& kv : remoteStates_) {
            std::ostringstream oss;
            oss << kv.second.valence;
            info.agentValues.emplace_back(kv.first, oss.str());
        }
        std::sort(info.agentValues.begin(), info.agentValues.end());
        std::ostringstream oss;
        oss << ((minValence + maxValence) / 2.0);
        info.resolvedValue = oss.str();
        info.autoResolved = true;
        conflicts.push_back(std::move(info));
    }
    return conflicts;
}

void CognitiveSyncProtocol::resolveConflict(const std::string& field,
                                            const std::string& resolution) {
    std::lock_guard<std::mutex> lock(syncMutex_);
    for (auto& kv : remoteStates_) {
        if (field == "mood") {
            kv.second.mood = resolution;
        } else if (field == "focus") {
            kv.second.focus = resolution;
        } else if (field == "valence") {
            try {
                kv.second.valence = std::stod(resolution);
            } catch (...) {
                // Non-numeric resolution for a numeric field is ignored.
            }
        }
    }
}

void CognitiveSyncProtocol::nominateSelfAsLeader() {
    std::lock_guard<std::mutex> lock(syncMutex_);
    currentLeader_ = localAgentId_;
    // Ensure the local agent is part of the registry once it leads.
    if (remoteStates_.find(localAgentId_) == remoteStates_.end()) {
        CognitiveState self;
        self.agentId = localAgentId_;
        remoteStates_[localAgentId_] = self;
    }
}

std::string CognitiveSyncProtocol::getCurrentLeader() const {
    std::lock_guard<std::mutex> lock(syncMutex_);
    return currentLeader_;
}

bool CognitiveSyncProtocol::isLeader() const {
    std::lock_guard<std::mutex> lock(syncMutex_);
    return !currentLeader_.empty() && currentLeader_ == localAgentId_;
}

CognitiveSyncProtocol::SyncStatus CognitiveSyncProtocol::getSyncStatus() const {
    std::lock_guard<std::mutex> lock(syncMutex_);
    SyncStatus status;
    status.pendingUpdates = 0;
    status.lastSyncLatency = std::chrono::milliseconds(0);
    // Divergence score: normalized spread of valence/arousal across agents.
    if (remoteStates_.size() < 2) {
        status.synchronized = true;
        status.divergenceScore = 0.0;
        return status;
    }
    double minV = 1.0, maxV = -1.0, minA = 1.0, maxA = 0.0;
    for (const auto& kv : remoteStates_) {
        minV = std::min(minV, kv.second.valence);
        maxV = std::max(maxV, kv.second.valence);
        minA = std::min(minA, kv.second.arousal);
        maxA = std::max(maxA, kv.second.arousal);
    }
    const double spreadV = std::max(0.0, maxV - minV) / 2.0;  // valence range is 2
    const double spreadA = std::max(0.0, maxA - minA);        // arousal range is 1
    status.divergenceScore = std::min(1.0, (spreadV + spreadA) / 2.0);
    status.synchronized = status.divergenceScore <= config_.conflictThreshold;
    return status;
}

// ============================================================================
// CognitiveStateVisualizer
// ============================================================================

CognitiveStateVisualizer::CognitiveStateVisualizer() = default;

void CognitiveStateVisualizer::addState(const CognitiveState& state) {
    std::lock_guard<std::mutex> lock(vizMutex_);
    states_.push_back(state);
}

void CognitiveStateVisualizer::addTransition(const CognitiveState& from,
                                             const CognitiveState& to) {
    std::lock_guard<std::mutex> lock(vizMutex_);
    states_.push_back(from);
    const size_t fromIdx = states_.size() - 1;
    states_.push_back(to);
    const size_t toIdx = states_.size() - 1;
    transitions_.emplace_back(fromIdx, toIdx);
}

std::string CognitiveStateVisualizer::exportTimeline(ExportFormat format) const {
    std::lock_guard<std::mutex> lock(vizMutex_);
    std::ostringstream oss;
    switch (format) {
        case ExportFormat::JSON:
        case ExportFormat::D3_COMPATIBLE: {
            oss << "{\"timeline\":[";
            for (size_t i = 0; i < states_.size(); ++i) {
                if (i) oss << ",";
                oss << stateToJson(states_[i]);
            }
            oss << "],\"count\":" << states_.size() << "}";
            break;
        }
        case ExportFormat::GRAPHVIZ_DOT: {
            oss << "digraph timeline {\n";
            for (size_t i = 0; i < states_.size(); ++i) {
                oss << "  s" << i << " [label=\"" << escapeJson(states_[i].mood)
                    << "/" << states_[i].echobeatsStep << "\"];\n";
                if (i > 0) {
                    oss << "  s" << (i - 1) << " -> s" << i << ";\n";
                }
            }
            oss << "}\n";
            break;
        }
        case ExportFormat::PROMETHEUS_METRICS: {
            oss << exportMetricsUnlocked();
            break;
        }
    }
    return oss.str();
}

std::string CognitiveStateVisualizer::exportStateGraph(ExportFormat format) const {
    std::lock_guard<std::mutex> lock(vizMutex_);
    std::ostringstream oss;
    if (format == ExportFormat::JSON || format == ExportFormat::D3_COMPATIBLE) {
        oss << "{\"nodes\":[";
        for (size_t i = 0; i < states_.size(); ++i) {
            if (i) oss << ",";
            oss << "{\"id\":" << i << ",\"state\":" << stateToJson(states_[i]) << "}";
        }
        oss << "],\"links\":[";
        for (size_t i = 0; i < transitions_.size(); ++i) {
            if (i) oss << ",";
            oss << "{\"source\":" << transitions_[i].first
                << ",\"target\":" << transitions_[i].second << "}";
        }
        oss << "]}";
        return oss.str();
    }
    // GRAPHVIZ_DOT (default for graphs) and PROMETHEUS fall through to DOT.
    oss << "digraph cognitive_states {\n";
    for (size_t i = 0; i < states_.size(); ++i) {
        oss << "  n" << i << " [label=\"" << escapeJson(states_[i].agentId) << ":"
            << escapeJson(states_[i].mood) << "\"];\n";
    }
    for (const auto& t : transitions_) {
        oss << "  n" << t.first << " -> n" << t.second << ";\n";
    }
    oss << "}\n";
    return oss.str();
}

std::string CognitiveStateVisualizer::exportMetricsUnlocked() const {
    // Assumes vizMutex_ is held by the caller.
    std::ostringstream oss;
    oss << "# HELP elizaos_cognitive_states_total Number of recorded states\n";
    oss << "# TYPE elizaos_cognitive_states_total counter\n";
    oss << "elizaos_cognitive_states_total " << states_.size() << "\n";
    double valenceSum = 0.0, arousalSum = 0.0;
    for (const auto& s : states_) {
        valenceSum += s.valence;
        arousalSum += s.arousal;
    }
    const double n = states_.empty() ? 1.0 : static_cast<double>(states_.size());
    oss << "# HELP elizaos_cognitive_valence_avg Mean emotional valence\n";
    oss << "# TYPE elizaos_cognitive_valence_avg gauge\n";
    oss << "elizaos_cognitive_valence_avg " << (valenceSum / n) << "\n";
    oss << "# HELP elizaos_cognitive_arousal_avg Mean arousal level\n";
    oss << "# TYPE elizaos_cognitive_arousal_avg gauge\n";
    oss << "elizaos_cognitive_arousal_avg " << (arousalSum / n) << "\n";
    oss << "# HELP elizaos_cognitive_transitions_total Recorded transitions\n";
    oss << "# TYPE elizaos_cognitive_transitions_total counter\n";
    oss << "elizaos_cognitive_transitions_total " << transitions_.size() << "\n";
    return oss.str();
}

std::string CognitiveStateVisualizer::exportMetrics(ExportFormat format) const {
    std::lock_guard<std::mutex> lock(vizMutex_);
    (void)format;  // Metrics are always Prometheus text exposition format.
    return exportMetricsUnlocked();
}

void CognitiveStateVisualizer::startStreaming(StreamCallback callback,
                                              std::chrono::milliseconds interval) {
    stopStreaming();
    {
        std::lock_guard<std::mutex> lock(vizMutex_);
        streamCallback_ = std::move(callback);
    }
    streaming_.store(true);
    streamThread_ = std::thread([this, interval]() {
        while (streaming_.load()) {
            StreamCallback cb;
            std::string payload;
            {
                std::lock_guard<std::mutex> lock(vizMutex_);
                cb = streamCallback_;
                if (!states_.empty()) {
                    payload = stateToJson(states_.back());
                }
            }
            if (cb && !payload.empty()) {
                cb(payload);
            }
            std::this_thread::sleep_for(interval);
        }
    });
}

void CognitiveStateVisualizer::stopStreaming() {
    streaming_.store(false);
    if (streamThread_.joinable()) {
        streamThread_.join();
    }
}

// ============================================================================
// EnhancedCognitiveBridge
// ============================================================================

EnhancedCognitiveBridge::EnhancedCognitiveBridge(const std::string& bridgeId)
    : CognitiveBridge(bridgeId),
      syncProtocol_(std::make_unique<CognitiveSyncProtocol>(
          bridgeId, CognitiveSyncProtocol::SyncConfig{})),
      visualizer_(std::make_unique<CognitiveStateVisualizer>()) {
    // Every published cognitive state feeds distributed sync + visualization,
    // making the enhanced bridge a strict superset of the base bridge.
    subscribeCognitiveState([this](const CognitiveState& state) {
        syncProtocol_->publishLocalState(state);
        visualizer_->addState(state);
        {
            std::lock_guard<std::mutex> lock(enhancedMutex_);
            ++syncEvents_;
        }
    });
}

EnhancedCognitiveBridge::~EnhancedCognitiveBridge() {
    stopReplay();
    if (visualizer_) {
        visualizer_->stopStreaming();
    }
}

CognitiveSyncProtocol& EnhancedCognitiveBridge::getSyncProtocol() {
    return *syncProtocol_;
}

void EnhancedCognitiveBridge::enableDistributedSync(
    CognitiveSyncProtocol::SyncMode mode) {
    CognitiveSyncProtocol::SyncConfig config;
    config.mode = mode;
    auto next = std::make_unique<CognitiveSyncProtocol>(id(), config);
    // Preserve registered agents across the mode switch.
    for (const auto& agentId : syncProtocol_->getRegisteredAgents()) {
        next->registerAgent(agentId);
    }
    std::lock_guard<std::mutex> lock(enhancedMutex_);
    syncProtocol_ = std::move(next);
}

void EnhancedCognitiveBridge::joinSyncCluster(const std::string& clusterUrl) {
    std::lock_guard<std::mutex> lock(enhancedMutex_);
    clusterUrl_ = clusterUrl;
    joinedCluster_ = true;
}

void EnhancedCognitiveBridge::leaveSyncCluster() {
    std::lock_guard<std::mutex> lock(enhancedMutex_);
    clusterUrl_.clear();
    joinedCluster_ = false;
}

void EnhancedCognitiveBridge::enableCompression(const std::string& algorithm) {
    std::lock_guard<std::mutex> lock(enhancedMutex_);
    compressionAlgorithm_ = algorithm;
    compressionEnabled_ = true;
}

CompressedCognitiveState EnhancedCognitiveBridge::getCompressedState() const {
    std::string algorithm;
    {
        std::lock_guard<std::mutex> lock(enhancedMutex_);
        algorithm = compressionEnabled_ ? compressionAlgorithm_ : "none";
    }
    const auto recent = recentCognitiveStates(1);
    CognitiveState latest = recent.empty() ? CognitiveState{} : recent.front();
    auto compressed = CompressedCognitiveState::compress(latest, algorithm);
    {
        std::lock_guard<std::mutex> lock(enhancedMutex_);
        compressedBytesTransferred_ += compressed.data.size();
    }
    return compressed;
}

std::string EnhancedCognitiveBridge::createSnapshot(const std::string& name) {
    CognitiveSnapshot snapshot;
    {
        std::lock_guard<std::mutex> lock(enhancedMutex_);
        std::ostringstream oss;
        oss << "snapshot-" << (snapshots_.size() + 1);
        snapshot.snapshotId = name.empty() ? oss.str() : name;
    }
    snapshot.agentId = id();
    snapshot.states = recentCognitiveStates(256);
    // recentCognitiveStates returns newest-first; store oldest-first so
    // replay proceeds chronologically.
    std::reverse(snapshot.states.begin(), snapshot.states.end());
    snapshot.startTime = snapshot.states.empty()
                             ? std::chrono::system_clock::now()
                             : snapshot.states.front().timestamp;
    snapshot.endTime = snapshot.states.empty()
                           ? snapshot.startTime
                           : snapshot.states.back().timestamp;
    std::lock_guard<std::mutex> lock(enhancedMutex_);
    snapshots_.push_back(snapshot);
    return snapshot.snapshotId;
}

void EnhancedCognitiveBridge::loadSnapshot(const std::string& snapshotId) {
    CognitiveSnapshot found;
    bool have = false;
    {
        std::lock_guard<std::mutex> lock(enhancedMutex_);
        for (const auto& s : snapshots_) {
            if (s.snapshotId == snapshotId) {
                found = s;
                have = true;
                break;
            }
        }
    }
    if (!have) return;
    // Loading republishes every state in order, restoring history and
    // notifying live subscribers.
    for (const auto& state : found.states) {
        publishCognitiveState(state);
    }
}

std::vector<CognitiveSnapshot> EnhancedCognitiveBridge::listSnapshots() const {
    std::lock_guard<std::mutex> lock(enhancedMutex_);
    return snapshots_;
}

void EnhancedCognitiveBridge::replaySnapshot(const std::string& snapshotId,
                                             double speedMultiplier) {
    stopReplay();
    CognitiveSnapshot found;
    bool have = false;
    {
        std::lock_guard<std::mutex> lock(enhancedMutex_);
        for (const auto& s : snapshots_) {
            if (s.snapshotId == snapshotId) {
                found = s;
                have = true;
                break;
            }
        }
    }
    if (!have || found.states.empty()) return;
    const double speed = speedMultiplier <= 0.0 ? 1.0 : speedMultiplier;
    replaying_.store(true);
    replayThread_ = std::thread([this, found, speed]() {
        for (size_t i = 0; i < found.states.size() && replaying_.load(); ++i) {
            publishCognitiveState(found.states[i]);
            if (i + 1 < found.states.size()) {
                auto gap = found.states[i + 1].timestamp - found.states[i].timestamp;
                auto scaled = std::chrono::duration_cast<std::chrono::milliseconds>(
                    gap / speed);
                // Bound the sleep so replays never hang tests.
                scaled = std::min(scaled, std::chrono::milliseconds(200));
                if (scaled.count() > 0) {
                    std::this_thread::sleep_for(scaled);
                }
            }
        }
        replaying_.store(false);
    });
}

void EnhancedCognitiveBridge::stopReplay() {
    replaying_.store(false);
    if (replayThread_.joinable()) {
        replayThread_.join();
    }
}

CognitiveStateVisualizer& EnhancedCognitiveBridge::getVisualizer() {
    return *visualizer_;
}

std::string EnhancedCognitiveBridge::exportForDashboard() const {
    std::ostringstream oss;
    const auto status = syncProtocol_->getSyncStatus();
    oss << "{\"bridgeId\":\"" << escapeJson(id()) << "\","
        << "\"synchronized\":" << (status.synchronized ? "true" : "false") << ","
        << "\"divergence\":" << status.divergenceScore << ","
        << "\"timeline\":"
        << visualizer_->exportTimeline(CognitiveStateVisualizer::ExportFormat::JSON)
        << "}";
    return oss.str();
}

void EnhancedCognitiveBridge::broadcastToCluster(const CognitiveState& state) {
    // In-process cluster semantics: publish to sync registry for every
    // registered agent, then emit locally so subscribers observe the event.
    for (const auto& agentId : syncProtocol_->getRegisteredAgents()) {
        CognitiveState copy = state;
        copy.metadata["broadcast_target"] = agentId;
        syncProtocol_->publishLocalState(copy);
    }
    publishCognitiveState(state);
}

void EnhancedCognitiveBridge::sendToAgent(const std::string& agentId,
                                          const CognitiveState& state) {
    CognitiveState copy = state;
    copy.agentId = agentId;
    syncProtocol_->publishLocalState(copy);
    std::lock_guard<std::mutex> lock(enhancedMutex_);
    ++syncEvents_;
}

EnhancedCognitiveBridge::EnhancedStats
EnhancedCognitiveBridge::getEnhancedStats() const {
    EnhancedStats out;
    out.base = stats();
    const auto status = syncProtocol_->getSyncStatus();
    std::lock_guard<std::mutex> lock(enhancedMutex_);
    out.syncEvents = syncEvents_;
    out.conflictsResolved = conflictsResolved_;
    out.averageSyncLatency =
        static_cast<double>(status.lastSyncLatency.count());
    out.compressedBytesTransferred = compressedBytesTransferred_;
    return out;
}

}  // namespace elizaos
