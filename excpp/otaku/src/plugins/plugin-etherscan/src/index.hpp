#include "actions/checkTransactionConfirmation.action.hpp"
#include "services/etherscan.service.hpp"
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

import type { Plugin } from "@elizaos/core";
;
;

const etherscanPlugin: Plugin = {
  name: "etherscan",
  description:
    "Etherscan integration for checking transaction confirmations, contract verification status, and blockchain data on Ethereum and other EVM chains",
  actions: [checkTransactionConfirmationAction],
  services: [EtherscanService],
  evaluators: [],
  providers: [],
};

default etherscanPlugin;
{ EtherscanService, checkTransactionConfirmationAction };


} // namespace elizaos
