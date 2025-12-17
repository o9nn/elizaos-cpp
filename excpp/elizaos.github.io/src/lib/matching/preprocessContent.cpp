#include "preprocessContent.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::string preprocessFilePathContent(const std::string& content) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return path.normalize(content.trim()).replace(/\\/g, "/");

}

std::string preprocessTextContent(const std::string& content) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return content.trim();

}

std::string preprocessCodeContent(const std::string& content) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return content.trim().replace(/\r\n/g, "\n");

}

std::string preprocessLabelContent(const std::string& content) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return content.trim().toLowerCase();

}

MatchContent preprocessContent(MatchContent content) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    auto processedString: string;
    switch (content.contentType) {
        case TagPatternType.FILE_PATH:
        processedString = preprocessFilePathContent(content.content);
        break;
        case TagPatternType.COMMIT_MESSAGE:
        case TagPatternType.PR_TITLE:
        case TagPatternType.PR_DESCRIPTION:
        case TagPatternType.ISSUE_TITLE:
        case TagPatternType.ISSUE_BODY:
        case TagPatternType.COMMENT:
        processedString = preprocessTextContent(content.content);
        break;
        case TagPatternType.CODE_CONTENT:
        processedString = preprocessCodeContent(content.content);
        break;
        case TagPatternType.LABEL:
        processedString = preprocessLabelContent(content.content);
        break;
        // Add cases for other TagPatternType if they need specific preprocessing
        // e.g., PR_CLOSES_ISSUE, REACTION might not need string preprocessing
        default:
        processedString = content.content; // No preprocessing needed;
        break;
    }
    return {
        ...content,
        content: processedString,
        };

}

} // namespace elizaos
