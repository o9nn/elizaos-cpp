// the_org.cpp - Organization Management System Implementation
// Provides multi-tenant organization support with SQLite database

#include "elizaos/core.hpp"
#include <string>
#include <vector>
#include <unordered_map>
#include <mutex>
#include <chrono>
#include <random>
#include <sstream>
#include <sqlite3.h>

namespace elizaos {
namespace the_org {

// ==============================================================================
// UTILITY FUNCTIONS
// ==============================================================================

namespace {

// Get current Unix timestamp
int64_t getCurrentTimestamp() {
    return std::chrono::duration_cast<std::chrono::seconds>(
        std::chrono::system_clock::now().time_since_epoch()
    ).count();
}

// Generate unique organization ID
std::string generateOrgId() {
    static int counter = 0;
    std::ostringstream oss;
    oss << "ORG_" << std::hex << getCurrentTimestamp() << "_" << (++counter);
    return oss.str();
}

} // anonymous namespace

// ==============================================================================
// DATA STRUCTURES
// ==============================================================================

struct Organization {
    std::string orgId;
    std::string name;
    std::string displayName;
    std::string description;
    std::string ownerId;
    std::string status;          // active, suspended, deleted
    std::string planType;        // free, pro, enterprise
    int maxMembers;
    int64_t createdAt;
    int64_t updatedAt;
};

struct OrgMember {
    std::string orgId;
    std::string userId;
    std::string role;            // owner, admin, member, viewer
    std::string status;          // active, invited, suspended
    std::string invitedBy;
    int64_t joinedAt;
};

struct OrgPermission {
    std::string orgId;
    std::string userId;
    std::string permission;
    std::string resourceType;
    std::string resourceId;
};

// ==============================================================================
// DATABASE MANAGER
// ==============================================================================

class OrgDatabase {
private:
    sqlite3* db_;
    std::mutex dbMutex_;
    std::string dbPath_;
    
public:
    OrgDatabase(const std::string& dbPath = "/var/lib/elizaos/organizations.db") 
        : db_(nullptr), dbPath_(dbPath) {
    }
    
    ~OrgDatabase() {
        close();
    }
    
