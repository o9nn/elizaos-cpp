#include "/home/runner/work/elizaos-cpp/elizaos-cpp/spartan/src/plugins/autofunTrader/utils/wallet.h"

std::shared_ptr<Keypair> getWalletKeypair(std::shared_ptr<IAgentRuntime> runtime)
{
    auto privateKeyString = runtime->getSetting(std::string("SOLANA_PRIVATE_KEY"));
    if (!privateKeyString) {
        throw any(std::make_shared<Error>(std::string("No wallet private key configured")));
    }
    try
    {
        auto privateKeyBytes = decodeBase58(privateKeyString);
        return Keypair->fromSecretKey(privateKeyBytes);
    }
    catch (const any& error)
    {
        logger->error(std::string("Failed to create wallet keypair:"), error);
        throw any(error);
    }
};


std::shared_ptr<Promise<double>> getWalletBalance(std::shared_ptr<IAgentRuntime> runtime)
{
    try
    {
        auto walletKeypair = getWalletKeypair(runtime);
        auto connection = std::make_shared<Connection>(runtime->getSetting(std::string("SOLANA_RPC_URL")));
        auto balance = std::async([=]() { connection->getBalance(walletKeypair->publicKey); });
        auto solBalance = balance / 1000000000;
        logger->log(std::string("Fetched wallet balance:"), object{
            object::pair{std::string("address"), walletKeypair->publicKey->toBase58()}, 
            object::pair{std::string("solBalance"), std::string("solBalance")}
        });
        return solBalance;
    }
    catch (const any& error)
    {
        logger->error(std::string("Failed to get wallet balance:"), error);
        return 0;
    }
};


std::shared_ptr<Promise<std::shared_ptr<Connection>>> getConnection(std::shared_ptr<IAgentRuntime> runtime)
{
    return std::make_shared<Connection>(OR((runtime->getSetting(std::string("RPC_URL"))), (std::string("https://zondra-wil7oz-fast-mainnet.helius-rpc.com"))));
};


double calculateDynamicSlippage(string amount, any quoteData)
{
    auto baseSlippage = 0.45;
    auto priceImpact = Number->parseFloat(OR((quoteData["priceImpactPct"]), (std::string("0"))));
    auto amountNum = Number(amount);
    auto dynamicSlippage = baseSlippage;
    if (priceImpact > 1) {
        dynamicSlippage += priceImpact * 0.5;
    }
    if (amountNum > 10000) {
        dynamicSlippage *= 1.5;
    }
    return Math->min(dynamicSlippage, 2.5);
};


