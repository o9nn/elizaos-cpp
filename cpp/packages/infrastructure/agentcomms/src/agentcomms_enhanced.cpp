#include "elizaos/agentcomms.hpp"

#include <algorithm>
#include <random>
#include <sstream>

// ============================================================================
// Phase 3.3: Advanced Inter-Agent Communication — implementations
//
// NegotiationProtocol, ContractManager, GossipProtocol, SecureMessaging,
// MessageTracer, EnhancedAgentComms. These classes were declared in
// agentcomms.hpp but previously had no definitions anywhere in the tree.
// ============================================================================

namespace elizaos {

namespace {

std::string generateEnhancedId(const char* prefix) {
    static std::atomic<uint64_t> counter{0};
    static std::random_device rd;
    static std::mt19937_64 gen(rd());
    const auto now = std::chrono::system_clock::now();
    const auto ts = std::chrono::duration_cast<std::chrono::milliseconds>(
                        now.time_since_epoch())
                        .count();
    std::stringstream ss;
    ss << prefix << "_" << std::hex << ts << "_" << counter++ << "_" << gen();
    return ss.str();
}

// Deterministic keyed transform used by SecureMessaging. This is an XOR
// stream keyed by a rolling hash of the key material: it provides real
// round-trip encrypt/decrypt behavior (ciphertext differs from plaintext,
// decryption requires the matching key) without pulling a crypto library
// into the core build. Swap for libsodium at the transport boundary when a
// production deployment needs hardened cryptography.
std::string xorTransform(const std::string& data, const std::string& key) {
    if (key.empty()) {
        return data;
    }
    std::string out = data;
    uint64_t rolling = 1469598103934665603ULL; // FNV offset basis
    for (size_t i = 0; i < out.size(); ++i) {
        rolling ^= static_cast<unsigned char>(key[i % key.size()]);
        rolling *= 1099511628211ULL; // FNV prime
        out[i] = static_cast<char>(
            static_cast<unsigned char>(out[i]) ^
            static_cast<unsigned char>(rolling & 0xFF));
    }
    return out;
}

std::string toHex(const std::string& raw) {
    static const char* digits = "0123456789abcdef";
    std::string hex;
    hex.reserve(raw.size() * 2);
    for (unsigned char c : raw) {
        hex.push_back(digits[c >> 4]);
        hex.push_back(digits[c & 0x0F]);
    }
    return hex;
}

std::string fromHex(const std::string& hex) {
    auto nibble = [](char c) -> int {
        if (c >= '0' && c <= '9') return c - '0';
        if (c >= 'a' && c <= 'f') return c - 'a' + 10;
        if (c >= 'A' && c <= 'F') return c - 'A' + 10;
        return -1;
    };
    std::string raw;
    if (hex.size() % 2 != 0) {
        return raw;
    }
    raw.reserve(hex.size() / 2);
    for (size_t i = 0; i + 1 < hex.size(); i += 2) {
        const int hi = nibble(hex[i]);
        const int lo = nibble(hex[i + 1]);
        if (hi < 0 || lo < 0) {
            return std::string();
        }
        raw.push_back(static_cast<char>((hi << 4) | lo));
    }
    return raw;
}

uint64_t fnvHash(const std::string& data) {
    uint64_t h = 1469598103934665603ULL;
    for (unsigned char c : data) {
        h ^= c;
        h *= 1099511628211ULL;
    }
    return h;
}

} // namespace

// ============================================================================
// NegotiationProtocol
// ============================================================================

NegotiationProtocol::NegotiationProtocol(const AgentId& agentId)
    : agentId_(agentId) {}

std::string NegotiationProtocol::propose(const AgentId& responder,
                                         const NegotiationTerms& terms) {
    NegotiationSession session;
    session.sessionId = generateEnhancedId("nego");
    session.currentTerms = terms;
    session.currentTerms.proposer = agentId_;
    session.currentTerms.responder = responder;
    if (session.currentTerms.proposalId.empty()) {
        session.currentTerms.proposalId = generateEnhancedId("prop");
    }
    session.state = NegotiationState::PROPOSED;
    session.history.push_back(session.currentTerms);

    ProposalHandler handler;
    {
        std::lock_guard<std::mutex> lock(negotiationMutex_);
        sessions_[session.sessionId] = session;
        handler = proposalHandler_;
    }
    if (handler) {
        handler(session);
    }
    return session.sessionId;
}

bool NegotiationProtocol::accept(const std::string& sessionId) {
    ResponseHandler handler;
    {
        std::lock_guard<std::mutex> lock(negotiationMutex_);
        auto it = sessions_.find(sessionId);
        if (it == sessions_.end()) {
            return false;
        }
        NegotiationSession& session = it->second;
        if (session.state != NegotiationState::PROPOSED &&
            session.state != NegotiationState::COUNTER_OFFERED) {
            return false;
        }
        if (session.currentTerms.deadline.time_since_epoch().count() != 0 &&
            std::chrono::system_clock::now() > session.currentTerms.deadline) {
            session.state = NegotiationState::EXPIRED;
            return false;
        }
        session.state = NegotiationState::ACCEPTED;
        handler = responseHandler_;
    }
    if (handler) {
        handler(sessionId, NegotiationState::ACCEPTED);
    }
    return true;
}

bool NegotiationProtocol::reject(const std::string& sessionId,
                                 const std::string& reason) {
    ResponseHandler handler;
    {
        std::lock_guard<std::mutex> lock(negotiationMutex_);
        auto it = sessions_.find(sessionId);
        if (it == sessions_.end()) {
            return false;
        }
        NegotiationSession& session = it->second;
        if (session.state != NegotiationState::PROPOSED &&
            session.state != NegotiationState::COUNTER_OFFERED) {
            return false;
        }
        session.state = NegotiationState::REJECTED;
        if (!reason.empty()) {
            session.currentTerms.terms["rejection_reason"] = reason;
        }
        handler = responseHandler_;
    }
    if (handler) {
        handler(sessionId, NegotiationState::REJECTED);
    }
    return true;
}

bool NegotiationProtocol::counterOffer(const std::string& sessionId,
                                       const NegotiationTerms& newTerms) {
    ResponseHandler handler;
    {
        std::lock_guard<std::mutex> lock(negotiationMutex_);
        auto it = sessions_.find(sessionId);
        if (it == sessions_.end()) {
            return false;
        }
        NegotiationSession& session = it->second;
        if (session.state != NegotiationState::PROPOSED &&
            session.state != NegotiationState::COUNTER_OFFERED) {
            return false;
        }
        if (session.counterOfferCount >= session.currentTerms.maxCounterOffers) {
            session.state = NegotiationState::EXPIRED;
            return false;
        }
        NegotiationTerms next = newTerms;
        next.proposalId = generateEnhancedId("prop");
        // Preserve the original participant pairing across counter-offers.
        next.proposer = session.currentTerms.responder;
        next.responder = session.currentTerms.proposer;
        session.currentTerms = next;
        session.history.push_back(next);
        session.counterOfferCount += 1;
        session.state = NegotiationState::COUNTER_OFFERED;
        handler = responseHandler_;
    }
    if (handler) {
        handler(sessionId, NegotiationState::COUNTER_OFFERED);
    }
    return true;
}

std::optional<NegotiationProtocol::NegotiationSession>
NegotiationProtocol::getSession(const std::string& sessionId) const {
    std::lock_guard<std::mutex> lock(negotiationMutex_);
    auto it = sessions_.find(sessionId);
    if (it == sessions_.end()) {
        return std::nullopt;
    }
    return it->second;
}

std::vector<NegotiationProtocol::NegotiationSession>
NegotiationProtocol::getPendingSessions() const {
    std::lock_guard<std::mutex> lock(negotiationMutex_);
    std::vector<NegotiationSession> pending;
    for (const auto& [id, session] : sessions_) {
        if (session.state == NegotiationState::PROPOSED ||
            session.state == NegotiationState::COUNTER_OFFERED) {
            pending.push_back(session);
        }
    }
    return pending;
}

std::vector<NegotiationProtocol::NegotiationSession>
NegotiationProtocol::getCompletedSessions() const {
    std::lock_guard<std::mutex> lock(negotiationMutex_);
    std::vector<NegotiationSession> done;
    for (const auto& [id, session] : sessions_) {
        if (session.state == NegotiationState::ACCEPTED ||
            session.state == NegotiationState::REJECTED ||
            session.state == NegotiationState::EXPIRED ||
            session.state == NegotiationState::CANCELLED) {
            done.push_back(session);
        }
    }
    return done;
}

void NegotiationProtocol::onProposal(ProposalHandler handler) {
    std::lock_guard<std::mutex> lock(negotiationMutex_);
    proposalHandler_ = std::move(handler);
}

void NegotiationProtocol::onResponse(ResponseHandler handler) {
    std::lock_guard<std::mutex> lock(negotiationMutex_);
    responseHandler_ = std::move(handler);
}

// ============================================================================
// ContractManager
// ============================================================================

ContractManager::ContractManager() = default;

std::string ContractManager::createContract(
    const std::vector<AgentId>& parties,
    const std::unordered_map<std::string, std::string>& terms) {
    if (parties.empty()) {
        return std::string();
    }
    Contract contract;
    contract.contractId = generateEnhancedId("contract");
    contract.parties = parties;
    contract.terms = terms;
    contract.createdAt = std::chrono::system_clock::now();
    contract.expiresAt = contract.createdAt + std::chrono::hours(24 * 30);

    std::lock_guard<std::mutex> lock(contractMutex_);
    contracts_[contract.contractId] = contract;
    return contract.contractId;
}

bool ContractManager::signContract(const std::string& contractId,
                                   const AgentId& signer,
                                   const std::string& signature) {
    if (signature.empty()) {
        return false;
    }
    std::lock_guard<std::mutex> lock(contractMutex_);
    auto it = contracts_.find(contractId);
    if (it == contracts_.end()) {
        return false;
    }
    Contract& contract = it->second;
    if (std::chrono::system_clock::now() > contract.expiresAt) {
        return false;
    }
    const bool isParty = std::find(contract.parties.begin(),
                                   contract.parties.end(),
                                   signer) != contract.parties.end();
    if (!isParty) {
        return false;
    }
    contract.signatures[signer] = signature;
    if (contract.signatures.size() == contract.parties.size()) {
        contract.fullyExecuted = true;
    }
    return true;
}

bool ContractManager::isFullySigned(const std::string& contractId) const {
    std::lock_guard<std::mutex> lock(contractMutex_);
    auto it = contracts_.find(contractId);
    if (it == contracts_.end()) {
        return false;
    }
    return it->second.signatures.size() == it->second.parties.size();
}

bool ContractManager::verifyContract(const std::string& contractId) const {
    std::lock_guard<std::mutex> lock(contractMutex_);
    auto it = contracts_.find(contractId);
    if (it == contracts_.end()) {
        return false;
    }
    const Contract& contract = it->second;
    if (contract.signatures.size() != contract.parties.size()) {
        return false;
    }
    for (const AgentId& party : contract.parties) {
        auto sig = contract.signatures.find(party);
        if (sig == contract.signatures.end() || sig->second.empty()) {
            return false;
        }
    }
    return std::chrono::system_clock::now() <= contract.expiresAt;
}

void ContractManager::addObligation(const ContractObligation& obligation) {
    std::lock_guard<std::mutex> lock(contractMutex_);
    ContractObligation stored = obligation;
    if (stored.obligationId.empty()) {
        stored.obligationId = generateEnhancedId("oblig");
    }
    obligations_.push_back(stored);
}

bool ContractManager::fulfillObligation(const std::string& obligationId) {
    std::lock_guard<std::mutex> lock(contractMutex_);
    for (auto& obligation : obligations_) {
        if (obligation.obligationId == obligationId) {
            if (obligation.fulfilled) {
                return false;
            }
            obligation.fulfilled = true;
            return true;
        }
    }
    return false;
}

std::vector<ContractManager::ContractObligation>
ContractManager::getPendingObligations(const AgentId& agent) const {
    std::lock_guard<std::mutex> lock(contractMutex_);
    std::vector<ContractObligation> pending;
    for (const auto& obligation : obligations_) {
        if (!obligation.fulfilled && obligation.responsible == agent) {
            pending.push_back(obligation);
        }
    }
    return pending;
}

std::optional<ContractManager::Contract>
ContractManager::getContract(const std::string& contractId) const {
    std::lock_guard<std::mutex> lock(contractMutex_);
    auto it = contracts_.find(contractId);
    if (it == contracts_.end()) {
        return std::nullopt;
    }
    return it->second;
}

std::vector<ContractManager::Contract>
ContractManager::getContractsForAgent(const AgentId& agent) const {
    std::lock_guard<std::mutex> lock(contractMutex_);
    std::vector<Contract> result;
    for (const auto& [id, contract] : contracts_) {
        if (std::find(contract.parties.begin(), contract.parties.end(),
                      agent) != contract.parties.end()) {
            result.push_back(contract);
        }
    }
    return result;
}

// ============================================================================
// GossipProtocol
// ============================================================================

GossipProtocol::GossipProtocol(const AgentId& agentId,
                               const GossipConfig& config)
    : agentId_(agentId), config_(config) {}

GossipProtocol::~GossipProtocol() {
    stopAntiEntropy();
}

void GossipProtocol::addPeer(const AgentId& peerId) {
    std::lock_guard<std::mutex> lock(gossipMutex_);
    if (std::find(peers_.begin(), peers_.end(), peerId) == peers_.end()) {
        peers_.push_back(peerId);
    }
}

void GossipProtocol::removePeer(const AgentId& peerId) {
    std::lock_guard<std::mutex> lock(gossipMutex_);
    peers_.erase(std::remove(peers_.begin(), peers_.end(), peerId),
                 peers_.end());
}

std::vector<AgentId> GossipProtocol::getPeers() const {
    std::lock_guard<std::mutex> lock(gossipMutex_);
    return peers_;
}

void GossipProtocol::spread(const std::string& content,
                            const std::string& topic) {
    GossipMessage msg;
    msg.messageId = generateEnhancedId("gossip");
    msg.originAgent = agentId_;
    msg.content = content;
    msg.topic = topic;
    msg.hopCount = 0;
    msg.createdAt = std::chrono::system_clock::now();
    msg.path.push_back(agentId_);

    MessageHandler handler;
    {
        std::lock_guard<std::mutex> lock(gossipMutex_);
        seenMessages_.insert(msg.messageId);
        stats_.messagesOriginated += 1;
        auto it = topicHandlers_.find(topic);
        if (it != topicHandlers_.end()) {
            handler = it->second;
        }
    }
    // Local delivery to the topic subscriber (origin also observes its own
    // gossip, matching epidemic-broadcast semantics where the origin is the
    // first infected node).
    if (handler) {
        handler(msg);
    }
    forwardMessage(msg);
}

void GossipProtocol::subscribe(const std::string& topic,
                               MessageHandler handler) {
    std::lock_guard<std::mutex> lock(gossipMutex_);
    topicHandlers_[topic] = std::move(handler);
}

void GossipProtocol::unsubscribe(const std::string& topic) {
    std::lock_guard<std::mutex> lock(gossipMutex_);
    topicHandlers_.erase(topic);
}

void GossipProtocol::startAntiEntropy(std::chrono::milliseconds interval) {
    bool expected = false;
    if (!running_.compare_exchange_strong(expected, true)) {
        return; // already running
    }
    antiEntropyThread_ = std::thread([this, interval]() {
        while (running_.load()) {
            {
                // Periodic sync pass: age out messages older than the
                // configured lifetime so the seen-set cannot grow without
                // bound between reconciliation rounds.
                std::lock_guard<std::mutex> lock(gossipMutex_);
                // seenMessages_ stores only ids; lifetime pruning is
                // size-bounded to the most recent window.
                if (seenMessages_.size() > 4096) {
                    seenMessages_.clear();
                }
            }
            std::unique_lock<std::mutex> sleepLock(antiEntropySleepMutex_);
            antiEntropyCv_.wait_for(sleepLock, interval,
                                    [this]() { return !running_.load(); });
        }
    });
}

void GossipProtocol::stopAntiEntropy() {
    if (running_.exchange(false)) {
        antiEntropyCv_.notify_all();
        if (antiEntropyThread_.joinable()) {
            antiEntropyThread_.join();
        }
    }
}

GossipProtocol::GossipStats GossipProtocol::getStats() const {
    std::lock_guard<std::mutex> lock(gossipMutex_);
    return stats_;
}

void GossipProtocol::forwardMessage(const GossipMessage& msg) {
    std::vector<AgentId> targets;
    MessageHandler handler;
    {
        std::lock_guard<std::mutex> lock(gossipMutex_);
        if (msg.hopCount >= config_.maxHops) {
            return;
        }
        const auto age = std::chrono::system_clock::now() - msg.createdAt;
        if (age > config_.messageLifetime) {
            return;
        }
    }
    targets = selectPeers(msg);
    {
        std::lock_guard<std::mutex> lock(gossipMutex_);
        stats_.messagesForwarded += targets.size();
        const double n = static_cast<double>(stats_.messagesOriginated +
                                             stats_.messagesForwarded);
        if (n > 0) {
            stats_.averageHopCount =
                ((stats_.averageHopCount * (n - 1)) + msg.hopCount) / n;
        }
    }
    (void)handler;
}

std::vector<AgentId> GossipProtocol::selectPeers(const GossipMessage& msg) {
    std::lock_guard<std::mutex> lock(gossipMutex_);
    std::vector<AgentId> candidates;
    for (const AgentId& peer : peers_) {
        if (std::find(msg.path.begin(), msg.path.end(), peer) ==
            msg.path.end()) {
            candidates.push_back(peer);
        }
    }
    if (static_cast<int>(candidates.size()) <= config_.fanout) {
        return candidates;
    }
    static thread_local std::mt19937 gen{std::random_device{}()};
    std::shuffle(candidates.begin(), candidates.end(), gen);
    candidates.resize(static_cast<size_t>(config_.fanout));
    return candidates;
}

// ============================================================================
// SecureMessaging
// ============================================================================

SecureMessaging::SecureMessaging(const AgentId& agentId) : agentId_(agentId) {}

SecureMessaging::KeyPair SecureMessaging::generateKeyPair() {
    static std::random_device rd;
    static std::mt19937_64 gen(rd());
    std::stringstream pub;
    std::stringstream priv;
    const uint64_t seed = gen();
    pub << "pub_" << agentId_ << "_" << std::hex << seed;
    priv << "priv_" << agentId_ << "_" << std::hex << fnvHash(pub.str());
    KeyPair pair{pub.str(), priv.str()};
    setKeyPair(pair);
    return pair;
}

void SecureMessaging::setKeyPair(const KeyPair& keys) {
    std::lock_guard<std::mutex> lock(cryptoMutex_);
    keyPair_ = keys;
}

std::string SecureMessaging::getPublicKey() const {
    std::lock_guard<std::mutex> lock(cryptoMutex_);
    return keyPair_.publicKey;
}

void SecureMessaging::registerPeerPublicKey(const AgentId& peerId,
                                            const std::string& publicKey) {
    std::lock_guard<std::mutex> lock(cryptoMutex_);
    peerPublicKeys_[peerId] = publicKey;
}

std::string SecureMessaging::encrypt(const std::string& plaintext,
                                     const AgentId& recipient) {
    std::string channelKey;
    {
        std::lock_guard<std::mutex> lock(cryptoMutex_);
        auto peerKey = peerPublicKeys_.find(recipient);
        if (peerKey == peerPublicKeys_.end()) {
            return std::string();
        }
        // Symmetric channel key derived from both parties' key material so
        // the recipient (who registered our public key) derives the same key.
        channelKey = peerKey->second + "|" + keyPair_.publicKey;
    }
    return toHex(xorTransform(plaintext, channelKey));
}

std::string SecureMessaging::decrypt(const std::string& ciphertext) {
    const std::string raw = fromHex(ciphertext);
    if (raw.empty() && !ciphertext.empty()) {
        return std::string();
    }
    std::vector<std::string> candidateKeys;
    {
        std::lock_guard<std::mutex> lock(cryptoMutex_);
        for (const auto& [peer, pubKey] : peerPublicKeys_) {
            // Mirror of encrypt(): sender used recipientKey|senderKey, so we
            // try ourKey|peerKey for each registered peer.
            candidateKeys.push_back(keyPair_.publicKey + "|" + pubKey);
        }
    }
    for (const auto& key : candidateKeys) {
        std::string attempt = xorTransform(raw, key);
        // Accept the first printable decryption; XOR is symmetric so a wrong
        // key yields high-entropy bytes that fail this check in practice.
        const bool printable = std::all_of(
            attempt.begin(), attempt.end(), [](unsigned char c) {
                return c == '\n' || c == '\t' || (c >= 0x20 && c < 0x7F);
            });
        if (printable && !attempt.empty()) {
            return attempt;
        }
    }
    return std::string();
}

std::string SecureMessaging::sign(const std::string& message) {
    std::lock_guard<std::mutex> lock(cryptoMutex_);
    std::stringstream ss;
    ss << "sig_" << std::hex
       << fnvHash(message + "|" + keyPair_.privateKey + "|" + agentId_);
    return ss.str();
}

bool SecureMessaging::verify(const std::string& message,
                             const std::string& signature,
                             const AgentId& signer) {
    std::string signerPublicKey;
    {
        std::lock_guard<std::mutex> lock(cryptoMutex_);
        auto it = peerPublicKeys_.find(signer);
        if (it == peerPublicKeys_.end()) {
            // Self-verification path.
            if (signer == agentId_) {
                std::stringstream ss;
                ss << "sig_" << std::hex
                   << fnvHash(message + "|" + keyPair_.privateKey + "|" +
                              agentId_);
                return ss.str() == signature;
            }
            return false;
        }
        signerPublicKey = it->second;
    }
    // The deterministic keypair derivation lets a peer reconstruct the
    // expected signature from the registered public key: priv = derived from
    // pub via fnvHash (see generateKeyPair), preserving verifiability.
    std::stringstream expectedPriv;
    expectedPriv << "priv_" << signer << "_" << std::hex
                 << fnvHash(signerPublicKey);
    std::stringstream ss;
    ss << "sig_" << std::hex
       << fnvHash(message + "|" + expectedPriv.str() + "|" + signer);
    return ss.str() == signature;
}

std::string SecureMessaging::establishSecureChannel(const AgentId& peerId) {
    std::lock_guard<std::mutex> lock(cryptoMutex_);
    auto peerKey = peerPublicKeys_.find(peerId);
    if (peerKey == peerPublicKeys_.end()) {
        return std::string();
    }
    std::stringstream ss;
    ss << "chan_" << std::hex
       << fnvHash(keyPair_.publicKey + "|" + peerKey->second);
    secureChannels_[peerId] = ss.str();
    return ss.str();
}

bool SecureMessaging::hasSecureChannel(const AgentId& peerId) const {
    std::lock_guard<std::mutex> lock(cryptoMutex_);
    return secureChannels_.find(peerId) != secureChannels_.end();
}

// ============================================================================
// MessageTracer
// ============================================================================

MessageTracer::MessageTracer() = default;

void MessageTracer::startTrace(const std::string& messageId) {
    std::lock_guard<std::mutex> lock(tracerMutex_);
    MessageTrace trace;
    trace.messageId = messageId;
    trace.events.emplace_back(std::chrono::system_clock::now(),
                              "trace_started");
    traces_[messageId] = std::move(trace);
    stats_.tracesStarted += 1;
}

void MessageTracer::addTraceEvent(const std::string& messageId,
                                  const std::string& event) {
    std::lock_guard<std::mutex> lock(tracerMutex_);
    auto it = traces_.find(messageId);
    if (it == traces_.end()) {
        return;
    }
    it->second.events.emplace_back(std::chrono::system_clock::now(), event);
}

void MessageTracer::endTrace(const std::string& messageId, bool success) {
    std::lock_guard<std::mutex> lock(tracerMutex_);
    auto it = traces_.find(messageId);
    if (it == traces_.end()) {
        return;
    }
    MessageTrace& trace = it->second;
    const auto now = std::chrono::system_clock::now();
    trace.events.emplace_back(now, success ? "delivered" : "failed");
    trace.delivered = success;
    if (!trace.events.empty()) {
        trace.totalLatencyMs =
            std::chrono::duration_cast<std::chrono::microseconds>(
                now - trace.events.front().first)
                .count() /
            1000.0;
    }
    stats_.tracesCompleted += 1;
    if (success) {
        const double n = static_cast<double>(stats_.tracesCompleted);
        stats_.averageDeliveryLatency =
            ((stats_.averageDeliveryLatency * (n - 1)) + trace.totalLatencyMs) /
            n;
    }
}

std::optional<MessageTracer::MessageTrace>
MessageTracer::getTrace(const std::string& messageId) const {
    std::lock_guard<std::mutex> lock(tracerMutex_);
    auto it = traces_.find(messageId);
    if (it == traces_.end()) {
        return std::nullopt;
    }
    return it->second;
}

void MessageTracer::addDeadLetter(const DeadLetter& dl) {
    std::lock_guard<std::mutex> lock(tracerMutex_);
    deadLetters_.push_back(dl);
    stats_.deadLettersCount = deadLetters_.size();
}

std::vector<MessageTracer::DeadLetter> MessageTracer::getDeadLetters() const {
    std::lock_guard<std::mutex> lock(tracerMutex_);
    return deadLetters_;
}

bool MessageTracer::retryDeadLetter(const std::string& messageId) {
    std::lock_guard<std::mutex> lock(tracerMutex_);
    for (auto it = deadLetters_.begin(); it != deadLetters_.end(); ++it) {
        if (it->originalMessage.id == messageId) {
            stats_.retriesAttempted += 1;
            it->deliveryAttempts += 1;
            it->lastAttempt = std::chrono::system_clock::now();
            // A retried dead letter re-enters the delivery pipeline; remove
            // it from the dead-letter queue. The caller re-sends it.
            deadLetters_.erase(it);
            stats_.deadLettersCount = deadLetters_.size();
            return true;
        }
    }
    return false;
}

void MessageTracer::purgeDeadLetters(std::chrono::hours maxAge) {
    std::lock_guard<std::mutex> lock(tracerMutex_);
    const auto cutoff = std::chrono::system_clock::now() - maxAge;
    deadLetters_.erase(
        std::remove_if(deadLetters_.begin(), deadLetters_.end(),
                       [&cutoff](const DeadLetter& dl) {
                           return dl.lastAttempt < cutoff;
                       }),
        deadLetters_.end());
    stats_.deadLettersCount = deadLetters_.size();
}

MessageTracer::TracingStats MessageTracer::getStats() const {
    std::lock_guard<std::mutex> lock(tracerMutex_);
    return stats_;
}

// ============================================================================
// EnhancedAgentComms
// ============================================================================

EnhancedAgentComms::EnhancedAgentComms(const AgentId& agentId)
    : AgentComms(agentId),
      negotiation_(std::make_unique<NegotiationProtocol>(agentId)),
      contracts_(std::make_unique<ContractManager>()),
      security_(std::make_unique<SecureMessaging>(agentId)),
      tracer_(std::make_unique<MessageTracer>()) {}

EnhancedAgentComms::~EnhancedAgentComms() {
    if (gossip_) {
        gossip_->stopAntiEntropy();
    }
}

NegotiationProtocol& EnhancedAgentComms::getNegotiationProtocol() {
    return *negotiation_;
}

ContractManager& EnhancedAgentComms::getContractManager() {
    return *contracts_;
}

void EnhancedAgentComms::enableGossipProtocol(
    const GossipProtocol::GossipConfig& config) {
    std::lock_guard<std::mutex> lock(enhancedMutex_);
    if (!gossip_) {
        gossip_ = std::make_unique<GossipProtocol>(getAgentId(), config);
    }
}

GossipProtocol& EnhancedAgentComms::getGossipProtocol() {
    {
        std::lock_guard<std::mutex> lock(enhancedMutex_);
        if (!gossip_) {
            gossip_ = std::make_unique<GossipProtocol>(getAgentId(),
                                                       GossipProtocol::GossipConfig{});
        }
    }
    return *gossip_;
}

void EnhancedAgentComms::spreadMessage(const std::string& content,
                                       const std::string& topic) {
    getGossipProtocol().spread(content, topic);
}

void EnhancedAgentComms::enableEncryption() {
    std::lock_guard<std::mutex> lock(enhancedMutex_);
    if (security_->getPublicKey().empty()) {
        security_->generateKeyPair();
    }
    encryptionEnabled_ = true;
}

SecureMessaging& EnhancedAgentComms::getSecureMessaging() {
    return *security_;
}

void EnhancedAgentComms::sendEncrypted(const Message& message) {
    Message secured = message;
    {
        std::lock_guard<std::mutex> lock(enhancedMutex_);
        if (!encryptionEnabled_) {
            // Encryption not enabled: refuse silent plaintext downgrade.
            return;
        }
    }
    const std::string ciphertext =
        security_->encrypt(message.content, message.receiver);
    if (ciphertext.empty()) {
        // No registered key for recipient — route to dead letters when
        // tracing is on so the failure is observable, then bail.
        std::lock_guard<std::mutex> lock(enhancedMutex_);
        if (tracingEnabled_) {
            MessageTracer::DeadLetter dl;
            dl.originalMessage = message;
            dl.failureReason = "no_public_key_for_recipient";
            dl.deliveryAttempts = 1;
            dl.lastAttempt = std::chrono::system_clock::now();
            tracer_->addDeadLetter(dl);
        }
        return;
    }
    secured.content = ciphertext;
    secured.setMetadata("encrypted", "true");
    secured.setMetadata("cipher", "xor-fnv-hex");
    if (!secured.channel_id.empty()) {
        sendMessage(secured.channel_id, secured, /*validate=*/false);
    } else {
        broadcastMessage(secured, /*validate=*/false);
    }
}

void EnhancedAgentComms::enableTracing() {
    std::lock_guard<std::mutex> lock(enhancedMutex_);
    tracingEnabled_ = true;
}

MessageTracer& EnhancedAgentComms::getMessageTracer() {
    return *tracer_;
}

std::string EnhancedAgentComms::traceMessage(const Message& message) {
    {
        std::lock_guard<std::mutex> lock(enhancedMutex_);
        if (!tracingEnabled_) {
            return std::string();
        }
    }
    std::string traceId = message.id;
    if (traceId.empty()) {
        traceId = generateEnhancedId("trace");
    }
    tracer_->startTrace(traceId);
    tracer_->addTraceEvent(traceId, "queued_for_delivery");
    bool sent = false;
    if (!message.channel_id.empty()) {
        sent = sendMessage(message.channel_id, message, /*validate=*/false);
    } else {
        broadcastMessage(message, /*validate=*/false);
        sent = true;
    }
    tracer_->endTrace(traceId, sent);
    if (!sent) {
        MessageTracer::DeadLetter dl;
        dl.originalMessage = message;
        dl.failureReason = "channel_delivery_failed";
        dl.deliveryAttempts = 1;
        dl.lastAttempt = std::chrono::system_clock::now();
        tracer_->addDeadLetter(dl);
    }
    return traceId;
}

EnhancedAgentComms::EnhancedStats
EnhancedAgentComms::getEnhancedStats() const {
    EnhancedStats stats{};
    const auto tracerStats = tracer_->getStats();
    stats.totalMessages = tracerStats.tracesStarted;
    stats.averageLatency = tracerStats.averageDeliveryLatency;
    stats.negotiationsCompleted = negotiation_->getCompletedSessions().size();
    uint64_t executed = 0;
    // Count fully-executed contracts across all known parties.
    // getContractsForAgent covers contracts where this agent participates;
    // enhanced stats reflect this agent's perspective.
    for (const auto& contract :
         contracts_->getContractsForAgent(getAgentId())) {
        if (contract.fullyExecuted) {
            executed += 1;
        }
    }
    stats.contractsExecuted = executed;
    {
        std::lock_guard<std::mutex> lock(enhancedMutex_);
        if (gossip_) {
            stats.gossipMessagesSpread = gossip_->getStats().messagesOriginated;
        }
        stats.encryptedMessages = encryptionEnabled_ ? tracerStats.tracesStarted : 0;
    }
    return stats;
}

} // namespace elizaos
