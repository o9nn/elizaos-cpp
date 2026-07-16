#include "elizaos/eliza.hpp"
#include <algorithm>
#include <sstream>
#include <fstream>
#include <random>
#include <cctype>
#include <iomanip>

namespace elizaos {

// Global Eliza instance
std::shared_ptr<ElizaCore> globalElizaCore = std::make_shared<ElizaCore>();

// Simple UUID generator for sessions
std::string generateElizaUUID() {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_int_distribution<> dis(0, 15);
    
    std::string uuid = "eliza-xxxx-xxxx-xxxx-xxxxxxxxxxxx";
    for (auto& c : uuid) {
        if (c == 'x') {
            c = "0123456789abcdef"[dis(gen)];
        }
    }
    return uuid;
}

// Helper std::function to convert to lowercase
std::string toLowercase(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(),
        [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
    std::transform(result.begin(), result.end(), result.begin(), [](unsigned char c){ return static_cast<char>(std::tolower(c)); });
    return result;
}

// Helper std::function to trim whitespace
std::string trim(const std::string& str) {
    size_t start = str.find_first_not_of(" \t\n\r");
    if (start == std::string::npos) return "";
    size_t end = str.find_last_not_of(" \t\n\r");
    return str.substr(start, end - start + 1);
}

// =====================================================
// ConversationTurn Implementation
// =====================================================

ConversationTurn::ConversationTurn(const std::string& input, const std::string& response)
    : input(input), response(response) {
    id = generateElizaUUID();
    timestamp = std::chrono::system_clock::now();
    emotionalState = "neutral";
}

// =====================================================
// ConversationContext Implementation
// =====================================================

ConversationContext::ConversationContext(const std::string& sessionId, const std::string& userId)
    : sessionId(sessionId), userId(userId) {
    startTime = std::chrono::system_clock::now();
    lastActivity = startTime;
}

void ConversationContext::addTurn(const ConversationTurn& turn) {
    history.push_back(turn);
    updateLastActivity();
    
    // Keep history manageable (last 50 turns)
    if (history.size() > 50) {
        history.erase(history.begin());
    }
}

std::vector<ConversationTurn> ConversationContext::getRecentHistory(int count) const {
    if (history.empty()) return {};
    
    int start = std::max(0, static_cast<int>(history.size()) - count);
    return std::vector<ConversationTurn>(history.begin() + start, history.end());
}

std::string ConversationContext::getContextSummary() const {
    std::stringstream summary;
    summary << "Session: " << sessionId << "\n";
    summary << "User: " << userId << "\n";
    summary << "Turns: " << history.size() << "\n";
    
    auto duration = std::chrono::duration_cast<std::chrono::minutes>(
        lastActivity - startTime
    ).count();
    summary << "Duration: " << duration << " minutes\n";
    
    return summary.str();
}

void ConversationContext::setSessionData(const std::string& key, const std::string& value) {
    sessionData[key] = value;
    updateLastActivity();
}

std::string ConversationContext::getSessionData(const std::string& key) const {
    auto it = sessionData.find(key);
    return it != sessionData.end() ? it->second : "";
}

void ConversationContext::updateLastActivity() {
    lastActivity = std::chrono::system_clock::now();
}

JsonValue ConversationContext::toJson() const {
    JsonValue json;
    json["sessionId"] = std::string(sessionId);
    json["userId"] = std::string(userId);
    json["characterId"] = std::string(characterId);
    json["turnCount"] = std::string(std::to_string(history.size()));
    json["startTime"] = std::string(std::to_string(std::chrono::system_clock::to_time_t(startTime)));
    json["lastActivity"] = std::string(std::to_string(std::chrono::system_clock::to_time_t(lastActivity)));
    return json;
}

ConversationContext ConversationContext::fromJson(const JsonValue& json) {
    auto getString = [&](const std::string& key) -> std::string {
        auto it = json.find(key);
        if (it != json.end()) {
            try {
                return std::any_cast<std::string>(it->second);
            } catch (const std::bad_any_cast&) {
                return "";
            }
        }
        return "";
    };
    
    ConversationContext context(getString("sessionId"), getString("userId"));
    context.characterId = getString("characterId");
    
    try {
        auto start_time_t = std::stoll(getString("startTime"));
        auto last_time_t = std::stoll(getString("lastActivity"));
        context.startTime = std::chrono::system_clock::from_time_t(start_time_t);
        context.lastActivity = std::chrono::system_clock::from_time_t(last_time_t);
    } catch (const std::exception&) {
        // Use current time if parsing fails
    }
    
    return context;
}

// =====================================================
// ResponsePattern Implementation
// =====================================================

ResponsePattern::ResponsePattern(const std::string& pattern, const std::vector<std::string>& responses,
                                const std::string& category)
    : pattern(pattern), responses(responses), category(category) {
    id = generateElizaUUID();
}

bool ResponsePattern::matches(const std::string& input) const {
    try {
        std::regex regex_pattern(pattern, std::regex_constants::icase);
        return std::regex_search(input, regex_pattern);
    } catch (const std::regex_error&) {
        // If regex is invalid, fall back to simple substring match
        return toLowercase(input).find(toLowercase(pattern)) != std::string::npos;
    }
}

std::string ResponsePattern::generateResponse(const std::unordered_map<std::string, std::string>& captures) const {
    if (responses.empty()) {
        return "I understand.";
    }
    
    // Select random response
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, responses.size() - 1);
    
    std::string response = responses[dis(gen)];
    
    // Simple variable substitution
    for (const auto& capture : captures) {
        std::string placeholder = "{" + capture.first + "}";
        size_t pos = response.find(placeholder);
        if (pos != std::string::npos) {
            response.replace(pos, placeholder.length(), capture.second);
        }
    }
    
    return response;
}

std::vector<std::string> ResponsePattern::extractCaptures(const std::string& input) const {
    std::vector<std::string> captures;
    
    try {
        std::regex regex_pattern(pattern, std::regex_constants::icase);
        std::smatch match;
        
        if (std::regex_search(input, match, regex_pattern)) {
            for (size_t i = 1; i < match.size(); ++i) {
                captures.push_back(match[i].str());
            }
        }
    } catch (const std::regex_error&) {
        // If regex fails, return empty captures
    }
    
    return captures;
}

JsonValue ResponsePattern::toJson() const {
    JsonValue json;
    json["id"] = std::string(id);
    json["pattern"] = std::string(pattern);
    json["category"] = std::string(category);
    json["priority"] = std::string(std::to_string(priority));
    return json;
}

ResponsePattern ResponsePattern::fromJson(const JsonValue& json) {
    auto getString = [&](const std::string& key) -> std::string {
        auto it = json.find(key);
        if (it != json.end()) {
            try {
                return std::any_cast<std::string>(it->second);
            } catch (const std::bad_any_cast&) {
                return "";
            }
        }
        return "";
    };
    
    ResponsePattern pattern(getString("pattern"), {}, getString("category"));
    pattern.id = getString("id");
    
    try {
        pattern.priority = std::stof(getString("priority"));
    } catch (const std::exception&) {
        pattern.priority = 1.0f;
    }
    
    return pattern;
}

// =====================================================
// EmotionalStateTracker Implementation
// =====================================================

void EmotionalStateTracker::updateFromInput(const std::string& input) {
    auto emotionalWords = detectEmotionalWords(input);
    
    for (const auto& word : emotionalWords) {
        if (word == "happy" || word == "joy" || word == "glad" || word == "excited") {
            happiness += 0.1f;
            excitement += 0.05f;
        } else if (word == "sad" || word == "upset" || word == "down" || word == "depressed") {
            sadness += 0.1f;
            happiness -= 0.05f;
        } else if (word == "angry" || word == "mad" || word == "furious" || word == "annoyed") {
            anger += 0.1f;
            calmness -= 0.05f;
        } else if (word == "scared" || word == "afraid" || word == "worried" || word == "anxious") {
            fear += 0.1f;
            calmness -= 0.05f;
        } else if (word == "surprised" || word == "shocked" || word == "amazed") {
            surprise += 0.1f;
        } else if (word == "calm" || word == "peaceful" || word == "relaxed") {
            calmness += 0.1f;
            anger -= 0.05f;
        }
    }
    
    normalizeEmotions();
}

void EmotionalStateTracker::updateFromInteraction(const std::string& outcome) {
    if (outcome == "positive") {
        happiness += 0.05f;
        excitement += 0.03f;
        sadness -= 0.02f;
    } else if (outcome == "negative") {
        sadness += 0.05f;
        anger += 0.03f;
        happiness -= 0.02f;
    } else if (outcome == "neutral") {
        calmness += 0.02f;
    }
    
    normalizeEmotions();
}

void EmotionalStateTracker::decay(float factor) {
    happiness *= factor;
    sadness *= factor;
    anger *= factor;
    fear *= factor;
    surprise *= factor;
    disgust *= factor;
    excitement *= factor;
    calmness = std::min(1.0f, calmness + (1.0f - factor) * 0.1f); // Trend toward calm
    
    normalizeEmotions();
}

std::string EmotionalStateTracker::getDominantEmotion() const {
    float maxValue = std::max({happiness, sadness, anger, fear, surprise, disgust, excitement, calmness});
    
    if (maxValue == happiness) return "happy";
    if (maxValue == sadness) return "sad";
    if (maxValue == anger) return "angry";
    if (maxValue == fear) return "fearful";
    if (maxValue == surprise) return "surprised";
    if (maxValue == disgust) return "disgusted";
    if (maxValue == excitement) return "excited";
    return "calm";
}

float EmotionalStateTracker::getEmotionalIntensity() const {
    float total = happiness + sadness + anger + fear + surprise + disgust + excitement;
    return std::min(1.0f, total);
}

void EmotionalStateTracker::adjustEmotion(const std::string& emotion, float adjustment) {
    if (emotion == "happiness") happiness = std::clamp(happiness + adjustment, 0.0f, 1.0f);
    else if (emotion == "sadness") sadness = std::clamp(sadness + adjustment, 0.0f, 1.0f);
    else if (emotion == "anger") anger = std::clamp(anger + adjustment, 0.0f, 1.0f);
    else if (emotion == "fear") fear = std::clamp(fear + adjustment, 0.0f, 1.0f);
    else if (emotion == "surprise") surprise = std::clamp(surprise + adjustment, 0.0f, 1.0f);
    else if (emotion == "disgust") disgust = std::clamp(disgust + adjustment, 0.0f, 1.0f);
    else if (emotion == "excitement") excitement = std::clamp(excitement + adjustment, 0.0f, 1.0f);
    else if (emotion == "calmness") calmness = std::clamp(calmness + adjustment, 0.0f, 1.0f);
    
    normalizeEmotions();
}

JsonValue EmotionalStateTracker::toJson() const {
    JsonValue json;
    json["happiness"] = std::string(std::to_string(happiness));
    json["sadness"] = std::string(std::to_string(sadness));
    json["anger"] = std::string(std::to_string(anger));
    json["fear"] = std::string(std::to_string(fear));
    json["surprise"] = std::string(std::to_string(surprise));
    json["disgust"] = std::string(std::to_string(disgust));
    json["excitement"] = std::string(std::to_string(excitement));
    json["calmness"] = std::string(std::to_string(calmness));
    return json;
}

EmotionalStateTracker EmotionalStateTracker::fromJson(const JsonValue& json) {
    auto getFloat = [&](const std::string& key, float defaultVal = 0.5f) -> float {
        auto it = json.find(key);
        if (it != json.end()) {
            try {
                return std::stof(std::any_cast<std::string>(it->second));
            } catch (const std::exception&) {
                return defaultVal;
            }
        }
        return defaultVal;
    };
    
    EmotionalStateTracker tracker;
    tracker.happiness = getFloat("happiness");
    tracker.sadness = getFloat("sadness");
    tracker.anger = getFloat("anger");
    tracker.fear = getFloat("fear");
    tracker.surprise = getFloat("surprise");
    tracker.disgust = getFloat("disgust");
    tracker.excitement = getFloat("excitement");
    tracker.calmness = getFloat("calmness");
    
    return tracker;
}

void EmotionalStateTracker::normalizeEmotions() {
    // Clamp all emotions to [0, 1] range
    happiness = std::clamp(happiness, 0.0f, 1.0f);
    sadness = std::clamp(sadness, 0.0f, 1.0f);
    anger = std::clamp(anger, 0.0f, 1.0f);
    fear = std::clamp(fear, 0.0f, 1.0f);
    surprise = std::clamp(surprise, 0.0f, 1.0f);
    disgust = std::clamp(disgust, 0.0f, 1.0f);
    excitement = std::clamp(excitement, 0.0f, 1.0f);
    calmness = std::clamp(calmness, 0.0f, 1.0f);
}

std::vector<std::string> EmotionalStateTracker::detectEmotionalWords(const std::string& input) const {
    std::vector<std::string> emotionalWords;
    std::string lowerInput = toLowercase(input);
    
    // Simple emotional word detection
    std::vector<std::string> emotions = {
        "happy", "sad", "angry", "mad", "excited", "calm", "peaceful",
        "worried", "anxious", "scared", "afraid", "surprised", "shocked",
        "amazed", "joy", "glad", "upset", "down", "depressed", "furious",
        "annoyed", "relaxed"
    };
    
    for (const auto& emotion : emotions) {
        if (lowerInput.find(emotion) != std::string::npos) {
            emotionalWords.push_back(emotion);
        }
    }
    
    return emotionalWords;
}

// =====================================================
// ResponseGenerator Implementation
// =====================================================

ResponseGenerator::ResponseGenerator() {
    loadDefaultPatterns();
}

std::string ResponseGenerator::generateResponse(const std::string& input, 
                                              const ConversationContext& context,
                                              const CharacterProfile* character) {
    // Character-based response if character is provided
    if (character && character->validate()) {
        return generateCharacterResponse(input, *character, context);
    }
    
    // Knowledge-based response if knowledge base is available
    if (knowledgeIntegrationEnabled_ && knowledgeBase_) {
        std::string knowledgeResponse = generateKnowledgeBasedResponse(input);
        if (!knowledgeResponse.empty() && knowledgeResponse != "I don't have specific knowledge about that.") {
            return knowledgeResponse;
        }
    }
    
    // Pattern-based response
    auto matchingPatterns = getMatchingPatterns(input);
    if (!matchingPatterns.empty()) {
        return selectBestResponse(matchingPatterns, input);
    }
    
    // Default fallback responses
    std::vector<std::string> fallbacks = {
        "I understand. Can you tell me more about that?",
        "That's interesting. How does that make you feel?",
        "I see. What else would you like to talk about?",
        "Please go on.",
        "Can you elaborate on that?",
        "What do you think about that?",
        "How do you feel about that situation?"
    };
    
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, fallbacks.size() - 1);
    
