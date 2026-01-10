#include "/home/runner/work/elizaos-cpp/elizaos-cpp/otaku/src/plugins/plugin-cdp/constants/timeouts.h"

void waitForTxConfirmation(std::shared_ptr<PublicClient> publicClient, std::shared_ptr<> hash);


double TX_CONFIRMATION_TIMEOUT = 20000;
double BRIDGE_CONFIRMATION_TIMEOUT = 120000;
double BRIDGE_POLL_INTERVAL = 2000;
double BRIDGE_MAX_POLL_ATTEMPTS = BRIDGE_CONFIRMATION_TIMEOUT / BRIDGE_POLL_INTERVAL;

void Main(void)
{
    std::string("0x") + string + string_empty , operationType;
    string = std::string("transaction") , timeout;
    number = TX_CONFIRMATION_TIMEOUT;
    Promise < Void() > object{
        object::pair{std::string("logger"), std::string("logger")}, 
        object::pair{string_empty, ->info(std::string("[Transaction Confirmation] Waiting for ") + operationType + std::string(" confirmation..."))}, 
        object::pair{std::string("const"), receipt = std::async([=]() { publicClient->waitForTransactionReceipt(object{
            object::pair{std::string("hash"), std::string("hash")}, 
            object::pair{std::string("timeout"), std::string("timeout")}
        }); })}, 
        , 
        object::pair{string_empty, ->status != std::string("success")}
    };
    {
        throw any(std::make_shared<Error>(string_empty + (operationType->charAt(0)->toUpperCase() + operationType->slice(1)) + std::string(" transaction reverted on-chain. ") + std::string("The ") + operationType + std::string(" failed - likely due to insufficient balance, slippage, or price impact.")));
    }
    logger->info(std::string("[Transaction Confirmation] ") + operationType + std::string(" confirmed successfully in block ") + receipt->blockNumber + string_empty);
}

MAIN
