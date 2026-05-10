#pragma once

/**
 * ElizaOS C++ - Eliza Module
 *
 * Conversation engine with session management, emotional state tracking,
 * response pattern matching, and knowledge integration.
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

// Forward declarations
class KnowledgeBase;

// ============================================================================
// ConversationTurn
// ============================================================================

struct ConversationTurn {
    std::string id;
    std::string input;
    std::string response;
    std::string emotionalState = "neutral";
    float       confidence     = 0.0f;
    std::chrono::system_clock::time_point timestamp;
    std::unordered_map<std::string, std::string> metadata;

    ConversationTurn() = default;
    ConversationTurn(const std::string& input, const std::string& response);
};

// ============================================================================
// ConversationContext
// ============================================================================

class ConversationContext {
public:
    std::string sessionId;
    std::string userId;
    std::vector<ConversationTurn> history;
    std::chrono::system_clock::time_point startTime;
    std::chrono::system_clock::time_point lastActivity;

    ConversationContext() = default;
    ConversationContext(const std::string& sessionId, const std::string& userId);

    void addTurn(const ConversationTurn& turn);
    std::vector<ConversationTurn> getRecentHistory(int count) const;
    std::string getContextSummary() const;
    void updateLastActivity();
    void clearHistory() { history.clear(); }

    void        setSessionData(const std::string& key, const std::string& value);
    std::string getSessionData(const std::string& key) const;

    JsonValue toJson() const;
    static ConversationContext fromJson(const JsonValue& json);

private:
    std::unordered_map<std::string, std::string> sessionData_;
};

// ============================================================================
// ResponsePattern
// ============================================================================

struct ResponsePattern {
    std::string              id;
    std::string              pattern;
    std::vector<std::string> responses;
    std::string              category;
    int                      priority = 0;

    ResponsePattern() = default;
    ResponsePattern(const std::string& pattern,
                    const std::vector<std::string>& responses,
                    const std::string& category = "general");

    bool        matches(const std::string& input) const;
    std::string generateResponse(const std::unordered_map<std::string, std::string>& captures) const;
    std::vector<std::string> extractCaptures(const std::string& input) const;

    JsonValue toJson() const;
    static ResponsePattern fromJson(const JsonValue& json);
};

// ============================================================================
// EmotionalStateTracker
// ============================================================================

class EmotionalStateTracker {
public:
    void updateFromInput(const std::string& input);
    void updateFromInteraction(const std::string& outcome);
    void decay(float factor = 0.95f);
    void adjustEmotion(const std::string& emotion, float adjustment);
    void normalizeEmotions();

    std::string getDominantEmotion() const;
    float       getEmotionalIntensity() const;
    float       getEmotionLevel(const std::string& emotion) const;

    JsonValue toJson() const;
    static EmotionalStateTracker fromJson(const JsonValue& json);

private:
    std::vector<std::string> detectEmotionalWords(const std::string& input) const;

    std::unordered_map<std::string, float> emotionLevels_{
        {"neutral", 1.0f}, {"joy", 0.0f}, {"sadness", 0.0f},
        {"anger", 0.0f}, {"fear", 0.0f}, {"surprise", 0.0f}
    };
};

// ============================================================================
// ResponseGenerator
// ============================================================================

class ResponseGenerator {
public:
    ResponseGenerator();

    std::string generateResponse(const std::string& input,
                                  const ConversationContext& context);

    void addPattern(const ResponsePattern& pattern);
    void removePattern(const std::string& patternId);
    std::vector<ResponsePattern> getMatchingPatterns(const std::string& input) const;

    void setKnowledgeBase(std::shared_ptr<KnowledgeBase> kb);
    void clear();
    void loadDefaultPatterns();

private:
    std::string generateKnowledgeBasedResponse(const std::string& input) const;
    std::string generateCharacterResponse(const std::string& input,
                                           const ConversationContext& context) const;
    std::string processResponseTemplate(const std::string& tmpl,
                                         const std::unordered_map<std::string, std::string>& vars) const;
    std::string selectBestResponse(const std::vector<ResponsePattern>& patterns,
                                    const std::string& input) const;
    std::unordered_map<std::string, std::string> extractVariables(const std::string& input) const;

    std::vector<ResponsePattern>    patterns_;
    std::shared_ptr<KnowledgeBase>  knowledgeBase_;
};

// ============================================================================
// ElizaConfig
// ============================================================================

struct ElizaConfig {
    std::string agentName        = "Eliza";
    std::string personality      = "helpful";
    int         maxContextLength = 50;
    bool        emotionalTracking = true;
    bool        knowledgeIntegration = false;
    bool        characterPersonality = false;
};

// ============================================================================
// ElizaCore
// ============================================================================

class ElizaCore {
public:
    ElizaCore();

    std::string processInput(const std::string& input,
                              const std::string& sessionId = "",
                              const std::string& userId    = "");

    std::string processInputWithCharacter(const std::string& input,
                                           const std::string& sessionId,
                                           const std::string& characterId);

    std::string createSession(const std::string& userId      = "",
                               const std::string& characterId = "");
    bool endSession(const std::string& sessionId);
    std::optional<ConversationContext> getSession(const std::string& sessionId);
    void clearAllSessions();
    size_t getSessionCount() const;

    void updateResponsePatterns(const std::string& input, const std::string& feedback);
    void setResponseGenerator(std::shared_ptr<ResponseGenerator> generator);

    void enableEmotionalTracking(bool enable);
    void enableKnowledgeIntegration(bool enable);
    void enableCharacterPersonality(bool enable);

    std::string getConversationAnalytics() const;
    std::vector<std::string> getFrequentTopics() const;
    std::unordered_map<std::string, int> getEmotionalStateStats() const;

    bool exportConversations(const std::string& filename) const;
    bool importConversations(const std::string& filename);

private:
    std::string generateSessionId();
    void saveSessionToMemory(const ConversationContext& session);
    std::optional<ConversationContext> loadSessionFromMemory(const std::string& sessionId);
    std::string preprocessInput(const std::string& input) const;
    std::string postprocessResponse(const std::string& response,
                                     const ConversationContext& context) const;
    void updateEmotionalState(const std::string& sessionId, const std::string& input);
    void trackConversationMetrics(const ConversationContext& context);

    std::unordered_map<std::string, ConversationContext> sessions_;
    std::shared_ptr<ResponseGenerator>   responseGenerator_;
    std::shared_ptr<EmotionalStateTracker> emotionalTracker_;
    std::shared_ptr<KnowledgeBase>       knowledgeBase_;

    bool emotionalTrackingEnabled_    = true;
    bool knowledgeIntegrationEnabled_ = false;
    bool characterPersonalityEnabled_ = false;

    mutable std::mutex elizaMutex_;
};

// ============================================================================
// Global instance and utility functions
// ============================================================================

extern std::shared_ptr<ElizaCore> globalElizaCore;

// Standalone utility functions
std::string normalizeInput(const std::string& input);
std::vector<std::string> tokenizeInput(const std::string& input);
std::string extractSentiment(const std::string& input);
bool isQuestion(const std::string& input);
bool isGreeting(const std::string& input);
bool isGoodbye(const std::string& input);

std::vector<ResponsePattern> getGreetingPatterns();
std::vector<ResponsePattern> getQuestionPatterns();
std::vector<ResponsePattern> getEmotionalPatterns();
std::vector<ResponsePattern> getReflectivePatterns();
std::vector<ResponsePattern> getDefaultPatterns();
std::vector<ResponsePattern> getAllPatterns();

} // namespace elizaos
