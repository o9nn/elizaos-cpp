#include "errors.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::vector<> handleBrowserError(Error error, std::optional<std::any> callback, std::optional<std::string> action) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    if (error instanceof StagehandError) {
        std::cerr << "Stagehand error [" + std::to_string(error.code) + "]:" << error.message << std::endl;
        callback.({
            text: error.userMessage,
            error: true,
            });
            } else {
                std::cerr << 'Unexpected browser error:' << error << std::endl;
                callback.({
                    text: action
                    "I encountered an error while trying to " + std::to_string(action) + ". Please try again.";
                    : 'I encountered an unexpected error. Please try again.',
                    error: true,
                    });
                }
                return [];

}

} // namespace elizaos
