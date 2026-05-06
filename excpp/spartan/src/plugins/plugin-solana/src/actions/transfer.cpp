#include "transfer.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

bool isTransferContent(TransferContent content) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    std::cout << "Content for transfer" << content << std::endl;

    // Base validation
    if (!content.recipient || typeof content.recipient != 'string' || !content.amount) {
        return false;
    }

    if (content.tokenAddress == 'null') {
        content.tokenAddress = nullptr;
    }

    return typeof content.amount == "string" || typeof content.amount == "number";

}

} // namespace elizaos
