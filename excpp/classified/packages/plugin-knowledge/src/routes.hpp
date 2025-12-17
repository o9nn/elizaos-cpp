#pragma once
#include <any>
#include <functional>
#include <future>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#include "elizaos/core.hpp"
#include "service.hpp"
#include "utils.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



 // For file operations in upload
 // For path operations
 // For handling multipart uploads
 // Import utils functions

// Create multer configuration function that uses runtime settings

// Add this type declaration to fix Express.Multer.File error
struct MulterFile {
    std::string fieldname;
    std::string originalname;
    std::string encoding;
    std::string mimetype;
    double size;
    std::string destination;
    std::string filename;
    std::string path;
    Buffer buffer;
};

// Helper to send success response
void sendSuccess(const std::any& res, const std::any& data, auto status);

// Helper to send error response
void sendError(const std::any& res, double status, const std::string& code, const std::string& message, std::optional<std::string> details);

// Helper to clean up a single file

// Helper to clean up multiple files

// Main upload handler (without multer, multer is applied by wrapper)
std::future<void> uploadKnowledgeHandler(const std::any& req, const std::any& res, IAgentRuntime runtime);

std::future<void> getKnowledgeDocumentsHandler(const std::any& req, const std::any& res, IAgentRuntime runtime);

std::future<void> deleteKnowledgeDocumentHandler(const std::any& req, const std::any& res, IAgentRuntime runtime);

std::future<void> getKnowledgeByIdHandler(const std::any& req, const std::any& res, IAgentRuntime runtime);

// Handler for the panel itself - serves the actual HTML frontend
std::future<void> knowledgePanelHandler(const std::any& req, const std::any& res, IAgentRuntime runtime);

// Generic handler to serve static assets from the dist/assets directory
std::future<void> frontendAssetHandler(const std::any& req, const std::any& res, IAgentRuntime runtime);

std::future<void> getKnowledgeChunksHandler(const std::any& req, const std::any& res, IAgentRuntime runtime);

std::future<void> searchKnowledgeHandler(const std::any& req, const std::any& res, IAgentRuntime runtime);

// Wrapper handler that applies multer middleware before calling the upload handler
std::future<void> uploadKnowledgeWithMulter(const std::any& req, const std::any& res, IAgentRuntime runtime);


} // namespace elizaos
