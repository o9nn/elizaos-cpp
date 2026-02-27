#include "withdraw.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::future<> execWithdrawTxSafe(Transaction tx, Connection connection, const std::any& wallet, PublicKey mint, auto maxRetries) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    signature: std::string; logs: std::string[]
}

std::future<> execWithdrawTx(Transaction tx, Connection connection, const std::any& wallet, auto maxRetries) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    signature: std::string; logs: std::string[]
}

std::future<> submitWithdrawTx(Transaction tx, Connection connection, const std::any& wallet, auto maxRetries) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    signature: std::string
}

std::future<> confirmWithdrawTx(const std::string& signature, Connection connection) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    logs: std::string[]
}

} // namespace elizaos