    return fallbacks[dis(gen)];
}

void ResponseGenerator::addPattern(const ResponsePattern& pattern) {
    std::lock_guard<std::mutex> lock(patternsMutex_);
    patterns_.push_back(pattern);
}

void ResponseGenerator::removePattern(const std::string& patternId) {
    std::lock_guard<std::mutex> lock(patternsMutex_);
    patterns_.erase(
        std::remove_if(patterns_.begin(), patterns_.end(),
                      [&patternId](const ResponsePattern& p) { return p.id == patternId; }),
        patterns_.end()
    );
}

std::vector<ResponsePattern> ResponseGenerator::getMatchingPatterns(const std::string& input) const {
    // Need to use const_cast for const method with std::mutex
    std::lock_guard<std::mutex> lock(const_cast<std::mutex&>(patternsMutex_));
    
    std::vector<ResponsePattern> matching;
    for (const auto& pattern : patterns_) {
        if (pattern.matches(input)) {
            matching.push_back(pattern);
        }
    }
    
    // Sort by priority (higher priority first)
    std::sort(matching.begin(), matching.end(),
              [](const ResponsePattern& a, const ResponsePattern& b) {
                  return a.priority > b.priority;
              });
    
    return matching;
}

void ResponseGenerator::setKnowledgeBase(std::shared_ptr<KnowledgeBase> kb) {
    knowledgeBase_ = kb;
}

