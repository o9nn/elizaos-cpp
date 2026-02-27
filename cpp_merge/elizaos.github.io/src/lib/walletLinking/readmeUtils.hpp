#ifndef _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_ELIZAOS_GITHUB_IO_SRC_LIB_WALLETLINKING_READMEUTILS_H
#define _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_ELIZAOS_GITHUB_IO_SRC_LIB_WALLETLINKING_READMEUTILS_H
#include "core.hpp"
#include "zod.hpp"
// External dependency removed

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
