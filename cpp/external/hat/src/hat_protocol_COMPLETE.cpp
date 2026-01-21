// hat_protocol.cpp - Human-Agent Token (HAT) Protocol Implementation
// Provides secure token-based authentication with SQLite and OpenSSL

#include "elizaos/core.hpp"
#include <string>
#include <vector>
#include <unordered_map>
#include <mutex>
#include <chrono>
#include <random>
#include <sstream>
#include <iomanip>
#include <sqlite3.h>
#include <openssl/sha.h>
#include <openssl/rand.h>

namespace elizaos {
namespace hat {

// ==============================================================================
// UTILITY FUNCTIONS
// ==============================================================================

namespace {

// Generate random bytes
std::string generateRandomBytes(size_t length) {
    std::vector<unsigned char> buffer(length);
    if (RAND_bytes(buffer.data(), length) != 1) {
        // Fallback to std::random if OpenSSL fails
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(0, 255);
        for (size_t i = 0; i < length; ++i) {
            buffer[i] = static_cast<unsigned char>(dis(gen));
        }
    }
    
    std::ostringstream oss;
    for (unsigned char byte : buffer) {
        oss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(byte);
    }
    return oss.str();
}

// Generate SHA-256 hash
std::string sha256(const std::string& input, const std::string& salt) {
    std::string salted = input + salt;
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256(reinterpret_cast<const unsigned char*>(salted.c_str()), 
           salted.length(), hash);
    
    std::ostringstream oss;
    for (int i = 0; i < SHA256_DIGEST_LENGTH; ++i) {
        oss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(hash[i]);
    }
    return oss.str();
}

// Get current Unix timestamp
int64_t getCurrentTimestamp() {
    return std::chrono::duration_cast<std::chrono::seconds>(
        std::chrono::system_clock::now().time_since_epoch()
    ).count();
}

// Generate unique token ID
std::string generateTokenId() {
    return "HAT_" + generateRandomBytes(16);
}

} // anonymous namespace

// ==============================================================================
// DATA STRUCTURES
// ==============================================================================

struct HATToken {
    std::string tokenId;
    std::string agentId;
    std::string userId;
    std::string tokenHash;
    std::string salt;
    int64_t issuedAt;
    int64_t expiresAt;
    int64_t lastUsedAt;
    bool isActive;
    std::vector<std::string> permissions;
    std::string metadata;
};

struct TokenValidationResult {
    bool isValid;
    std::string tokenId;
    std::string userId;
    std::string agentId;
    std::vector<std::string> permissions;
    std::string errorMessage;
};

// ==============================================================================
// DATABASE MANAGER
// ==============================================================================

class HATDatabase {
private:
    sqlite3* db_;
    std::mutex dbMutex_;
    std::string dbPath_;
    
public:
    HATDatabase(const std::string& dbPath = "/var/lib/elizaos/hat_protocol.db") 
        : db_(nullptr), dbPath_(dbPath) {
    }
    
    ~HATDatabase() {
        close();
    }
    
    bool open() {
        std::lock_guard<std::mutex> lock(dbMutex_);
        
        int rc = sqlite3_open(dbPath_.c_str(), &db_);
        if (rc != SQLITE_OK) {
            return false;
        }
        
        // Enable WAL mode for better concurrency
        sqlite3_exec(db_, "PRAGMA journal_mode=WAL;", nullptr, nullptr, nullptr);
        sqlite3_exec(db_, "PRAGMA synchronous=NORMAL;", nullptr, nullptr, nullptr);
        
        return initializeSchema();
    }
    
    void close() {
        std::lock_guard<std::mutex> lock(dbMutex_);
        if (db_) {
            sqlite3_close(db_);
            db_ = nullptr;
        }
    }
    