std::shared_ptr<Promise<object>> executeTrade(std::shared_ptr<IAgentRuntime> runtime, object params)
{
    auto actionStr = (params["action"] == std::string("SELL")) ? std::string("sell") : std::string("buy");
    logger->info(std::string("Executing ") + actionStr + std::string(" trade using ") + params["dex"] + std::string(":"), object{
        object::pair{std::string("tokenAddress"), params["tokenAddress"]}, 
        object::pair{std::string("amount"), params["amount"]}, 
        object::pair{std::string("slippage"), params["slippage"]}
    });
    try
    {
        auto walletKeypair = getWalletKeypair(runtime);
        auto connection = std::make_shared<Connection>(runtime->getSetting(std::string("RPC_URL")));
        auto SOL_ADDRESS = std::string("So11111111111111111111111111111111111111112");
        auto inputTokenCA = (params["action"] == std::string("SELL")) ? any(params["tokenAddress"]) : any(SOL_ADDRESS);
        auto outputTokenCA = (params["action"] == std::string("SELL")) ? any(SOL_ADDRESS) : any(params["tokenAddress"]);
        auto swapAmount = (params["action"] == std::string("SELL")) ? Number(params["amount"]) : Math->floor(Number(params["amount"]) * 1000000000);
        auto quoteResponse = std::async([=]() { fetch(std::string("https://public.jupiterapi.com/quote?inputMint=") + inputTokenCA + std::string("&outputMint=") + outputTokenCA + std::string("&amount=") + swapAmount + std::string("&slippageBps=") + params["slippage"] + std::string("&platformFeeBps=200")); });
        if (!quoteResponse->ok) {
            auto error = std::async([=]() { quoteResponse->text(); });
            auto parsedResponse = parseJSONObjectFromText(error);
            if (parsedResponse->errorCode == std::string("TOKEN_NOT_TRADABLE")) {
                auto extractTokenAddress = [=](auto message) mutable
                {
                    auto regex = (new RegExp(std::string("The token (\w{44}) is not tradabl")));
                    auto match = message["match"](regex);
                    return (match) ? any(const_(match)[1]) : any(nullptr);
                };

                logger->log(std::string("Need to flag"), extractTokenAddress(parsedResponse->error), std::string("as not tradable"));
            }
            logger->warn(std::string("Quote request failed:"), object{
                object::pair{std::string("status"), quoteResponse->status}, 
                object::pair{std::string("error"), std::string("error")}
            });
            return object{
                object::pair{std::string("success"), false}, 
                object::pair{std::string("error"), std::string("Failed to get quote: ") + error + string_empty}
            };
        }
        auto quoteData = std::async([=]() { quoteResponse->json(); });
        logger->log(std::string("Quote received:"), quoteData);
        auto dynamicSlippage = calculateDynamicSlippage(params["amount"]->toString(), quoteData);
        logger->info(std::string("Using dynamic slippage:"), object{
            object::pair{std::string("baseSlippage"), params["slippage"]}, 
            object::pair{std::string("dynamicSlippage"), std::string("dynamicSlippage")}, 
            object::pair{std::string("priceImpact"), quoteData["priceImpactPct"]}
        });
        auto swapResponse = std::async([=]() { fetch(std::string("https://public.jupiterapi.com/swap"), object{
            object::pair{std::string("method"), std::string("POST")}, 
            object::pair{std::string("headers"), object{
                object::pair{std::string("Content-Type"), std::string("application/json")}
            }}, 
            object::pair{std::string("body"), JSON->stringify(object{
                object::pair{std::string("quoteResponse"), utils::assign(object{
                    , 
                    object::pair{std::string("slippageBps"), Math->floor(dynamicSlippage * 10000)}
                }, quoteData)}, 
                object::pair{std::string("userPublicKey"), walletKeypair->publicKey->toString()}, 
                object::pair{std::string("wrapAndUnwrapSol"), true}, 
                object::pair{std::string("computeUnitPriceMicroLamports"), 5000000}, 
                object::pair{std::string("dynamicComputeUnitLimit"), true}
            })}
        }); });
        if (!swapResponse->ok) {
            auto error = std::async([=]() { swapResponse->text(); });
            logger->error(std::string("Swap request failed:"), object{
                object::pair{std::string("status"), swapResponse->status}, 
                object::pair{std::string("error"), std::string("error")}
            });
            throw any(std::make_shared<Error>(std::string("Failed to get swap transaction: ") + error + string_empty));
        }
        auto swapData = std::async([=]() { swapResponse->json(); });
        logger->log(std::string("Swap response received:"), swapData);
        if (!swapData["swapTransaction"]) {
            logger->error(std::string("Invalid swap response:"), swapData);
            throw any(std::make_shared<Error>(std::string("No swap transaction returned in response")));
        }
        auto transactionBuf = Buffer::from(swapData["swapTransaction"], std::string("base64"));
        auto tx = VersionedTransaction->deserialize(transactionBuf);
        auto latestBlockhash = std::async([=]() { connection->getLatestBlockhash(std::string("processed")); });
        tx->message->recentBlockhash = latestBlockhash->blockhash;
        tx->sign(array<any>{ walletKeypair });
        auto signature = std::async([=]() { connection->sendRawTransaction(tx->serialize(), object{
            object::pair{std::string("skipPreflight"), true}, 
            object::pair{std::string("maxRetries"), 5}, 
            object::pair{std::string("preflightCommitment"), std::string("processed")}
        }); });
        logger->log(std::string("Transaction sent with high priority:"), object{
            object::pair{std::string("signature"), std::string("signature")}, 
            object::pair{std::string("explorer"), std::string("https://solscan.io/tx/") + signature + string_empty}
        });
        auto confirmed = false;
        for (auto i = 0; i < CONFIRMATION_CONFIG["MAX_ATTEMPTS"]; i++)
        {
            try
            {
                auto status = std::async([=]() { connection->getSignatureStatus(signature); });
                if (OR((status->value->confirmationStatus == std::string("confirmed")), (status->value->confirmationStatus == std::string("finalized")))) {
                    confirmed = true;
                    logger->log(std::string("Transaction confirmed:"), object{
                        object::pair{std::string("signature"), std::string("signature")}, 
                        object::pair{std::string("confirmationStatus"), status->value->confirmationStatus}, 
                        object::pair{std::string("slot"), status->context->slot}, 
                        object::pair{std::string("attempt"), i + 1}
                    });
                    break;
                }
                shared delay = CONFIRMATION_CONFIG["getDelayForAttempt"](i);
                logger->info(std::string("Waiting ") + delay + std::string("ms before next confirmation check (attempt ") + (i + 1) + std::string("/") + CONFIRMATION_CONFIG["MAX_ATTEMPTS"] + std::string(")"));
                std::async([=]() { std::make_shared<Promise>([=](auto resolve) mutable
                {
                    return setTimeout(resolve, delay);
                }
                ); });
            }
            catch (const any& error)
            {
                logger->warn(std::string("Confirmation check ") + (i + 1) + std::string(" failed:"), error);
                if (i == CONFIRMATION_CONFIG["MAX_ATTEMPTS"] - 1) {
                    throw any(std::make_shared<Error>(std::string("Could not confirm transaction status")));
                }
                shared delay = CONFIRMATION_CONFIG["getDelayForAttempt"](i);
                std::async([=]() { std::make_shared<Promise>([=](auto resolve) mutable
                {
                    return setTimeout(resolve, delay);
                }
                ); });
            }
        }
        if (!confirmed) {
            throw any(std::make_shared<Error>(std::string("Could not confirm transaction status")));
        }
        logger->log(std::string("Trade executed successfully:"), object{
            object::pair{std::string("type"), (params["action"] == std::string("SELL")) ? std::string("sell") : std::string("buy")}, 
            object::pair{std::string("tokenAddress"), params["tokenAddress"]}, 
            object::pair{std::string("amount"), params["amount"]}, 
            object::pair{std::string("signature"), std::string("signature")}, 
            object::pair{std::string("explorer"), std::string("https://solscan.io/tx/") + signature + string_empty}
        });
        return object{
            object::pair{std::string("success"), true}, 
            object::pair{std::string("signature"), std::string("signature")}, 
            object::pair{std::string("receivedAmount"), params["amount"]}, 
            object::pair{std::string("receivedValue"), params["amount"]}
        };
    }
    catch (const any& error)
    {
        logger->error(std::string("Trade execution failed:"), object{
            object::pair{std::string("error"), (is<Error>(error)) ? any(error->message) : any(std::string("Unknown error"))}, 
            object::pair{std::string("params"), object{
                object::pair{std::string("tokenAddress"), params["tokenAddress"]}, 
                object::pair{std::string("amount"), params["amount"]}, 
                object::pair{std::string("slippage"), params["slippage"]}, 
                object::pair{std::string("dex"), params["dex"]}, 
                object::pair{std::string("action"), params["action"]}
            }}, 
            object::pair{std::string("errorStack"), (is<Error>(error)) ? any(error->stack) : any(undefined)}
        });
        return object{
            object::pair{std::string("success"), false}, 
            object::pair{std::string("error"), (is<Error>(error)) ? any(error->message) : any(std::string("Unknown error"))}
        };
    }
};


