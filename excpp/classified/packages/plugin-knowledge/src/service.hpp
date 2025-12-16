#include "config.hpp"
#include "docs-loader.hpp"
#include "document-processor.ts.hpp"
#include "elizaos/core.hpp"
#include "types.ts.hpp"
#include "utils.ts.hpp"
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
 * Knowledge Service - Provides retrieval augmented generation capabilities
 */
class KnowledgeService extends Service {
  static readonly serviceType = KnowledgeServiceType.KNOWLEDGE;
  public override config: Metadata = {};

  /**
   * Create a new Knowledge service
   * @param runtime Agent runtime
   */

      // Use a small delay to ensure runtime is fully ready if needed, though constructor implies it should be.

  /**
   * Start the Knowledge service
   * @param runtime Agent runtime
   * @returns Initialized Knowledge service
   */

      // Validate the model configuration

      // just for debug/check

      // Help inform how this was detected

      // Use the most permissive check during initialization

      // Log the operational mode

    // Process character knowledge AFTER service is initialized
      // Run in background, don't await here to prevent blocking startup

  /**
   * Stop the Knowledge service
   * @param runtime Agent runtime
   */
    // If we need to perform specific cleanup on the KnowledgeService instance

  /**
   * Stop the service
   */

  /**
   * Add knowledge to the system
   * @param options Knowledge options
   * @returns Promise with document processing result
   */
    // Use agentId from options if provided (from frontend), otherwise fall back to runtime

    // Generate content-based ID to ensure consistency

    // Check if document already exists in database using content-based ID

        // Count existing fragments for this document

        // Filter fragments related to this specific document

      // Document doesn't exist or other error, continue with processing

    // Process the document with the content-based ID

  /**
   * Process a document regardless of type - Called by public addKnowledge
   * @param options Document options
   * @returns Promise with document processing result
   */
    // Use agentId from options if provided (from frontend), otherwise fall back to runtime

        // For PDFs: extract text for fragments but store original base64 in main document
        // For other binary files: extract text and store as plain text
        // For text files (including markdown): content is already plain text or needs decoding from base64
        // Routes always send base64, but docs-loader sends plain text

        // First, check if this looks like base64
            // Try to decode from base64
            // Check if it's valid UTF-8

            // Verify the decoded text doesn't contain too many invalid characters

              // More than 10% invalid characters, probably not a text file

            // If it looked like base64 but failed to decode properly, this is an error
          // Content doesn't look like base64, treat as plain text

      // Create document memory using the clientDocumentId as the memory ID

  // --- Knowledge methods moved from AgentRuntime ---

    // This checks if a specific memory (fragment or document) ID exists.
    // In the context of processCharacterKnowledge, knowledgeId is a UUID derived from the content.

  /**
   * Enrich a conversation memory with RAG metadata
   * This can be called after response generation to add RAG tracking data
   * @param memoryId The ID of the conversation memory to enrich
   * @param ragMetadata The RAG metadata to add
   */
      // Get the existing memory

      // Add RAG metadata to the memory

      // Update the memory

  /**
   * Set the current response memory ID for RAG tracking
   * This is called by the knowledge provider to track which response memory to enrich
   */

  /**
   * Store RAG metadata for the next conversation memory that gets created
   * @param ragMetadata The RAG metadata to associate with the next memory
   */
    // Clean up old pending enrichments (older than 30 seconds)

    // Add new pending enrichment

  /**
   * Try to enrich recent conversation memories with pending RAG metadata
   * This is called periodically to catch memories that were created after RAG retrieval
   */

      // Get recent conversation memories (last 10 seconds)

      // Match pending RAG metadata with recent memories
        // Find a memory created after this RAG metadata was generated

          // Remove this pending enrichment

    // Wait briefly to allow services to initialize fully

        // Generate content-based ID for character knowledge

        // Using _internalAddKnowledge for character knowledge
            // Scope to the agent itself for character knowledge

      // Default scope for internal additions (like character knowledge)

    // For _internalAddKnowledge, we assume item.content.text is always present
    // and it's not a binary file needing Knowledge plugin's special handling for extraction.
    // This path is for already-textual content like character knowledge or direct text additions.

      // Add embedding to the fragment
      // Runtime's addEmbeddingToMemory will use runtime.useModel(ModelType.TEXT_EMBEDDING, ...)

      // Store the fragment in the knowledge table

    // TODO: Consider using DEFAULT_CHUNK_TOKEN_SIZE and DEFAULT_CHUNK_OVERLAP_TOKENS from ctx-embeddings
    // For now, using passed in values or defaults from _internalAddKnowledge.

      // Create a unique ID for the fragment based on document ID, index, and timestamp

          // Ensure we don't overwrite essential fragment metadata with document's
          // For example, source might be different or more specific for the fragment.
          // Here, we primarily inherit and then set fragment-specifics.

  // ADDED METHODS START
  /**
   * Retrieves memories, typically documents, for the agent.
   * Corresponds to GET /plugins/knowledge/documents
   */

  /**
   * Deletes a specific memory item (knowledge document) by its ID.
   * Corresponds to DELETE /plugins/knowledge/documents/:knowledgeId
   * Assumes the memoryId corresponds to an item in the 'documents' table or that
   * runtime.deleteMemory can correctly identify it.
   */
    // The core runtime.deleteMemory is expected to handle deletion.
    // If it needs a tableName, and we are sure it's 'documents', it could be passed.
    // However, the previous error indicated runtime.deleteMemory takes 1 argument.
  // ADDED METHODS END

} // namespace elizaos