    bool initializeSchema() {
        const char* schema = R"(
            CREATE TABLE IF NOT EXISTS hat_tokens (
                token_id TEXT PRIMARY KEY,
                agent_id TEXT NOT NULL,
                user_id TEXT NOT NULL,
                token_hash TEXT NOT NULL,
                salt TEXT NOT NULL,
                issued_at INTEGER NOT NULL,
                expires_at INTEGER NOT NULL,
                last_used_at INTEGER,
                is_active INTEGER DEFAULT 1,
                revoked_at INTEGER,
                revoked_by TEXT,
                revoke_reason TEXT,
                metadata TEXT,
                created_at INTEGER NOT NULL DEFAULT (strftime('%s', 'now'))
            );
            
            CREATE INDEX IF NOT EXISTS idx_hat_tokens_agent ON hat_tokens(agent_id);
            CREATE INDEX IF NOT EXISTS idx_hat_tokens_user ON hat_tokens(user_id);
            CREATE INDEX IF NOT EXISTS idx_hat_tokens_active ON hat_tokens(is_active, expires_at);
            
            CREATE TABLE IF NOT EXISTS hat_permissions (
                id INTEGER PRIMARY KEY AUTOINCREMENT,
                token_id TEXT NOT NULL,
                permission TEXT NOT NULL,
                resource TEXT,
                granted_at INTEGER NOT NULL DEFAULT (strftime('%s', 'now')),
                FOREIGN KEY (token_id) REFERENCES hat_tokens(token_id) ON DELETE CASCADE,
                UNIQUE(token_id, permission, resource)
            );
            
            CREATE INDEX IF NOT EXISTS idx_hat_permissions_token ON hat_permissions(token_id);
            
            CREATE TABLE IF NOT EXISTS hat_token_usage (
                id INTEGER PRIMARY KEY AUTOINCREMENT,
                token_id TEXT NOT NULL,
                used_at INTEGER NOT NULL DEFAULT (strftime('%s', 'now')),
                action TEXT,
                ip_address TEXT,
                user_agent TEXT,
                success INTEGER DEFAULT 1,
                error_message TEXT,
                FOREIGN KEY (token_id) REFERENCES hat_tokens(token_id) ON DELETE CASCADE
            );
            
            CREATE INDEX IF NOT EXISTS idx_hat_usage_token ON hat_token_usage(token_id);
        )";
        
        char* errMsg = nullptr;
        int rc = sqlite3_exec(db_, schema, nullptr, nullptr, &errMsg);
        
        if (rc != SQLITE_OK) {
            if (errMsg) {
                sqlite3_free(errMsg);
            }
            return false;
        }
        
        return true;
    }
    
    bool insertToken(const HATToken& token) {
        std::lock_guard<std::mutex> lock(dbMutex_);
        
        const char* sql = R"(
            INSERT INTO hat_tokens 
            (token_id, agent_id, user_id, token_hash, salt, issued_at, expires_at, 
             is_active, metadata)
            VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?)
        )";
        