std::string ResponseGenerator::generateKnowledgeBasedResponse(const std::string& input) const {
    if (!knowledgeBase_) {
        return "";
    }
    
    // Search knowledge base for relevant information
    auto results = knowledgeBase_->searchByText(input, 3);
    
    if (results.empty()) {
        return "I don't have specific knowledge about that.";
    }
    
    // Generate response based on knowledge
    std::stringstream response;
    response << "Based on what I know, ";
    
    if (results.size() == 1) {
        response << results[0].content;
    } else {
        response << "there are several relevant points: ";
        for (size_t i = 0; i < results.size() && i < 2; ++i) {
            if (i > 0) response << " Also, ";
            response << results[i].content.substr(0, 100);
            if (results[i].content.length() > 100) response << "...";
        }
    }
    
    return response.str();
}

std::string ResponseGenerator::generateCharacterResponse(const std::string& input,
                                                       const CharacterProfile& character,
                                                       const ConversationContext& context) const {
    // Use character's generateResponse method
    std::string contextStr = context.getContextSummary();
    return character.generateResponse(input, contextStr);
}

std::string ResponseGenerator::processResponseTemplate(const std::string& template_,
                                                     const std::unordered_map<std::string, std::string>& variables) const {
    std::string result = template_;
    
    for (const auto& var : variables) {
        std::string placeholder = "{" + var.first + "}";
        size_t pos = 0;
        while ((pos = result.find(placeholder, pos)) != std::string::npos) {
            result.replace(pos, placeholder.length(), var.second);
            pos += var.second.length();
        }
    }
    
    return result;
}

void ResponseGenerator::loadDefaultPatterns() {
    // Load all predefined patterns
    auto allPatterns = ElizaPatterns::getAllPatterns();
    for (const auto& pattern : allPatterns) {
        addPattern(pattern);
    }
}

void ResponseGenerator::clear() {
    std::lock_guard<std::mutex> lock(patternsMutex_);
    patterns_.clear();
}

std::string ResponseGenerator::selectBestResponse(const std::vector<ResponsePattern>& patterns,
                                                const std::string& input) const {
    if (patterns.empty()) {
        return "I understand.";
    }
    
    // Use the highest priority pattern
    const auto& bestPattern = patterns[0];
    
    // Extract captures for variable substitution
    auto captures = bestPattern.extractCaptures(input);
    std::unordered_map<std::string, std::string> variables;
    
    for (size_t i = 0; i < captures.size(); ++i) {
        variables["capture" + std::to_string(i)] = captures[i];
    }
    
    // Add some common variables
    variables["input"] = input;
    variables["user"] = "you";
    
    return bestPattern.generateResponse(variables);
}

std::unordered_map<std::string, std::string> ResponseGenerator::extractVariables(const std::string& input) const {
    std::unordered_map<std::string, std::string> variables;
    
    // Simple variable extraction - could be enhanced
    variables["input"] = input;
    variables["length"] = std::to_string(input.length());
    variables["uppercase"] = input;
    std::transform(variables["uppercase"].begin(), variables["uppercase"].end(), 
                  variables["uppercase"].begin(), ::toupper);
    
    return variables;
}

// =====================================================
// ElizaCore Implementation
// =====================================================

ElizaCore::ElizaCore() {
    responseGenerator_ = std::make_shared<ResponseGenerator>();
    memory_ = std::make_shared<AgentMemoryManager>();
    logger_ = std::make_shared<AgentLogger>();
    
    logger_->log("Eliza core initialized", "info", "eliza");
}

ElizaCore::~ElizaCore() = default;

std::string ElizaCore::generateSessionId() {
    return generateElizaUUID();
}

std::string ElizaCore::processInput(const std::string& input,
                                  const std::string& sessionId,
                                  const std::string& userId) {
    std::lock_guard<std::mutex> lock(sessionsMutex_);
    
    std::string activeSessionId = sessionId;
    if (activeSessionId.empty()) {
        activeSessionId = createSession(userId);
    }
    
    // Get or create session
    auto sessionIt = sessions_.find(activeSessionId);
    if (sessionIt == sessions_.end()) {
        sessions_[activeSessionId] = ConversationContext(activeSessionId, userId);
        sessionIt = sessions_.find(activeSessionId);
    }
    
    auto& context = sessionIt->second;
    
    // Preprocess input
    std::string processedInput = preprocessInput(input);
    
    // Update emotional state if enabled
    if (emotionalTrackingEnabled_) {
        updateEmotionalState(activeSessionId, processedInput);
    }
    
    // Generate response. When character personality is enabled and the
    // session is bound to a character, resolve the profile through the
    // character manager (which owns its own mutex, distinct from
    // sessionsMutex_, so no lock-order inversion) and keep it alive in a
    // local optional for the duration of response generation. A profile
    // that fails structural validation is treated as absent so generation
    // degrades gracefully to knowledge/pattern responses instead of
    // speaking through a corrupt persona.
    std::optional<CharacterProfile> sessionCharacter;
    const CharacterProfile* character = nullptr;
    if (characterPersonalityEnabled_ && characterManager_ && !context.characterId.empty()) {
        auto charOpt = characterManager_->getCharacter(context.characterId);
        if (charOpt && charOpt->validate()) {
            sessionCharacter = std::move(charOpt);
            character = &(*sessionCharacter);
        } else if (charOpt) {
            logger_->log("Character profile failed validation; generating without persona: " +
                             context.characterId,
                         "warning", "eliza");
        }
    }
    
    std::string response = responseGenerator_->generateResponse(processedInput, context, character);
    
    // Postprocess response
    response = postprocessResponse(response, context);
    
    // Create conversation turn
    ConversationTurn turn(processedInput, response);
    context.addTurn(turn);
    
    // Save session
    saveSessionToMemory(context);
    
    // Track metrics
    trackConversationMetrics(context);
    
    logger_->log("Processed input: " + processedInput.substr(0, 50) + "...", "info", "eliza");
    
    return response;
}

std::string ElizaCore::processInputWithCharacter(const std::string& input,
                                               const std::string& characterId,
                                               const std::string& sessionId,
                                               const std::string& userId) {
    std::string activeSessionId = sessionId;
    if (activeSessionId.empty()) {
        activeSessionId = createSession(userId, characterId);
    } else {
        setCharacter(activeSessionId, characterId);
    }
    
    return processInput(input, activeSessionId, userId);
}

std::string ElizaCore::createSession(const std::string& userId, const std::string& characterId) {
    std::lock_guard<std::mutex> lock(sessionsMutex_);
    
    std::string sessionId = generateSessionId();
    ConversationContext context(sessionId, userId);
    context.characterId = characterId;
    
    sessions_[sessionId] = context;
    saveSessionToMemory(context);
    
    logger_->log("Created session: " + sessionId, "info", "eliza");
    return sessionId;
}

