#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#pragma once

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use

;

struct CustomWallet {
    PublicKey publicKey;
    (tx: any) => Promise<any> signTransaction;
    (txs: any[]) => Promise<any[]> signAllTransactions;
    Keypair payer;
};


class Wallet implements CustomWallet {
  public payer: Keypair;
  constructor(private keypair: Keypair) {
    this.payer = keypair;
  }

  get publicKey() {
    return this.keypair.publicKey;
  }

  async signTransaction(tx: any) {
    tx.partialSign(this.keypair);
    return tx;
  }

  async signAllTransactions(txs: any[]) {
    for (const tx of txs) {
      tx.partialSign(this.keypair);
    }
    return txs;
  }
}

} // namespace elizaos
