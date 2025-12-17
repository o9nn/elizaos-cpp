#pragma once
#include <algorithm>
#include <cstdint>
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include "services/TauriService.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



struct Backup {
    std::string id;
    std::string timestamp;
    std::string backup_type;
    double size_bytes;
    std::string name;
    std::string component_type;
    double size_bytes;
    std::string agent_name;
    std::string eliza_version;
    std::optional<std::string> notes;
};

struct BackupConfig {
    bool auto_backup_enabled;
    double auto_backup_interval_hours;
    double max_backups_to_keep;
    std::string backup_directory;
};

struct RestoreOptions {
    bool restore_database;
    bool restore_agent_state;
    bool restore_knowledge;
    bool restore_logs;
    bool force;
};


} // namespace elizaos