        sqlite3_stmt* stmt;
        int rc = sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr);
        if (rc != SQLITE_OK) return false;
        
        sqlite3_bind_text(stmt, 1, token.tokenId.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, 2, token.agentId.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, 3, token.userId.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, 4, token.tokenHash.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, 5, token.salt.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_int64(stmt, 6, token.issuedAt);
        sqlite3_bind_int64(stmt, 7, token.expiresAt);
        sqlite3_bind_int(stmt, 8, token.isActive ? 1 : 0);
        sqlite3_bind_text(stmt, 9, token.metadata.c_str(), -1, SQLITE_TRANSIENT);
        
        rc = sqlite3_step(stmt);
        sqlite3_finalize(stmt);
        
        if (rc != SQLITE_DONE) return false;
        
        // Insert permissions
        for (const auto& perm : token.permissions) {
            if (!insertPermission(token.tokenId, perm)) {
                return false;
            }
        }
        
        return true;
    }
    
    bool insertPermission(const std::string& tokenId, const std::string& permission) {
        const char* sql = "INSERT INTO hat_permissions (token_id, permission) VALUES (?, ?)";
        
        sqlite3_stmt* stmt;
        int rc = sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr);
        if (rc != SQLITE_OK) return false;
        
        sqlite3_bind_text(stmt, 1, tokenId.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, 2, permission.c_str(), -1, SQLITE_TRANSIENT);
        
        rc = sqlite3_step(stmt);
        sqlite3_finalize(stmt);
        
        return rc == SQLITE_DONE;
    }
    
    HATToken getToken(const std::string& tokenId) {
        std::lock_guard<std::mutex> lock(dbMutex_);
        
        const char* sql = "SELECT * FROM hat_tokens WHERE token_id = ?";
        
        sqlite3_stmt* stmt;
        HATToken token;
        
        int rc = sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr);
        if (rc != SQLITE_OK) return token;
        
        sqlite3_bind_text(stmt, 1, tokenId.c_str(), -1, SQLITE_TRANSIENT);
        
        if (sqlite3_step(stmt) == SQLITE_ROW) {
            token.tokenId = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
            token.agentId = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
            token.userId = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
            token.tokenHash = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
            token.salt = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4));
            token.issuedAt = sqlite3_column_int64(stmt, 5);
            token.expiresAt = sqlite3_column_int64(stmt, 6);
            token.lastUsedAt = sqlite3_column_int64(stmt, 7);
            token.isActive = sqlite3_column_int(stmt, 8) == 1;
            
            const char* metadata = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 12));
            if (metadata) token.metadata = metadata;
        }
        
        sqlite3_finalize(stmt);
        
        // Load permissions
        if (!token.tokenId.empty()) {
            token.permissions = getPermissions(tokenId);
        }
        
        return token;
    }
    
    std::vector<std::string> getPermissions(const std::string& tokenId) {
        std::vector<std::string> permissions;
        
        const char* sql = "SELECT permission FROM hat_permissions WHERE token_id = ?";
        
        sqlite3_stmt* stmt;
        int rc = sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr);
        if (rc != SQLITE_OK) return permissions;
        
        sqlite3_bind_text(stmt, 1, tokenId.c_str(), -1, SQLITE_TRANSIENT);
        
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            permissions.push_back(
                reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0))
            );
        }
        
        sqlite3_finalize(stmt);
        return permissions;
    }
    
    bool revokeToken(const std::string& tokenId, const std::string& revokedBy, 
                     const std::string& reason) {
        std::lock_guard<std::mutex> lock(dbMutex_);
        
        const char* sql = R"(
            UPDATE hat_tokens 
            SET is_active = 0, revoked_at = ?, revoked_by = ?, revoke_reason = ?
            WHERE token_id = ?
        )";
        
        sqlite3_stmt* stmt;
        int rc = sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr);
        if (rc != SQLITE_OK) return false;
        
        int64_t now = getCurrentTimestamp();
        sqlite3_bind_int64(stmt, 1, now);
        sqlite3_bind_text(stmt, 2, revokedBy.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, 3, reason.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, 4, tokenId.c_str(), -1, SQLITE_TRANSIENT);
        
        rc = sqlite3_step(stmt);
        sqlite3_finalize(stmt);
        
        return rc == SQLITE_DONE;
    }
    
    bool updateLastUsed(const std::string& tokenId) {
        std::lock_guard<std::mutex> lock(dbMutex_);
        
        const char* sql = "UPDATE hat_tokens SET last_used_at = ? WHERE token_id = ?";
        
        sqlite3_stmt* stmt;
        int rc = sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr);
        if (rc != SQLITE_OK) return false;
        
        int64_t now = getCurrentTimestamp();
        sqlite3_bind_int64(stmt, 1, now);
        sqlite3_bind_text(stmt, 2, tokenId.c_str(), -1, SQLITE_TRANSIENT);
        
        rc = sqlite3_step(stmt);
        sqlite3_finalize(stmt);
        
        return rc == SQLITE_DONE;
    }
    
    bool logUsage(const std::string& tokenId, const std::string& action, 
                  bool success, const std::string& errorMessage = "") {
        std::lock_guard<std::mutex> lock(dbMutex_);
        
        const char* sql = R"(
            INSERT INTO hat_token_usage (token_id, action, success, error_message)
            VALUES (?, ?, ?, ?)
        )";
        
        sqlite3_stmt* stmt;
        int rc = sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr);
        if (rc != SQLITE_OK) return false;
        
        sqlite3_bind_text(stmt, 1, tokenId.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, 2, action.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_int(stmt, 3, success ? 1 : 0);
        sqlite3_bind_text(stmt, 4, errorMessage.c_str(), -1, SQLITE_TRANSIENT);
        
        rc = sqlite3_step(stmt);
        sqlite3_finalize(stmt);
        
        return rc == SQLITE_DONE;
    }
    
    int cleanupExpiredTokens() {
        std::lock_guard<std::mutex> lock(dbMutex_);
        
        const char* sql = "DELETE FROM hat_tokens WHERE expires_at < ? AND is_active = 0";
        
        sqlite3_stmt* stmt;
        int rc = sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr);
        if (rc != SQLITE_OK) return 0;
        
        int64_t now = getCurrentTimestamp();
        sqlite3_bind_int64(stmt, 1, now);
        
        sqlite3_step(stmt);
        int deleted = sqlite3_changes(db_);
        sqlite3_finalize(stmt);
        
        return deleted;
    }
};

