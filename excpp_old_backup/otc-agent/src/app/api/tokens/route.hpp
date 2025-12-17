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



std::future<void> GET(NextRequest request);

std::future<void> POST(NextRequest request);

/**
 * PATCH /api/tokens - Update token metadata
 * Body: { tokenId, updates: { logoUrl?, name?, symbol?, ... } }
 */
std::future<void> PATCH(NextRequest request);

/**
 * DELETE /api/tokens - Clear all test/seeded tokens
 * Use with ?confirm=true to actually delete
 * Use with ?tokenId=xxx to delete a specific token
 */
std::future<void> DELETE(NextRequest request);

} // namespace elizaos
