#include "security.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

void validateSecureAction(const std::string& url, UrlValidator validator) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        if (!url) {
            return;
        }

        const auto validation = validator.validateUrl(url);
        if (!validation.valid) {
            throw new BrowserSecurityError(`URL validation failed: ${validation.error}`, {
                url,
                error: validation.error,
                });
            }

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

} // namespace elizaos
