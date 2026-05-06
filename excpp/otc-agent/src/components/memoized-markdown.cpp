#include "memoized-markdown.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::vector<std::string> parseMarkdownIntoBlocks(const std::string& markdown) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto lexer = new Lexer();
    const auto tokens = lexer.lex(markdown || "");
    return tokens.map((token) => token.raw);

}

} // namespace elizaos