    bool open() {
        std::lock_guard<std::mutex> lock(dbMutex_);
        
        int rc = sqlite3_open(dbPath_.c_str(), &db_);
        if (rc != SQLITE_OK) {
            return false;
        }
        
        // Enable WAL mode
        sqlite3_exec(db_, "PRAGMA journal_mode=WAL;", nullptr, nullptr, nullptr);
        sqlite3_exec(db_, "PRAGMA synchronous=NORMAL;", nullptr, nullptr, nullptr);
        sqlite3_exec(db_, "PRAGMA foreign_keys=ON;", nullptr, nullptr, nullptr);
        
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
            CREATE TABLE IF NOT EXISTS organizations (
                org_id TEXT PRIMARY KEY,
                name TEXT NOT NULL,
                display_name TEXT,
                description TEXT,
                owner_id TEXT NOT NULL,
                status TEXT DEFAULT 'active',
                plan_type TEXT DEFAULT 'free',
                max_members INTEGER DEFAULT 10,
                created_at INTEGER NOT NULL DEFAULT (strftime('%s', 'now')),
                updated_at INTEGER NOT NULL DEFAULT (strftime('%s', 'now')),
                deleted_at INTEGER
            );
            
            CREATE INDEX IF NOT EXISTS idx_orgs_owner ON organizations(owner_id);
            CREATE INDEX IF NOT EXISTS idx_orgs_status ON organizations(status);
            
            CREATE TABLE IF NOT EXISTS org_members (
                id INTEGER PRIMARY KEY AUTOINCREMENT,
                org_id TEXT NOT NULL,
                user_id TEXT NOT NULL,
                role TEXT NOT NULL,
                status TEXT DEFAULT 'active',
                invited_by TEXT,
                joined_at INTEGER NOT NULL DEFAULT (strftime('%s', 'now')),
                updated_at INTEGER NOT NULL DEFAULT (strftime('%s', 'now')),
                FOREIGN KEY (org_id) REFERENCES organizations(org_id) ON DELETE CASCADE,
                UNIQUE(org_id, user_id)
            );
            
            CREATE INDEX IF NOT EXISTS idx_org_members_org ON org_members(org_id);
            CREATE INDEX IF NOT EXISTS idx_org_members_user ON org_members(user_id);
            
            CREATE TABLE IF NOT EXISTS org_permissions (
                id INTEGER PRIMARY KEY AUTOINCREMENT,
                org_id TEXT NOT NULL,
                user_id TEXT NOT NULL,
                permission TEXT NOT NULL,
                resource_type TEXT,
                resource_id TEXT,
                granted_at INTEGER NOT NULL DEFAULT (strftime('%s', 'now')),
                granted_by TEXT,
                FOREIGN KEY (org_id) REFERENCES organizations(org_id) ON DELETE CASCADE,
                UNIQUE(org_id, user_id, permission, resource_type, resource_id)
            );
            
            CREATE INDEX IF NOT EXISTS idx_org_perms_org ON org_permissions(org_id);
            CREATE INDEX IF NOT EXISTS idx_org_perms_user ON org_permissions(user_id);
            
            CREATE TABLE IF NOT EXISTS org_audit_log (
                id INTEGER PRIMARY KEY AUTOINCREMENT,
                org_id TEXT NOT NULL,
                user_id TEXT,
                action TEXT NOT NULL,
                resource_type TEXT,
                resource_id TEXT,
                old_value TEXT,
                new_value TEXT,
                ip_address TEXT,
                user_agent TEXT,
                created_at INTEGER NOT NULL DEFAULT (strftime('%s', 'now')),
                FOREIGN KEY (org_id) REFERENCES organizations(org_id) ON DELETE CASCADE
            );
            
            CREATE INDEX IF NOT EXISTS idx_org_audit_org ON org_audit_log(org_id);
            CREATE INDEX IF NOT EXISTS idx_org_audit_time ON org_audit_log(created_at);
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
    
    bool insertOrganization(const Organization& org) {
        std::lock_guard<std::mutex> lock(dbMutex_);
        
        const char* sql = R"(
            INSERT INTO organizations 
            (org_id, name, display_name, description, owner_id, status, plan_type, max_members)
            VALUES (?, ?, ?, ?, ?, ?, ?, ?)
        )";
        
        sqlite3_stmt* stmt;
        int rc = sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr);
        if (rc != SQLITE_OK) return false;
        
        sqlite3_bind_text(stmt, 1, org.orgId.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, 2, org.name.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, 3, org.displayName.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, 4, org.description.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, 5, org.ownerId.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, 6, org.status.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, 7, org.planType.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_int(stmt, 8, org.maxMembers);
        
        rc = sqlite3_step(stmt);
        sqlite3_finalize(stmt);
        
        return rc == SQLITE_DONE;
    }
    
    Organization getOrganization(const std::string& orgId) {
        std::lock_guard<std::mutex> lock(dbMutex_);
        
        const char* sql = "SELECT * FROM organizations WHERE org_id = ? AND status != 'deleted'";
        
        sqlite3_stmt* stmt;
        Organization org;
        
        int rc = sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr);
        if (rc != SQLITE_OK) return org;
        
        sqlite3_bind_text(stmt, 1, orgId.c_str(), -1, SQLITE_TRANSIENT);
        
