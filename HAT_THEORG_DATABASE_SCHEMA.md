# Database Schema Design: HAT Protocol & The Org

## Overview

This document defines the SQLite database schema for the HAT Protocol (Human-Agent Token) and The Org (Organization Management) modules.

---

## HAT Protocol Database Schema

### Purpose
Secure token-based authentication and authorization system for agent-human interactions.

### Tables

#### 1. `hat_tokens`
Stores issued HAT tokens with metadata and permissions.

```sql
CREATE TABLE hat_tokens (
    token_id TEXT PRIMARY KEY,
    agent_id TEXT NOT NULL,
    user_id TEXT NOT NULL,
    token_hash TEXT NOT NULL,          -- SHA-256 hash of the token
    salt TEXT NOT NULL,                -- Random salt for hashing
    issued_at INTEGER NOT NULL,        -- Unix timestamp
    expires_at INTEGER NOT NULL,       -- Unix timestamp
    last_used_at INTEGER,              -- Unix timestamp
    is_active INTEGER DEFAULT 1,       -- 1 = active, 0 = revoked
    revoked_at INTEGER,                -- Unix timestamp
    revoked_by TEXT,                   -- User ID who revoked
    revoke_reason TEXT,
    metadata TEXT,                     -- JSON metadata
    created_at INTEGER NOT NULL DEFAULT (strftime('%s', 'now'))
);

CREATE INDEX idx_hat_tokens_agent ON hat_tokens(agent_id);
CREATE INDEX idx_hat_tokens_user ON hat_tokens(user_id);
CREATE INDEX idx_hat_tokens_active ON hat_tokens(is_active, expires_at);
CREATE INDEX idx_hat_tokens_expires ON hat_tokens(expires_at);
```

#### 2. `hat_permissions`
Defines permissions associated with tokens.

```sql
CREATE TABLE hat_permissions (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    token_id TEXT NOT NULL,
    permission TEXT NOT NULL,
    resource TEXT,                     -- Optional resource identifier
    granted_at INTEGER NOT NULL DEFAULT (strftime('%s', 'now')),
    FOREIGN KEY (token_id) REFERENCES hat_tokens(token_id) ON DELETE CASCADE,
    UNIQUE(token_id, permission, resource)
);

CREATE INDEX idx_hat_permissions_token ON hat_permissions(token_id);
CREATE INDEX idx_hat_permissions_perm ON hat_permissions(permission);
```

#### 3. `hat_token_usage`
Tracks token usage for analytics and security.

```sql
CREATE TABLE hat_token_usage (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    token_id TEXT NOT NULL,
    used_at INTEGER NOT NULL DEFAULT (strftime('%s', 'now')),
    action TEXT,                       -- Action performed
    ip_address TEXT,
    user_agent TEXT,
    success INTEGER DEFAULT 1,         -- 1 = success, 0 = failure
    error_message TEXT,
    FOREIGN KEY (token_id) REFERENCES hat_tokens(token_id) ON DELETE CASCADE
);

CREATE INDEX idx_hat_usage_token ON hat_token_usage(token_id);
CREATE INDEX idx_hat_usage_time ON hat_token_usage(used_at);
```

#### 4. `hat_refresh_tokens`
Stores refresh tokens for token renewal.

```sql
CREATE TABLE hat_refresh_tokens (
    refresh_token_id TEXT PRIMARY KEY,
    token_id TEXT NOT NULL,
    refresh_hash TEXT NOT NULL,
    issued_at INTEGER NOT NULL,
    expires_at INTEGER NOT NULL,
    is_used INTEGER DEFAULT 0,
    used_at INTEGER,
    FOREIGN KEY (token_id) REFERENCES hat_tokens(token_id) ON DELETE CASCADE
);

CREATE INDEX idx_hat_refresh_token ON hat_refresh_tokens(token_id);
```

---

## The Org Database Schema

### Purpose
Multi-tenant organization management with role-based access control.

### Tables

#### 1. `organizations`
Core organization data.

```sql
CREATE TABLE organizations (
    org_id TEXT PRIMARY KEY,
    name TEXT NOT NULL,
    display_name TEXT,
    description TEXT,
    owner_id TEXT NOT NULL,
    status TEXT DEFAULT 'active',      -- active, suspended, deleted
    plan_type TEXT DEFAULT 'free',     -- free, pro, enterprise
    max_members INTEGER DEFAULT 10,
    created_at INTEGER NOT NULL DEFAULT (strftime('%s', 'now')),
    updated_at INTEGER NOT NULL DEFAULT (strftime('%s', 'now')),
    deleted_at INTEGER
);

CREATE INDEX idx_orgs_owner ON organizations(owner_id);
CREATE INDEX idx_orgs_status ON organizations(status);
CREATE INDEX idx_orgs_created ON organizations(created_at);
```

