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

using Cluster = std::variant<"mainnet", "devnet">;
const getRpcUrl = () => {
  const env = process.env;
  return process.env.NETWORK === "devnet"
    ? process.env.DEVNET_SOLANA_RPC_URL!
    : process.env.MAINNET_SOLANA_RPC_URL!;
};

const txVersion = TxVersion.V0;

let raydium: Raydium | undefined;
const initSdk = async (params: {
  loadToken?: boolean;
  owner?: PublicKey;
}) => {
  const cluster = process.env.NETWORK as Cluster;
  const connection = new Connection(getRpcUrl());
  const owner: Keypair = Keypair.fromSecretKey(
    Uint8Array.from(JSON.parse(process.env.EXECUTOR_PRIVATE_KEY!)),
  );

  if (raydium) return raydium;
  console.log(
    `Raydium SDK: Connected to RPC ${connection.rpcEndpoint} in ${cluster}`,
  );
  raydium = await Raydium.load({
    owner: params?.owner || owner as any,
    connection,
    cluster,
    disableFeatureCheck: true,
    disableLoadToken: !params?.loadToken,
    blockhashCommitment: "finalized",
  });

  return raydium;
};

const fetchTokenAccountData = async () => {
  const env = process.env;
  const connection = new Connection(getRpcUrl());
  const owner: Keypair = Keypair.fromSecretKey(
    Uint8Array.from(JSON.parse(process.env.EXECUTOR_PRIVATE_KEY!)),
  );
  const solAccountResp = await connection.getAccountInfo(owner.publicKey);
  const tokenAccountResp = await connection.getTokenAccountsByOwner(
    owner.publicKey,
    { programId: TOKEN_PROGRAM_ID },
  );
  const token2022Req = await connection.getTokenAccountsByOwner(
    owner.publicKey,
    { programId: TOKEN_2022_PROGRAM_ID },
  );
  const tokenAccountData = parseTokenAccountResp({
    owner: owner.publicKey,
    solAccountResp,
    tokenAccountResp: {
      context: tokenAccountResp.context,
      value: [...tokenAccountResp.value, ...token2022Req.value],
    },
  });
  return tokenAccountData;
};

} // namespace elizaos