        if (sqlite3_step(stmt) == SQLITE_ROW) {
            org.orgId = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
            org.name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
            
            const char* displayName = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
            if (displayName) org.displayName = displayName;
            
            const char* description = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
            if (description) org.description = description;
            
            org.ownerId = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4));
            org.status = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 5));
            org.planType = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 6));
            org.maxMembers = sqlite3_column_int(stmt, 7);
            org.createdAt = sqlite3_column_int64(stmt, 8);
            org.updatedAt = sqlite3_column_int64(stmt, 9);
        }
        
        sqlite3_finalize(stmt);
        return org;
    }
    
    bool insertMember(const OrgMember& member) {
        std::lock_guard<std::mutex> lock(dbMutex_);
        
        const char* sql = R"(
            INSERT INTO org_members (org_id, user_id, role, status, invited_by)
            VALUES (?, ?, ?, ?, ?)
        )";
        
        sqlite3_stmt* stmt;
        int rc = sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr);
        if (rc != SQLITE_OK) return false;
        
        sqlite3_bind_text(stmt, 1, member.orgId.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, 2, member.userId.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, 3, member.role.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, 4, member.status.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, 5, member.invitedBy.c_str(), -1, SQLITE_TRANSIENT);
        
        rc = sqlite3_step(stmt);
        sqlite3_finalize(stmt);
        
        return rc == SQLITE_DONE;
    }
    
    std::vector<OrgMember> getMembers(const std::string& orgId) {
        std::lock_guard<std::mutex> lock(dbMutex_);
        
        std::vector<OrgMember> members;
        const char* sql = "SELECT * FROM org_members WHERE org_id = ? AND status = 'active'";
        
        sqlite3_stmt* stmt;
        int rc = sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr);
        if (rc != SQLITE_OK) return members;
        
        sqlite3_bind_text(stmt, 1, orgId.c_str(), -1, SQLITE_TRANSIENT);
        
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            OrgMember member;
            member.orgId = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
            member.userId = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
            member.role = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
            member.status = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4));
            
            const char* invitedBy = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 5));
            if (invitedBy) member.invitedBy = invitedBy;
            
            member.joinedAt = sqlite3_column_int64(stmt, 6);
            members.push_back(member);
        }
        
        sqlite3_finalize(stmt);
        return members;
    }
    
    bool removeMember(const std::string& orgId, const std::string& userId) {
        std::lock_guard<std::mutex> lock(dbMutex_);
        
        const char* sql = "DELETE FROM org_members WHERE org_id = ? AND user_id = ?";
        
        sqlite3_stmt* stmt;
        int rc = sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr);
        if (rc != SQLITE_OK) return false;
        
        sqlite3_bind_text(stmt, 1, orgId.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, 2, userId.c_str(), -1, SQLITE_TRANSIENT);
        
        rc = sqlite3_step(stmt);
        sqlite3_finalize(stmt);
        
        return rc == SQLITE_DONE;
    }
    
    OrgMember getMember(const std::string& orgId, const std::string& userId) {
        std::lock_guard<std::mutex> lock(dbMutex_);
        
        const char* sql = "SELECT * FROM org_members WHERE org_id = ? AND user_id = ?";
        
        sqlite3_stmt* stmt;
        OrgMember member;
        
        int rc = sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr);
        if (rc != SQLITE_OK) return member;
        
        sqlite3_bind_text(stmt, 1, orgId.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, 2, userId.c_str(), -1, SQLITE_TRANSIENT);
        
        if (sqlite3_step(stmt) == SQLITE_ROW) {
            member.orgId = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
            member.userId = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
            member.role = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
            member.status = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4));
            member.joinedAt = sqlite3_column_int64(stmt, 6);
        }
        
        sqlite3_finalize(stmt);
        return member;
    }
    
    std::vector<OrgPermission> getPermissions(const std::string& orgId, 
                                             const std::string& userId) {
        std::lock_guard<std::mutex> lock(dbMutex_);
        
        std::vector<OrgPermission> permissions;
        const char* sql = "SELECT * FROM org_permissions WHERE org_id = ? AND user_id = ?";
        
        sqlite3_stmt* stmt;
        int rc = sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr);
        if (rc != SQLITE_OK) return permissions;
        
        sqlite3_bind_text(stmt, 1, orgId.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, 2, userId.c_str(), -1, SQLITE_TRANSIENT);
        
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            OrgPermission perm;
            perm.orgId = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
            perm.userId = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
            perm.permission = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
            
            const char* resourceType = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4));
            if (resourceType) perm.resourceType = resourceType;
            
            const char* resourceId = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 5));
            if (resourceId) perm.resourceId = resourceId;
            
            permissions.push_back(perm);
        }
        
        sqlite3_finalize(stmt);
        return permissions;
    }
};

// ==============================================================================
// ORGANIZATION MANAGER
// ==============================================================================

class OrganizationManager {
private:
    OrgDatabase db_;
    std::mutex mutex_;
    
    std::vector<std::string> getRolePermissions(const std::string& role) {
        if (role == "owner") {
            return {"all"};
        } else if (role == "admin") {
            return {"read", "write", "delete", "invite", "manage_members"};
        } else if (role == "member") {
            return {"read", "write"};
        } else if (role == "viewer") {
            return {"read"};
        }
        return {};
    }
    
public:
    OrganizationManager(const std::string& dbPath = "/var/lib/elizaos/organizations.db")
        : db_(dbPath) {
    }
    
