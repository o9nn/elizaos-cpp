#include "/home/runner/work/elizaos-cpp/elizaos-cpp/spartan/src/plugins/autofunTrader/services/walletService.h"

WalletService::WalletService(std::shared_ptr<IAgentRuntime> runtime_) : runtime(runtime_)  {
    this->_runtime = runtime;
    this->CONFIRMATION_CONFIG = object{
        object::pair{std::string("MAX_ATTEMPTS"), 12}, 
        object::pair{std::string("INITIAL_TIMEOUT"), 2000}, 
        object::pair{std::string("MAX_TIMEOUT"), 20000}, 
        object::pair{std::string("getDelayForAttempt"), [=](auto attempt) mutable
        {
            return Math->min(2000 * Math->pow(1.5, attempt), 20000);
        }
        }
    };
}

std::shared_ptr<Promise<void>> WalletService::initialize()
{
    try
    {
        auto rpcUrl = this->runtime->getSetting(std::string("SOLANA_RPC_URL"));
        if (!rpcUrl) {
            throw any(std::make_shared<Error>(std::string("Solana RPC URL not configured")));
        }
        this->connection = std::make_shared<Connection>(rpcUrl);
        auto privateKey = this->runtime->getSetting(std::string("SOLANA_PRIVATE_KEY"));
        if (!privateKey) {
            throw any(std::make_shared<Error>(std::string("Solana private key not configured")));
        }
        auto decodedKey = bs58->decode(privateKey);
        this->keypair = Keypair->fromSecretKey(decodedKey);
        logger->info(std::string("Wallet service initialized successfully"));
    }
    catch (const any& error)
    {
        console->log(std::string("Failed to initialize wallet service:"), error);
        throw any(error);
    }
    return std::shared_ptr<Promise<void>>();
}

std::shared_ptr<Promise<void>> WalletService::stop()
{
    this->connection = nullptr;
    this->keypair = nullptr;
    return std::shared_ptr<Promise<void>>();
}

any WalletService::getWallet()
{
    if (OR((!this->keypair), (!this->connection))) {
        throw any(std::make_shared<Error>(std::string("Wallet not initialized")));
    }
    shared keypair = this->keypair;
    return object{
        object::pair{std::string("publicKey"), this->keypair["publicKey"]}, 
        object::pair{std::string("connection"), this->connection}, 
        object::pair{std::string("CONFIRMATION_CONFIG"), this->CONFIRMATION_CONFIG}, 
        , 
        , 
    };
}

any WalletService::getWalletBalances()
{
    try
    {
        auto connection = std::make_shared<Connection>(this->_runtime["getSetting"](std::string("SOLANA_RPC_URL")));
        auto solBalance = std::async([=]() { connection->getBalance(this->keypair["publicKey"]); });
        auto tokenAccounts = std::async([=]() { connection->getParsedTokenAccountsByOwner(this->keypair["publicKey"], object{
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
}

std::shared_ptr<Promise<double>> WalletService::getBalance()
{
    if (OR((!this->keypair), (!this->connection))) {
        throw any(std::make_shared<Error>(std::string("Wallet not initialized")));
    }
    try
    {
        auto balance = std::async([=]() { this->connection["getBalance"](this->keypair["publicKey"]); });
        return balance / 1000000000;
    }
    catch (const any& error)
    {
        console->log(std::string("Error getting wallet balance:"), error);
        throw any(error);
    }
}

