#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_OTC-AGENT_SRC_SERVICES_TOKEN-REGISTRATION-LISTENER-SOLANA_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_OTC-AGENT_SRC_SERVICES_TOKEN-REGISTRATION-LISTENER-SOLANA_H
#include "core.h"
#include "@solana/web3.js.h"

extern boolean isListening;
extern any connection;
void startSolanaListener();

void handleProgramLogs(std::shared_ptr<Logs> logs);

void parseRegisterTokenTransaction(std::shared_ptr<VersionedTransactionResponse> tx);

void backfillSolanaEvents(array<string> signatures = undefined);

#endif
