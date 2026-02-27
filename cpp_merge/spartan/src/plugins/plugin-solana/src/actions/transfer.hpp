#ifndef _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_SPARTAN_SRC_PLUGINS_PLUGIN_SOLANA_SRC_ACTIONS_TRANSFER_H
#define _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_SPARTAN_SRC_PLUGINS_PLUGIN_SOLANA_SRC_ACTIONS_TRANSFER_H
#include "core.hpp"
// External dependency removed
// External dependency removed
// External dependency removed
#include "../keypairUtils.h"

class TransferContent;

class TransferContent : public Content, public std::enable_shared_from_this<TransferContent> {
public:
    using std::enable_shared_from_this<TransferContent>::shared_from_this;
    any tokenAddress;

    string recipient;

    any amount;
};

boolean isTransferContent(std::shared_ptr<TransferContent> content);

extern string transferTemplate;
#endif
