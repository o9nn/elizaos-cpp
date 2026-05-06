#pragma once
#include <algorithm>
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



struct ExportManifest {
    std::string version;
    std::string exportedAt;
    std::string agentId;
    std::string agentName;
    std::vector<std::string> tables;
    double fileCount;
    std::string elizaVersion;
};

struct ExportOptions {
    std::optional<bool> includeFiles;
    std::optional<bool> compress;
};

class AgentExportService {
public:
    AgentExportService();
    std::future<NodeJS::ReadableStream> exportToZip(ExportOptions options = {});
    std::future<ExportManifest> exportDatabaseData(archiver::Archiver archive);
    std::future<double> exportUploadedFiles(archiver::Archiver archive);
    std::future<void> cleanup();


} // namespace elizaos
