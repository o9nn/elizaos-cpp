#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_SPARTAN_SRC_PLUGINS_PLUGIN-SOLANA_SRC_ACTIONS_TRANSFER_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_SPARTAN_SRC_PLUGINS_PLUGIN-SOLANA_SRC_ACTIONS_TRANSFER_H
#include "core.h"
#include "@elizaos/core.h"
#include "@solana/spl-token.h"
#include "@solana/web3.js.h"
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
