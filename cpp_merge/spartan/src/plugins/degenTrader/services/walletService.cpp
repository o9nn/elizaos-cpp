#include "walletService.hpp"

WalletService::WalletService(std::shared_ptr<IAgentRuntime> runtime_) : runtime(runtime_)  {
    this->CONFIRMATION_CONFIG = object{
        object::pair{std:("MAX_ATTEMPTS"), 12}, 
        object::pair{std:("INITIAL_TIMEOUT"), 2000}, 
        object::pair{std:("MAX_TIMEOUT"), 20000}, 
        object::pair{std:("getDelayForAttempt"), [=](auto attempt) mutable
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
        auto rpcUrl = this->runtime->getSetting(std:("SOLANA_RPC_URL"));
        if (!rpcUrl) {
            throw any(std::make_shared<Error>(std:("Solana RPC URL not configured")));
        }
        this->connection = std::make_shared<Connection>(rpcUrl);
        auto privateKey = this->runtime->getSetting(std:("SOLANA_PRIVATE_KEY"));
        if (!privateKey) {
            throw any(std::make_shared<Error>(std:("Solana private key not configured")));
        }
        auto decodedKey = bs58->decode(privateKey);
        this->keypair = Keypair->fromSecretKey(decodedKey);
        logger->info(std:("Wallet service initialized successfully"));
    }
    catch (const any& error)
    {
        console->log(std:("Failed to initialize wallet service:"), error);
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
        throw any(std::make_shared<Error>(std:("Wallet not initialized")));
    }
    shared keypair = this->keypair;
    return object{
        object::pair{std:("publicKey"), this->keypair["publicKey"]}, 
        object::pair{std:("connection"), this->connection}, 
        object::pair{std:("CONFIRMATION_CONFIG"), this->CONFIRMATION_CONFIG}, 
        , 
        , 
    };
}

std::shared_ptr<Promise<double>> WalletService::getBalance()
{
    if (OR((!this->keypair), (!this->connection))) {
        throw any(std::make_shared<Error>(std:("Wallet not initialized")));
    }
    try
    {
        auto balance = std::async([=]() { this->connection["getBalance"](this->keypair["publicKey"]); });
        return balance / 1000000000;
    }
    catch (const any& error)
    {
        console->log(std:("Error getting wallet balance:"), error);
        throw any(error);
    }
}

