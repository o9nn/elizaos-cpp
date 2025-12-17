#include "enhance-prompt.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

void enhancePrompt(const std::string& userPrompt, const std::any& tokenMetadata) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return `Enhance this prompt for image generation by combining it with the token metadata. Create a single, coherent image prompt that incorporates both the user"s ideas and the token"s identity.;

    Token Metadata:
    - Name: ${tokenMetadata.name}
    - Symbol: ${tokenMetadata.symbol}
    - Description: ${tokenMetadata.description || ""}
    - Original token prompt: ${tokenMetadata.prompt || ""}

    User's prompt: "${userPrompt}"

    "Return only the enhanced prompt, nothing else. The prompt should be detailed and descriptive, focusing on visual elements that would create a compelling image.";

}

} // namespace elizaos