std::shared_ptr<Promise<object>> executeRaydiumTrade(std::shared_ptr<IAgentRuntime> runtime, object params)
{
    try
    {
        auto walletKeypair = getWalletKeypair(runtime);
        auto connection = std::async([=]() { getConnection(runtime); });
        auto SOL_ADDRESS = std::string("So11111111111111111111111111111111111111112");
        auto quoteResponse = std::async([=]() { fetch(std::string("https://api.raydium.io/v2/main/quote"), object{
            object::pair{std::string("method"), std::string("POST")}, 
            object::pair{std::string("headers"), object{
                object::pair{std::string("Content-Type"), std::string("application/json")}
            }}, 
            object::pair{std::string("body"), JSON->stringify(object{
                object::pair{std::string("inputMint"), (params["isSell"]) ? any(params["tokenAddress"]) : any(SOL_ADDRESS)}, 
                object::pair{std::string("outputMint"), (params["isSell"]) ? any(SOL_ADDRESS) : any(params["tokenAddress"])}, 
                object::pair{std::string("amount"), params["amount"]}, 
                object::pair{std::string("slippage"), params["slippage"] * 100}, 
                object::pair{std::string("onlyDirectRoute"), true}
            })}
        }); });
        if (!quoteResponse->ok) {
            throw any(std::make_shared<Error>(std::string("Raydium quote failed: ") + std::async([=]() { quoteResponse->text(); }) + string_empty));
        }
        auto quoteData = std::async([=]() { quoteResponse->json(); });
        logger->log(std::string("Raydium quote received:"), quoteData);
        auto swapResponse = std::async([=]() { fetch(std::string("https://api.raydium.io/v2/main/swap"), object{
            object::pair{std::string("method"), std::string("POST")}, 
            object::pair{std::string("headers"), object{
                object::pair{std::string("Content-Type"), std::string("application/json")}
            }}, 
            object::pair{std::string("body"), JSON->stringify(utils::assign(object{
                , 
                object::pair{std::string("wallet"), walletKeypair->publicKey->toString()}, 
                object::pair{std::string("computeUnitPriceMicroLamports"), 5000000}
            }, quoteData))}
        }); });
        auto swapData = std::async([=]() { swapResponse->json(); });
        if (!swapData["swapTransaction"]) {
            throw any(std::make_shared<Error>(std::string("No swap transaction returned")));
        }
        auto transactionBuf = Buffer::from(swapData["swapTransaction"], std::string("base64"));
        auto tx = VersionedTransaction->deserialize(transactionBuf);
        auto latestBlockhash = std::async([=]() { connection->getLatestBlockhash(std::string("processed")); });
        tx->message->recentBlockhash = latestBlockhash->blockhash;
        tx->sign(array<any>{ walletKeypair });
        auto signature = std::async([=]() { connection->sendRawTransaction(tx->serialize(), object{
            object::pair{std::string("skipPreflight"), true}, 
            object::pair{std::string("maxRetries"), 5}, 
            object::pair{std::string("preflightCommitment"), std::string("processed")}
        }); });
        logger->log(std::string("Transaction sent with high priority:"), object{
            object::pair{std::string("signature"), std::string("signature")}, 
            object::pair{std::string("explorer"), std::string("https://solscan.io/tx/") + signature + string_empty}
        });
        auto confirmed = false;
        for (auto i = 0; i < 8; i++)
        {
            try
            {
                auto status = std::async([=]() { connection->getSignatureStatus(signature); });
                if (OR((status->value->confirmationStatus == std::string("confirmed")), (status->value->confirmationStatus == std::string("finalized")))) {
                    confirmed = true;
                    logger->log(std::string("Transaction confirmed:"), object{
                        object::pair{std::string("signature"), std::string("signature")}, 
                        object::pair{std::string("confirmationStatus"), status->value->confirmationStatus}, 
                        object::pair{std::string("slot"), status->context->slot}
                    });
                    break;
                }
                shared delay = Math->min(1000 * Math.pow(1.5, i), 10000);
                std::async([=]() { std::make_shared<Promise>([=](auto resolve) mutable
                {
                    return setTimeout(resolve, delay);
                }
                ); });
            }
            catch (const any& error)
            {
                logger->warn(std::string("Confirmation check ") + (i + 1) + std::string(" failed:"), error);
            }
        }
        if (!confirmed) {
            throw any(std::make_shared<Error>(std::string("Could not confirm transaction status")));
        }
        logger->log(std::string("Trade executed successfully:"), object{
            object::pair{std::string("type"), (params["isSell"]) ? std::string("sell") : std::string("buy")}, 
            object::pair{std::string("tokenAddress"), params["tokenAddress"]}, 
            object::pair{std::string("amount"), params["amount"]}, 
            object::pair{std::string("signature"), std::string("signature")}, 
            object::pair{std::string("explorer"), std::string("https://solscan.io/tx/") + signature + string_empty}
        });
        return object{
            object::pair{std::string("success"), true}, 
            object::pair{std::string("signature"), std::string("signature")}
        };
    }
    catch (const any& error)
    {
        logger->error(std::string("Raydium trade execution failed:"), error);
        return object{
            object::pair{std::string("success"), false}, 
            object::pair{std::string("error"), (is<Error>(error)) ? any(error->message) : any(std::string("Unknown error"))}
        };
    }
};