bool ElizaCore::endSession(const std::string& sessionId) {
    std::lock_guard<std::mutex> lock(sessionsMutex_);
    
    auto it = sessions_.find(sessionId);
    if (it != sessions_.end()) {
        // Save final session state
        saveSessionToMemory(it->second);
        sessions_.erase(it);
        
        logger_->log("Ended session: " + sessionId, "info", "eliza");
        return true;
    }
    
    return false;
}

std::optional<ConversationContext> ElizaCore::getSession(const std::string& sessionId) {
    std::lock_guard<std::mutex> lock(sessionsMutex_);
    
    auto it = sessions_.find(sessionId);
    if (it != sessions_.end()) {
        return it->second;
    }
    
    // Try loading from memory
    auto memorySession = loadSessionFromMemory(sessionId);
    if (memorySession) {
        sessions_[sessionId] = *memorySession;
        return *memorySession;
    }
    
    return std::nullopt;
}

std::vector<ConversationContext> ElizaCore::getAllSessions() const {
    std::lock_guard<std::mutex> lock(sessionsMutex_);
    
    std::vector<ConversationContext> result;
    for (const auto& [key, val] : sessions_) {
        result.push_back(val);
    }
    
    return result;
}

void ElizaCore::cleanupOldSessions(std::chrono::hours maxAge) {
    std::lock_guard<std::mutex> lock(sessionsMutex_);
    
    auto cutoffTime = std::chrono::system_clock::now() - maxAge;
    
    auto it = sessions_.begin();
    while (it != sessions_.end()) {
        if (it->second.lastActivity < cutoffTime) {
            logger_->log("Cleaning up old session: " + it->first, "info", "eliza");
            it = sessions_.erase(it);
        } else {
            ++it;
        }
    }
}

void ElizaCore::setCharacter(const std::string& sessionId, const std::string& characterId) {
    std::lock_guard<std::mutex> lock(sessionsMutex_);
    
    auto it = sessions_.find(sessionId);
    if (it != sessions_.end()) {
        it->second.characterId = characterId;
        it->second.updateLastActivity();
        saveSessionToMemory(it->second);
    }
}

std::optional<CharacterProfile> ElizaCore::getSessionCharacter(const std::string& sessionId) {
    auto session = getSession(sessionId);
    if (session && characterManager_ && !session->characterId.empty()) {
        return characterManager_->getCharacter(session->characterId);
    }
    
    return std::nullopt;
}

void ElizaCore::setKnowledgeBase(std::shared_ptr<KnowledgeBase> kb) {
    knowledgeBase_ = kb;
    if (responseGenerator_) {
        responseGenerator_->setKnowledgeBase(kb);
    }
}

void ElizaCore::setCharacterManager(std::shared_ptr<CharacterManager> cm) {
    characterManager_ = cm;
}

void ElizaCore::learnFromConversation(const std::string& sessionId) {
    auto session = getSession(sessionId);
    if (!session || session->history.empty()) {
        return;
    }
    
    // Simple learning from conversation patterns
    for (const auto& turn : session->history) {
        if (knowledgeBase_) {
            // Add conversation knowledge
            KnowledgeEntry conversationKnowledge(
                "User said: " + turn.input + " Response: " + turn.response,
                KnowledgeType::EXPERIENCE
            );
            conversationKnowledge.source = KnowledgeSource::OBSERVED;
            conversationKnowledge.confidence = ConfidenceLevel::LOW;
            conversationKnowledge.addTag("conversation");
            conversationKnowledge.addTag("interaction");
            
            knowledgeBase_->addKnowledge(conversationKnowledge);
        }
    }
    
    logger_->log("Learned from conversation: " + sessionId, "info", "eliza");
}

void ElizaCore::updateResponsePatterns(const std::string& input, const std::string& feedback) {
    // Simple pattern updating based on feedback
    // Use the input parameter to avoid warning
    if (feedback == "good" || feedback == "helpful") {
        // Could enhance patterns that led to this input
        logger_->log("Positive feedback received for input: " + input.substr(0, 20), "info", "eliza");
    } else if (feedback == "bad" || feedback == "unhelpful") {
        // Could reduce priority of patterns that led to this input
        logger_->log("Negative feedback received for input: " + input.substr(0, 20), "info", "eliza");
    }
}

void ElizaCore::setResponseGenerator(std::shared_ptr<ResponseGenerator> generator) {
    responseGenerator_ = generator;
    if (knowledgeBase_) {
        responseGenerator_->setKnowledgeBase(knowledgeBase_);
    }
}

void ElizaCore::enableEmotionalTracking(bool enable) {
    emotionalTrackingEnabled_ = enable;
    logger_->log("Emotional tracking " + std::string(enable ? "enabled" : "disabled"), "info", "eliza");
}

void ElizaCore::enableKnowledgeIntegration(bool enable) {
    knowledgeIntegrationEnabled_ = enable;
    logger_->log("Knowledge integration " + std::string(enable ? "enabled" : "disabled"), "info", "eliza");
}

void ElizaCore::enableCharacterPersonality(bool enable) {
    characterPersonalityEnabled_ = enable;
    logger_->log("Character personality " + std::string(enable ? "enabled" : "disabled"), "info", "eliza");
}

std::string ElizaCore::getConversationAnalytics() const {
    std::lock_guard<std::mutex> lock(sessionsMutex_);
    
    std::stringstream analytics;
    analytics << "Eliza Core Analytics:" << std::endl;
    analytics << "Active sessions: " << sessions_.size() << std::endl;
    
    int totalTurns = 0;
    for (const auto& [key, val] : sessions_) {
        totalTurns += val.history.size();
    }
    analytics << "Total conversation turns: " << totalTurns << std::endl;
    
    return analytics.str();
}

std::vector<std::string> ElizaCore::getFrequentTopics() const {
    // Simple implementation - could be enhanced with topic modeling
    std::vector<std::string> topics = {"emotions", "relationships", "work", "family", "future"};
    return topics;
}

std::unordered_map<std::string, int> ElizaCore::getEmotionalStateStats() const {
    std::unordered_map<std::string, int> stats;
    // Simple implementation - would track emotional states across sessions
    stats["happy"] = 10;
    stats["sad"] = 5;
    stats["neutral"] = 15;
    return stats;
}

bool ElizaCore::exportConversations(const std::string& filename) const {
    try {
        std::ofstream file(filename);
        if (!file.is_open()) {
            return false;
        }
        
        file << getConversationAnalytics() << std::endl;
        file.close();
        return true;
    } catch (const std::exception&) {
        return false;
    }
}

bool ElizaCore::importConversations(const std::string& filename) {
    try {
        std::ifstream file(filename);
        if (!file.is_open()) {
            return false;
        }
        
        logger_->log("Importing conversations from: " + filename, "info", "eliza");
        file.close();
        return true;
    } catch (const std::exception&) {
        return false;
    }
}

void ElizaCore::clearAllSessions() {
    std::lock_guard<std::mutex> lock(sessionsMutex_);
    sessions_.clear();
    memory_->clear();
    logger_->log("Cleared all sessions", "info", "eliza");
}

size_t ElizaCore::getSessionCount() const {
    std::lock_guard<std::mutex> lock(sessionsMutex_);
    return sessions_.size();
}

void ElizaCore::saveSessionToMemory(const ConversationContext& session) {
    UUID memoryId(session.sessionId);
    UUID entityId = generateElizaUUID();
    UUID agentId = generateElizaUUID();
    
    // Create CustomMetadata for the session
    CustomMetadata customMeta;
    customMeta.customData["sessionId"] = session.sessionId;
    customMeta.customData["userId"] = session.userId;
    customMeta.customData["characterId"] = session.characterId;
    customMeta.customData["turnCount"] = std::to_string(session.history.size());
    customMeta.customData["startTime"] = std::to_string(std::chrono::system_clock::to_time_t(session.startTime));
    customMeta.customData["lastActivity"] = std::to_string(std::chrono::system_clock::to_time_t(session.lastActivity));
    
    MemoryMetadata metadata = customMeta;
    auto memory = std::make_shared<Memory>(memoryId, session.getContextSummary(), 
                                          entityId, agentId, metadata);
    
    memory_->createMemory(memory, "conversations");
}

