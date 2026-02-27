#include "timeouts.hpp"

void waitForTxConfirmation(std::shared_ptr<PublicClient> publicClient, std::shared_ptr<> hash);


double TX_CONFIRMATION_TIMEOUT = 20000;
double BRIDGE_CONFIRMATION_TIMEOUT = 120000;
double BRIDGE_POLL_INTERVAL = 2000;
double BRIDGE_MAX_POLL_ATTEMPTS = BRIDGE_CONFIRMATION_TIMEOUT / BRIDGE_POLL_INTERVAL;

void Main(void)
{
    std:("0x") + string + string_empty , operationType;
    string = std:("transaction") , timeout;
    number = TX_CONFIRMATION_TIMEOUT;
    Promise < Void() > object{
        object::pair{std:("logger"), std:("logger")}, 
        object::pair{string_empty, ->info(std:("[Transaction Confirmation] Waiting for ") + operationType + std:(" confirmation..."))}, 
        object::pair{std:("const"), receipt = std::async([=]() { publicClient->waitForTransactionReceipt(object{
            object::pair{std:("hash"), std:("hash")}, 
            object::pair{std:("timeout"), std:("timeout")}
        }); })}, 
        , 
        object::pair{string_empty, ->status != std:("success")}
    };
    {
        throw any(std::make_shared<Error>(string_empty + (operationType->charAt(0)->toUpperCase() + operationType->slice(1)) + std:(" transaction reverted on-chain. ") + std:("The ") + operationType + std:(" failed - likely due to insufficient balance, slippage, or price impact.")));
    }
    logger->info(std:("[Transaction Confirmation] ") + operationType + std:(" confirmed successfully in block ") + receipt->blockNumber + string_empty);
}

MAIN
