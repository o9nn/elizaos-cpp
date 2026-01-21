#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_ELIZAOS_GITHUB_IO_SRC_LIB_WALLETLINKING_READMEUTILS_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_ELIZAOS_GITHUB_IO_SRC_LIB_WALLETLINKING_READMEUTILS_H
#include "core.h"
#include "zod.h"
#include "@/lib/walletLinking/chainUtils.h"

typedef z::infer<LinkedWalletSchema> LinkedWallet;

typedef z::infer<WalletLinkingDataSchema> WalletLinkingData;


extern any LinkedWalletSchema;
extern any WalletLinkingDataSchema;
extern string WALLET_SECTION_BEGIN_MARKER;
extern string WALLET_SECTION_END_MARKER;
any parseWalletLinkingDataFromReadme(string readmeContent);

object generateUpdatedReadmeWithWalletInfo(string currentReadme, array<LinkedWallet> wallets);

string generateReadmeWalletSection(array<LinkedWallet> wallets);

string getWalletAddressForChain(any data, string chain);

#endif
