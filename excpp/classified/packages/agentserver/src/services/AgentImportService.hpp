#pragma once
#include <functional>
#include <future>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#include ".schema.hpp"
#include "elizaos/core.hpp"
#include "elizaos/plugin-sql.hpp"

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
    std::future<ImportResult> importFromZip(Buffer zipBuffer, ImportOptions options = {});
    std::future<ExportManifest> validateAndReadManifest(AdmZip zip, bool validate);
    std::future<void> deleteExistingAgentData(unknown tx);
    std::future<void> importAgent(unknown tx, AdmZip zip);
    std::future<void> importEntities(unknown tx, AdmZip zip);
    std::future<void> importWorlds(unknown tx, AdmZip zip);
    std::future<void> importRooms(unknown tx, AdmZip zip);
    std::future<void> importParticipants(unknown tx, AdmZip zip);
    std::future<void> importMemories(unknown tx, AdmZip zip);
    std::future<void> importRelationships(unknown tx, AdmZip zip);
    std::future<void> importTasks(unknown tx, AdmZip zip);
    std::future<void> importServerAgents(unknown tx, AdmZip zip);
    std::future<double> importUploadedFiles(AdmZip zip);
    std::future<void> cleanup();


} // namespace elizaos
