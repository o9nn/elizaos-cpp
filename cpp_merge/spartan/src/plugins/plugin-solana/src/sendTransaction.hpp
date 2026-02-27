#ifndef _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_SPARTAN_SRC_PLUGINS_PLUGIN_SOLANA_SRC_SENDTRANSACTION_H
#define _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_SPARTAN_SRC_PLUGINS_PLUGIN_SOLANA_SRC_SENDTRANSACTION_H
#include "core.hpp"
// External dependency removed
// External dependency removed

std::shared_ptr<Promise<string>> sendTransaction(std::shared_ptr<Connection> connection, array<any> instructions, std::shared_ptr<Keypair> wallet);

#endif
