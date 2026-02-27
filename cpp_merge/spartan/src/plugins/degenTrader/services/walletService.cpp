#include "/home/runner/work/elizaos-cpp/elizaos-cpp/spartan/src/plugins/degenTrader/services/walletService.h"

WalletService::WalletService(std::shared_ptr<IAgentRuntime> runtime_) : runtime(runtime_)  {
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