    bool initialize() {
        return db_.open();
    }
    
    std::string createOrganization(const std::string& name, 
                                   const std::string& displayName,
                                   const std::string& description,
                                   const std::string& ownerId) {
        std::lock_guard<std::mutex> lock(mutex_);
        
        std::string orgId = generateOrgId();
        int64_t now = getCurrentTimestamp();
        
        Organization org{
            orgId,
            name,
            displayName,
            description,
            ownerId,
            "active",
            "free",
            10,
            now,
            now
        };
        
        if (!db_.insertOrganization(org)) {
            return "";
        }
        
        // Add owner as first member
        OrgMember owner{
            orgId,
            ownerId,
            "owner",
            "active",
            "",
            now
        };
        
        db_.insertMember(owner);
        return orgId;
    }
    
    Organization getOrganization(const std::string& orgId) {
        return db_.getOrganization(orgId);
    }
    
    bool addMember(const std::string& orgId, const std::string& userId, 
                   const std::string& role, const std::string& invitedBy) {
        std::lock_guard<std::mutex> lock(mutex_);
        
        int64_t now = getCurrentTimestamp();
        
        OrgMember member{
            orgId,
            userId,
            role,
            "active",
            invitedBy,
            now
        };
        
        return db_.insertMember(member);
    }
    
    bool removeMember(const std::string& orgId, const std::string& userId) {
        std::lock_guard<std::mutex> lock(mutex_);
        return db_.removeMember(orgId, userId);
    }
    
    std::vector<OrgMember> getMembers(const std::string& orgId) {
        return db_.getMembers(orgId);
    }
    
    bool hasPermission(const std::string& orgId, const std::string& userId,
                      const std::string& permission) {
        // Get member info
        OrgMember member = db_.getMember(orgId, userId);
        
        if (member.orgId.empty() || member.status != "active") {
            return false;
        }
        
        // Check role-based permissions
        auto rolePerms = getRolePermissions(member.role);
        for (const auto& perm : rolePerms) {
            if (perm == "all" || perm == permission) {
                return true;
            }
        }
        
        // Check explicit permissions
        auto userPerms = db_.getPermissions(orgId, userId);
        for (const auto& perm : userPerms) {
            if (perm.permission == "all" || perm.permission == permission) {
                return true;
            }
        }
        
        return false;
    }
};

// ==============================================================================
// GLOBAL INSTANCE
// ==============================================================================

static OrganizationManager* globalOrgManager = nullptr;
static std::mutex globalMutex;

OrganizationManager& getGlobalOrgManager() {
    std::lock_guard<std::mutex> lock(globalMutex);
    if (!globalOrgManager) {
        globalOrgManager = new OrganizationManager();
        globalOrgManager->initialize();
    }
    return *globalOrgManager;
}

// ==============================================================================
// EXPORTED API
// ==============================================================================

bool initializeOrgSystem(const std::string& dbPath) {
    std::lock_guard<std::mutex> lock(globalMutex);
    if (globalOrgManager) {
        delete globalOrgManager;
    }
    globalOrgManager = new OrganizationManager(dbPath);
    return globalOrgManager->initialize();
}

std::string createOrganization(const std::string& name, const std::string& displayName,
                              const std::string& description, const std::string& ownerId) {
    return getGlobalOrgManager().createOrganization(name, displayName, description, ownerId);
}

bool addOrgMember(const std::string& orgId, const std::string& userId, 
                  const std::string& role, const std::string& invitedBy) {
    return getGlobalOrgManager().addMember(orgId, userId, role, invitedBy);
}

bool removeOrgMember(const std::string& orgId, const std::string& userId) {
    return getGlobalOrgManager().removeMember(orgId, userId);
}

Organization getOrganization(const std::string& orgId) {
    return getGlobalOrgManager().getOrganization(orgId);
}

std::vector<OrgMember> getOrgMembers(const std::string& orgId) {
    return getGlobalOrgManager().getMembers(orgId);
}

bool checkOrgPermission(const std::string& orgId, const std::string& userId,
                       const std::string& permission) {
    return getGlobalOrgManager().hasPermission(orgId, userId, permission);
}

} // namespace the_org
} // namespace elizaos
