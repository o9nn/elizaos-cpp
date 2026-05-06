#pragma once
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use


/**
 * Class representing a DocumentOrganizer.
 */

class DocumentOrganizer {
public:
    OrganizedDocs organizeDocumentation(const std::vector<ASTQueueItem>& docs);
    std::vector<FileDocsGroup> groupDocsByFile(OrganizedDocs docs);
};


} // namespace elizaos
