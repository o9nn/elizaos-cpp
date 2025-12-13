} // namespace hat
} // namespace elizaos

// ==============================================================================
// THE ORG
// ==============================================================================
// File: cpp/the_org/src/the_org_COMPLETE.cpp

#include "elizaos/core.hpp"
#include <string>
#include <vector>
#include <unordered_map>
#include <mutex>

namespace elizaos {
namespace the_org {

// Organization management system for ElizaOS
// Provides multi-tenant organization structure with roles and permissions

struct Organization {
    std::string orgId;
    std::string name;
    std::string description;
    std::vector<std::string> members;
    std::unordered_map<std::string, std::string> settings;
    std::chrono::system_clock::time_point createdAt;
    bool isActive;
};

struct OrgMember {
    std::string userId;
    std::string orgId;
    std::string role;  // owner, admin, member, viewer
    std::vector<std::string> permissions;
    std::chrono::system_clock::time_point joinedAt;
};

class OrganizationManager {
private:
    std::unordered_map<std::string, Organization> organizations_;
    std::unordered_map<std::string, std::vector<OrgMember>> orgMembers_;
    std::mutex mutex_;
    
public:
    std::string createOrganization(const std::string& name, const std::string& description, 
                                   const std::string& ownerId) {
        std::lock_guard<std::mutex> lock(mutex_);
        
        std::string orgId = generateOrgId();
        
        Organization org{
            orgId,
            name,
            description,
            {ownerId},
            {},
            std::chrono::system_clock::now(),
            true
        };
        
        organizations_[orgId] = org;
        
        // Add owner as first member
        OrgMember owner{
            ownerId,
            orgId,
            "owner",
            {"all"},
            std::chrono::system_clock::now()
        };
        
        orgMembers_[orgId].push_back(owner);
        
        return orgId;
    }
    
    bool addMember(const std::string& orgId, const std::string& userId, const std::string& role) {
        std::lock_guard<std::mutex> lock(mutex_);
        
        auto it = organizations_.find(orgId);
        if (it == organizations_.end()) {
            return false;
        }
        
        OrgMember member{
            userId,
            orgId,
            role,
            getRolePermissions(role),
            std::chrono::system_clock::now()
        };
        
        orgMembers_[orgId].push_back(member);
        it->second.members.push_back(userId);
        
        return true;
    }
    
    bool removeMember(const std::string& orgId, const std::string& userId) {
        std::lock_guard<std::mutex> lock(mutex_);
        
        auto& members = orgMembers_[orgId];
        members.erase(
            std::remove_if(members.begin(), members.end(),
                          [&userId](const OrgMember& m) { return m.userId == userId; }),
            members.end()
        );
        
        auto it = organizations_.find(orgId);
        if (it != organizations_.end()) {
            auto& orgMembers = it->second.members;
            orgMembers.erase(
                std::remove(orgMembers.begin(), orgMembers.end(), userId),
                orgMembers.end()
            );
        }
        
        return true;
    }
    
    Organization getOrganization(const std::string& orgId) {
        std::lock_guard<std::mutex> lock(mutex_);
        
        auto it = organizations_.find(orgId);
        return (it != organizations_.end()) ? it->second : Organization{};
    }
    
    std::vector<OrgMember> getMembers(const std::string& orgId) {
        std::lock_guard<std::mutex> lock(mutex_);
        
        auto it = orgMembers_.find(orgId);
        return (it != orgMembers_.end()) ? it->second : std::vector<OrgMember>{};
    }
    
    bool hasPermission(const std::string& orgId, const std::string& userId, 
                      const std::string& permission) {
        std::lock_guard<std::mutex> lock(mutex_);
        
        auto it = orgMembers_.find(orgId);
        if (it == orgMembers_.end()) {
            return false;
        }
        
        for (const auto& member : it->second) {
            if (member.userId == userId) {
                const auto& perms = member.permissions;
                return std::find(perms.begin(), perms.end(), permission) != perms.end() ||
                       std::find(perms.begin(), perms.end(), "all") != perms.end();
            }
        }
        
        return false;
    }
    
    void updateOrgSettings(const std::string& orgId, const std::string& key, 
                          const std::string& value) {
        std::lock_guard<std::mutex> lock(mutex_);
        
        auto it = organizations_.find(orgId);
        if (it != organizations_.end()) {
            it->second.settings[key] = value;
        }
    }
    
private:
    std::string generateOrgId() {
        static int counter = 0;
        return "ORG_" + std::to_string(++counter);
    }
    
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
};

// Global organization manager
static OrganizationManager globalOrgManager;

// Exported API
std::string createOrganization(const std::string& name, const std::string& description, 
                              const std::string& ownerId) {
    return globalOrgManager.createOrganization(name, description, ownerId);
}

bool addOrgMember(const std::string& orgId, const std::string& userId, const std::string& role) {
    return globalOrgManager.addMember(orgId, userId, role);
}

bool removeOrgMember(const std::string& orgId, const std::string& userId) {
    return globalOrgManager.removeMember(orgId, userId);
}

Organization getOrganization(const std::string& orgId) {
    return globalOrgManager.getOrganization(orgId);
}

std::vector<OrgMember> getOrgMembers(const std::string& orgId) {
    return globalOrgManager.getMembers(orgId);
}

bool checkOrgPermission(const std::string& orgId, const std::string& userId, 
                       const std::string& permission) {
    return globalOrgManager.hasPermission(orgId, userId, permission);
}

} // namespace the_org
} // namespace elizaos
