#include "/home/runner/work/elizaos-cpp/elizaos-cpp/otaku/src/plugins/plugin-etherscan/src/actions/checkTransactionConfirmation.action.h"

std::shared_ptr<Action> checkTransactionConfirmationAction = as<std::shared_ptr<Action>>(object{
    object::pair{std::string("name"), std::string("CHECK_TRANSACTION_CONFIRMATION")}, 
    object::pair{std::string("similes"), array<string>{ std::string("CHECK_TX_CONFIRMATION"), std::string("VERIFY_TRANSACTION"), std::string("CHECK_TX_STATUS"), std::string("TRANSACTION_STATUS"), std::string("CONFIRM_TRANSACTION"), std::string("TX_CONFIRMATION"), std::string("CHECK_TRANSACTION") }}, 
    object::pair{std::string("suppressInitialMessage"), true}, 
    object::pair{std::string("description"), std::string("Check the confirmation status of an EVM chain transaction including number of confirmations, success/failure status, gas used, and other transaction details. Automatically extracts transaction hash from the message.")}, 
    object::pair{std::string("parameters"), object{
        object::pair{std::string("transactionHash"), object{
            object::pair{std::string("type"), std::string("string")}, 
            object::pair{std::string("description"), std::string("Ethereum transaction hash starting with 0x followed by 64 hexadecimal characters (e.g., 0x1234567890abcdef...). This will be automatically extracted from the user's message.")}, 
            object::pair{std::string("required"), true}
        }}, 
        object::pair{std::string("chain"), object{
            object::pair{std::string("type"), std::string("string")}, 
            object::pair{std::string("description"), std::string("Blockchain network to check (ethereum, polygon, arbitrum, optimism, base, bsc, avalanche, fantom). Defaults to ethereum if not specified.")}, 
            object::pair{std::string("required"), false}
        }}
    }}, 
    object::pair{std::string("validate"), [=](auto runtime, auto _message, auto _state = undefined) mutable
    {
        try
        {
            auto etherscanService = as<any>(runtime->getService(EtherscanService::serviceType));
            if (!etherscanService) {
                logger->warn(std::string("[CHECK_TRANSACTION_CONFIRMATION] EtherscanService not available"));
                return false;
            }
            auto apiKey = runtime->getSetting(std::string("ETHERSCAN_API_KEY"));
            return AND((type_of(apiKey) == std::string("string")), (apiKey->indexOf(std::string("YourApiKeyToken")) != 0));
        }
        catch (const any& err)
        {
            logger->warn(std::string("[CHECK_TRANSACTION_CONFIRMATION] Validation failed:"), (as<std::shared_ptr<Error>>(err))->message);
            return false;
        }
    }
    }, 
    object::pair{std::string("handler"), [=](auto runtime, auto message, auto _state, auto _options = undefined, auto callback = undefined) mutable
    {
        try
        {
            auto composedState = std::async([=]() { runtime->composeState(message, array<string>{ std::string("ACTION_STATE") }, true); });
            auto params = OR((OR((composedState->data->actionParams), (composedState->data->checkTransactionConfirmation))), (object{}));
            auto txHash = params["transactionHash"]["trim"]();
            auto chain = params["chain"]["trim"]();
            auto messageText = OR(((as<object>(message->content))["text"]), (string_empty));
            if (!txHash) {
                auto txHashMatch = messageText->match((new RegExp(std::string("0x[a-fA-F0-9]{64"))));
                if (txHashMatch) {
                    txHash = (*const_(txHashMatch))[0];
                }
            }
            if (!txHash) {
                auto errorMsg = std::string("Please provide a valid Ethereum transaction hash (0x followed by 64 hex characters).\
\
Example: `0x1234567890abcdef...`");
                logger->error(std::string("[CHECK_TRANSACTION_CONFIRMATION] ") + errorMsg + string_empty);
                auto errorResult = as<any>(object{
                    object::pair{std::string("text"), errorMsg}, 
                    object::pair{std::string("success"), false}, 
                    object::pair{std::string("error"), std::string("missing_transaction_hash")}, 
                    object::pair{std::string("input"), object{
                        object::pair{std::string("transactionHash"), undefined}, 
                        object::pair{std::string("chain"), undefined}
                    }}
                });
                if (callback) {
                    std::async([=]() { callback(object{
                        object::pair{std::string("text"), errorResult->text}, 
                        object::pair{std::string("content"), object{
                            object::pair{std::string("error"), std::string("missing_transaction_hash")}, 
                            object::pair{std::string("details"), errorMsg}
                        }}
                    }); });
                }
                return errorResult;
            }
            if (!chain) {
                auto chainKeywords = array<string>{ std::string("ethereum"), std::string("eth"), std::string("mainnet"), std::string("polygon"), std::string("matic"), std::string("arbitrum"), std::string("arb"), std::string("optimism"), std::string("op"), std::string("base"), std::string("bsc"), std::string("binance"), std::string("avalanche"), std::string("avax"), std::string("fantom"), std::string("ftm") };
                auto lowerMessage = messageText->toLowerCase();
                for (auto& keyword : chainKeywords)
                {
                    if (lowerMessage->includes(keyword)) {
                        chain = keyword;
                        break;
                    }
                }
            }
            logger->info(std::string("[CHECK_TRANSACTION_CONFIRMATION] Checking transaction ") + txHash + std::string(" on ") + (OR((chain), (std::string("ethereum")))) + string_empty);
            auto inputParams = object{
                object::pair{std::string("transactionHash"), txHash}, 
                object::pair{std::string("chain"), OR((chain), (std::string("ethereum")))}
            };
            auto etherscanService = as<std::shared_ptr<EtherscanService>>(runtime->getService(EtherscanService::serviceType));
            if (!etherscanService) {
                throw any(std::make_shared<Error>(std::string("Etherscan service not found. Please ensure the Etherscan plugin is properly initialized.")));
            }
            auto receipt = std::async([=]() { etherscanService->getTransactionReceipt(txHash, chain); });
            auto statusText = (receipt->success) ? std::string("✅ SUCCESS") : std::string("❌ FAILED");
            auto chainName = (chain) ? any(chain->charAt(0)->toUpperCase() + chain->slice(1)) : any(std::string("Ethereum"));
            auto blockNumberDec = parseInt(receipt->blockNumber, 16);
            auto gasUsedDec = parseInt(receipt->gasUsed, 16);
            auto gasUsedGwei = (gasUsedDec / 1000000000)->toFixed(4);
            auto responseText = std::string("Transaction ") + statusText + std::string("\
\
");
            responseText += std::string("**Chain:** ") + chainName + std::string("\
");
            responseText += std::string("**Hash:** `") + receipt->transactionHash + std::string("`\
");
            responseText += std::string("**Confirmations:** ") + receipt->confirmations + std::string(" blocks\
");
            responseText += std::string("**Block:** ") + blockNumberDec + std::string("\
");
            responseText += std::string("**From:** `") + receipt->from + std::string("`\
");
            responseText += std::string("**To:** `") + (OR((receipt->to), (std::string("Contract Creation")))) + std::string("`\
");
            if (receipt->contractAddress) {
                responseText += std::string("**Contract Created:** `") + receipt->contractAddress + std::string("`\
");
            }
            responseText += std::string("**Gas Used:** ") + gasUsedDec->toLocaleString() + std::string(" (") + gasUsedGwei + std::string(" Gwei)\
");
            if (receipt->confirmations >= 12) {
                responseText += std::string("\
🔒 **Highly Confirmed** - Transaction is considered final");
            } else if (receipt->confirmations >= 6) {
                responseText += std::string("\
✓ **Well Confirmed** - Transaction is secure");
            } else if (receipt->confirmations >= 1) {
                responseText += std::string("\
⏳ **Recently Confirmed** - Wait for more confirmations");
            }
            auto responseData = object{
                object::pair{std::string("chain"), chainName}, 
                object::pair{std::string("transactionHash"), receipt->transactionHash}, 
                object::pair{std::string("status"), statusText}, 
                object::pair{std::string("confirmations"), receipt->confirmations}, 
                object::pair{std::string("blockNumber"), blockNumberDec}, 
                object::pair{std::string("blockHash"), receipt->blockHash}, 
                object::pair{std::string("from"), receipt->from}, 
                object::pair{std::string("to"), receipt->to}, 
                object::pair{std::string("contractAddress"), receipt->contractAddress}, 
                object::pair{std::string("gasUsed"), gasUsedDec}, 
                object::pair{std::string("effectiveGasPrice"), receipt->effectiveGasPrice}, 
                object::pair{std::string("isSuccess"), receipt->success}
            };
            auto result = as<any>(object{
                object::pair{std::string("text"), responseText}, 
                object::pair{std::string("success"), true}, 
                object::pair{std::string("data"), responseData}, 
                object::pair{std::string("values"), responseData}, 
                object::pair{std::string("input"), inputParams}
            });
            if (callback) {
                std::async([=]() { callback(object{
                    object::pair{std::string("text"), result->text}, 
                    object::pair{std::string("actions"), array<string>{ std::string("CHECK_TRANSACTION_CONFIRMATION") }}, 
                    object::pair{std::string("data"), result->data}, 
                    object::pair{std::string("source"), message->content->source}
                }); });
            }
            return result;
        }
        catch (const any& error)
        {
            auto msg = (is<Error>(error)) ? error->message : String(error);
            logger->error(std::string("[CHECK_TRANSACTION_CONFIRMATION] Action failed: ") + msg + string_empty);
            auto composedState = std::async([=]() { runtime->composeState(message, array<string>{ std::string("ACTION_STATE") }, true); });
            auto params = OR((OR((composedState->data->actionParams), (composedState->data->checkTransactionConfirmation))), (object{}));
            auto failureInputParams = object{
                object::pair{std::string("transactionHash"), params["transactionHash"]}, 
                object::pair{std::string("chain"), params["chain"]}
            };
            auto errorResult = as<any>(object{
                object::pair{std::string("text"), std::string("Failed to check transaction confirmation: ") + msg + string_empty}, 
                object::pair{std::string("success"), false}, 
                object::pair{std::string("error"), msg}, 
                object::pair{std::string("input"), failureInputParams}
            });
            if (callback) {
                std::async([=]() { callback(object{
                    object::pair{std::string("text"), errorResult->text}, 
                    object::pair{std::string("content"), object{
                        object::pair{std::string("error"), std::string("action_failed")}, 
                        object::pair{std::string("details"), msg}
                    }}
                }); });
            }
            return errorResult;
        }
    }
    }, 
    object::pair{std::string("examples"), array<array<any>>{ array<object>{ object{
        object::pair{std::string("name"), std::string("{{user}}")}, 
        object::pair{std::string("content"), object{
            object::pair{std::string("text"), std::string("Check confirmation status for transaction 0x1234567890abcdef1234567890abcdef1234567890abcdef1234567890abcdef")}
        }}
    }, object{
        object::pair{std::string("name"), std::string("{{agent}}")}, 
        object::pair{std::string("content"), object{
            object::pair{std::string("text"), std::string("Checking transaction confirmation status:")}, 
            object::pair{std::string("action"), std::string("CHECK_TRANSACTION_CONFIRMATION")}
        }}
    } }, array<object>{ object{
        object::pair{std::string("name"), std::string("{{user}}")}, 
        object::pair{std::string("content"), object{
            object::pair{std::string("text"), std::string("Has my transaction 0xabcdef1234567890abcdef1234567890abcdef1234567890abcdef1234567890 been confirmed?")}
        }}
    }, object{
        object::pair{std::string("name"), std::string("{{agent}}")}, 
        object::pair{std::string("content"), object{
            object::pair{std::string("text"), std::string("Verifying transaction confirmations:")}, 
            object::pair{std::string("action"), std::string("CHECK_TRANSACTION_CONFIRMATION")}
        }}
    } }, array<object>{ object{
        object::pair{std::string("name"), std::string("{{user}}")}, 
        object::pair{std::string("content"), object{
            object::pair{std::string("text"), std::string("Verify tx 0x9876543210fedcba9876543210fedcba9876543210fedcba9876543210fedcba on polygon")}
        }}
    }, object{
        object::pair{std::string("name"), std::string("{{agent}}")}, 
        object::pair{std::string("content"), object{
            object::pair{std::string("text"), std::string("Checking Polygon transaction:")}, 
            object::pair{std::string("action"), std::string("CHECK_TRANSACTION_CONFIRMATION")}
        }}
    } }, array<object>{ object{
        object::pair{std::string("name"), std::string("{{user}}")}, 
        object::pair{std::string("content"), object{
            object::pair{std::string("text"), std::string("check 0xcdaaa18476d16d96fa34c9e64e115a8226b45297a20b0bfe225ec4b18c99dbcf")}
        }}
    }, object{
        object::pair{std::string("name"), std::string("{{agent}}")}, 
        object::pair{std::string("content"), object{
            object::pair{std::string("text"), std::string("Looking up transaction details:")}, 
            object::pair{std::string("action"), std::string("CHECK_TRANSACTION_CONFIRMATION")}, 
            object::pair{std::string("actionParams"), object{
                object::pair{std::string("transactionHash"), std::string("0xcdaaa18476d16d96fa34c9e64e115a8226b45297a20b0bfe225ec4b18c99dbcf")}, 
                object::pair{std::string("chain"), std::string("ethereum")}
            }}
        }}
    } } }}
});

void Main(void)
{
}

MAIN