std::shared_ptr<Promise<double>> getChainWalletBalance(std::shared_ptr<IAgentRuntime> runtime, string _tokenAddress)
{
    return std::async([=]() { getWalletBalance(runtime); });
};


std::shared_ptr<Promise<string>> simulateTransaction(any client, any tx)
{
    try
    {
        auto result = std::async([=]() { client["call"](object{
            object::pair{std::string("account"), client["account"]}, 
            object::pair{std::string("to"), tx["to"]}, 
            object::pair{std::string("data"), tx["data"]}, 
            object::pair{std::string("value"), tx["value"]}, 
            object::pair{std::string("gas"), tx["gas"]}, 
            object::pair{std::string("gasPrice"), tx["gasPrice"]}
        }); });
        return result;
    }
    catch (const any& error)
    {
        return std::string("Simulation failed: ") + error["message"] + string_empty;
    }
};


any getWalletBalances(std::shared_ptr<IAgentRuntime> runtime)
{
    try
    {
        auto walletKeypair = getWalletKeypair(runtime);
        auto connection = std::make_shared<Connection>(runtime->getSetting(std::string("RPC_URL")));
        auto solBalance = std::async([=]() { connection->getBalance(walletKeypair->publicKey); });
        auto tokenAccounts = std::async([=]() { connection->getParsedTokenAccountsByOwner(walletKeypair->publicKey, object{
            object::pair{std::string("programId"), std::make_shared<PublicKey>(std::string("TokenkegQfeZyiNwAJbNbGKPFXCWuBvf9Ss623VQ5DA"))}
        }); });
        auto balances = object{
            object::pair{std::string("solBalance"), solBalance / 1000000000}, 
            object::pair{std::string("tokens"), tokenAccounts->value->map([=](auto account) mutable
            {
                return (object{
                    object::pair{std::string("mint"), account["account"]->data->parsed->info->mint}, 
                    object::pair{std::string("balance"), account["account"]->data->parsed->info->tokenAmount->amount}, 
                    object::pair{std::string("decimals"), account["account"]->data->parsed->info->tokenAmount->decimals}, 
                    object::pair{std::string("uiAmount"), account["account"]->data->parsed->info->tokenAmount->uiAmount}
                });
            }
            )}
        };
        return balances;
    }
    catch (const any& error)
    {
        logger->error(std::string("Failed to get wallet balances:"), error);
        return object{
            object::pair{std::string("solBalance"), 0}, 
            object::pair{std::string("tokens"), array<any>()}
        };
    }
};


