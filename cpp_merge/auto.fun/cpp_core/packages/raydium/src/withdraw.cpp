#include "withdraw.hpp"
#include <future>
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::future<> execWithdrawTxSafe(Transaction tx, Connection connection, const std:& wallet, PublicKey mint, auto maxRetries) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    signature: std:; logs: std:[]
}

std::future<> execWithdrawTx(Transaction tx, Connection connection, const std:& wallet, auto maxRetries) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    signature: std:; logs: std:[]
}

std::future<> submitWithdrawTx(Transaction tx, Connection connection, const std:& wallet, auto maxRetries) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    signature: std:
}

std::future<> confirmWithdrawTx(const std:& signature, Connection connection) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    logs: std:[]
}

} // namespace elizaos
