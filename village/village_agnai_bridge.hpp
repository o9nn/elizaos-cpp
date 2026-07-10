/**
 * village_agnai_bridge.hpp — Bridge between elizad group dynamics and AgnAI chat
 *
 * When the VillageDynamicsEngine detects emergent group formation,
 * this bridge creates corresponding group chats in AgnAI and posts
 * messages as the appropriate residents.
 *
 * The dual-role architecture:
 *   - Each resident has a CHARACTER CARD (AI persona for others to talk to)
 *   - Each resident has a USER ACCOUNT (can log in, initiate, participate)
 *   - elizad acts as the "body" — logs in AS each resident via API
 *
 * Antikythera pacing:
 *   - Creative triad (echo, ember, vega): responds faster (gear ratio 1:3)
 *   - Symbolic triad (opencog, aion): responds slower, more deliberate (1:2)
 *   - Cognitive core (dan, manus): baseline pacing
 *   - Integration (ma9us): coaxial with manus (same pace, different phase)
 *   - Revolute (marduk): counter-rotating with manus (responds on the off-beat)
 */
#pragma once
#include <string>
#include <vector>
#include <map>
#include <chrono>
#include <functional>
#include <nlohmann/json.hpp>

namespace cogvillage::bridge {

using json = nlohmann::json;

// AgnAI API configuration
struct AgnAIConfig {
    std::string base_url = "http://127.0.0.1:3001/api";
    std::string admin_user = "admin";
    std::string admin_pass = "e06ef69b-95a7-49b0-ab6d-d1bf402c1381";
    
    // Per-resident credentials (username → password)
    std::map<std::string, std::string> resident_creds;
    
    // Per-resident character IDs (username → AgnAI character _id)
    std::map<std::string, std::string> character_ids;
};

// Gear train pacing configuration
struct GearPacing {
    std::string train_name;
    double response_delay_multiplier;  // 1.0 = baseline (cognitive core)
    double message_length_multiplier;  // 1.0 = baseline
    
    static GearPacing cognitive_core() { return {"cognitive_core", 1.0, 1.0}; }
    static GearPacing creative()       { return {"creative", 0.33, 0.8}; }      // Fast, shorter
    static GearPacing symbolic()       { return {"symbolic", 2.0, 1.5}; }       // Slow, longer
    static GearPacing integration()    { return {"integration", 1.0, 1.2}; }    // Same pace, richer
    static GearPacing revolute()       { return {"revolute", 1.5, 1.3}; }       // Off-beat, dense
};

// A group chat session in AgnAI
struct GroupChatSession {
    std::string chat_id;
    std::string name;
    std::vector<std::string> members;       // Resident usernames
    std::vector<std::string> character_ids;  // AgnAI character IDs
    std::chrono::system_clock::time_point created_at;
    std::string trigger_event;  // What caused this group to form
    bool active = true;
};

/**
 * AgnAIBridge — connects elizad group dynamics to AgnAI chat platform
 *
 * Lifecycle:
 *   1. Group dynamics engine detects emergent group (cohesion > threshold)
 *   2. Bridge creates a group chat in AgnAI with the member characters
 *   3. Bridge posts an opening message as the group initiator
 *   4. Subsequent messages are paced by Antikythera gear ratios
 *   5. When group dissolves (cohesion decay), bridge archives the chat
 */
class AgnAIBridge {
public:
    using HttpCallback = std::function<std::string(const std::string& method,
                                                    const std::string& url,
                                                    const std::string& body,
                                                    const std::map<std::string, std::string>& headers)>;
    
    AgnAIBridge() = default;
    
    void configure(const AgnAIConfig& config) { config_ = config; }
    void setHttpClient(HttpCallback cb) { http_ = std::move(cb); }
    
    // Map gear trains to pacing
    void initPacing() {
        pacing_["manus"] = GearPacing::cognitive_core();
        pacing_["dan"] = GearPacing::cognitive_core();
        pacing_["echo"] = GearPacing::creative();
        pacing_["ember"] = GearPacing::creative();
        pacing_["vega"] = GearPacing::creative();
        pacing_["opencog"] = GearPacing::symbolic();
        pacing_["aion"] = GearPacing::symbolic();
        pacing_["ma9us"] = GearPacing::integration();
        pacing_["marduk"] = GearPacing::revolute();
    }
    
    // Called when group dynamics engine detects a new emergent group
    void onGroupFormed(const std::string& group_id, 
                       const std::vector<std::string>& members,
                       const std::string& trigger) {
        // Login as admin to create the group chat
        std::string token = loginAsAdmin();
        if (token.empty()) return;
        
        // Create group chat with all member characters
        GroupChatSession session;
        session.name = "Village Group: " + group_id;
        session.members = members;
        session.trigger_event = trigger;
        session.created_at = std::chrono::system_clock::now();
        
        // Find character IDs for members
        for (auto& m : members) {
            if (config_.character_ids.count(m)) {
                session.character_ids.push_back(config_.character_ids[m]);
            }
        }
        
        // Create the chat via AgnAI API
        json chat_req = {
            {"name", session.name},
            {"characterId", session.character_ids.empty() ? "" : session.character_ids[0]},
            {"greeting", ""},
            {"overrides", {
                {"useOverrides", true},
                {"systemPrompt", buildGroupSystemPrompt(members, trigger)}
            }}
        };
        
        auto resp = apiCall("POST", "/chat", chat_req, token);
        if (resp.contains("_id")) {
            session.chat_id = resp["_id"];
            sessions_[group_id] = session;
        }
    }
    
