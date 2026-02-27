#ifndef _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_OTC_AGENT_SRC_SERVICES_TOKEN_REGISTRATION_LISTENER_SOLANA_H
#define _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_OTC_AGENT_SRC_SERVICES_TOKEN_REGISTRATION_LISTENER_SOLANA_H
#include "core.hpp"
// External dependency removed

extern boolean isListening;
extern any connection;
void startSolanaListener();

void handleProgramLogs(std::shared_ptr<Logs> logs);

void parseRegisterTokenTransaction(std::shared_ptr<VersionedTransactionResponse> tx);

void backfillSolanaEvents(array<string> signatures = undefined);

#endif
