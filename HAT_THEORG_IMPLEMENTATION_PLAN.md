# Implementation Plan: HAT Protocol & The Org

**Author:** Manus AI  
**Date:** December 13, 2024  
**Version:** 1.0

---

## 1. Introduction

This document provides a detailed technical implementation plan for completing the **HAT Protocol** (Human-Agent Token) and **The Org** (Organization Management) modules in the ElizaOS C++ repository. The goal is to replace the existing placeholder implementations with production-ready code featuring full database integration and cryptographic security.

### 1.1. Objectives

- **Complete HAT Protocol:** Implement a secure token-based authentication system using SQLite for storage and OpenSSL for cryptography.
- **Complete The Org:** Implement a multi-tenant organization management system with role-based access control (RBAC) using SQLite.
- **Provide Production-Ready Code:** Deliver high-quality, thread-safe, and well-documented C++ code.
- **Deliver Integration Guide:** Provide clear instructions for integrating the new modules into the existing build system.

### 1.2. Key Deliverables

1.  **Database Schema:** Complete SQLite schema for both modules.
2.  **C++ Implementation:** Fully functional C++ code for `hat_protocol.cpp` and `the_org.cpp`.
3.  **CMakeLists.txt Updates:** Configuration files for integrating dependencies (SQLite, OpenSSL).
4.  **Implementation Plan:** This document.

---

## 2. Database Schema & Architecture

A robust SQLite database schema is the foundation for both modules. The design prioritizes security, performance, and scalability.

### 2.1. Database File

A single database file, `elizaos.db`, will be used to store all tables for simplicity. This file should be located at `/var/lib/elizaos/elizaos.db`.

### 2.2. Performance Optimizations

- **Write-Ahead Logging (WAL):** Enabled for high-concurrency reads and writes.
- **Indexes:** Comprehensive indexing on foreign keys and frequently queried columns.
- **Prepared Statements:** Used for all SQL queries to prevent SQL injection and improve performance.

### 2.3. HAT Protocol Schema

**Purpose:** Manages secure tokens for agent-human interaction.

| Table Name          | Description                                         |
| ------------------- | --------------------------------------------------- |
| `hat_tokens`        | Stores issued tokens, hashes, and metadata.         |
| `hat_permissions`   | Defines permissions associated with each token.     |
| `hat_token_usage`   | Tracks token usage for analytics and security.      |
| `hat_refresh_tokens`| Stores refresh tokens for long-lived sessions.      |

**Full Schema:** See `HAT_THEORG_DATABASE_SCHEMA.md` for the complete SQL script.

### 2.4. The Org Schema

**Purpose:** Manages multi-tenant organizations, members, and roles.

| Table Name          | Description                                         |
| ------------------- | --------------------------------------------------- |
| `organizations`     | Core organization data (name, owner, plan).         |
| `org_members`       | Manages membership and roles (owner, admin, member).|
| `org_permissions`   | Stores fine-grained, user-specific permissions.     |
| `org_invitations`   | Tracks pending invitations to join an organization. |
| `org_settings`      | Stores key-value settings for each organization.    |
| `org_audit_log`     | Provides an audit trail for all organization activities.|

**Full Schema:** See `HAT_THEORG_DATABASE_SCHEMA.md` for the complete SQL script.

---

## 3. C++ Implementation Details

The C++ implementation is designed to be modular, thread-safe, and easy to use.

### 3.1. HAT Protocol Implementation

**File:** `cpp/hat/src/hat_protocol_COMPLETE.cpp`

**Key Classes:**

- `HATDatabase`: Manages all SQLite database interactions for the HAT protocol. Handles connection management, schema initialization, and all CRUD operations.
- `HATProtocolManager`: The main service class that orchestrates token issuance, validation, and revocation. It uses `HATDatabase` for persistence.

**Core Functionality:**

