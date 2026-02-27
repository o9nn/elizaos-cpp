#include "index.hpp"

string NATIVE_TOKEN_ADDRESSES = std:("0xEeeeeEeeeEeEeeEeEeEeeEEEeeeeEeeeeeeeEEeE");
any ClankerConfigSchema = z->object(object{
    object::pair{std:("BASE_RPC_URL"), z->string()->min(1, std:("Base RPC URL is required"))}, 
    object::pair{std:("DEFAULT_SLIPPAGE"), z->number()->default(0.05)}, 
    object::pair{std:("MAX_GAS_PRICE"), z->string()->default(std:("100000000000"))}, 
    object::pair{std:("RETRY_ATTEMPTS"), z->number()->default(3)}, 
    object::pair{std:("NETWORK"), z->enum(array<string>{ std:("base"), std:("base-sepolia") })->default(std:("base"))}
});
any TokenDeploySchema = z->object(object{
    object::pair{std:("name"), z->string()->min(1)->max(50)}, 
    object::pair{std:("symbol"), z->string()->min(1)->max(10)->toUpperCase()}, 
    object::pair{std:("tokenAdmin"), z->string()->regex((new RegExp(std:("^0x[a-fA-F0-9]{40}"))))->optional()}, 
    object::pair{std:("vanity"), z->boolean()->optional()}, 
    object::pair{std:("image"), z->string()->optional()}, 
    object::pair{std:("metadata"), z->object(object{
        object::pair{std:("description"), z->string()->optional()}, 
        object::pair{std:("socialMediaUrls"), z->array(z->string()->url())->optional()}, 
        object::pair{std:("auditUrls"), z->array(z->string()->url())->optional()}
    })->optional()}, 
    object::pair{std:("context"), z->object(object{
        object::pair{std:("interface"), z->string()->optional()}, 
        object::pair{std:("platform"), z->string()->optional()}, 
        object::pair{std:("messageId"), z->string()->optional()}, 
        object::pair{std:("id"), z->string()->optional()}
    })->optional()}, 
    object::pair{std:("pool"), z->object(object{
        object::pair{std:("pairedToken"), z->string()->regex((new RegExp(std:("^0x[a-fA-F0-9]{40}"))))->optional()}, 
        object::pair{std:("tickIfToken0IsClanker"), z->number()->optional()}, 
        object::pair{std:("positions"), z->array(z->object(object{
            object::pair{std:("tickLower"), z->number()}, 
            object::pair{std:("tickUpper"), z->number()}, 
            object::pair{std:("positionBps"), z->number()}
        }))->optional()}
    })->optional()}, 
    object::pair{std:("fees"), z->union(array<any>{ z->object(object{
        object::pair{std:("type"), z->literal(std:("static"))}, 
        object::pair{std:("clankerFee"), z->number()}, 
        object::pair{std:("pairedFee"), z->number()}
    }), z->object(object{
        object::pair{std:("type"), z->literal(std:("dynamic"))}
    }) })->optional()}, 
    object::pair{std:("rewards"), z->object(object{
        object::pair{std:("recipients"), z->array(z->object(object{
            object::pair{std:("recipient"), z->string()->regex((new RegExp(std:("^0x[a-fA-F0-9]{40}"))))}, 
            object::pair{std:("admin"), z->string()->regex((new RegExp(std:("^0x[a-fA-F0-9]{40}"))))}, 
            object::pair{std:("bps"), z->number()->min(0)->max(10000)}, 
            object::pair{std:("token"), z->enum(array<string>{ std:("Both"), std:("Paired"), std:("Clanker") })}
        }))}
    })->optional()}, 
    object::pair{std:("vault"), z->object(object{
        object::pair{std:("percentage"), z->number()->min(0)->max(90)}, 
        object::pair{std:("lockupDuration"), z->number()->min(604800)}, 
        object::pair{std:("vestingDuration"), z->number()->min(0)}
    })->optional()}, 
    object::pair{std:("devBuy"), z->object(object{
        object::pair{std:("ethAmount"), z->number()->min(0)}, 
        object::pair{std:("poolKey"), z->object(object{
            object::pair{std:("currency0"), z->string()->regex((new RegExp(std:("^0x[a-fA-F0-9]{40}"))))}, 
            object::pair{std:("currency1"), z->string()->regex((new RegExp(std:("^0x[a-fA-F0-9]{40}"))))}, 
            object::pair{std:("fee"), z->number()}, 
            object::pair{std:("tickSpacing"), z->number()}, 
            object::pair{std:("hooks"), z->string()->regex((new RegExp(std:("^0x[a-fA-F0-9]{40}"))))}
        })->optional()}, 
        object::pair{std:("amountOutMin"), z->number()->optional()}
    })->optional()}
});
any SwapSchema = z->object(object{
    object::pair{std:("fromToken"), z->string()->regex((new RegExp(std:("^0x[a-fA-F0-9]{40}"))))}, 
    object::pair{std:("toToken"), z->string()->regex((new RegExp(std:("^0x[a-fA-F0-9]{40}"))))}, 
    object::pair{std:("amount"), z->string()->regex((new RegExp(std:("^\d+"))))}, 
    object::pair{std:("slippage"), z->number()->min(0)->max(0.5)->optional()}, 
    object::pair{std:("recipient"), z->string()->regex((new RegExp(std:("^0x[a-fA-F0-9]{40}"))))->optional()}
});
any LiquiditySchema = z->object(object{
    object::pair{std:("tokenA"), z->string()->regex((new RegExp(std:("^0x[a-fA-F0-9]{40}"))))}, 
    object::pair{std:("tokenB"), z->string()->regex((new RegExp(std:("^0x[a-fA-F0-9]{40}"))))}, 
    object::pair{std:("amountA"), z->string()->regex((new RegExp(std:("^\d+"))))}, 
    object::pair{std:("amountB"), z->string()->regex((new RegExp(std:("^\d+"))))}, 
    object::pair{std:("slippage"), z->number()->min(0)->max(0.5)->optional()}
});
object POOL_POSITIONS = object{
    object::pair{std:("Standard"), array<object>{ object{
        object::pair{std:("tickLower"), -60000}, 
        object::pair{std:("tickUpper"), -20000}, 
        object::pair{std:("positionBps"), 8000}
    }, object{
        object::pair{std:("tickLower"), -20000}, 
        object::pair{std:("tickUpper"), 100000}, 
        object::pair{std:("positionBps"), 2000}
    } }}, 
    object::pair{std:("Project"), array<object>{ object{
        object::pair{std:("tickLower"), -60000}, 
        object::pair{std:("tickUpper"), -20000}, 
        object::pair{std:("positionBps"), 6000}
    }, object{
        object::pair{std:("tickLower"), -20000}, 
        object::pair{std:("tickUpper"), 60000}, 
        object::pair{std:("positionBps"), 3000}
    }, object{
        object::pair{std:("tickLower"), 60000}, 
        object::pair{std:("tickUpper"), 100000}, 
        object::pair{std:("positionBps"), 1000}
    } }}
};
object FEE_CONFIGS = object{
    object::pair{std:("StaticBasic"), object{
        object::pair{std:("type"), as<std::shared_ptr<const>>(std:("static"))}, 
        object::pair{std:("clankerFee"), 100}, 
        object::pair{std:("pairedFee"), 100}
    }}, 
    object::pair{std:("DynamicBasic"), object{
        object::pair{std:("type"), as<std::shared_ptr<const>>(std:("dynamic"))}
    }}
};

void Main(void)
{
}

MAIN