std::optional<ConversationContext> ElizaCore::loadSessionFromMemory(const std::string& sessionId) {
    UUID memoryId(sessionId);
    auto memory = memory_->getMemoryById(memoryId);
    
    if (!memory) {
        return std::nullopt;
    }
    
    // Parse metadata if it's CustomMetadata
    if (std::holds_alternative<CustomMetadata>(memory->getMetadata())) {
        const auto& customMeta = std::get<CustomMetadata>(memory->getMetadata());
        
        auto getValue = [&](const std::string& key) -> std::string {
            auto it = customMeta.customData.find(key);
            return it != customMeta.customData.end() ? it->second : "";
        };
        
        std::string userId = getValue("userId");
        
        if (!sessionId.empty()) {
            ConversationContext context(sessionId, userId);
            context.characterId = getValue("characterId");
            
            // Parse timestamps
            try {
                auto start_time_t = std::stoll(getValue("startTime"));
                auto last_time_t = std::stoll(getValue("lastActivity"));
                context.startTime = std::chrono::system_clock::from_time_t(start_time_t);
                context.lastActivity = std::chrono::system_clock::from_time_t(last_time_t);
            } catch (const std::exception&) {
                // Use current time if parsing fails
            }
            
            return context;
        }
    }
    
    return std::nullopt;
}

std::string ElizaCore::preprocessInput(const std::string& input) const {
    std::string processed = trim(input);
    
    // Basic preprocessing
    if (processed.empty()) {
        processed = "Hello";
    }
    
    // Remove excessive punctuation
    while (processed.length() > 1 && processed.back() == processed[processed.length()-2] &&
           (processed.back() == '!' || processed.back() == '?' || processed.back() == '.')) {
        processed.pop_back();
    }
    
    return processed;
}

std::string ElizaCore::postprocessResponse(const std::string& response, 
                                         const ConversationContext& context) const {
    std::string processed = response;
    
    // Add personality touches based on conversation length
    if (context.history.size() > 10) {
        // Long conversation - show familiarity
        if (processed.find("I understand") == 0) {
            processed = "I really understand, we've been talking for a while. " + processed.substr(12);
        }
    }
    
    return processed;
}

void ElizaCore::updateEmotionalState(const std::string& sessionId, const std::string& input) {
    // Simple emotional state tracking
    // In a full implementation, this would update the session's emotional state
    if (!input.empty()) {
        logger_->log("Updated emotional state for session: " + sessionId, "debug", "eliza");
    }
}

void ElizaCore::trackConversationMetrics(const ConversationContext& context) {
    // Track various conversation metrics
    logger_->log("Tracked metrics for session: " + context.sessionId + 
                " (turns: " + std::to_string(context.history.size()) + ")", "debug", "eliza");
}

// =====================================================
// Utility Functions
// =====================================================

std::string normalizeInput(const std::string& input) {
    return trim(toLowercase(input));
}

std::vector<std::string> tokenizeInput(const std::string& input) {
    std::vector<std::string> tokens;
    std::stringstream ss(input);
    std::string token;
    
    while (ss >> token) {
        tokens.push_back(token);
    }
    
    return tokens;
}

std::string extractSentiment(const std::string& input) {
    std::string lowerInput = toLowercase(input);
    
    // Simple sentiment analysis
    std::vector<std::string> positiveWords = {"good", "great", "excellent", "happy", "love", "like", "wonderful"};
    std::vector<std::string> negativeWords = {"bad", "terrible", "awful", "sad", "hate", "dislike", "horrible"};
    
    int positiveScore = 0;
    int negativeScore = 0;
    
    for (const auto& word : positiveWords) {
        if (lowerInput.find(word) != std::string::npos) {
            positiveScore++;
        }
    }
    
    for (const auto& word : negativeWords) {
        if (lowerInput.find(word) != std::string::npos) {
            negativeScore++;
        }
    }
    
    if (positiveScore > negativeScore) return "positive";
    if (negativeScore > positiveScore) return "negative";
    return "neutral";
}

bool isQuestion(const std::string& input) {
    std::string trimmed = trim(input);
    return !trimmed.empty() && (trimmed.back() == '?' || 
                               toLowercase(trimmed).find("what") == 0 ||
                               toLowercase(trimmed).find("how") == 0 ||
                               toLowercase(trimmed).find("why") == 0 ||
                               toLowercase(trimmed).find("when") == 0 ||
                               toLowercase(trimmed).find("where") == 0);
}

bool isGreeting(const std::string& input) {
    std::string lowerInput = toLowercase(trim(input));
    return lowerInput == "hello" || lowerInput == "hi" || lowerInput == "hey" ||
           lowerInput.find("good morning") != std::string::npos ||
           lowerInput.find("good afternoon") != std::string::npos ||
           lowerInput.find("good evening") != std::string::npos;
}

bool isGoodbye(const std::string& input) {
    std::string lowerInput = toLowercase(trim(input));
    return lowerInput == "goodbye" || lowerInput == "bye" || lowerInput == "see you" ||
           lowerInput.find("talk to you later") != std::string::npos ||
           lowerInput.find("have a good") != std::string::npos;
}

// =====================================================
// Predefined Response Patterns
// =====================================================

