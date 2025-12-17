#pragma once
#include <algorithm>
#include <any>
#include <cstdint>
#include <functional>
#include <future>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include "elizaos/core.hpp"
#include "elizaos/plugin-sql.hpp"
#include "schema.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



struct ImportOptions {
    std::optional<bool> overwrite;
    std::optional<bool> validateManifest;
    std::optional<double> maxFileSize;
};

struct ImportResult {
    bool success;
    std::string agentId;
    std::string agentName;
    std::vector<std::string> tablesImported;
    double filesImported;
    std::optional<std::vector<std::string>> errors;
};

class AgentImportService {
public:
    AgentImportService();
    std::future<ImportResult> importFromZip(const std::vector<uint8_t>& zipBuffer, ImportOptions options = {});
    std::future<ExportManifest> validateAndReadManifest(AdmZip zip, bool validate);
    std::future<void> deleteExistingAgentData(const std::any& tx);
    std::future<void> importAgent(const std::any& tx, AdmZip zip);
    std::future<void> importEntities(const std::any& tx, AdmZip zip);
    std::future<void> importWorlds(const std::any& tx, AdmZip zip);
    std::future<void> importRooms(const std::any& tx, AdmZip zip);
    std::future<void> importParticipants(const std::any& tx, AdmZip zip);
    std::future<void> importMemories(const std::any& tx, AdmZip zip);
    std::future<void> importRelationships(const std::any& tx, AdmZip zip);
    std::future<void> importTasks(const std::any& tx, AdmZip zip);
    std::future<void> importServerAgents(const std::any& tx, AdmZip zip);
    std::future<double> importUploadedFiles(AdmZip zip);
    std::future<void> cleanup();


} // namespace elizaos