// ==============================================================================
// HAT PROTOCOL MANAGER
// ==============================================================================

class HATProtocolManager {
private:
    HATDatabase db_;
    std::mutex mutex_;
    int64_t defaultExpirationSeconds_;
    
public:
    HATProtocolManager(const std::string& dbPath = "/var/lib/elizaos/hat_protocol.db",
                       int64_t defaultExpiration = 86400) // 24 hours default
        : db_(dbPath), defaultExpirationSeconds_(defaultExpiration) {
    }
    
    bool initialize() {
        return db_.open();
    }
    
    std::pair<std::string, std::string> issueToken(
        const std::string& agentId,
        const std::string& userId,
        const std::vector<std::string>& permissions,
        int64_t expirationSeconds = 0,
        const std::string& metadata = "") {
        
        std::lock_guard<std::mutex> lock(mutex_);
        
        // Generate token components
        std::string tokenId = generateTokenId();
        std::string rawToken = generateRandomBytes(32);
        std::string salt = generateRandomBytes(16);
        std::string tokenHash = sha256(rawToken, salt);
        
        // Calculate expiration
        int64_t now = getCurrentTimestamp();
        int64_t expiration = expirationSeconds > 0 ? expirationSeconds : defaultExpirationSeconds_;
        int64_t expiresAt = now + expiration;
        
        // Create token object
        HATToken token{
            tokenId,
            agentId,
            userId,
            tokenHash,
            salt,
            now,
            expiresAt,
            0,
            true,
            permissions,
            metadata
        };
        
        // Store in database
        if (!db_.insertToken(token)) {
            return {"", ""};
        }
        
        // Return token ID and raw token (only time raw token is available)
        return {tokenId, rawToken};
    }
    
