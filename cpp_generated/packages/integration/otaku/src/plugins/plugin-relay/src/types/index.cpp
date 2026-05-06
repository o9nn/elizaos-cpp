#include "/home/runner/work/elizaos-cpp/elizaos-cpp/otaku/src/plugins/plugin-relay/src/types/index.h"

object RelaySupportedChains = as<std::shared_ptr<const>>(object{
    object::pair{std::string("ETHEREUM"), 1}, 
    object::pair{std::string("BASE"), 8453}, 
    object::pair{std::string("ARBITRUM"), 42161}, 
    object::pair{std::string("POLYGON"), 137}, 
    object::pair{std::string("OPTIMISM"), 10}, 
    object::pair{std::string("ZORA"), 7777777}, 
    object::pair{std::string("BLAST"), 81457}, 
    object::pair{std::string("SCROLL"), 534352}, 
    object::pair{std::string("LINEA"), 59144}
});
any RelayCurrencies = as<std::shared_ptr<const>>(array<std::shared_ptr<const>>{ std::string("eth"), std::string("usdc"), std::string("usdt"), std::string("weth"), std::string("usdc.e"), std::string("wbtc"), std::string("degen"), std::string("tia") });
any QuoteRequestSchema = z->object(object{
    object::pair{std::string("user"), z->string()->describe(std::string("User wallet address"))}, 
    object::pair{std::string("chainId"), z->number()->describe(std::string("Origin chain ID"))}, 
    object::pair{std::string("toChainId"), z->number()->describe(std::string("Destination chain ID"))}, 
    object::pair{std::string("currency"), z->string()->describe(std::string("Currency on origin chain"))}, 
    object::pair{std::string("toCurrency"), z->string()->optional()->describe(std::string("Currency on destination chain"))}, 
    object::pair{std::string("amount"), z->string()->describe(std::string("Amount in wei"))}, 
    object::pair{std::string("recipient"), z->string()->optional()->describe(std::string("Recipient address (defaults to user)"))}, 
    object::pair{std::string("tradeType"), z->enum(array<string>{ std::string("EXACT_INPUT"), std::string("EXACT_OUTPUT") })->optional()->default(std::string("EXACT_INPUT"))}, 
    object::pair{std::string("referrer"), z->string()->optional()->describe(std::string("Referrer address for fees"))}
});
any BridgeRequestSchema = z->object(object{
    object::pair{std::string("originChain"), z->string()->describe(std::string("Origin chain name (e.g., 'ethereum', 'base', 'arbitrum')"))}, 
    object::pair{std::string("destinationChain"), z->string()->describe(std::string("Destination chain name (e.g., 'ethereum', 'base', 'arbitrum')"))}, 
    object::pair{std::string("currency"), z->string()->describe(std::string("Currency to bridge (symbol like 'eth', 'usdc')"))}, 
    object::pair{std::string("amount"), z->string()->describe(std::string("Amount in human-readable format (e.g., '0.5' for 0.5 ETH)"))}, 
    object::pair{std::string("recipient"), z->string()->optional()->describe(std::string("Recipient address (defaults to user's wallet)"))}, 
    object::pair{std::string("useExactInput"), z->boolean()->optional()->default(true)}, 
    object::pair{std::string("useExternalLiquidity"), z->boolean()->optional()->default(false)}, 
    object::pair{std::string("referrer"), z->string()->optional()->describe(std::string("Referrer address"))}
});
any ExecuteCallRequestSchema = z->object(object{
    object::pair{std::string("user"), z->string()->describe(std::string("User wallet address"))}, 
    object::pair{std::string("originChainId"), z->number()->describe(std::string("Origin chain ID"))}, 
    object::pair{std::string("destinationChainId"), z->number()->describe(std::string("Destination chain ID"))}, 
    object::pair{std::string("originCurrency"), z->string()->describe(std::string("Currency on origin chain"))}, 
    object::pair{std::string("amount"), z->string()->describe(std::string("Amount in wei"))}, 
    object::pair{std::string("txs"), z->array(z->object(object{
        object::pair{std::string("to"), z->string()->describe(std::string("Contract address to call"))}, 
        object::pair{std::string("value"), z->string()->describe(std::string("Value to send"))}, 
        object::pair{std::string("data"), z->string()->describe(std::string("Calldata"))}
    }))->describe(std::string("Array of transactions to execute on destination"))}, 
    object::pair{std::string("recipient"), z->string()->optional()->describe(std::string("Recipient address"))}
});
any StatusRequestSchema = z->object(object{
    object::pair{std::string("requestId"), z->string()->optional()->describe(std::string("Request ID to check status"))}, 
    object::pair{std::string("txHash"), z->string()->optional()->describe(std::string("Transaction hash to check status"))}, 
    object::pair{std::string("user"), z->string()->optional()->describe(std::string("Filter by user address"))}
});

void Main(void)
{
}

MAIN
