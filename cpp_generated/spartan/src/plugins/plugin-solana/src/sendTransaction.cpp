#include "/home/runner/work/elizaos-cpp/elizaos-cpp/spartan/src/plugins/plugin-solana/src/sendTransaction.h"

std::shared_ptr<Promise<string>> sendTransaction(std::shared_ptr<Connection> connection, array<any> instructions, std::shared_ptr<Keypair> wallet)
{
    auto latestBlockhash = std::async([=]() { connection->getLatestBlockhash(); });
    auto messageV0 = ((std::make_shared<TransactionMessage>(object{
        object::pair{std::string("payerKey"), wallet->publicKey}, 
        object::pair{std::string("recentBlockhash"), latestBlockhash->blockhash}, 
        object::pair{std::string("instructions"), std::string("instructions")}
    })))->compileToV0Message();
    auto simulatedTx = std::make_shared<VersionedTransaction>(messageV0);
    simulatedTx->sign(array<any>{ wallet });
    auto simulation = std::async([=]() { connection->simulateTransaction(simulatedTx); });
    auto computeUnits = OR((simulation->value->unitsConsumed), (200000));
    auto safeComputeUnits = Math->ceil(Math->max(computeUnits * 1.3, computeUnits + 100000));
    auto recentPrioritizationFees = std::async([=]() { connection->getRecentPrioritizationFees(); });
    shared prioritizationFee = const_(recentPrioritizationFees->map([=](auto fee) mutable
    {
        return fee["prioritizationFee"];
    }
    )->sort([=](auto a, auto b) mutable
    {
        return a - b;
    }
    ))[Math->ceil(0.95 * recentPrioritizationFees->length) - 1];
    auto computeBudgetInstructions = array<any>{ ComputeBudgetProgram->setComputeUnitLimit(object{
        object::pair{std::string("units"), safeComputeUnits}
    }), ComputeBudgetProgram->setComputeUnitPrice(object{
        object::pair{std::string("microLamports"), prioritizationFee}
    }) };
    auto finalMessage = ((std::make_shared<TransactionMessage>(object{
        object::pair{std::string("payerKey"), wallet->publicKey}, 
        object::pair{std::string("recentBlockhash"), latestBlockhash->blockhash}, 
        object::pair{std::string("instructions"), array<any>{ computeBudgetInstructions, instructions }}
    })))->compileToV0Message();
    auto transaction = std::make_shared<VersionedTransaction>(finalMessage);
    transaction->sign(array<any>{ wallet });
    auto timeoutMs = 90000;
    auto startTime = Date->now();
    while (Date->now() - startTime < timeoutMs)
    {
        auto transactionStartTime = Date->now();
        auto signature = std::async([=]() { connection->sendTransaction(transaction, object{
            object::pair{std::string("maxRetries"), 0}, 
            object::pair{std::string("skipPreflight"), true}
        }); });
        auto statuses = std::async([=]() { connection->getSignatureStatuses(array<any>{ signature }); });
        if (const_(statuses->value)[0]) {
            if (!const_(statuses->value)[0]->err) {
                elizaLogger->log(std::string("Transaction confirmed: ") + signature + string_empty);
                return signature;
            } else {
                throw any(std::make_shared<Error>(std::string("Transaction failed: ") + const_(statuses->value)[0]->err->toString() + string_empty));
            }
        }
        auto elapsedTime = Date->now() - transactionStartTime;
        shared remainingTime = Math->max(0, 1000 - elapsedTime);
        if (remainingTime > 0) {
            std::async([=]() { std::make_shared<Promise>([=](auto resolve) mutable
            {
                return setTimeout(resolve, remainingTime);
            }
            ); });
        }
    }
    throw any(std::make_shared<Error>(std::string("Transaction timeout")));
};