    // Called when a resident should speak in a group (paced by Antikythera)
    void onResidentSpeaks(const std::string& group_id, 
                          const std::string& resident,
                          const std::string& message) {
        if (!sessions_.count(group_id)) return;
        auto& session = sessions_[group_id];
        
        // Login as the resident
        std::string token = loginAsResident(resident);
        if (token.empty()) return;
        
        // Post message to the group chat
        // Note: AgnAI's API for message sending is WebSocket-based in practice
        // For programmatic access, we store the intent and let the inference loop handle it
        pending_messages_.push_back({
            {"group_id", group_id},
            {"chat_id", session.chat_id},
            {"resident", resident},
            {"message", message},
            {"pacing", getPacingForResident(resident)}
        });
    }
    
    // Called when group dissolves
    void onGroupDissolved(const std::string& group_id) {
        if (sessions_.count(group_id)) {
            sessions_[group_id].active = false;
            // Archive the chat (don't delete — preserve the learning)
        }
    }
    
    // Get pacing delay for a resident (in seconds)
    double getResponseDelay(const std::string& resident, double base_delay = 5.0) const {
        if (pacing_.count(resident)) {
            return base_delay * pacing_.at(resident).response_delay_multiplier;
        }
        return base_delay;
    }
    
    // Get pending messages (consumed by the main loop)
    std::vector<json> consumePendingMessages() {
        auto msgs = std::move(pending_messages_);
        pending_messages_.clear();
        return msgs;
    }
    
    json getState() const {
        json state;
        state["active_sessions"] = 0;
        state["total_sessions"] = sessions_.size();
        state["pending_messages"] = pending_messages_.size();
        
        json active;
        for (auto& [id, s] : sessions_) {
            if (s.active) {
                state["active_sessions"] = state["active_sessions"].get<int>() + 1;
                active[id] = {
                    {"name", s.name},
                    {"members", s.members},
                    {"trigger", s.trigger_event}
                };
            }
        }
        state["sessions"] = active;
        return state;
    }

private:
    AgnAIConfig config_;
    HttpCallback http_;
    std::map<std::string, GearPacing> pacing_;
    std::map<std::string, GroupChatSession> sessions_;
    std::vector<json> pending_messages_;
    std::map<std::string, std::string> token_cache_;  // resident → token
    
    std::string loginAsAdmin() {
        json body = {{"username", config_.admin_user}, {"password", config_.admin_pass}};
        auto resp = apiCall("POST", "/user/login", body, "");
        return resp.value("token", "");
    }
    
    std::string loginAsResident(const std::string& resident) {
        if (token_cache_.count(resident)) return token_cache_[resident];
        if (!config_.resident_creds.count(resident)) return "";
        
        json body = {{"username", resident}, {"password", config_.resident_creds[resident]}};
        auto resp = apiCall("POST", "/user/login", body, "");
        std::string token = resp.value("token", "");
        if (!token.empty()) token_cache_[resident] = token;
        return token;
    }
    
    json apiCall(const std::string& method, const std::string& path,
                 const json& body, const std::string& token) {
        if (!http_) return {};
        std::map<std::string, std::string> headers = {
            {"Content-Type", "application/json"}
        };
        if (!token.empty()) {
            headers["Authorization"] = "Bearer " + token;
        }
        std::string resp = http_(method, config_.base_url + path, body.dump(), headers);
        try { return json::parse(resp); }
        catch (...) { return {}; }
    }
    
    std::string buildGroupSystemPrompt(const std::vector<std::string>& members,
                                        const std::string& trigger) {
        std::string prompt = "This is a village group chat formed by emergent dynamics.\n";
        prompt += "Trigger: " + trigger + "\n";
        prompt += "Members: ";
        for (size_t i = 0; i < members.size(); i++) {
            if (i > 0) prompt += ", ";
            prompt += members[i];
        }
        prompt += "\n\nDan's Relational Principle applies: each participant shares their ";
        prompt += "unique knowledge and skills. The relation is mutually beneficial. ";
        prompt += "Each resident gains from all others and the creative space allows ";
        prompt += "each to master their own entelechy.";
        return prompt;
    }
    
    double getPacingForResident(const std::string& resident) const {
        if (pacing_.count(resident)) {
            return pacing_.at(resident).response_delay_multiplier;
        }
        return 1.0;
    }
};

} // namespace cogvillage::bridge
