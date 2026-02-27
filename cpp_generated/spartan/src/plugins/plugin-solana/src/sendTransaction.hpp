#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_SPARTAN_SRC_PLUGINS_PLUGIN-SOLANA_SRC_SENDTRANSACTION_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_SPARTAN_SRC_PLUGINS_PLUGIN-SOLANA_SRC_SENDTRANSACTION_H
#include "core.h"
#include "@elizaos/core.h"
#include "@solana/web3.js.h"

std::shared_ptr<Promise<string>> sendTransaction(std::shared_ptr<Connection> connection, array<any> instructions, std::shared_ptr<Keypair> wallet);

#endif
