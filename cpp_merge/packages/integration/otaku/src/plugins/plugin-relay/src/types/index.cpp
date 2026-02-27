#include "index.hpp"

object RelaySupportedChains = as<std::shared_ptr<const>>(object{
    object::pair{std:("ETHEREUM"), 1}, 
    object::pair{std:("BASE"), 8453}, 
    object::pair{std:("ARBITRUM"), 42161}, 
    object::pair{std:("POLYGON"), 137}, 
    object::pair{std:("OPTIMISM"), 10}, 
    object::pair{std:("ZORA"), 7777777}, 
    object::pair{std:("BLAST"), 81457}, 
    object::pair{std:("SCROLL"), 534352}, 
    object::pair{std:("LINEA"), 59144}
});
any RelayCurrencies = as<std::shared_ptr<const>>(array<std::shared_ptr<const>>{ std:("eth"), std:("usdc"), std:("usdt"), std:("weth"), std:("usdc.e"), std:("wbtc"), std:("degen"), std:("tia") });
any QuoteRequestSchema = z->object(object{
    object::pair{std:("user"), z->string()->describe(std:("User wallet address"))}, 
    object::pair{std:("chainId"), z->number()->describe(std:("Origin chain ID"))}, 
    object::pair{std:("toChainId"), z->number()->describe(std:("Destination chain ID"))}, 
    object::pair{std:("currency"), z->string()->describe(std:("Currency on origin chain"))}, 
    object::pair{std:("toCurrency"), z->string()->optional()->describe(std:("Currency on destination chain"))}, 
    object::pair{std:("amount"), z->string()->describe(std:("Amount in wei"))}, 
    object::pair{std:("recipient"), z->string()->optional()->describe(std:("Recipient address (defaults to user)"))}, 
    object::pair{std:("tradeType"), z->enum(array<string>{ std:("EXACT_INPUT"), std:("EXACT_OUTPUT") })->optional()->default(std:("EXACT_INPUT"))}, 
    object::pair{std:("referrer"), z->string()->optional()->describe(std:("Referrer address for fees"))}
});
any BridgeRequestSchema = z->object(object{
    object::pair{std:("originChain"), z->string()->describe(std:("Origin chain name (e.g., 'ethereum', 'base', 'arbitrum')"))}, 
    object::pair{std:("destinationChain"), z->string()->describe(std:("Destination chain name (e.g., 'ethereum', 'base', 'arbitrum')"))}, 
    object::pair{std:("currency"), z->string()->describe(std:("Currency to bridge (symbol like 'eth', 'usdc')"))}, 
    object::pair{std:("amount"), z->string()->describe(std:("Amount in human-readable format (e.g., '0.5' for 0.5 ETH)"))}, 
    object::pair{std:("recipient"), z->string()->optional()->describe(std:("Recipient address (defaults to user's wallet)"))}, 
    object::pair{std:("useExactInput"), z->boolean()->optional()->default(true)}, 
    object::pair{std:("useExternalLiquidity"), z->boolean()->optional()->default(false)}, 
    object::pair{std:("referrer"), z->string()->optional()->describe(std:("Referrer address"))}
});
any ExecuteCallRequestSchema = z->object(object{
    object::pair{std:("user"), z->string()->describe(std:("User wallet address"))}, 
    object::pair{std:("originChainId"), z->number()->describe(std:("Origin chain ID"))}, 
    object::pair{std:("destinationChainId"), z->number()->describe(std:("Destination chain ID"))}, 
    object::pair{std:("originCurrency"), z->string()->describe(std:("Currency on origin chain"))}, 
    object::pair{std:("amount"), z->string()->describe(std:("Amount in wei"))}, 
    object::pair{std:("txs"), z->array(z->object(object{
        object::pair{std:("to"), z->string()->describe(std:("Contract address to call"))}, 
        object::pair{std:("value"), z->string()->describe(std:("Value to send"))}, 
        object::pair{std:("data"), z->string()->describe(std:("Calldata"))}
    }))->describe(std:("Array of transactions to execute on destination"))}, 
    object::pair{std:("recipient"), z->string()->optional()->describe(std:("Recipient address"))}
});
any StatusRequestSchema = z->object(object{
    object::pair{std:("requestId"), z->string()->optional()->describe(std:("Request ID to check status"))}, 
    object::pair{std:("txHash"), z->string()->optional()->describe(std:("Transaction hash to check status"))}, 
    object::pair{std:("user"), z->string()->optional()->describe(std:("Filter by user address"))}
});

void Main(void)
{
}

MAIN
