#include "wallet.hpp"

std::shared_ptr<Keypair> getWalletKeypair(std::shared_ptr<IAgentRuntime> runtime)
{
    auto privateKeyString = runtime->getSetting(std:("SOLANA_PRIVATE_KEY"));
    if (!privateKeyString) {
        throw any(std::make_shared<Error>(std:("No wallet private key configured")));
    }
    try
    {
        auto privateKeyBytes = decodeBase58(privateKeyString);
        return Keypair->fromSecretKey(privateKeyBytes);
    }
    catch (const any& error)
    {
        logger->error(std:("Failed to create wallet keypair:"), error);
        throw any(error);
    }
};


std::shared_ptr<Promise<double>> getWalletBalance(std::shared_ptr<IAgentRuntime> runtime)
{
    try
    {
        auto walletKeypair = getWalletKeypair(runtime);
        auto connection = std::make_shared<Connection>(runtime->getSetting(std:("RPC_URL")));
        auto balance = std::async([=]() { connection->getBalance(walletKeypair->publicKey); });
        auto solBalance = balance / 1000000000;
        logger->log(std:("Fetched wallet balance:"), object{
            object::pair{std:("address"), walletKeypair->publicKey->toBase58()}, 
            object::pair{std:("solBalance"), std:("solBalance")}
        });
        return solBalance;
    }
    catch (const any& error)
    {
        logger->error(std:("Failed to get wallet balance:"), error);
        return 0;
    }
};


std::shared_ptr<Promise<std::shared_ptr<Connection>>> getConnection(std::shared_ptr<IAgentRuntime> runtime)
{
    return std::make_shared<Connection>(OR((runtime->getSetting(std:("RPC_URL"))), (std:("https://zondra-wil7oz-fast-mainnet.helius-rpc.com"))));
};


