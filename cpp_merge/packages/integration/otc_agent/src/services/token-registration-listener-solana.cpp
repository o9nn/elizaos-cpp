#include "/home/runner/work/elizaos-cpp/elizaos-cpp/otc-agent/src/services/token-registration-listener-solana.h"

void startSolanaListener()
{
    if (isListening) {
        console->warn(std:("[Solana Listener] Already listening"));
        return std::shared_ptr<Promise<void>>();
    }
    auto programId = process->env->NEXT_PUBLIC_SOLANA_PROGRAM_ID;
    if (!programId) {
        console->error(std:("[Solana Listener] SOLANA_PROGRAM_ID not configured"));
        return std::shared_ptr<Promise<void>>();
    }
    auto rpcUrl = OR((process->env->NEXT_PUBLIC_SOLANA_RPC), (std:("https://api.mainnet-beta.solana.com")));
    connection = std::make_shared<Connection>(rpcUrl, std:("confirmed"));
    try
    {
        console->log(std:("[Solana Listener] Starting listener for program"), programId);
        isListening = true;
        shared subscriptionId = connection["onLogs"](std::make_shared<PublicKey>(programId), [=](auto logs) mutable
        {
            std::async([=]() { handleProgramLogs(logs); });
        }
        , std:("confirmed"));
        process->on(std:("SIGINT"), [=]() mutable
        {
            console->log(std:("[Solana Listener] Stopping..."));
            if (connection) {
                std::async([=]() { connection["removeOnLogsListener"](subscriptionId); });
            }
            isListening = false;
        }
        );
        process->on(std:("SIGTERM"), [=]() mutable
        {
            console->log(std:("[Solana Listener] Stopping..."));
            if (connection) {
                std::async([=]() { connection["removeOnLogsListener"](subscriptionId); });
            }
            isListening = false;
        }
        );
        console->log(std:("[Solana Listener] Now listening for token registrations"));
    }
    catch (const any& error)
    {
        console->error(std:("[Solana Listener] Failed to start:"), error);
        isListening = false;
    }
};


void handleProgramLogs(std::shared_ptr<Logs> logs)
{
    auto logMessages = logs->logs;
    auto hasRegisterToken = logMessages->some([=](auto log) mutable
    {
        return OR((log->includes(std:("Instruction: RegisterToken"))), (log->includes(std:("register_token"))));
    }
    );
    if (!hasRegisterToken) {
        return std::shared_ptr<Promise<void>>();
    }
    console->log(std:("[Solana Listener] Token registration detected:"), logs->signature);
    if (connection) {
        auto tx = std::async([=]() { connection["getTransaction"](logs->signature, object{
            object::pair{std:("commitment"), std:("confirmed")}, 
            object::pair{std:("maxSupportedTransactionVersion"), 0}
        }); });
        if (AND((AND((tx), (tx["meta"]))), (tx["meta"]["logMessages"]))) {
            std::async([=]() { parseRegisterTokenTransaction(tx); });
        }
    }
};


void parseRegisterTokenTransaction(std::shared_ptr<VersionedTransactionResponse> tx)
{
    auto signature = const_(tx->transaction->signatures)[0];
    console->log(std:("[Solana Listener] Detected token registration transaction:"), signature);
    console->warn(std:("[Solana Listener] Solana registration parsing not yet implemented"));
};


void backfillSolanaEvents(array<string> signatures)
{
    auto programId = process->env->NEXT_PUBLIC_SOLANA_PROGRAM_ID;
    if (!programId) {
        throw any(std::make_shared<Error>(std:("SOLANA_PROGRAM_ID not configured")));
    }
    auto rpcUrl = OR((process->env->NEXT_PUBLIC_SOLANA_RPC), (std:("https://api.mainnet-beta.solana.com")));
    auto conn = std::make_shared<Connection>(rpcUrl, std:("confirmed"));
    console->log(std:("[Solana Backfill] Fetching recent transactions for program"), programId);
    auto sigs = OR((signatures), ((std::async([=]() { conn->getSignaturesForAddress(std::make_shared<PublicKey>(programId), object{
        object::pair{std:("limit"), 100}
    }); }))->map([=](auto s) mutable
    {
        return s["signature"];
    }
    )));
    console->log(std:("[Solana Backfill] Found ") + sigs["length"] + std:(" transactions"));
    for (auto& sig : sigs)
    {
        try
        {
            auto tx = std::async([=]() { conn->getTransaction(sig, object{
                object::pair{std:("commitment"), std:("confirmed")}, 
                object::pair{std:("maxSupportedTransactionVersion"), 0}
            }); });
            if (AND((AND((tx), (tx->meta))), (tx->meta->logMessages))) {
                auto hasRegisterToken = tx->meta->logMessages->some([=](auto log) mutable
                {
                    return OR((log["includes"](std:("Instruction: RegisterToken"))), (log["includes"](std:("register_token"))));
                }
                );
                if (hasRegisterToken) {
                    std::async([=]() { parseRegisterTokenTransaction(tx); });
                }
            }
        }
        catch (const any& error)
        {
            console->warn(std:("[Solana Backfill] Failed to process signature ") + sig + std:(":"), error);
        }
    }
    console->log(std:("[Solana Backfill] ✅ Backfill complete"));
};


boolean isListening = false;
any connection = nullptr;

void Main(void)
{
}

MAIN
