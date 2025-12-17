#include "facts.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

void formatFacts(const std::vector<Memory>& facts) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return facts;
    .reverse();
    .map((fact: Memory) => fact.content.text)
    .join('\n');

}

} // namespace elizaos