#### 2. `org_members`
Organization membership and roles.

```sql
CREATE TABLE org_members (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    org_id TEXT NOT NULL,
    user_id TEXT NOT NULL,
    role TEXT NOT NULL,                -- owner, admin, member, viewer
    status TEXT DEFAULT 'active',      -- active, invited, suspended
    invited_by TEXT,
    joined_at INTEGER NOT NULL DEFAULT (strftime('%s', 'now')),
    updated_at INTEGER NOT NULL DEFAULT (strftime('%s', 'now')),
    FOREIGN KEY (org_id) REFERENCES organizations(org_id) ON DELETE CASCADE,
    UNIQUE(org_id, user_id)
);

CREATE INDEX idx_org_members_org ON org_members(org_id);
CREATE INDEX idx_org_members_user ON org_members(user_id);
CREATE INDEX idx_org_members_role ON org_members(role);
```

#### 3. `org_permissions`
Fine-grained permissions for organization members.

```sql
CREATE TABLE org_permissions (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    org_id TEXT NOT NULL,
    user_id TEXT NOT NULL,
    permission TEXT NOT NULL,
    resource_type TEXT,                -- agent, memory, action, etc.
    resource_id TEXT,
    granted_at INTEGER NOT NULL DEFAULT (strftime('%s', 'now')),
    granted_by TEXT,
    FOREIGN KEY (org_id) REFERENCES organizations(org_id) ON DELETE CASCADE,
    UNIQUE(org_id, user_id, permission, resource_type, resource_id)
);

CREATE INDEX idx_org_perms_org ON org_permissions(org_id);
CREATE INDEX idx_org_perms_user ON org_permissions(user_id);
CREATE INDEX idx_org_perms_perm ON org_permissions(permission);
```

#### 4. `org_invitations`
Pending organization invitations.

```sql
CREATE TABLE org_invitations (
    invitation_id TEXT PRIMARY KEY,
    org_id TEXT NOT NULL,
    email TEXT NOT NULL,
    role TEXT NOT NULL,
    invited_by TEXT NOT NULL,
    invitation_token TEXT NOT NULL UNIQUE,
    status TEXT DEFAULT 'pending',     -- pending, accepted, declined, expired
    created_at INTEGER NOT NULL DEFAULT (strftime('%s', 'now')),
    expires_at INTEGER NOT NULL,
    accepted_at INTEGER,
    FOREIGN KEY (org_id) REFERENCES organizations(org_id) ON DELETE CASCADE
);

CREATE INDEX idx_org_invites_org ON org_invitations(org_id);
CREATE INDEX idx_org_invites_email ON org_invitations(email);
CREATE INDEX idx_org_invites_status ON org_invitations(status);
```

#### 5. `org_settings`
Organization-specific settings and configuration.

```sql
CREATE TABLE org_settings (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    org_id TEXT NOT NULL,
    setting_key TEXT NOT NULL,
    setting_value TEXT,
    setting_type TEXT DEFAULT 'string', -- string, number, boolean, json
    updated_at INTEGER NOT NULL DEFAULT (strftime('%s', 'now')),
    updated_by TEXT,
    FOREIGN KEY (org_id) REFERENCES organizations(org_id) ON DELETE CASCADE,
    UNIQUE(org_id, setting_key)
);

CREATE INDEX idx_org_settings_org ON org_settings(org_id);
CREATE INDEX idx_org_settings_key ON org_settings(setting_key);
```

#### 6. `org_audit_log`
Audit trail for organization activities.

```sql
CREATE TABLE org_audit_log (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    org_id TEXT NOT NULL,
    user_id TEXT,
    action TEXT NOT NULL,              -- create, update, delete, invite, etc.
    resource_type TEXT,
    resource_id TEXT,
    old_value TEXT,                    -- JSON
    new_value TEXT,                    -- JSON
    ip_address TEXT,
    user_agent TEXT,
    created_at INTEGER NOT NULL DEFAULT (strftime('%s', 'now')),
    FOREIGN KEY (org_id) REFERENCES organizations(org_id) ON DELETE CASCADE
);

CREATE INDEX idx_org_audit_org ON org_audit_log(org_id);
CREATE INDEX idx_org_audit_user ON org_audit_log(user_id);
CREATE INDEX idx_org_audit_time ON org_audit_log(created_at);
CREATE INDEX idx_org_audit_action ON org_audit_log(action);
```

---

## Shared Tables

#### `users` (Optional - may already exist in core)
Basic user information referenced by both modules.

```sql
CREATE TABLE IF NOT EXISTS users (
    user_id TEXT PRIMARY KEY,
    username TEXT UNIQUE,
    email TEXT UNIQUE,
    display_name TEXT,
    created_at INTEGER NOT NULL DEFAULT (strftime('%s', 'now')),
    updated_at INTEGER NOT NULL DEFAULT (strftime('%s', 'now'))
);

CREATE INDEX idx_users_username ON users(username);
CREATE INDEX idx_users_email ON users(email);
```

