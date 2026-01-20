               << "```bash\n"
               << "mkdir build && cd build\n"
               << "cmake ..\n"
               << "make\n"
               << "```\n\n"
               << "## Usage\n\n"
               << "Load this plugin in your ElizaOS application.\n";
        
        std::ofstream readmeFile(pluginPath + "/README.md");
        readmeFile << readme.str();
        readmeFile.close();
        
        return true;
    }
};

bool generatePluginTemplate(const std::string& pluginPath, const std::string& pluginName) {
    return PluginTemplateGenerator::generatePlugin(pluginPath, pluginName);
}

} // namespace plugin_starter
} // namespace elizaos

// ==============================================================================
// HAT PROTOCOL
// ==============================================================================
// File: cpp/hat/src/hat_COMPLETE.cpp

#include "elizaos/core.hpp"
#include <string>
#include <vector>
#include <unordered_map>
#include <mutex>

namespace elizaos {
namespace hat {

// HAT (Human-Agent Token) Protocol implementation
// Provides token-based authentication and authorization for agent interactions

struct HATToken {
    std::string tokenId;
    std::string agentId;
    std::string userId;
    std::vector<std::string> permissions;
    std::chrono::system_clock::time_point issuedAt;
    std::chrono::system_clock::time_point expiresAt;
    bool isActive;
};

class HATProtocol {
private:
    std::unordered_map<std::string, HATToken> tokens_;
    std::mutex mutex_;
    
public:
    std::string issueToken(const std::string& agentId, const std::string& userId, 
                          const std::vector<std::string>& permissions) {
        std::lock_guard<std::mutex> lock(mutex_);
        
        std::string tokenId = generateTokenId();
        auto now = std::chrono::system_clock::now();
        auto expiry = now + std::chrono::hours(24);  // 24 hour expiry
        
        HATToken token{
            tokenId,
            agentId,
            userId,
            permissions,
            now,
            expiry,
            true
        };
        
        tokens_[tokenId] = token;
        return tokenId;
    }
    
    bool validateToken(const std::string& tokenId) {
        std::lock_guard<std::mutex> lock(mutex_);
        
        auto it = tokens_.find(tokenId);
        if (it == tokens_.end()) {
            return false;
        }
        
        auto& token = it->second;
        auto now = std::chrono::system_clock::now();
        
        return token.isActive && token.expiresAt > now;
    }
    
    bool hasPermission(const std::string& tokenId, const std::string& permission) {
        std::lock_guard<std::mutex> lock(mutex_);
        
        auto it = tokens_.find(tokenId);
        if (it == tokens_.end()) {
            return false;
        }
        
        const auto& perms = it->second.permissions;
        return std::find(perms.begin(), perms.end(), permission) != perms.end();
    }
    
    void revokeToken(const std::string& tokenId) {
        std::lock_guard<std::mutex> lock(mutex_);
        
        auto it = tokens_.find(tokenId);
        if (it != tokens_.end()) {
            it->second.isActive = false;
        }
    }
    
    HATToken getToken(const std::string& tokenId) {
        std::lock_guard<std::mutex> lock(mutex_);
        
        auto it = tokens_.find(tokenId);
        return (it != tokens_.end()) ? it->second : HATToken{};
    }
    
private:
    std::string generateTokenId() {
        // Simple token ID generation (in production, use cryptographic random)
        static int counter = 0;
        return "HAT_" + std::to_string(++counter) + "_" + 
               std::to_string(std::chrono::system_clock::now().time_since_epoch().count());
    }
};

// Global HAT protocol instance
static HATProtocol globalHATProtocol;

// Exported API
std::string issueHATToken(const std::string& agentId, const std::string& userId, 
                         const std::vector<std::string>& permissions) {
    return globalHATProtocol.issueToken(agentId, userId, permissions);
}

bool validateHATToken(const std::string& tokenId) {
    return globalHATProtocol.validateToken(tokenId);
}

bool checkHATPermission(const std::string& tokenId, const std::string& permission) {
    return globalHATProtocol.hasPermission(tokenId, permission);
}

void revokeHATToken(const std::string& tokenId) {
    globalHATProtocol.revokeToken(tokenId);
}
