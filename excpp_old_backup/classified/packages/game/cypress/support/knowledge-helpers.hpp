#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#pragma once

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use

/**
 * Knowledge Base Test Helpers
 * Utility functions for testing knowledge base CRUD operations
 */

struct KnowledgeDocument {
    std::string id;
    std::string title;
    std::optional<std::string> description;
    std::string createdAt;
    std::optional<std::unordered_map<std::string, std::any>> metadata;
};

struct KnowledgeChunk {
    std::string id;
    { content;
    std::string text;
    std::optional<std::unordered_map<std::string, std::any>> metadata;
    std::optional<std::vector<double>> embedding;
};

struct KnowledgeSearchResult {
    { content;
    std::string text;
    std::optional<std::unordered_map<std::string, std::any>> metadata;
    std::optional<double> similarity;
};

struct UploadResponse {
    bool success;
    { data;
    std::string id;
    std::optional<std::string> message;
    std::optional<{> error;
    std::string message;
    std::optional<std::string> code;
};

class KnowledgeTestHelper {
  private baseUrl = 'http://localhost:7777/knowledge';
  private agentId: string;

  constructor(agentId: string = '00000000-0000-0000-0000-000000000001') {
    this.agentId = agentId;
  }

  /**
   * Upload a file to the knowledge base
   */

  /**
   * Upload a document from URL
   */

  /**
   * Get all knowledge documents
   */

  /**
   * Get chunks for a specific document
   */

  /**
   * Search the knowledge base
   */

  /**
   * Delete a knowledge document
   */

  /**
   * Wait for document to be processed (embedded and chunked)
   */

          return checkProcessing();
          return checkProcessing();

    return checkProcessing();

  /**
   * Verify document exists and has content
   */

  /**
   * Clean up - delete all test documents
   */
      // Try to get documents, but continue if it fails

              // Convert to promise for proper error handling

            // Error handler for getDocuments

  /**
   * Create test documents with specific content for testing
   */

  /**
   * Validate API response structure
   */

  /**
   * Test file upload with various file types
   */

// Cypress commands for easier usage in tests
    struct Chainable {
    std::string fileName;
    std::string content;
    std::optional<std::string> fileType;
};

// Register custom commands


} // namespace elizaos
