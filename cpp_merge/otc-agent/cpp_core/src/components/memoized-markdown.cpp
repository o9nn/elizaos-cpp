#include "memoized-markdown.hpp"
#include <string>
#include <vector>
#include <map>
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::vector<std::string> parseMarkdownIntoBlocks(const std::string& markdown) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto lexer = std::make_unique<Lexer>();
    const auto tokens = lexer.lex(markdown || "");
    return tokens.std::map[&]((token) { return token.raw); };

}

} // namespace elizaos
