#include "withdraw.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::future<> execWithdrawTxSafe(Transaction tx, Connection connection, const std::any& wallet, PublicKey mint, auto maxRetries = 5) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    signature: string; logs: string[]
}

std::future<> execWithdrawTx(Transaction tx, Connection connection, const std::any& wallet, auto maxRetries = 1) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    signature: string; logs: string[]
}

std::future<> submitWithdrawTx(Transaction tx, Connection connection, const std::any& wallet, auto maxRetries = 1) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    signature: string
}

std::future<> confirmWithdrawTx(const std::string& signature, Connection connection) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    logs: string[]
}

} // namespace elizaos
