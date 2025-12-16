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

const withdrawTx = async (
  user: PublicKey,
  token: PublicKey,
  connection: Connection,
  program: Program<Autofun>,
) => {
  const tx = await program.methods
    .withdraw()
    .accounts({
      admin: user,
      tokenMint: token,
    })
    .transaction();

  tx.feePayer = user;
  tx.recentBlockhash = (await connection.getLatestBlockhash()).blockhash;

  return tx;
};


std::future<{ signature: string; logs: string[] }> execWithdrawTxSafe(Transaction tx, Connection connection, const std::any& wallet, PublicKey mint, auto maxRetries = 5);

  const signature = await connection.sendRawTransaction(
    signedTx.serialize(),
    { skipPreflight: true, preflightCommitment: "confirmed", maxRetries: 3 }
  );


  for (let attempt = 0; attempt < maxRetries; attempt++) {
    try {
      // each time, fetch latest blockhash fresh
      const { blockhash, lastValidBlockHeight } =
        await connection.getLatestBlockhash();

      const confirmation = await connection.confirmTransaction(
        { signature, blockhash, lastValidBlockHeight },
        "confirmed"
      );

      if (
        confirmation.value.err &&
        JSON.stringify(confirmation.value.err).includes("ProgramFailedToComplete")
      ) {
        console.log(
          `[Withdraw] program error on confirm, attempt ${attempt + 1}`
        );

      } else if (confirmation.value.err) {

        throw new Error(
          `Transaction failed: ${JSON.stringify(confirmation.value.err)}`
        );
      } else {
        const info = await connection.getTransaction(signature, {
          maxSupportedTransactionVersion: 0,
        });
        const onChainLogs = info?.meta?.logMessages || [];
        return {
          signature,
          logs: [...preflightLogs, ...onChainLogs],
        };
      }

      await new Promise((r) => setTimeout(r, 2000));
    } catch (err) {
      if (attempt === maxRetries - 1) break;
      console.warn(`[Withdraw] confirm attempt #${attempt + 1} threw:`, err);
      await new Promise((r) => setTimeout(r, 2000));
    }
  }

  console.warn(
    `[Withdraw] all confirm attempts failed; fetching last tx for ${mint.toBase58()}`
  );
  const sigInfos = await connection.getSignaturesForAddress(mint, {
    limit: 1,
  });
  if (sigInfos.length === 0) {
    throw new Error(
      `Could not confirm tx ${signature}, and no fallback tx found for ${mint.toBase58()}`
    );
  }

  const lastSig = sigInfos[0].signature;
  const lastInfo = await connection.getTransaction(lastSig, {
    maxSupportedTransactionVersion: 0,
  });
  const backupLogs = lastInfo?.meta?.logMessages || [];
  const foundWithdraw = backupLogs.some((log) =>
    log.includes("Program log: Instruction: Withdraw")
  );
  if (!foundWithdraw) {
    throw new Error(
      `Fallback tx ${lastSig} did not include "Instruction: Withdraw" in its logs`
    );
  }
  console.log(
    `[Withdraw] using fallback signature ${lastSig}, collected ${backupLogs.length} logs`
  );

  return {
    signature: lastSig,
    logs: [...preflightLogs, ...backupLogs],
  };
}

std::future<{ signature: string; logs: string[] }> execWithdrawTx(Transaction tx, Connection connection, const std::any& wallet, auto maxRetries = 1);,
    );

    if (!signature) {
      throw new Error("Transaction failed to send");
    }
    let logs;
    // Wait for confirmation
    const confirmation = await connection.confirmTransaction(
      {
        signature,
        blockhash: tx.recentBlockhash!,
        lastValidBlockHeight: (await connection.getLatestBlockhash())
          .lastValidBlockHeight,
      },
      "confirmed",
    );

    // Check if we got ProgramFailedToComplete but program actually succeeded
    if (
      confirmation.value.err === "ProgramFailedToComplete" ||
      (confirmation.value.err &&
        JSON.stringify(confirmation.value.err).includes(
          "ProgramFailedToComplete",
        ))
    ) {
      // Get transaction logs to verify actual execution
      const txInfo = await connection.getTransaction(signature, {
        maxSupportedTransactionVersion: 0,
      });

      if (
        txInfo?.meta?.logMessages?.some((log) =>
          log.includes(`Program success`),
        )
      ) {
        console.log(
          "Transaction succeeded despite ProgramFailedToComplete error",
        );
        logs = txInfo.meta.logMessages;
        return { signature, logs: txInfo.meta.logMessages };
      }
    } else if (confirmation.value.err) {
      throw new Error(
        `Transaction failed: ${JSON.stringify(confirmation.value.err)}`,
      );
    }

    console.log("Transaction succeeded");

    return { signature, logs: logs as unknown as string[] };
  } catch (error: any) {
    lastError = error;
    console.error(`Withdrawal execution failed:`, error);

    throw error;
  }
}

// Submit the withdrawal transaction without waiting for full confirmation.
std::future<{ signature: string }> submitWithdrawTx(Transaction tx, Connection connection, const std::any& wallet, auto maxRetries = 1);`,
        );
      }
      console.log("Simulation result:", simulation);

      const signature = await connection.sendRawTransaction(
        signedTx.serialize(),
        {
          skipPreflight: true,
          maxRetries: 2,
          preflightCommitment: "confirmed",
        },
      );

      if (!signature) {
        throw new Error("Transaction failed to send");
      }

      console.log("Transaction submitted with signature:", signature);
      return { signature };
    } catch (error: any) {
      lastError = error;
      console.error(`Submit withdrawal attempt ${i + 1} failed:`, error);
      await new Promise((resolve) =>
        setTimeout(resolve, Math.min(1000 * Math.pow(2, i), 15000)),
      );
    }
  }

  throw (
    lastError || new Error("Max retries exceeded while submitting withdrawal")
  );
}

// Wait for confirmation and return logs.
std::future<{ logs: string[] }> confirmWithdrawTx(const std::string& signature, Connection connection);,
    "confirmed",
  );

  if (confirmation.value.err) {
    throw new Error(
      `Transaction failed: ${JSON.stringify(confirmation.value.err)}`,
    );
  }

  const txInfo = await connection.getTransaction(signature, {
    maxSupportedTransactionVersion: 0,
  });

  if (!txInfo || !txInfo.meta) {
    throw new Error("Unable to fetch transaction info");
  }

  return { logs: txInfo.meta.logMessages || [] };
}

} // namespace elizaos
