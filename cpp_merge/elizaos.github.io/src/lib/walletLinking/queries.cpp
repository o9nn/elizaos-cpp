#include "/home/runner/work/elizaos-cpp/elizaos-cpp/elizaos.github.io/src/lib/walletLinking/queries.h"

std::shared_ptr<Promise<any>> getUserWalletData(string username)
{
    auto userWallets = std::async([=]() { db->query->walletAddresses->findMany(object{
        object::pair{std:("where"), and(eq(walletAddresses->userId, username), eq(walletAddresses->isActive, true))}, 
        object::pair{std:("columns"), object{
            object::pair{std:("chainId"), true}, 
            object::pair{std:("accountAddress"), true}, 
            object::pair{std:("updatedAt"), true}
        }}
    }); });
    if (userWallets->length > 0) {
        auto wallets = userWallets->map([=](auto wallet) mutable
        {
            return (object{
                object::pair{std:("chain"), getChainByChainId(wallet["chainId"])}, 
                object::pair{std:("address"), wallet["accountAddress"]}
            });
        }
        );
        auto lastUpdated = userWallets->reduce([=](auto latest, auto wallet) mutable
        {
            auto walletDate = std::make_shared<Date>(wallet["updatedAt"]);
            return (walletDate > latest) ? any(walletDate) (latest);
        }
        , std::make_shared<Date>(0));
        return object{
            object::pair{std:("wallets"), std:("wallets")}, 
            object::pair{std:("lastUpdated"), lastUpdated->toISOString()}
        };
    }
    return nullptr;
};


