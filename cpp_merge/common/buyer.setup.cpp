#include "/home/runner/work/elizaos-cpp/elizaos-cpp/otc-agent/tests/wallet-setup/buyer.setup.h"

string BUYER_SEED = OR((process->env->BUYER_SEED_PHRASE), (std:("test test test test test test test test test test test junk")));
string PASSWORD = OR((process->env->WALLET_PASSWORD), (std:("Tester@1234")));
object buyerSetup = object{
    object::pair{std:("walletPassword"), PASSWORD}, 
    object::pair{std:("seedPhrase"), BUYER_SEED}
};

void Main(void)
{
}

MAIN
