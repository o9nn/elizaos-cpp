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

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



  // New fields

// Fees schema

// Messages schema

// User schema

// MediaGenerations schema

// CachePrices schema

// PreGeneratedTokens schema

// OAuthVerifiers schema

// AccessTokens schema

// TokenAgents schema

// Default local database connection string

void getDB();

// Type definitions for common query results
using Token = typeof schema.tokens.$inferSelect;
using TokenInsert = typeof schema.tokens.$inferInsert;

using Fee = typeof schema.fees.$inferSelect;
using FeeInsert = typeof schema.fees.$inferInsert;

using Message = typeof schema.messages.$inferSelect;
using MessageInsert = typeof schema.messages.$inferInsert;

using User = typeof schema.users.$inferSelect;
using UserInsert = typeof schema.users.$inferInsert;

using PreGeneratedToken = typeof schema.preGeneratedTokens.$inferSelect;
using PreGeneratedTokenInsert = typeof schema.preGeneratedTokens.$inferInsert;

using TokenAgent = typeof tokenAgents::$inferSelect;
using TokenAgentInsert = typeof tokenAgents::$inferInsert;

using Metadata = typeof metadata::$inferSelect;
using MetadataInsert = typeof metadata::$inferInsert;

// Schema for all tables

// Export schema for type inference

} // namespace elizaos
