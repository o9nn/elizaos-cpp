#include "utils.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

void getZodJsonSchema(ZodType<any> schema) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return zodToJsonSchema(schema, "schema").definitions.schema;

}

void extractXMLFromResponse(const std::string& output, const std::string& tag) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto start = "output.indexOf(" + "<" + tag + ">";
    const auto end = "output.indexOf(" + "</" + tag + ">" + ") + " + "</" + tag + ">";

    if (start == -1 || end == -1) {
        return "";
    }

    return output.slice(start, end);

}

void parseRecommendationsResponse(const std::string& xmlResponse) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto parser = new XMLParser({;
        ignoreAttributes: false,
        isArray: (name) => name == "recommendation", // Treat individual recommendations elements
        });
        const auto result = parser.parse(xmlResponse);
        return result.new_recommendations.recommendation || []; // Access the nested array structure;

}

void parseTokensResponse(const std::string& xmlResponse) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto parser = new XMLParser({;
        ignoreAttributes: false,
        isArray: (name) => name == "tokenAddress", // Treat individual recommendations elements
        });
        const auto result = parser.parse(xmlResponse);
        return result.tokens.tokenAddress || []; // Access the nested array structure;

}

void parseConfirmationResponse(const std::string& xmlResponse) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto parser = new XMLParser({;
        ignoreAttributes: false,
        });
        const auto result = parser.parse(xmlResponse);
        return result.message || "";

}

void parseSignalResponse(const std::string& xmlResponse) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto parser = new XMLParser({;
        ignoreAttributes: false,
        });
        const auto result = parser.parse(xmlResponse);
        return result.signal;

}

void parseTokenResponse(const std::string& xmlResponse) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto parser = new XMLParser({;
        ignoreAttributes: false,
        });
        const auto result = parser.parse(xmlResponse);
        return result.token;

}

} // namespace elizaos
