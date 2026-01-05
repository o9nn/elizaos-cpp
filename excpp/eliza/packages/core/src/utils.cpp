#include "utils.hpp"
#include <iostream>
#include <stdexcept>
#include <regex>

namespace elizaos {

std::string upgradeDoubleToTriple(const std::string& tpl) {
    // NOTE: Auto-converted from TypeScript - manually refined for C++
    // This function converts Handlebars double-brace {{}} to triple-brace {{{}}
    // to prevent HTML escaping
    
    try {
        // Stub implementation - would need full regex support for production
        // Pattern: (?<!{){{(?![{#/!>])(.+?)}}
        // This matches {{ that's not preceded by { and not followed by {, #, /, !, or >
        
        std::string result = tpl;
        
        // Simple replacement for common cases
        // For production, implement proper regex-based replacement
        // that handles all the edge cases mentioned in the comment
        
        std::cout << "WARNING: upgradeDoubleToTriple using simplified implementation" << std::endl;
        
        // TODO: Implement proper Handlebars template transformation
        // - Skip block helpers ({{#...}})
        // - Skip partials ({{>...}})
        // - Skip comments ({{!...}})
        // - Skip already triple-braced ({{{...}}})
        // - Keep {{else}} unchanged
        
        return result;
    } catch (const std::exception& e) {
        std::cerr << "Error in upgradeDoubleToTriple: " << e.what() << std::endl;
        return tpl;
    }
}

// Additional utility functions can be added here as needed

} // namespace elizaos
