#include "/home/runner/work/elizaos-cpp/elizaos-cpp/auto.fun/packages/server/src/prompts/enhance-prompt.h"

any enhancePrompt(string userPrompt, any tokenMetadata)
{
    return std::string("Enhance this prompt for image generation by combining it with the token metadata. Create a single, coherent image prompt that incorporates both the user's ideas and the token's identity.\
\
Token Metadata:\
- Name: ") + tokenMetadata["name"] + std::string("\
- Symbol: ") + tokenMetadata["symbol"] + std::string("\
- Description: ") + (OR((tokenMetadata["description"]), (string_empty))) + std::string("\
- Original token prompt: ") + (OR((tokenMetadata["prompt"]), (string_empty))) + std::string("\
\
User's prompt: "") + userPrompt + std::string(""\
\
Return only the enhanced prompt, nothing else. The prompt should be detailed and descriptive, focusing on visual elements that would create a compelling image.");
};


