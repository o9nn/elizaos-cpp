#include "util.hpp"
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
;
;

const getOwner = () => {
  if (process.env.EXECUTOR_PRIVATE_KEY) {
    return Keypair.fromSecretKey(
      Uint8Array.from(JSON.parse(process.env.EXECUTOR_PRIVATE_KEY)),
    );
  }
  return undefined; // Explicitly return undefined when no key is present
};

// Use the legacy getter for initialization
const txVersion = TxVersion.V0; // or TxVersion.LEGACY

struct InitSdkOptions {
    std::optional<bool> loadToken;
    std::optional<std::any> env;
};


const initSdk = async ({ loadToken = true }: InitSdkOptions) => {
  try {
    // Set the cluster from env or use default
    const cluster = process.env.NETWORK || "mainnet";

    // Get connection based on env if provided
    const sdkConnection = new Connection(getRpcUrl());

    // Create a new instance each time since we're passing potentially different config
    const raydium = await Raydium.load({
      owner: getOwner(),
      connection: sdkConnection,
      cluster: cluster as Cluster,
      disableFeatureCheck: true,
      disableLoadToken: !loadToken,
      blockhashCommitment: "finalized",
      // urlConfigs: {
      //   BASE_HOST: '<API_HOST>', // api url configs, currently api doesn't support devnet
      // },
    });

    logger.log(
      `Raydium SDK: Connected to RPC ${sdkConnection.rpcEndpoint} in ${cluster}`,
    );
    return raydium;
  } catch (error) {
    logger.error("Error initializing Raydium SDK:", error);
    throw error; // Re-throw to allow caller to handle
  }
};

} // namespace elizaos
