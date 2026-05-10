#pragma once

// Public helper API for ingesting documents into a KnowledgeBase. Provides
// deterministic sliding-window chunking, lightweight content sniffing, and
// document reassembly utilities. Designed to be standalone and dependency
// free so that downstream agents and tests can drive ingestion without
// pulling in heavy NLP or vector-store libraries.

#include "elizaos/knowledge.hpp"

#include <string>
#include <vector>

namespace elizaos {
namespace knowledge {

// Split the input text into chunkSize-character chunks with `overlap`
// characters of overlap between adjacent chunks. Returns an empty vector
// when text is empty or chunkSize is 0. Never throws.
std::vector<std::string> chunkText(const std::string& text,
                                   size_t chunkSize = 800,
                                   size_t overlap = 120);

// Read an entire file from disk into a string. Returns an empty string on
// failure (missing file, unreadable, etc.).
std::string readWholeFile(const std::string& path);

// Ingest a string of text into a KnowledgeBase as one or more entries.
// Returns the IDs of the created entries (one per chunk).
std::vector<std::string> ingestText(KnowledgeBase& kb,
                                    const std::string& text,
                                    const std::string& documentName,
                                    KnowledgeType type = KnowledgeType::FACT,
                                    const std::vector<std::string>& tags = {},
                                    size_t chunkSize = 800,
                                    size_t overlap = 120);

// Ingest a file from disk. The KnowledgeType is inferred from the file
// extension (e.g. .rule -> RULE, .procedure -> PROCEDURE). Returns the IDs
// of the created chunks; empty vector on failure.
std::vector<std::string> ingestFile(KnowledgeBase& kb,
                                    const std::string& filePath,
                                    const std::vector<std::string>& tags = {},
                                    size_t chunkSize = 800,
                                    size_t overlap = 120);

// Retrieve all knowledge entries that were ingested under a specific
// document name (matched against the "document" metadata key). Sorted by
// chunk_index ascending.
std::vector<KnowledgeEntry> findChunksByDocument(KnowledgeBase& kb,
                                                 const std::string& documentName);

// Reconstruct the original text of a document from its stored chunks,
// stripping the configured chunk overlap between successive chunks.
std::string reconstructDocument(KnowledgeBase& kb,
                                const std::string& documentName);

// Internal symbol kept stable for backwards-compatible linkage. Do not call.
void knowledge_helpers_placeholder();

}  // namespace knowledge
}  // namespace elizaos
