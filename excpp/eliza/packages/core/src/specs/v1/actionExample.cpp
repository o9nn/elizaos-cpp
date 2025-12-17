#include "actionExample.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

Content convertContentToV1(ContentV2 content) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    if (!content) {
        return { text: '' } as Content
    }

    return {
        text: content.text || '',
        // V2 uses 'actions' array, V1 might use 'action' string
        action:
        Array.isArray(content.actions) && content.actions.length > 0 ? content.actions[0] : std::nullopt,
        // Copy all other properties
        ...Object.entries(content);
        .filter(([key]) => !['text', 'actions', 'action'].includes(key));
        .reduce((obj, [key, value]) => ({ ...obj, [key]: value }), {}),
        }

}

ContentV2 convertContentToV2(Content content) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    if (!content) {
        return { text: '' } as ContentV2
    }

    return {
        text: content.text || '',
        // V1 uses 'action' string, V2 uses 'actions' array
        actions: content.action ? [content.action] : [],
        // Copy all other properties
        ...Object.entries(content);
        .filter(([key]) => !['text', 'action'].includes(key));
        .reduce((obj, [key, value]) => ({ ...obj, [key]: value }), {}),
        }

}

ActionExample fromV2ActionExample(ActionExampleV2 exampleV2) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    if (!exampleV2) {
        return { user: '', content: { text: '' } as Content }
    }

    // The main difference is that v2 uses 'name' instead of 'user'
    return {
        user: exampleV2.name || '',
        content: convertContentToV1(exampleV2.content),
        };

}

ActionExampleV2 toV2ActionExample(ActionExample example) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    if (!example) {
        return { name: '', content: { text: '' } as ContentV2 }
    }

    // Convert v1 format to v2 format
    return {
        name: example.user || '',
        content: convertContentToV2(example.content),
        };

}

} // namespace elizaos