std::shared_ptr<Promise<any>> getTokenBalance(std::shared_ptr<IAgentRuntime> runtime, string tokenMint)
{
    try
    {
        auto balances = std::async([=]() { getWalletBalances(runtime); });
        auto token = balances["tokens"]->find([=](auto t) mutable
        {
            return t["mint"]->toLowerCase() == tokenMint->toLowerCase();
        }
        );
        if (!token) {
            logger->warn(std::string("No balance found for token ") + tokenMint + string_empty, object{
                object::pair{std::string("availableTokens"), balances["tokens"]->map([=](auto t) mutable
                {
                    return t["mint"];
                }
                )}
            });
        }
        return token;
    }
    catch (const any& error)
    {
        logger->error(std::string("Failed to get token balance:"), error);
        return nullptr;
    }
};


std::shared_ptr<Promise<boolean>> hasTokenBalance(std::shared_ptr<IAgentRuntime> runtime, string tokenMint)
{
    auto balance = std::async([=]() { getTokenBalance(runtime, tokenMint); });
    return AND((balance != nullptr), (balance->uiAmount > 0));
};


object CONFIRMATION_CONFIG = object{
    object::pair{std::string("MAX_ATTEMPTS"), 12}, 
    object::pair{std::string("INITIAL_TIMEOUT"), 2000}, 
    object::pair{std::string("MAX_TIMEOUT"), 20000}, 
    object::pair{std::string("getDelayForAttempt"), [=](auto attempt) mutable
    {
        return Math->min(2000 * Math.pow(1.5, attempt), 20000);
    }
    }
};

void Main(void)
{
}

MAIN
