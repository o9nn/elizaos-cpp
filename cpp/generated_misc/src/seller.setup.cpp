#include "/home/runner/work/elizaos-cpp/elizaos-cpp/otc-agent/tests/wallet-setup/seller.setup.h"

string SELLER_SEED = OR((process->env->SELLER_SEED_PHRASE), (std::string("test test test test test test test test test test test junk")));
string PASSWORD = OR((process->env->WALLET_PASSWORD), (std::string("Tester@1234")));
object sellerSetup = object{
    object::pair{std::string("walletPassword"), PASSWORD}, 
    object::pair{std::string("seedPhrase"), SELLER_SEED}
};

void Main(void)
{
}

MAIN