1.  **Token Issuance (`issueToken`):**
    - Generates a unique token ID (`HAT_...`).
    - Generates a 32-byte cryptographically secure raw token (using OpenSSL `RAND_bytes`).
    - Generates a 16-byte random salt.
    - Hashes the raw token with the salt using SHA-256.
    - Stores the `token_hash` and `salt` in the `hat_tokens` table (never the raw token).
    - Returns the `tokenId` and the `rawToken` to the caller. The `rawToken` must be handled securely and is never stored.

2.  **Token Validation (`validateToken`):**
    - Retrieves the token record from the database by `tokenId`.
    - Verifies the token is active and not expired.
    - Re-computes the SHA-256 hash of the provided `rawToken` and the stored `salt`.
    - Compares the computed hash with the stored `token_hash`.
    - Returns a `TokenValidationResult` with the outcome.

3.  **Permission Check (`hasPermission`):**
    - Validates the token is active and not expired.
    - Checks for the requested permission in the `hat_permissions` table or for a wildcard (`*` or `all`) permission.

**Security:**

- **No Plaintext Storage:** Raw tokens are never stored, only their SHA-256 hashes.
- **Salting:** Each token hash is salted to prevent rainbow table attacks.
- **Secure Randomness:** OpenSSL's `RAND_bytes` is used for generating tokens and salts.

### 3.2. The Org Implementation

**File:** `cpp/the_org/src/the_org_COMPLETE.cpp`

**Key Classes:**

- `OrgDatabase`: Manages all SQLite database interactions for The Org module.
- `OrganizationManager`: The main service class for managing organizations, members, and permissions.

**Core Functionality:**

1.  **Organization Creation (`createOrganization`):**
    - Generates a unique `orgId` (`ORG_...`).
    - Inserts a new record into the `organizations` table.
    - Automatically adds the `ownerId` as the first member with the `owner` role.
    - Creates an audit log entry for the creation event.

2.  **Member Management (`addMember`, `removeMember`):**
    - Adds or removes users from the `org_members` table.
    - When adding, automatically grants a set of default permissions based on the assigned role.
    - Logs all membership changes to the `org_audit_log`.

3.  **Permission Model (`hasPermission`):**
    - Implements a hierarchical RBAC model.
    - A user's permissions are a union of their role-based permissions and any explicitly granted permissions.
    - The `owner` role implicitly has all permissions (`all`).

---

## 4. Build System Integration (CMake)

To integrate the new modules, the `CMakeLists.txt` files for each module and the main project must be updated to find and link the required dependencies.

### 4.1. Dependencies

- **SQLite3:** Required by both modules for database storage.
- **OpenSSL:** Required by the HAT Protocol for cryptographic functions (`SHA256`, `RAND_bytes`).

### 4.2. `cpp/hat/CMakeLists.txt`

```cmake
# Find required dependencies
find_package(PkgConfig REQUIRED)
find_package(OpenSSL REQUIRED)
find_package(SQLite3 REQUIRED)

# Create the library
add_library(elizaos-hat STATIC
    src/hat_protocol.cpp
)

# Link dependencies
target_link_libraries(elizaos-hat PRIVATE
    OpenSSL::SSL
    OpenSSL::Crypto
    SQLite::SQLite3
)

# Include directories
target_include_directories(elizaos-hat PUBLIC
    $<INSTALL_INTERFACE:include>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

# Installation rules
install(TARGETS elizaos-hat DESTINATION lib)
install(FILES include/elizaos/hat.hpp DESTINATION include/elizaos)
```

### 4.3. `cpp/the_org/CMakeLists.txt`

```cmake
# Find required dependencies
find_package(PkgConfig REQUIRED)
find_package(SQLite3 REQUIRED)

# Create the library
add_library(elizaos-the_org STATIC
    src/the_org.cpp
)

# Link dependencies
target_link_libraries(elizaos-the_org PRIVATE
    SQLite::SQLite3
)

# Include directories
target_include_directories(elizaos-the_org PUBLIC
    $<INSTALL_INTERFACE:include>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

# Installation rules
install(TARGETS elizaos-the_org DESTINATION lib)
install(FILES include/elizaos/the_org.hpp DESTINATION include/elizaos)
```

