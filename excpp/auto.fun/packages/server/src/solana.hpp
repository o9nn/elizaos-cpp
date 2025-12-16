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
;
const idl: Autofun = JSON.parse(JSON.stringify(idlJson));

;
;
// Initialize the Solana configuration with the provided environment


  // Create wallet if private key is available
  let wallet: Keypair | undefined;

  if(!process.env.EXECUTOR_PRIVATE_KEY){
    throw new Error("missing EXECUTOR_PRIVATE_KEY env var");
  }

    try {
      wallet = Keypair.fromSecretKey(
        Uint8Array.from(JSON.parse(process.env.EXECUTOR_PRIVATE_KEY)),
      );
      console.log("Created wallet from process.env.EXECUTOR_PRIVATE_KEY");
    } catch (error) {
      console.error("Failed to create wallet from env:", error);
    }

  // Return configuration object
  return {
    umi,
    connection: new Connection(rpcUrl),
    program: null, // Will be initialized later if anchor is used
    programId: new PublicKey(programId),
    wallet,
    network,
  };
}

const getProgram = (connection: Connection, wallet: any) => {
  const provider = new AnchorProvider(connection, wallet, {
    skipPreflight: true,
    commitment: "confirmed",
  });

  return new Program<Autofun>(idl, provider);
};

} // namespace elizaos
