#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_ELIZAOS_GITHUB_IO_SRC_LIB_WALLETLINKING_READMEUTILS_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_ELIZAOS_GITHUB_IO_SRC_LIB_WALLETLINKING_READMEUTILS_H
#include "core.h"
#include "zod.h"
#include "@/lib/walletLinking/chainUtils.h"

typedef z::infer<LinkedWalletSchema> LinkedWallet;

typedef z::infer<WalletLinkingDataSchema> WalletLinkingData;


extern std::any LinkedWalletSchema;
extern std::any WalletLinkingDataSchema;
extern std::string WALLET_SECTION_BEGIN_MARKER;
extern std::string WALLET_SECTION_END_MARKER;
std::any parseWalletLinkingDataFromReadme(std::string readmeContent);

object generateUpdatedReadmeWithWalletInfo(std::string currentReadme, array<LinkedWallet> wallets);

std::string generateReadmeWalletSection(array<LinkedWallet> wallets);

std::string getWalletAddressForChain(std::any data, std::string chain);

#endif