### 4.4. Main `CMakeLists.txt`

Ensure the main project file can locate the dependencies. Add the following lines near the top:

```cmake
find_package(OpenSSL)
find_package(SQLite3)
```

---

## 5. Integration & Usage Guide

### 5.1. File Replacement

1.  Replace `cpp/hat/src/placeholder.cpp` with `cpp/hat/src/hat_protocol_COMPLETE.cpp` and rename it to `hat_protocol.cpp`.
2.  Replace `cpp/the_org/src/placeholder.cpp` with `cpp/the_org/src/the_org_COMPLETE.cpp` and rename it to `the_org.cpp`.
3.  Update the respective `CMakeLists.txt` files as described above.

### 5.2. Initialization

Before using the modules, initialize them once at application startup:

```cpp
#include "elizaos/hat.hpp"
#include "elizaos/the_org.hpp"

int main() {
    // Initialize with the path to the database file
    elizaos::hat::initializeHATProtocol("/var/lib/elizaos/elizaos.db");
    elizaos::the_org::initializeOrgSystem("/var/lib/elizaos/elizaos.db");

    // ... application logic ...

    return 0;
}
```

### 5.3. Example Usage

**HAT Protocol:**

```cpp
// 1. Issue a new token
auto tokenPair = elizaos::hat::issueHATToken(
    "agent_007", 
    "user_james_bond", 
    {"read", "write"}, 
    3600 // Expires in 1 hour
);

std::string tokenId = tokenPair.first;
std::string rawToken = tokenPair.second;

// 2. Validate the token
bool isValid = elizaos::hat::validateHATToken(tokenId, rawToken);

// 3. Check permissions
if (isValid && elizaos::hat::checkHATPermission(tokenId, "write")) {
    // Perform write operation
}

// 4. Revoke the token
elizaos::hat::revokeHATToken(tokenId, "user_james_bond", "Mission complete");
```

**The Org:**

```cpp
// 1. Create a new organization
std::string orgId = elizaos::the_org::createOrganization(
    "mi6", 
    "MI6", 
    "Secret Intelligence Service", 
    "user_m"
);

// 2. Add a member
elizaos::the_org::addOrgMember(orgId, "user_james_bond", "member", "user_m");

// 3. Check permissions
if (elizaos::the_org::checkOrgPermission(orgId, "user_james_bond", "read")) {
    // User can read resources
}

// 4. Get organization details
auto org = elizaos::the_org::getOrganization(orgId);
auto members = elizaos::the_org::getOrgMembers(orgId);
```

---

## 6. Testing Strategy

### 6.1. Unit Tests

Create a separate test file for each module using `gtest`.

-   **HAT Protocol Tests:**
    -   Test token issuance and validation success.
    -   Test validation failure for incorrect tokens, expired tokens, and revoked tokens.
    -   Test permission checking logic.
    -   Test token revocation.
-   **The Org Tests:**
    -   Test organization creation and retrieval.
    -   Test member addition, removal, and role assignments.
    -   Test permission checking for different roles (`owner`, `admin`, `member`, `viewer`).

### 6.2. Integration Tests

-   Test the interaction between the HAT Protocol and The Org.
-   Example: An agent with a HAT token attempts to access a resource belonging to an organization. The system must verify both the HAT token and the user's organization permissions.

### 6.3. Database Tests

-   Verify that data is correctly written to and read from the SQLite database.
-   Test database schema creation and migration.
-   Test database connection handling and thread safety.

---

## 7. Conclusion

This plan outlines a clear path to completing the HAT Protocol and The Org modules. By following these steps, the ElizaOS C++ repository will gain critical, production-ready features for security and multi-tenancy. The provided code and configurations are designed for immediate integration and use.