namespace ElizaPatterns {

std::vector<ResponsePattern> getGreetingPatterns() {
    std::vector<ResponsePattern> patterns;
    
    patterns.emplace_back(
        R"((hello|hi|hey))",
        std::vector<std::string>{
            "Hello! How are you feeling today?",
            "Hi there! What would you like to talk about?",
            "Hello! I'm here to listen. What's on your mind?"
        },
        "greeting"
    );
    
    patterns.emplace_back(
        R"(good (morning|afternoon|evening))",
        std::vector<std::string>{
            "Good {capture0}! How has your day been?",
            "Good {capture0}! What brings you here today?",
            "Good {capture0}! I hope you're doing well."
        },
        "greeting"
    );
    
    for (auto& pattern : patterns) {
        pattern.priority = 2.0f; // High priority for greetings
    }
    
    return patterns;
}

std::vector<ResponsePattern> getQuestionPatterns() {
    std::vector<ResponsePattern> patterns;
    
    patterns.emplace_back(
        R"(what (is|are) (.+)\?)",
        std::vector<std::string>{
            "That's an interesting question about {capture1}. What do you think?",
            "When you ask about {capture1}, what comes to mind?",
            "What would you like to know specifically about {capture1}?"
        },
        "question"
    );
    
    patterns.emplace_back(
        R"(how (do|can) (.+)\?)",
        std::vector<std::string>{
            "How do you feel about {capture1}?",
            "What's your experience with {capture1}?",
            "When you think about {capture1}, what happens?"
        },
        "question"
    );
    
    patterns.emplace_back(
        R"(why (.+)\?)",
        std::vector<std::string>{
            "Why do you think {capture0}?",
            "What makes you wonder about {capture0}?",
            "How important is it to understand why {capture0}?"
        },
        "question"
    );
    
    for (auto& pattern : patterns) {
        pattern.priority = 1.8f;
    }
    
    return patterns;
}

std::vector<ResponsePattern> getEmotionalPatterns() {
    std::vector<ResponsePattern> patterns;
    
    patterns.emplace_back(
        R"(i feel (sad|depressed|down|upset))",
        std::vector<std::string>{
            "I'm sorry you're feeling {capture0}. Can you tell me more about what's causing these feelings?",
            "It sounds like you're going through a difficult time. What's been on your mind?",
            "Feeling {capture0} can be really hard. Would you like to talk about what's happening?"
        },
        "emotional"
    );
    
    patterns.emplace_back(
        R"(i feel (happy|excited|great|wonderful))",
        std::vector<std::string>{
            "That's wonderful to hear! What's making you feel {capture0}?",
            "I'm glad you're feeling {capture0}. What's been going well for you?",
            "It's great that you're feeling {capture0}. Can you share what's brought you joy?"
        },
        "emotional"
    );
    
    patterns.emplace_back(
        R"(i'm (angry|mad|frustrated|annoyed))",
        std::vector<std::string>{
            "It sounds like something is really bothering you. What's making you feel {capture0}?",
            "I can hear that you're {capture0}. What happened?",
            "When you feel {capture0}, what helps you cope with those feelings?"
        },
        "emotional"
    );
    
    for (auto& pattern : patterns) {
        pattern.priority = 2.2f; // Very high priority for emotional content
    }
    
    return patterns;
}

std::vector<ResponsePattern> getReflectivePatterns() {
    std::vector<ResponsePattern> patterns;
    
    patterns.emplace_back(
        R"(i think (.+))",
        std::vector<std::string>{
            "You think {capture0}. What led you to that conclusion?",
            "That's an interesting thought about {capture0}. How long have you felt this way?",
            "When you think {capture0}, how does that affect you?"
        },
        "reflective"
    );
    
    patterns.emplace_back(
        R"(i believe (.+))",
        std::vector<std::string>{
            "Your belief about {capture0} seems important to you. Can you tell me more?",
            "What experiences have shaped your belief that {capture0}?",
            "How does believing {capture0} influence your daily life?"
        },
        "reflective"
    );
    
    patterns.emplace_back(
        R"(i remember (.+))",
        std::vector<std::string>{
            "That memory about {capture0} seems significant. How do you feel about it now?",
            "When you remember {capture0}, what emotions come up for you?",
            "Memories like {capture0} can be powerful. What does this one mean to you?"
        },
        "reflective"
    );
    
    for (auto& pattern : patterns) {
        pattern.priority = 1.5f;
    }
    
    return patterns;
}

std::vector<ResponsePattern> getDefaultPatterns() {
    std::vector<ResponsePattern> patterns;
    
    patterns.emplace_back(
        R"((.+))",
        std::vector<std::string>{
            "I understand. Can you tell me more about that?",
            "That's interesting. How does that make you feel?",
            "Please continue. I'm listening.",
            "What do you think about that?",
            "How do you feel about that situation?"
        },
        "default"
    );
    
    for (auto& pattern : patterns) {
        pattern.priority = 0.5f; // Low priority - fallback patterns
    }
    
    return patterns;
}

std::vector<ResponsePattern> getAllPatterns() {
    std::vector<ResponsePattern> allPatterns;
    
    auto greetings = getGreetingPatterns();
    auto questions = getQuestionPatterns();
    auto emotional = getEmotionalPatterns();
    auto reflective = getReflectivePatterns();
    auto defaults = getDefaultPatterns();
    
    allPatterns.insert(allPatterns.end(), greetings.begin(), greetings.end());
    allPatterns.insert(allPatterns.end(), questions.begin(), questions.end());
    allPatterns.insert(allPatterns.end(), emotional.begin(), emotional.end());
    allPatterns.insert(allPatterns.end(), reflective.begin(), reflective.end());
    allPatterns.insert(allPatterns.end(), defaults.begin(), defaults.end());
    
    return allPatterns;
}

} // namespace ElizaPatterns

// =====================================================
// DialogueSlot Implementation
// =====================================================

bool DialogueSlot::validate(const std::any& val) const {
    if (!val.has_value()) {
        return !required;
    }
    
    try {
        if (type == "string") {
            auto str = std::any_cast<std::string>(val);
            if (!allowedValues.empty()) {
                return std::find(allowedValues.begin(), allowedValues.end(), str) != allowedValues.end();
            }
            return true;
        } else if (type == "number") {
            std::any_cast<double>(val);
            return true;
        } else if (type == "boolean") {
            std::any_cast<bool>(val);
            return true;
        } else if (type == "enum") {
            auto str = std::any_cast<std::string>(val);
            return std::find(allowedValues.begin(), allowedValues.end(), str) != allowedValues.end();
        }
    } catch (const std::bad_any_cast&) {
        return false;
    }
    
    return true;
}

std::string DialogueSlot::getPrompt() const {
    if (!prompt.empty()) {
        return prompt;
    }
    return "Please provide " + name + ":";
}

// =====================================================
// DialogueFrame Implementation
// =====================================================

bool DialogueFrame::allSlotsFilledStatus() const {
    for (const auto& slot : slots) {
        if (slot.required && !slot.filled) {
            return false;
        }
    }
    return true;
}

std::vector<std::string> DialogueFrame::getMissingSlots() const {
    std::vector<std::string> missing;
    for (const auto& slot : slots) {
        if (slot.required && !slot.filled) {
            missing.push_back(slot.name);
        }
    }
    return missing;
}

std::string DialogueFrame::getNextPrompt() const {
    for (const auto& slot : slots) {
        if (slot.required && !slot.filled) {
            return slot.getPrompt();
        }
    }
    return "";
}

// =====================================================
// DialogueStateMachine Implementation
// =====================================================

DialogueStateMachine::DialogueStateMachine() {
    loadIntentPatterns();
    loadEntityPatterns();
    initializeDefaultTransitions();
}

void DialogueStateMachine::loadIntentPatterns() {
    intentPatterns_[DialogueIntent::GREETING] = {
        std::regex(R"(\b(hi|hello|hey|greetings|good\s*(morning|afternoon|evening))\b)", std::regex::icase)
    };
    
    intentPatterns_[DialogueIntent::GOODBYE] = {
        std::regex(R"(\b(bye|goodbye|see\s*you|farewell|take\s*care)\b)", std::regex::icase)
    };
    
    intentPatterns_[DialogueIntent::QUESTION] = {
        std::regex(R"(\b(what|who|where|when|why|how|can|could|would|should|is|are|do|does)\b.*\?)", std::regex::icase),
        std::regex(R"(\?$)")
    };
    
    intentPatterns_[DialogueIntent::CONFIRMATION] = {
        std::regex(R"(\b(yes|yeah|yep|sure|okay|ok|correct|right|exactly|absolutely)\b)", std::regex::icase)
    };
    
    intentPatterns_[DialogueIntent::NEGATION] = {
        std::regex(R"(\b(no|nope|not|never|neither|none|don't|doesn't|won't|can't)\b)", std::regex::icase)
    };
    
    intentPatterns_[DialogueIntent::HELP] = {
        std::regex(R"(\b(help|assist|support|guide|explain)\b)", std::regex::icase)
    };
    
    intentPatterns_[DialogueIntent::CLARIFICATION] = {
        std::regex(R"(\b(what\s*do\s*you\s*mean|could\s*you\s*clarify|i\s*don't\s*understand|sorry\s*what)\b)", std::regex::icase)
    };
    
    intentPatterns_[DialogueIntent::TOPIC_CHANGE] = {
        std::regex(R"(\b(by\s*the\s*way|anyway|speaking\s*of|let's\s*talk\s*about|changing\s*topic)\b)", std::regex::icase)
    };
}

void DialogueStateMachine::loadEntityPatterns() {
    entityPatterns_.push_back({"name", std::regex(R"(\b(my\s*name\s*is|i'm|i\s*am)\s+([A-Za-z]+))", std::regex::icase)});
    entityPatterns_.push_back({"email", std::regex(R"(\b([a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\.[a-zA-Z]{2,})\b)")});
    entityPatterns_.push_back({"number", std::regex(R"(\b(\d+(?:\.\d+)?)\b)")});
    entityPatterns_.push_back({"date", std::regex(R"(\b(\d{1,2}[/-]\d{1,2}[/-]\d{2,4})\b)")});
}

