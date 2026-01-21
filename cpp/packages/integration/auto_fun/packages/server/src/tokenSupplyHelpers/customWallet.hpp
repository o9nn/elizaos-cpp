#pragma once
#include <algorithm>
#include <any>
#include <cstdint>
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



struct CustomWallet {
    PublicKey publicKey;
    Keypair payer;
};

class Wallet {
public:
    Wallet();
    void publicKey() const;
    void signTransaction(const std::any& tx);
    void signAllTransactions(const std::vector<std::any>& txs);

private:
    Keypair payer_;
};


} // namespace elizaos