---

## Database Initialization SQL

### Complete Schema Creation Script

```sql
-- HAT Protocol Tables
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

-- The Org Tables
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
```

---

## Database File Structure

### Recommended File Organization

```
/var/lib/elizaos/
├── elizaos.db              # Main database (all tables)
├── elizaos.db-wal          # Write-Ahead Log
└── elizaos.db-shm          # Shared memory
```

### Alternative: Separate Databases

```
/var/lib/elizaos/
├── hat_protocol.db         # HAT Protocol only
├── organizations.db        # The Org only
└── core.db                 # Core ElizaOS data
```

---

## Performance Considerations

### Indexes
- All foreign keys have indexes
- Commonly queried columns have indexes
- Composite indexes for multi-column queries

### Write-Ahead Logging (WAL)
```sql
PRAGMA journal_mode=WAL;
PRAGMA synchronous=NORMAL;
```

### Connection Pooling
- Recommended: 5-10 connections per module
- Read-only connections for queries
- Write connections for transactions

---

## Security Considerations

### Token Storage
- Store only SHA-256 hashes, never plaintext tokens
- Use random salts for each token
- Implement token rotation

### Database Encryption
- Use SQLCipher for encryption at rest
- Encrypt sensitive columns (token_hash, metadata)

### Access Control
- Limit database access to application only
- Use prepared statements to prevent SQL injection
- Implement row-level security where needed

---

## Migration Strategy

### Version Control
```sql
CREATE TABLE schema_migrations (
    version INTEGER PRIMARY KEY,
    applied_at INTEGER NOT NULL DEFAULT (strftime('%s', 'now')),
    description TEXT
);

INSERT INTO schema_migrations (version, description) VALUES 
(1, 'Initial HAT Protocol schema'),
(2, 'Initial The Org schema');
```

### Upgrade Path
1. Check current schema version
2. Apply migrations sequentially
3. Verify data integrity
4. Update schema version

---

## Backup Strategy

### Automated Backups
```bash
# Daily backup
sqlite3 /var/lib/elizaos/elizaos.db ".backup /var/backups/elizaos-$(date +%Y%m%d).db"

# Incremental backup (WAL)
cp /var/lib/elizaos/elizaos.db-wal /var/backups/
```

### Retention Policy
- Daily backups: Keep 7 days
- Weekly backups: Keep 4 weeks
- Monthly backups: Keep 12 months

---

## Testing Data

### Sample HAT Token
```sql
INSERT INTO hat_tokens (token_id, agent_id, user_id, token_hash, salt, issued_at, expires_at)
VALUES (
    'HAT_123456789',
    'agent_001',
    'user_001',
    'a665a45920422f9d417e4867efdc4fb8a04a1f3fff1fa07e998e86f7f7a27ae3',
    'random_salt_123',
    strftime('%s', 'now'),
    strftime('%s', 'now', '+30 days')
);

INSERT INTO hat_permissions (token_id, permission)
VALUES 
    ('HAT_123456789', 'read'),
    ('HAT_123456789', 'write'),
    ('HAT_123456789', 'execute');
```

### Sample Organization
```sql
INSERT INTO organizations (org_id, name, display_name, owner_id)
VALUES (
    'ORG_001',
    'acme-corp',
    'ACME Corporation',
    'user_001'
);

INSERT INTO org_members (org_id, user_id, role)
VALUES 
    ('ORG_001', 'user_001', 'owner'),
    ('ORG_001', 'user_002', 'admin'),
    ('ORG_001', 'user_003', 'member');
```

---

## Monitoring & Maintenance

### Health Checks
```sql
-- Check database integrity
PRAGMA integrity_check;

-- Check index usage
PRAGMA index_list('hat_tokens');

-- Check table statistics
SELECT * FROM sqlite_stat1;
```

### Cleanup Tasks
```sql
-- Remove expired tokens (run daily)
DELETE FROM hat_tokens 
WHERE expires_at < strftime('%s', 'now') 
AND is_active = 0;

-- Archive old audit logs (run monthly)
DELETE FROM org_audit_log 
WHERE created_at < strftime('%s', 'now', '-1 year');

-- Vacuum database (run weekly)
VACUUM;
```

---

## Summary

This schema provides:
- ✅ Secure token management with hashing
- ✅ Comprehensive permission system
- ✅ Multi-tenant organization support
- ✅ Role-based access control
- ✅ Audit logging
- ✅ Performance optimization
- ✅ Security best practices

**Total Tables:** 10 (4 HAT + 6 Org)  
**Total Indexes:** 20+  
**Estimated Size:** ~10MB for 10K tokens, 1K orgs, 10K members
