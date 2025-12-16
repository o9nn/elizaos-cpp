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

const fixedPoint = parseFloat("1000000000");
const vaultConfigSeed = "raydium_vault_config";
const positionSeed = "raydium_position";
const claimerInfoSeed = "raydium_claimer_info";
const nftFaucetSeed = "raydium_vault_nft_seed";

/// USDC
const token0 = new anchor.web3.PublicKey(
  "EPjFWdd5AufqSSqeM2qN1xzybapC8G4wEGGkZwyTDt1v",
);

const claimer_address_0 = new anchor.web3.PublicKey(
  "6HHoqvXfNF1aQpwhn4k13CL7iyzFpjghLhG2eBG6xMVV",
);

const devnetEndpoint = "https://api.devnet.solana.com";

async  catch (error) {
      if (attempt === maxRetries - 1) throw error;
      await new Promise((resolve) => setTimeout(resolve, delay));
    }
  }
  throw new Error("Unreachable");
}

const sendSolTo = async (
  amount: any,
  signerWallet: any,
  recvWallet: anchor.web3.PublicKey,
  connection: anchor.web3.Connection,
) => {
  const beforeBal = await connection.getBalance(recvWallet);
  console.log("beforeBal: ", parseFloat(beforeBal.toString()) / fixedPoint);
  const transaction = new Transaction().add(
    SystemProgram.transfer({
      fromPubkey: signerWallet.publicKey,
      toPubkey: recvWallet,
      lamports: amount,
    }),
  );

  try {
    const signature = await sendAndConfirmTransaction(connection, transaction, [
      signerWallet,
    ]);
    console.log("confirmed transaction with signature ", signature);
    const afterBal = await connection.getBalance(recvWallet);
    console.log("afterBal: ", parseFloat(afterBal.toString()) / fixedPoint);
    return signature;
  } catch (error: any) {
    console.log("transaction failed: ", error);
  }

  // const afterBal = await connection.getBalance(recvWallet);
  // console.log("afterBal: ", parseFloat(afterBal.toString()) / fixedPoint);
};

const sendTokenTo = async (
  amount: any,
  signerWallet: any,
  recvWallet: anchor.web3.PublicKey,
  tokenAddress: anchor.web3.PublicKey,
  connection: anchor.web3.Connection,
) => {
  const signerTokenAccount = spl.getAssociatedTokenAddressSync(
    tokenAddress,
    signerWallet.publicKey,
  );
  const bobTokenAccount = spl.getAssociatedTokenAddressSync(
    tokenAddress,
    recvWallet,
  );

  const beforeSignerBal =
    await connection.getTokenAccountBalance(signerTokenAccount);
  const beforeBobBal = await connection.getTokenAccountBalance(bobTokenAccount);

  const transaction = new Transaction().add(
    spl.createTransferInstruction(
      signerTokenAccount,
      bobTokenAccount,
      signerWallet.publicKey,
      amount,
      [],
      spl.TOKEN_PROGRAM_ID,
    ),
  );
  const signature = await connection.sendTransaction(transaction, [
    signerWallet,
  ]);
  await connection.confirmTransaction(signature, "confirmed");

  const afterSignerBal =
    await connection.getTokenAccountBalance(signerTokenAccount);
  const afterBobBal = await connection.getTokenAccountBalance(bobTokenAccount);
};

const sendNftTo = async (
  signerWallet: Keypair,
  recvWallet: anchor.web3.PublicKey,
  nftMinted: anchor.web3.PublicKey,
  connection: anchor.web3.Connection,
) => {
  try {
    // Derive the associated token addresses
    const signerTokenAccount = spl.getAssociatedTokenAddressSync(
      nftMinted,
      signerWallet.publicKey,
    );
    const bobTokenAccount = spl.getAssociatedTokenAddressSync(
      nftMinted,
      recvWallet,
    );

    // 1) If recipient ATA doesn't exist, create it
    const toAtaInfo = await connection.getAccountInfo(bobTokenAccount);
    const instructions = [] as TransactionInstruction[];
    if (!toAtaInfo) {
      instructions.push(
        spl.createAssociatedTokenAccountInstruction(
          signerWallet.publicKey, // payer
          bobTokenAccount,
          recvWallet,
          nftMinted, // mint
        ) as any, // Type assertion to avoid type error
      );
    }

    // Create the transfer instruction
    const transferIx = spl.createTransferInstruction(
      signerTokenAccount,
      bobTokenAccount,
      signerWallet.publicKey,
      1, // transferring one NFT (1 token)
      [],
      spl.TOKEN_PROGRAM_ID,
    );
    instructions.push(transferIx);

    // Get the latest blockhash needed for the transaction
    const latestBlockhash = await connection.getLatestBlockhash();

    // Create a message using the new TransactionMessage API
    const messageV0 = new TransactionMessage({
      payerKey: signerWallet.publicKey,
      recentBlockhash: latestBlockhash.blockhash,
      instructions: instructions,
    }).compileToV0Message();

    // Build the versioned transaction
    const transaction = new VersionedTransaction(messageV0);

    // Sign the transaction with the signer wallet
    transaction.sign([signerWallet]);

    // Send the versioned transaction
    const signature = await connection.sendTransaction(transaction);

    // Confirm the transaction using the latest blockhash context
    await retryOperation(
      async () => {
        await connection.confirmTransaction(
          {
            signature,
            blockhash: latestBlockhash.blockhash,
            lastValidBlockHeight: latestBlockhash.lastValidBlockHeight,
          },
          "finalized",
        );
      },
      3, // 3 attempts
      2000, // 2 seconds delay
    );
    return signature;
  } catch (error) {
    console.error("Error in sendNftTo:", error);
    throw error;
  }
};

const isDevnet = (connection: anchor.web3.Connection): boolean => {
  return connection.rpcEndpoint == devnetEndpoint;
};

} // namespace elizaos
