#include "create-form.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::string extractFormType(const std::string& text) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const std::unordered_map<std::string, std::vector<std::string>> formTypes = {;
        contact: ["contact", "reach out", "get in touch", "message"],
        feedback: ["feedback", "review", "opinion", "suggestion"],
        application: ["apply", "application", "job", "position"],
        survey: ["survey", "questionnaire", "poll"],
        registration: ["register", "sign up", "enroll", "join"],
        };

        for (const int [type, keywords] of Object.entries(formTypes)) {
            if (keywords.some((keyword) => text.includes(keyword))) {
                return type;
            }
        }

        return nullptr;

}

} // namespace elizaos