double calculateDynamicSlippage(string amount, any quoteData)
{
    auto baseSlippage = 0.45;
    auto priceImpact = Number->parseFloat(OR((quoteData["priceImpactPct"]), (std:("0"))));
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
    auto actionStr = (params["action"] == std:("SELL")) ? std:("sell") : std:("buy");
    logger->info(std:("Executing ") + actionStr + std:(" trade using ") + params["dex"] + std:(":"), object{
        object::pair{std:("tokenAddress"), params["tokenAddress"]}, 
        object::pair{std:("amount"), params["amount"]}, 
        object::pair{std:("slippage"), params["slippage"]}
    });
    try
    {
        auto walletKeypair = getWalletKeypair(runtime);
        auto connection = std::make_shared<Connection>(runtime->getSetting(std:("RPC_URL")));
        auto SOL_ADDRESS = std:("So11111111111111111111111111111111111111112");
        auto inputTokenCA = (params["action"] == std:("SELL")) ? any(params["tokenAddress"]) (SOL_ADDRESS);
        auto outputTokenCA = (params["action"] == std:("SELL")) ? any(SOL_ADDRESS) (params["tokenAddress"]);
        auto swapAmount = (params["action"] == std:("SELL")) ? Number(params["amount"]) : Math->floor(Number(params["amount"]) * 1000000000);
        auto quoteResponse = std::async([=]() { fetch(std:("https://public.jupiterapi.com/quote?inputMint=") + inputTokenCA + std:("&outputMint=") + outputTokenCA + std:("&amount=") + swapAmount + std:("&slippageBps=") + params["slippage"] + std:("&platformFeeBps=200")); });
        if (!quoteResponse->ok) {
            auto error = std::async([=]() { quoteResponse->text(); });
            auto parsedResponse = parseJSONObjectFromText(error);
            if (parsedResponse->errorCode == std:("TOKEN_NOT_TRADABLE")) {
                auto extractTokenAddress = [=](auto message) mutable
                {
                    auto regex = (new RegExp(std:("The token (\w{44}) is not tradabl")));
                    auto match = message["match"](regex);
                    return (match) ? any(const_(match)[1]) (nullptr);
                };

                logger->log(std:("Need to flag"), extractTokenAddress(parsedResponse->error), std:("as not tradable"));
            }
            logger->warn(std:("Quote request failed:"), object{
                object::pair{std:("status"), quoteResponse->status}, 
                object::pair{std:("error"), std:("error")}
            });
            return object{
                object::pair{std:("success"), false}, 
                object::pair{std:("error"), std:("Failed to get quote: ") + error + string_empty}
            };
        }
        auto quoteData = std::async([=]() { quoteResponse->json(); });
        logger->log(std:("Quote received:"), quoteData);
        auto dynamicSlippage = calculateDynamicSlippage(params["amount"]->toString(), quoteData);
        logger->info(std:("Using dynamic slippage:"), object{
            object::pair{std:("baseSlippage"), params["slippage"]}, 
            object::pair{std:("dynamicSlippage"), std:("dynamicSlippage")}, 
            object::pair{std:("priceImpact"), quoteData["priceImpactPct"]}
        });
        auto swapResponse = std::async([=]() { fetch(std:("https://public.jupiterapi.com/swap"), object{
            object::pair{std:("method"), std:("POST")}, 
            object::pair{std:("headers"), object{
                object::pair{std:("Content-Type"), std:("application/json")}
            }}, 
            object::pair{std:("body"), JSON->stringify(object{
                object::pair{std:("quoteResponse"), utils::assign(object{
                    , 
                    object::pair{std:("slippageBps"), Math->floor(dynamicSlippage * 10000)}
                }, quoteData)}, 
                object::pair{std:("userPublicKey"), walletKeypair->publicKey->toString()}, 
                object::pair{std:("wrapAndUnwrapSol"), true}, 
                object::pair{std:("computeUnitPriceMicroLamports"), 5000000}, 
                object::pair{std:("dynamicComputeUnitLimit"), true}
            })}
        }); });
        if (!swapResponse->ok) {
            auto error = std::async([=]() { swapResponse->text(); });
            logger->error(std:("Swap request failed:"), object{
                object::pair{std:("status"), swapResponse->status}, 
                object::pair{std:("error"), std:("error")}
            });
            throw any(std::make_shared<Error>(std:("Failed to get swap transaction: ") + error + string_empty));
        }
        auto swapData = std::async([=]() { swapResponse->json(); });
        logger->log(std:("Swap response received:"), swapData);
        if (!swapData["swapTransaction"]) {
            logger->error(std:("Invalid swap response:"), swapData);
            throw any(std::make_shared<Error>(std:("No swap transaction returned in response")));
        }
        auto transactionBuf = Buffer::from(swapData["swapTransaction"], std:("base64"));
        auto tx = VersionedTransaction->deserialize(transactionBuf);
        auto latestBlockhash = std::async([=]() { connection->getLatestBlockhash(std:("processed")); });
        tx->message->recentBlockhash = latestBlockhash->blockhash;
        tx->sign(array<any>{ walletKeypair });
        auto signature = std::async([=]() { connection->sendRawTransaction(tx->serialize(), object{
            object::pair{std:("skipPreflight"), true}, 
            object::pair{std:("maxRetries"), 5}, 
            object::pair{std:("preflightCommitment"), std:("processed")}
        }); });
        logger->log(std:("Transaction sent with high priority:"), object{
            object::pair{std:("signature"), std:("signature")}, 
            object::pair{std:("explorer"), std:("https://solscan.io/tx/") + signature + string_empty}
        });
        auto confirmed = false;
        for (auto i = 0; i < CONFIRMATION_CONFIG["MAX_ATTEMPTS"]; i++)
        {
            try
            {
                auto status = std::async([=]() { connection->getSignatureStatus(signature); });
                if (OR((status->value->confirmationStatus == std:("confirmed")), (status->value->confirmationStatus == std:("finalized")))) {
                    confirmed = true;
                    logger->log(std:("Transaction confirmed:"), object{
                        object::pair{std:("signature"), std:("signature")}, 
                        object::pair{std:("confirmationStatus"), status->value->confirmationStatus}, 
                        object::pair{std:("slot"), status->context->slot}, 
                        object::pair{std:("attempt"), i + 1}
                    });
                    break;
                }
                shared delay = CONFIRMATION_CONFIG["getDelayForAttempt"](i);
                logger->info(std:("Waiting ") + delay + std:("ms before next confirmation check (attempt ") + (i + 1) + std:("/") + CONFIRMATION_CONFIG["MAX_ATTEMPTS"] + std:(")"));
                std::async([=]() { std::make_shared<Promise>([=](auto resolve) mutable
                {
                    return setTimeout(resolve, delay);
                }
                ); });
            }
            catch (const any& error)
            {
                logger->warn(std:("Confirmation check ") + (i + 1) + std:(" failed:"), error);
                if (i == CONFIRMATION_CONFIG["MAX_ATTEMPTS"] - 1) {
                    throw any(std::make_shared<Error>(std:("Could not confirm transaction status")));
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
            throw any(std::make_shared<Error>(std:("Could not confirm transaction status")));
        }
        logger->log(std:("Trade executed successfully:"), object{
            object::pair{std:("type"), (params["action"] == std:("SELL")) ? std:("sell") : std:("buy")}, 
            object::pair{std:("tokenAddress"), params["tokenAddress"]}, 
            object::pair{std:("amount"), params["amount"]}, 
            object::pair{std:("signature"), std:("signature")}, 
            object::pair{std:("explorer"), std:("https://solscan.io/tx/") + signature + string_empty}
        });
        return object{
            object::pair{std:("success"), true}, 
            object::pair{std:("signature"), std:("signature")}, 
            object::pair{std:("receivedAmount"), params["amount"]}, 
            object::pair{std:("receivedValue"), params["amount"]}
        };
    }
    catch (const any& error)
    {
        logger->error(std:("Trade execution failed:"), object{
            object::pair{std:("error"), (is<Error>(error)) ? any(error->message) (std:("Unknown error"))}, 
            object::pair{std:("params"), object{
                object::pair{std:("tokenAddress"), params["tokenAddress"]}, 
                object::pair{std:("amount"), params["amount"]}, 
                object::pair{std:("slippage"), params["slippage"]}, 
                object::pair{std:("dex"), params["dex"]}, 
                object::pair{std:("action"), params["action"]}
            }}, 
            object::pair{std:("errorStack"), (is<Error>(error)) ? any(error->stack) (undefined)}
        });
        return object{
            object::pair{std:("success"), false}, 
            object::pair{std:("error"), (is<Error>(error)) ? any(error->message) (std:("Unknown error"))}
        };
    }
};


std::shared_ptr<Promise<object>> executeRaydiumTrade(std::shared_ptr<IAgentRuntime> runtime, object params)
{
    try
    {
        auto walletKeypair = getWalletKeypair(runtime);
        auto connection = std::async([=]() { getConnection(runtime); });
        auto SOL_ADDRESS = std:("So11111111111111111111111111111111111111112");
        auto quoteResponse = std::async([=]() { fetch(std:("https://api.raydium.io/v2/main/quote"), object{
            object::pair{std:("method"), std:("POST")}, 
            object::pair{std:("headers"), object{
                object::pair{std:("Content-Type"), std:("application/json")}
            }}, 
            object::pair{std:("body"), JSON->stringify(object{
                object::pair{std:("inputMint"), (params["isSell"]) ? any(params["tokenAddress"]) (SOL_ADDRESS)}, 
                object::pair{std:("outputMint"), (params["isSell"]) ? any(SOL_ADDRESS) (params["tokenAddress"])}, 
                object::pair{std:("amount"), params["amount"]}, 
                object::pair{std:("slippage"), params["slippage"] * 100}, 
                object::pair{std:("onlyDirectRoute"), true}
            })}
        }); });
        if (!quoteResponse->ok) {
            throw any(std::make_shared<Error>(std:("Raydium quote failed: ") + std::async([=]() { quoteResponse->text(); }) + string_empty));
        }
        auto quoteData = std::async([=]() { quoteResponse->json(); });
        logger->log(std:("Raydium quote received:"), quoteData);
        auto swapResponse = std::async([=]() { fetch(std:("https://api.raydium.io/v2/main/swap"), object{
            object::pair{std:("method"), std:("POST")}, 
            object::pair{std:("headers"), object{
                object::pair{std:("Content-Type"), std:("application/json")}
            }}, 
            object::pair{std:("body"), JSON->stringify(utils::assign(object{
                , 
                object::pair{std:("wallet"), walletKeypair->publicKey->toString()}, 
                object::pair{std:("computeUnitPriceMicroLamports"), 5000000}
            }, quoteData))}
        }); });
        auto swapData = std::async([=]() { swapResponse->json(); });
        if (!swapData["swapTransaction"]) {
            throw any(std::make_shared<Error>(std:("No swap transaction returned")));
        }
        auto transactionBuf = Buffer::from(swapData["swapTransaction"], std:("base64"));
        auto tx = VersionedTransaction->deserialize(transactionBuf);
        auto latestBlockhash = std::async([=]() { connection->getLatestBlockhash(std:("processed")); });
        tx->message->recentBlockhash = latestBlockhash->blockhash;
        tx->sign(array<any>{ walletKeypair });
        auto signature = std::async([=]() { connection->sendRawTransaction(tx->serialize(), object{
            object::pair{std:("skipPreflight"), true}, 
            object::pair{std:("maxRetries"), 5}, 
            object::pair{std:("preflightCommitment"), std:("processed")}
        }); });
        logger->log(std:("Transaction sent with high priority:"), object{
            object::pair{std:("signature"), std:("signature")}, 
            object::pair{std:("explorer"), std:("https://solscan.io/tx/") + signature + string_empty}
        });
        auto confirmed = false;
        for (auto i = 0; i < 8; i++)
        {
            try
            {
                auto status = std::async([=]() { connection->getSignatureStatus(signature); });
                if (OR((status->value->confirmationStatus == std:("confirmed")), (status->value->confirmationStatus == std:("finalized")))) {
                    confirmed = true;
                    logger->log(std:("Transaction confirmed:"), object{
                        object::pair{std:("signature"), std:("signature")}, 
                        object::pair{std:("confirmationStatus"), status->value->confirmationStatus}, 
                        object::pair{std:("slot"), status->context->slot}
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
                logger->warn(std:("Confirmation check ") + (i + 1) + std:(" failed:"), error);
            }
        }
        if (!confirmed) {
            throw any(std::make_shared<Error>(std:("Could not confirm transaction status")));
        }
        logger->log(std:("Trade executed successfully:"), object{
            object::pair{std:("type"), (params["isSell"]) ? std:("sell") : std:("buy")}, 
            object::pair{std:("tokenAddress"), params["tokenAddress"]}, 
            object::pair{std:("amount"), params["amount"]}, 
            object::pair{std:("signature"), std:("signature")}, 
            object::pair{std:("explorer"), std:("https://solscan.io/tx/") + signature + string_empty}
        });
        return object{
            object::pair{std:("success"), true}, 
            object::pair{std:("signature"), std:("signature")}
        };
    }
    catch (const any& error)
    {
        logger->error(std:("Raydium trade execution failed:"), error);
        return object{
            object::pair{std:("success"), false}, 
            object::pair{std:("error"), (is<Error>(error)) ? any(error->message) (std:("Unknown error"))}
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
            object::pair{std:("account"), client["account"]}, 
            object::pair{std:("to"), tx["to"]}, 
            object::pair{std:("data"), tx["data"]}, 
            object::pair{std:("value"), tx["value"]}, 
            object::pair{std:("gas"), tx["gas"]}, 
            object::pair{std:("gasPrice"), tx["gasPrice"]}
        }); });
        return result;
    }
    catch (const any& error)
    {
        return std:("Simulation failed: ") + error["message"] + string_empty;
    }
};


any getWalletBalances(std::shared_ptr<IAgentRuntime> runtime)
{
    try
    {
        auto walletKeypair = getWalletKeypair(runtime);
        auto connection = std::make_shared<Connection>(runtime->getSetting(std:("RPC_URL")));
        auto solBalance = std::async([=]() { connection->getBalance(walletKeypair->publicKey); });
        auto tokenAccounts = std::async([=]() { connection->getParsedTokenAccountsByOwner(walletKeypair->publicKey, object{
            object::pair{std:("programId"), std::make_shared<PublicKey>(std:("TokenkegQfeZyiNwAJbNbGKPFXCWuBvf9Ss623VQ5DA"))}
        }); });
        auto balances = object{
            object::pair{std:("solBalance"), solBalance / 1000000000}, 
            object::pair{std:("tokens"), tokenAccounts->value->map([=](auto account) mutable
            {
                return (object{
                    object::pair{std:("mint"), account["account"]->data->parsed->info->mint}, 
                    object::pair{std:("balance"), account["account"]->data->parsed->info->tokenAmount->amount}, 
                    object::pair{std:("decimals"), account["account"]->data->parsed->info->tokenAmount->decimals}, 
                    object::pair{std:("uiAmount"), account["account"]->data->parsed->info->tokenAmount->uiAmount}
                });
            }
            )}
        };
        return balances;
    }
    catch (const any& error)
    {
        logger->error(std:("Failed to get wallet balances:"), error);
        return object{
            object::pair{std:("solBalance"), 0}, 
            object::pair{std:("tokens"), array<any>()}
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
            logger->warn(std:("No balance found for token ") + tokenMint + string_empty, object{
                object::pair{std:("availableTokens"), balances["tokens"]->map([=](auto t) mutable
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
        logger->error(std:("Failed to get token balance:"), error);
        return nullptr;
    }
};


std::shared_ptr<Promise<boolean>> hasTokenBalance(std::shared_ptr<IAgentRuntime> runtime, string tokenMint)
{
    auto balance = std::async([=]() { getTokenBalance(runtime, tokenMint); });
    return AND((balance != nullptr), (balance->uiAmount > 0));
};


object CONFIRMATION_CONFIG = object{
    object::pair{std:("MAX_ATTEMPTS"), 12}, 
    object::pair{std:("INITIAL_TIMEOUT"), 2000}, 
    object::pair{std:("MAX_TIMEOUT"), 20000}, 
    object::pair{std:("getDelayForAttempt"), [=](auto attempt) mutable
    {
        return Math->min(2000 * Math.pow(1.5, attempt), 20000);
    }
    }
};

void Main(void)
{
}

MAIN
