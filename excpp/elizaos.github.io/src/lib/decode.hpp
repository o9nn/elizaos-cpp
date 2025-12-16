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
 * Decodes a base64 encoded string with proper UTF-8 handling
 * @param encodedString - The base64 encoded string to decode
 * @returns The decoded string with proper UTF-8 character support
 */


    // Use TextDecoder to properly decode UTF-8 content
    const decodedContent = new TextDecoder().decode(bytes);
    return decodedContent;
  } catch (error) {
    throw new Error(
      `Failed to decode base64 string: ${error instanceof Error ? error.message : "Unknown error"}`,
    );
  }
}

} // namespace elizaos