void DialogueStateMachine::initializeDefaultTransitions() {
    // IDLE -> GREETING
    transitions_.push_back({
        DialogueState::IDLE,
        DialogueIntent::GREETING,
        DialogueState::GREETING,
        "Hello! How can I help you today?",
        nullptr,
        nullptr,
        1.0f
    });
    
    // GREETING -> TOPIC_DISCOVERY
    transitions_.push_back({
        DialogueState::GREETING,
        DialogueIntent::STATEMENT,
        DialogueState::TOPIC_DISCOVERY,
        "I see. Tell me more about that.",
        nullptr,
        nullptr,
        1.0f
    });
    
    transitions_.push_back({
        DialogueState::GREETING,
        DialogueIntent::QUESTION,
        DialogueState::PROCESSING,
        "",
        nullptr,
        nullptr,
        1.0f
    });
    
    // TOPIC_DISCOVERY -> INFORMATION_GATHERING
    transitions_.push_back({
        DialogueState::TOPIC_DISCOVERY,
        DialogueIntent::STATEMENT,
        DialogueState::INFORMATION_GATHERING,
        "That's interesting. Can you provide more details?",
        nullptr,
        nullptr,
        0.8f
    });
    
    // INFORMATION_GATHERING -> PROCESSING
    transitions_.push_back({
        DialogueState::INFORMATION_GATHERING,
        DialogueIntent::CONFIRMATION,
        DialogueState::PROCESSING,
        "Great, let me process that information.",
        nullptr,
        nullptr,
        1.0f
    });
    
    // PROCESSING -> RESPONSE_DELIVERY
    transitions_.push_back({
        DialogueState::PROCESSING,
        DialogueIntent::UNKNOWN,
        DialogueState::RESPONSE_DELIVERY,
        "",
        nullptr,
        nullptr,
        0.5f
    });
    
    // RESPONSE_DELIVERY -> FOLLOW_UP
    transitions_.push_back({
        DialogueState::RESPONSE_DELIVERY,
        DialogueIntent::QUESTION,
        DialogueState::FOLLOW_UP,
        "",
        nullptr,
        nullptr,
        1.0f
    });
    
    // FOLLOW_UP -> PROCESSING
    transitions_.push_back({
        DialogueState::FOLLOW_UP,
        DialogueIntent::STATEMENT,
        DialogueState::PROCESSING,
        "",
        nullptr,
        nullptr,
        0.8f
    });
    
    // Any state -> CLOSING on goodbye
    for (auto state : {DialogueState::GREETING, DialogueState::TOPIC_DISCOVERY, 
                       DialogueState::INFORMATION_GATHERING, DialogueState::RESPONSE_DELIVERY,
                       DialogueState::FOLLOW_UP}) {
        transitions_.push_back({
            state,
            DialogueIntent::GOODBYE,
            DialogueState::CLOSING,
            "Goodbye! It was nice talking with you.",
            nullptr,
            nullptr,
            2.0f  // High priority
        });
    }
    
    // CLOSING -> IDLE
    transitions_.push_back({
        DialogueState::CLOSING,
        DialogueIntent::UNKNOWN,
        DialogueState::IDLE,
        "",
        nullptr,
        nullptr,
        1.0f
    });
    
    // Any state -> CLARIFICATION
    for (auto state : {DialogueState::TOPIC_DISCOVERY, DialogueState::INFORMATION_GATHERING,
                       DialogueState::RESPONSE_DELIVERY}) {
        transitions_.push_back({
            state,
            DialogueIntent::CLARIFICATION,
            DialogueState::CLARIFICATION,
            "Let me explain that more clearly...",
            nullptr,
            nullptr,
            1.5f
        });
    }
    
    // CLARIFICATION -> previous state or TOPIC_DISCOVERY
    transitions_.push_back({
        DialogueState::CLARIFICATION,
        DialogueIntent::CONFIRMATION,
        DialogueState::TOPIC_DISCOVERY,
        "Great, I'm glad that's clearer now.",
        nullptr,
        nullptr,
        1.0f
    });
}

DialogueIntent DialogueStateMachine::detectIntent(const std::string& input) const {
    std::string normalizedInput = toLowercase(trim(input));
    
    // Check each intent pattern
    for (const auto& [intent, patterns] : intentPatterns_) {
        for (const auto& pattern : patterns) {
            if (std::regex_search(normalizedInput, pattern)) {
                return intent;
            }
        }
    }
    
    // Default to statement if no other intent detected
    if (!input.empty()) {
        return DialogueIntent::STATEMENT;
    }
    
    return DialogueIntent::UNKNOWN;
}

std::unordered_map<std::string, std::string> DialogueStateMachine::extractEntities(const std::string& input) const {
    std::unordered_map<std::string, std::string> entities;
    
    for (const auto& [entityType, pattern] : entityPatterns_) {
        std::smatch match;
        if (std::regex_search(input, match, pattern)) {
            // Get the last capture group (the actual value)
            entities[entityType] = match[match.size() - 1].str();
        }
    }
    
    return entities;
}

DialogueTransition* DialogueStateMachine::findTransition(DialogueState fromState, DialogueIntent intent, const ConversationContext& context) {
    DialogueTransition* bestMatch = nullptr;
    float bestPriority = -1.0f;
    
    for (auto& transition : transitions_) {
        if (transition.fromState == fromState && transition.intent == intent) {
            if (transition.priority > bestPriority) {
                // Check guard condition if present - evaluate with context
                if (!transition.guard || transition.guard(context)) {
                    bestMatch = &transition;
                    bestPriority = transition.priority;
                }
            }
        }
    }
    
    // If no exact match, try with UNKNOWN intent as fallback
    if (!bestMatch && intent != DialogueIntent::UNKNOWN) {
        for (auto& transition : transitions_) {
            if (transition.fromState == fromState && transition.intent == DialogueIntent::UNKNOWN) {
                if (transition.priority > bestPriority) {
                    // Check guard condition for fallback transitions too
                    if (!transition.guard || transition.guard(context)) {
                        bestMatch = &transition;
                        bestPriority = transition.priority;
                    }
                }
            }
        }
    }
    
    return bestMatch;
}

std::string DialogueStateMachine::executeTransition(DialogueTransition& transition, 
                                                     ConversationContext& context) {
    DialogueState oldState = currentState_;
    
    // Execute action if present
    if (transition.action) {
        transition.action(context);
    }
    
    // Transition to new state
    currentState_ = transition.toState;
    recordStateTransition(oldState, currentState_);
    
    return transition.responseTemplate;
}

void DialogueStateMachine::recordStateTransition(DialogueState from, DialogueState to) {
    // Maintain an ordered trace of visited states. Seed the trace with the
    // originating state the first time, then always append the destination so
    // that both endpoints of every transition (including forceState() jumps)
    // are captured. Previously only `from` was recorded, which silently
    // dropped the destination state from getStateHistory() until the *next*
    // transition and lost forced-state targets entirely.
    if (stateHistory_.empty()) {
        stateHistory_.push_back(from);
    }
    stateHistory_.push_back(to);
    while (stateHistory_.size() > 100) {
        stateHistory_.erase(stateHistory_.begin());
    }
}