    TokenValidationResult validateToken(const std::string& tokenId, 
                                        const std::string& rawToken) {
        std::lock_guard<std::mutex> lock(mutex_);
        
        TokenValidationResult result{false, "", "", "", {}, ""};
        
        // Retrieve token from database
        HATToken token = db_.getToken(tokenId);
        
        if (token.tokenId.empty()) {
            result.errorMessage = "Token not found";
            return result;
        }
        
        // Check if active
        if (!token.isActive) {
            result.errorMessage = "Token has been revoked";
            db_.logUsage(tokenId, "validate", false, "Token revoked");
            return result;
        }
        
        // Check expiration
        int64_t now = getCurrentTimestamp();
        if (token.expiresAt < now) {
            result.errorMessage = "Token has expired";
            db_.logUsage(tokenId, "validate", false, "Token expired");
            return result;
        }
        
        // Verify token hash
        std::string computedHash = sha256(rawToken, token.salt);
        if (computedHash != token.tokenHash) {
            result.errorMessage = "Invalid token";
            db_.logUsage(tokenId, "validate", false, "Invalid token hash");
            return result;
        }
        
        // Token is valid
        result.isValid = true;
        result.tokenId = token.tokenId;
        result.userId = token.userId;
        result.agentId = token.agentId;
        result.permissions = token.permissions;
        
        // Update last used timestamp
        db_.updateLastUsed(tokenId);
        db_.logUsage(tokenId, "validate", true);
        
        return result;
    }
    
    bool hasPermission(const std::string& tokenId, const std::string& permission) {
        HATToken token = db_.getToken(tokenId);
        
        if (token.tokenId.empty() || !token.isActive) {
            return false;
        }
        
        // Check expiration
        int64_t now = getCurrentTimestamp();
        if (token.expiresAt < now) {
            return false;
        }
        
        // Check for permission
        for (const auto& perm : token.permissions) {
            if (perm == permission || perm == "all" || perm == "*") {
                return true;
            }
        }
        
        return false;
    }
    
    bool revokeToken(const std::string& tokenId, const std::string& revokedBy,
                     const std::string& reason = "Revoked by user") {
        std::lock_guard<std::mutex> lock(mutex_);
        return db_.revokeToken(tokenId, revokedBy, reason);
    }
    
    std::vector<std::string> getTokenPermissions(const std::string& tokenId) {
        return db_.getPermissions(tokenId);
    }
    
    int cleanupExpiredTokens() {
        return db_.cleanupExpiredTokens();
    }
    
    HATToken getTokenInfo(const std::string& tokenId) {
        return db_.getToken(tokenId);
    }
};

// ==============================================================================
// GLOBAL INSTANCE
// ==============================================================================

static HATProtocolManager* globalHATManager = nullptr;
static std::mutex globalMutex;

HATProtocolManager& getGlobalHATManager() {
    std::lock_guard<std::mutex> lock(globalMutex);
    if (!globalHATManager) {
        globalHATManager = new HATProtocolManager();
        globalHATManager->initialize();
    }
    return *globalHATManager;
}

// ==============================================================================
// EXPORTED API
// ==============================================================================

bool initializeHATProtocol(const std::string& dbPath) {
    std::lock_guard<std::mutex> lock(globalMutex);
    if (globalHATManager) {
        delete globalHATManager;
    }
    globalHATManager = new HATProtocolManager(dbPath);
    return globalHATManager->initialize();
}

std::pair<std::string, std::string> issueHATToken(
    const std::string& agentId,
    const std::string& userId,
    const std::vector<std::string>& permissions,
    int64_t expirationSeconds) {
    
    return getGlobalHATManager().issueToken(agentId, userId, permissions, expirationSeconds);
}

bool validateHATToken(const std::string& tokenId, const std::string& rawToken) {
    TokenValidationResult result = getGlobalHATManager().validateToken(tokenId, rawToken);
    return result.isValid;
}

bool checkHATPermission(const std::string& tokenId, const std::string& permission) {
    return getGlobalHATManager().hasPermission(tokenId, permission);
}

void revokeHATToken(const std::string& tokenId, const std::string& revokedBy, 
                    const std::string& reason) {
    getGlobalHATManager().revokeToken(tokenId, revokedBy, reason);
}

std::vector<std::string> getHATTokenPermissions(const std::string& tokenId) {
    return getGlobalHATManager().getTokenPermissions(tokenId);
}

int cleanupExpiredHATTokens() {
    return getGlobalHATManager().cleanupExpiredTokens();
}

} // namespace hat
} // namespace elizaos
