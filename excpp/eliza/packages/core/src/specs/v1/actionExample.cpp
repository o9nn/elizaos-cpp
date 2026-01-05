#include "actionExample.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

Content convertContentToV1(const ContentV2& content) {
    // NOTE: Auto-converted from TypeScript - manually refined for C++
    
    Content result;
    
    if (content.text.empty()) {
        result.text = "";
        return result;
    }
    
    result.text = content.text;
    
    // V2 uses 'actions' array, V1 might use 'action' string
    if (!content.actions.empty()) {
        result.action = content.actions[0];
    }
    
    // TODO: Copy other properties if needed
    // This would require reflection or a more complex approach in C++
    
    return result;
}

ContentV2 convertContentToV2(const Content& content) {
    // NOTE: Auto-converted from TypeScript - manually refined for C++
    
    ContentV2 result;
    
    if (content.text.empty()) {
        result.text = "";
        return result;
    }
    
    result.text = content.text;
    
    // V1 uses 'action' string, V2 uses 'actions' array
    if (content.action.has_value()) {
        result.actions.push_back(content.action.value());
    }
    
    // TODO: Copy other properties if needed
    
    return result;
}

ActionExample fromV2ActionExample(const ActionExampleV2& exampleV2) {
    // NOTE: Auto-converted from TypeScript - manually refined for C++
    
    ActionExample result;
    
    // The main difference is that v2 uses 'name' instead of 'user'
    result.user = exampleV2.name;
    result.content = convertContentToV1(exampleV2.content);
    
    return result;
}

ActionExampleV2 toV2ActionExample(const ActionExample& example) {
    // NOTE: Auto-converted from TypeScript - manually refined for C++
    
    ActionExampleV2 result;
    
    // Convert v1 format to v2 format
    result.name = example.user;
    result.content = convertContentToV2(example.content);
    
    return result;
}

} // namespace elizaos