std::string DialogueStateMachine::processInput(const std::string& input, ConversationContext& context) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    // Detect intent
    DialogueIntent intent = detectIntent(input);
    
    // Extract entities and store in context
    auto entities = extractEntities(input);
    for (const auto& [key, value] : entities) {
        context.setSessionData("entity_" + key, value);
    }
    
    // Try to fill slots in current frame
    if (!frameStack_.empty()) {
        auto& currentFrame = frameStack_.back();
        for (auto& slot : currentFrame.slots) {
            if (!slot.filled) {
                auto it = entities.find(slot.name);
                if (it != entities.end()) {
                    slot.value = it->second;
                    slot.filled = true;
                }
            }
        }
        
        // Check if all required slots are filled
        if (currentFrame.allSlotsFilledStatus()) {
            currentFrame.completed = true;
        }
    }
    
    // Find and execute transition
    DialogueTransition* transition = findTransition(currentState_, intent, context);
    if (transition) {
        return executeTransition(*transition, context);
    }
    
    // No valid transition found - stay in current state
    return "I understand. Please continue.";
}

DialogueState DialogueStateMachine::getCurrentState() const {
    return currentState_;
}

std::string DialogueStateMachine::getCurrentStateString() const {
    return dialogueStateToString(currentState_);
}

void DialogueStateMachine::transitionTo(DialogueState newState) {
    std::lock_guard<std::mutex> lock(mutex_);
    recordStateTransition(currentState_, newState);
    currentState_ = newState;
}

void DialogueStateMachine::registerTransition(const DialogueTransition& transition) {
    std::lock_guard<std::mutex> lock(mutex_);
    transitions_.push_back(transition);
}

void DialogueStateMachine::clearTransitions() {
    std::lock_guard<std::mutex> lock(mutex_);
    transitions_.clear();
}

void DialogueStateMachine::pushFrame(const DialogueFrame& frame) {
    std::lock_guard<std::mutex> lock(mutex_);
    frameStack_.push_back(frame);
}

std::optional<DialogueFrame> DialogueStateMachine::popFrame() {
    std::lock_guard<std::mutex> lock(mutex_);
    if (frameStack_.empty()) {
        return std::nullopt;
    }
    DialogueFrame frame = frameStack_.back();
    frameStack_.pop_back();
    return frame;
}

std::optional<DialogueFrame> DialogueStateMachine::getCurrentFrame() const {
    std::lock_guard<std::mutex> lock(mutex_);
    if (frameStack_.empty()) {
        return std::nullopt;
    }
    return frameStack_.back();
}

bool DialogueStateMachine::fillSlot(const std::string& slotName, const std::any& value) {
    std::lock_guard<std::mutex> lock(mutex_);
    if (frameStack_.empty()) {
        return false;
    }
    
    auto& currentFrame = frameStack_.back();
    for (auto& slot : currentFrame.slots) {
        if (slot.name == slotName) {
            if (slot.validate(value)) {
                slot.value = value;
                slot.filled = true;
                return true;
            }
            return false;
        }
    }
    return false;
}

void DialogueStateMachine::reset() {
    std::lock_guard<std::mutex> lock(mutex_);
    currentState_ = DialogueState::IDLE;
    frameStack_.clear();
    stateHistory_.clear();
}

std::vector<DialogueState> DialogueStateMachine::getStateHistory() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return stateHistory_;
}

JsonValue DialogueStateMachine::toJson() const {
    JsonValue json;
    json["currentState"] = getCurrentStateString();
    json["historySize"] = std::to_string(stateHistory_.size());
    json["frameStackSize"] = std::to_string(frameStack_.size());
    return json;
}

std::unique_ptr<DialogueStateMachine> DialogueStateMachine::fromJson(const JsonValue& json) {
    auto machine = std::make_unique<DialogueStateMachine>();
    auto it = json.find("currentState");
    if (it != json.end()) {
        try {
            std::string stateStr = std::any_cast<std::string>(it->second);
            machine->transitionTo(stringToDialogueState(stateStr));
        } catch (const std::bad_any_cast&) {
            // Keep default state
        }
    }
    return machine;
}

// =====================================================
// Utility Functions
// =====================================================

std::string dialogueStateToString(DialogueState state) {
    switch (state) {
        case DialogueState::IDLE: return "IDLE";
        case DialogueState::GREETING: return "GREETING";
        case DialogueState::TOPIC_DISCOVERY: return "TOPIC_DISCOVERY";
        case DialogueState::INFORMATION_GATHERING: return "INFORMATION_GATHERING";
        case DialogueState::CLARIFICATION: return "CLARIFICATION";
        case DialogueState::PROCESSING: return "PROCESSING";
        case DialogueState::RESPONSE_DELIVERY: return "RESPONSE_DELIVERY";
        case DialogueState::CONFIRMATION: return "CONFIRMATION";
        case DialogueState::FOLLOW_UP: return "FOLLOW_UP";
        case DialogueState::CLOSING: return "CLOSING";
        case DialogueState::ERROR_RECOVERY: return "ERROR_RECOVERY";
        default: return "UNKNOWN";
    }
}

DialogueState stringToDialogueState(const std::string& str) {
    if (str == "IDLE") return DialogueState::IDLE;
    if (str == "GREETING") return DialogueState::GREETING;
    if (str == "TOPIC_DISCOVERY") return DialogueState::TOPIC_DISCOVERY;
    if (str == "INFORMATION_GATHERING") return DialogueState::INFORMATION_GATHERING;
    if (str == "CLARIFICATION") return DialogueState::CLARIFICATION;
    if (str == "PROCESSING") return DialogueState::PROCESSING;
    if (str == "RESPONSE_DELIVERY") return DialogueState::RESPONSE_DELIVERY;
    if (str == "CONFIRMATION") return DialogueState::CONFIRMATION;
    if (str == "FOLLOW_UP") return DialogueState::FOLLOW_UP;
    if (str == "CLOSING") return DialogueState::CLOSING;
    if (str == "ERROR_RECOVERY") return DialogueState::ERROR_RECOVERY;
    return DialogueState::IDLE;
}

std::string dialogueIntentToString(DialogueIntent intent) {
    switch (intent) {
        case DialogueIntent::UNKNOWN: return "UNKNOWN";
        case DialogueIntent::GREETING: return "GREETING";
        case DialogueIntent::QUESTION: return "QUESTION";
        case DialogueIntent::STATEMENT: return "STATEMENT";
        case DialogueIntent::REQUEST: return "REQUEST";
        case DialogueIntent::CONFIRMATION: return "CONFIRMATION";
        case DialogueIntent::NEGATION: return "NEGATION";
        case DialogueIntent::CLARIFICATION: return "CLARIFICATION";
        case DialogueIntent::GOODBYE: return "GOODBYE";
        case DialogueIntent::HELP: return "HELP";
        case DialogueIntent::EMOTION_EXPRESSION: return "EMOTION_EXPRESSION";
        case DialogueIntent::TOPIC_CHANGE: return "TOPIC_CHANGE";
        default: return "UNKNOWN";
    }
}

DialogueIntent stringToDialogueIntent(const std::string& str) {
    if (str == "GREETING") return DialogueIntent::GREETING;
    if (str == "QUESTION") return DialogueIntent::QUESTION;
    if (str == "STATEMENT") return DialogueIntent::STATEMENT;
    if (str == "REQUEST") return DialogueIntent::REQUEST;
    if (str == "CONFIRMATION") return DialogueIntent::CONFIRMATION;
    if (str == "NEGATION") return DialogueIntent::NEGATION;
    if (str == "CLARIFICATION") return DialogueIntent::CLARIFICATION;
    if (str == "GOODBYE") return DialogueIntent::GOODBYE;
    if (str == "HELP") return DialogueIntent::HELP;
    if (str == "EMOTION_EXPRESSION") return DialogueIntent::EMOTION_EXPRESSION;
    if (str == "TOPIC_CHANGE") return DialogueIntent::TOPIC_CHANGE;
    return DialogueIntent::UNKNOWN;
}

} // namespace elizaos
