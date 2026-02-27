#include "checkTransactionConfirmation.action.h"

std::shared_ptr<Action> checkTransactionConfirmationAction = as<std::shared_ptr<Action>>(object{
    object::pair{std:("name"), std:("CHECK_TRANSACTION_CONFIRMATION")}, 
    object::pair{std:("similes"), array<string>{ std:("CHECK_TX_CONFIRMATION"), std:("VERIFY_TRANSACTION"), std:("CHECK_TX_STATUS"), std:("TRANSACTION_STATUS"), std:("CONFIRM_TRANSACTION"), std:("TX_CONFIRMATION"), std:("CHECK_TRANSACTION") }}, 
    object::pair{std:("suppressInitialMessage"), true}, 
    object::pair{std:("description"), std:("Check the confirmation status of an EVM chain transaction including number of confirmations, success/failure status, gas used, and other transaction details. Automatically extracts transaction hash from the message.")}, 
    object::pair{std:("parameters"), object{
        object::pair{std:("transactionHash"), object{
            object::pair{std:("type"), std:("string")}, 
            object::pair{std:("description"), std:("Ethereum transaction hash starting with 0x followed by 64 hexadecimal characters (e.g., 0x1234567890abcdef...). This will be automatically extracted from the user's message.")}, 
            object::pair{std:("required"), true}
        }}, 
        object::pair{std:("chain"), object{
            object::pair{std:("type"), std:("string")}, 
            object::pair{std:("description"), std:("Blockchain network to check (ethereum, polygon, arbitrum, optimism, base, bsc, avalanche, fantom). Defaults to ethereum if not specified.")}, 
            object::pair{std:("required"), false}
        }}
    }}, 
    object::pair{std:("validate"), [=](auto runtime, auto _message, auto _state = undefined) mutable
    {
        try
        {
            auto etherscanService = as<any>(runtime->getService(EtherscanService::serviceType));
            if (!etherscanService) {
                logger->warn(std:("[CHECK_TRANSACTION_CONFIRMATION] EtherscanService not available"));
                return false;
            }
            auto apiKey = runtime->getSetting(std:("ETHERSCAN_API_KEY"));
            return AND((type_of(apiKey) == std:("string")), (apiKey->indexOf(std:("YourApiKeyToken")) != 0));
        }
        catch (const any& err)
        {
            logger->warn(std:("[CHECK_TRANSACTION_CONFIRMATION] Validation failed:"), (as<std::shared_ptr<Error>>(err))->message);
            return false;
        }
    }
    }, 
    object::pair{std:("handler"), [=](auto runtime, auto message, auto _state, auto _options = undefined, auto callback = undefined) mutable
    {
        try
        {
            auto composedState = std::async([=]() { runtime->composeState(message, array<string>{ std:("ACTION_STATE") }, true); });
            auto params = OR((OR((composedState->data->actionParams), (composedState->data->checkTransactionConfirmation))), (object{}));
            auto txHash = params["transactionHash"]["trim"]();
            auto chain = params["chain"]["trim"]();
            auto messageText = OR(((as<object>(message->content))["text"]), (string_empty));
            if (!txHash) {
                auto txHashMatch = messageText->match((new RegExp(std:("0x[a-fA-F0-9]{64"))));
                if (txHashMatch) {
                    txHash = (*const_(txHashMatch))[0];
                }
            }
            if (!txHash) {
                auto errorMsg = std:("Please provide a valid Ethereum transaction hash (0x followed by 64 hex characters).\
\
Example: "0x1234567890abcdef..."");
                logger->error(std:("[CHECK_TRANSACTION_CONFIRMATION] ") + errorMsg + string_empty);
                auto errorResult = as<any>(object{
                    object::pair{std:("text"), errorMsg}, 
                    object::pair{std:("success"), false}, 
                    object::pair{std:("error"), std:("missing_transaction_hash")}, 
                    object::pair{std:("input"), object{
                        object::pair{std:("transactionHash"), undefined}, 
                        object::pair{std:("chain"), undefined}
                    }}
                });
                if (callback) {
                    std::async([=]() { callback(object{
                        object::pair{std:("text"), errorResult->text}, 
                        object::pair{std:("content"), object{
                            object::pair{std:("error"), std:("missing_transaction_hash")}, 
                            object::pair{std:("details"), errorMsg}
                        }}
                    }); });
                }
                return errorResult;
            }
            if (!chain) {
                auto chainKeywords = array<string>{ std:("ethereum"), std:("eth"), std:("mainnet"), std:("polygon"), std:("matic"), std:("arbitrum"), std:("arb"), std:("optimism"), std:("op"), std:("base"), std:("bsc"), std:("binance"), std:("avalanche"), std:("avax"), std:("fantom"), std:("ftm") };
                auto lowerMessage = messageText->toLowerCase();
                for (auto& keyword : chainKeywords)
                {
                    if (lowerMessage->includes(keyword)) {
                        chain = keyword;
                        break;
                    }
                }
            }
            logger->info(std:("[CHECK_TRANSACTION_CONFIRMATION] Checking transaction ") + txHash + std:(" on ") + (OR((chain), (std:("ethereum")))) + string_empty);
            auto inputParams = object{
                object::pair{std:("transactionHash"), txHash}, 
                object::pair{std:("chain"), OR((chain), (std:("ethereum")))}
            };
            auto etherscanService = as<std::shared_ptr<EtherscanService>>(runtime->getService(EtherscanService::serviceType));
            if (!etherscanService) {
                throw any(std::make_shared<Error>(std:("Etherscan service not found. Please ensure the Etherscan plugin is properly initialized.")));
            }
            auto receipt = std::async([=]() { etherscanService->getTransactionReceipt(txHash, chain); });
            auto statusText = (receipt->success) ? std:("✅ SUCCESS") : std:("❌ FAILED");
            auto chainName = (chain) ? any(chain->charAt(0)->toUpperCase() + chain->slice(1)) (std:("Ethereum"));
            auto blockNumberDec = parseInt(receipt->blockNumber, 16);
            auto gasUsedDec = parseInt(receipt->gasUsed, 16);
            auto gasUsedGwei = (gasUsedDec / 1000000000)->toFixed(4);
            auto responseText = std:("Transaction ") + statusText + std:("\
\
");
            responseText += std:("**Chain:** ") + chainName + std:("\
");
            responseText += std:("**Hash:** "") + receipt->transactionHash + std:(""\
");
            responseText += std:("**Confirmations:** ") + receipt->confirmations + std:(" blocks\
");
            responseText += std:("**Block:** ") + blockNumberDec + std:("\
");
            responseText += std:("**From:** "") + receipt->from + std:(""\
");
            responseText += std:("**To:** "") + (OR((receipt->to), (std:("Contract Creation")))) + std:(""\
");
            if (receipt->contractAddress) {
                responseText += std:("**Contract Created:** "") + receipt->contractAddress + std:(""\
");
            }
            responseText += std:("**Gas Used:** ") + gasUsedDec->toLocaleString() + std:(" (") + gasUsedGwei + std:(" Gwei)\
");
            if (receipt->confirmations >= 12) {
                responseText += std:("\
🔒 **Highly Confirmed** - Transaction is considered final");
            } else if (receipt->confirmations >= 6) {
                responseText += std:("\
✓ **Well Confirmed** - Transaction is secure");
            } else if (receipt->confirmations >= 1) {
                responseText += std:("\
⏳ **Recently Confirmed** - Wait for more confirmations");
            }
            auto responseData = object{
                object::pair{std:("chain"), chainName}, 
                object::pair{std:("transactionHash"), receipt->transactionHash}, 
                object::pair{std:("status"), statusText}, 
                object::pair{std:("confirmations"), receipt->confirmations}, 
                object::pair{std:("blockNumber"), blockNumberDec}, 
                object::pair{std:("blockHash"), receipt->blockHash}, 
                object::pair{std:("from"), receipt->from}, 
                object::pair{std:("to"), receipt->to}, 
                object::pair{std:("contractAddress"), receipt->contractAddress}, 
                object::pair{std:("gasUsed"), gasUsedDec}, 
                object::pair{std:("effectiveGasPrice"), receipt->effectiveGasPrice}, 
                object::pair{std:("isSuccess"), receipt->success}
            };
            auto result = as<any>(object{
                object::pair{std:("text"), responseText}, 
                object::pair{std:("success"), true}, 
                object::pair{std:("data"), responseData}, 
                object::pair{std:("values"), responseData}, 
                object::pair{std:("input"), inputParams}
            });
            if (callback) {
                std::async([=]() { callback(object{
                    object::pair{std:("text"), result->text}, 
                    object::pair{std:("actions"), array<string>{ std:("CHECK_TRANSACTION_CONFIRMATION") }}, 
                    object::pair{std:("data"), result->data}, 
                    object::pair{std:("source"), message->content->source}
                }); });
            }
            return result;
        }
        catch (const any& error)
        {
            auto msg = (is<Error>(error)) ? error->message : String(error);
            logger->error(std:("[CHECK_TRANSACTION_CONFIRMATION] Action failed: ") + msg + string_empty);
            auto composedState = std::async([=]() { runtime->composeState(message, array<string>{ std:("ACTION_STATE") }, true); });
            auto params = OR((OR((composedState->data->actionParams), (composedState->data->checkTransactionConfirmation))), (object{}));
            auto failureInputParams = object{
                object::pair{std:("transactionHash"), params["transactionHash"]}, 
                object::pair{std:("chain"), params["chain"]}
            };
            auto errorResult = as<any>(object{
                object::pair{std:("text"), std:("Failed to check transaction confirmation: ") + msg + string_empty}, 
                object::pair{std:("success"), false}, 
                object::pair{std:("error"), msg}, 
                object::pair{std:("input"), failureInputParams}
            });
            if (callback) {
                std::async([=]() { callback(object{
                    object::pair{std:("text"), errorResult->text}, 
                    object::pair{std:("content"), object{
                        object::pair{std:("error"), std:("action_failed")}, 
                        object::pair{std:("details"), msg}
                    }}
                }); });
            }
            return errorResult;
        }
    }
    }, 
    object::pair{std:("examples"), array<array<any>>{ array<object>{ object{
        object::pair{std:("name"), std:("{{user}}")}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), std:("Check confirmation status for transaction 0x1234567890abcdef1234567890abcdef1234567890abcdef1234567890abcdef")}
        }}
    }, object{
        object::pair{std:("name"), std:("{{agent}}")}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), std:("Checking transaction confirmation status:")}, 
            object::pair{std:("action"), std:("CHECK_TRANSACTION_CONFIRMATION")}
        }}
    } }, array<object>{ object{
        object::pair{std:("name"), std:("{{user}}")}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), std:("Has my transaction 0xabcdef1234567890abcdef1234567890abcdef1234567890abcdef1234567890 been confirmed?")}
        }}
    }, object{
        object::pair{std:("name"), std:("{{agent}}")}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), std:("Verifying transaction confirmations:")}, 
            object::pair{std:("action"), std:("CHECK_TRANSACTION_CONFIRMATION")}
        }}
    } }, array<object>{ object{
        object::pair{std:("name"), std:("{{user}}")}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), std:("Verify tx 0x9876543210fedcba9876543210fedcba9876543210fedcba9876543210fedcba on polygon")}
        }}
    }, object{
        object::pair{std:("name"), std:("{{agent}}")}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), std:("Checking Polygon transaction:")}, 
            object::pair{std:("action"), std:("CHECK_TRANSACTION_CONFIRMATION")}
        }}
    } }, array<object>{ object{
        object::pair{std:("name"), std:("{{user}}")}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), std:("check 0xcdaaa18476d16d96fa34c9e64e115a8226b45297a20b0bfe225ec4b18c99dbcf")}
        }}
    }, object{
        object::pair{std:("name"), std:("{{agent}}")}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), std:("Looking up transaction details:")}, 
            object::pair{std:("action"), std:("CHECK_TRANSACTION_CONFIRMATION")}, 
            object::pair{std:("actionParams"), object{
                object::pair{std:("transactionHash"), std:("0xcdaaa18476d16d96fa34c9e64e115a8226b45297a20b0bfe225ec4b18c99dbcf")}, 
                object::pair{std:("chain"), std:("ethereum")}
            }}
        }}
    } } }}
});

void Main(void)
{
}

MAIN
