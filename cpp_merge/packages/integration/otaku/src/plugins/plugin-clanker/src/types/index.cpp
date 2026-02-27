#include "/home/runner/work/elizaos-cpp/elizaos-cpp/otaku/src/plugins/plugin-clanker/src/types/index.h"

string NATIVE_TOKEN_ADDRESSES = std::string("0xEeeeeEeeeEeEeeEeEeEeeEEEeeeeEeeeeeeeEEeE");
any ClankerConfigSchema = z->object(object{
    object::pair{std::string("BASE_RPC_URL"), z->string()->min(1, std::string("Base RPC URL is required"))}, 
    object::pair{std::string("DEFAULT_SLIPPAGE"), z->number()->default(0.05)}, 
    object::pair{std::string("MAX_GAS_PRICE"), z->string()->default(std::string("100000000000"))}, 
    object::pair{std::string("RETRY_ATTEMPTS"), z->number()->default(3)}, 
    object::pair{std::string("NETWORK"), z->enum(array<string>{ std::string("base"), std::string("base-sepolia") })->default(std::string("base"))}
});
any TokenDeploySchema = z->object(object{
    object::pair{std::string("name"), z->string()->min(1)->max(50)}, 
    object::pair{std::string("symbol"), z->string()->min(1)->max(10)->toUpperCase()}, 
    object::pair{std::string("tokenAdmin"), z->string()->regex((new RegExp(std::string("^0x[a-fA-F0-9]{40}"))))->optional()}, 
    object::pair{std::string("vanity"), z->boolean()->optional()}, 
    object::pair{std::string("image"), z->string()->optional()}, 
    object::pair{std::string("metadata"), z->object(object{
        object::pair{std::string("description"), z->string()->optional()}, 
        object::pair{std::string("socialMediaUrls"), z->array(z->string()->url())->optional()}, 
        object::pair{std::string("auditUrls"), z->array(z->string()->url())->optional()}
    })->optional()}, 
    object::pair{std::string("context"), z->object(object{
        object::pair{std::string("interface"), z->string()->optional()}, 
        object::pair{std::string("platform"), z->string()->optional()}, 
        object::pair{std::string("messageId"), z->string()->optional()}, 
        object::pair{std::string("id"), z->string()->optional()}
    })->optional()}, 
    object::pair{std::string("pool"), z->object(object{
        object::pair{std::string("pairedToken"), z->string()->regex((new RegExp(std::string("^0x[a-fA-F0-9]{40}"))))->optional()}, 
        object::pair{std::string("tickIfToken0IsClanker"), z->number()->optional()}, 
        object::pair{std::string("positions"), z->array(z->object(object{
            object::pair{std::string("tickLower"), z->number()}, 
            object::pair{std::string("tickUpper"), z->number()}, 
            object::pair{std::string("positionBps"), z->number()}
        }))->optional()}
    })->optional()}, 
    object::pair{std::string("fees"), z->union(array<any>{ z->object(object{
        object::pair{std::string("type"), z->literal(std::string("static"))}, 
        object::pair{std::string("clankerFee"), z->number()}, 
        object::pair{std::string("pairedFee"), z->number()}
    }), z->object(object{
        object::pair{std::string("type"), z->literal(std::string("dynamic"))}
    }) })->optional()}, 
    object::pair{std::string("rewards"), z->object(object{
        object::pair{std::string("recipients"), z->array(z->object(object{
            object::pair{std::string("recipient"), z->string()->regex((new RegExp(std::string("^0x[a-fA-F0-9]{40}"))))}, 
            object::pair{std::string("admin"), z->string()->regex((new RegExp(std::string("^0x[a-fA-F0-9]{40}"))))}, 
            object::pair{std::string("bps"), z->number()->min(0)->max(10000)}, 
            object::pair{std::string("token"), z->enum(array<string>{ std::string("Both"), std::string("Paired"), std::string("Clanker") })}
        }))}
    })->optional()}, 
    object::pair{std::string("vault"), z->object(object{
        object::pair{std::string("percentage"), z->number()->min(0)->max(90)}, 
        object::pair{std::string("lockupDuration"), z->number()->min(604800)}, 
        object::pair{std::string("vestingDuration"), z->number()->min(0)}
    })->optional()}, 
    object::pair{std::string("devBuy"), z->object(object{
        object::pair{std::string("ethAmount"), z->number()->min(0)}, 
        object::pair{std::string("poolKey"), z->object(object{
            object::pair{std::string("currency0"), z->string()->regex((new RegExp(std::string("^0x[a-fA-F0-9]{40}"))))}, 
            object::pair{std::string("currency1"), z->string()->regex((new RegExp(std::string("^0x[a-fA-F0-9]{40}"))))}, 
            object::pair{std::string("fee"), z->number()}, 
            object::pair{std::string("tickSpacing"), z->number()}, 
            object::pair{std::string("hooks"), z->string()->regex((new RegExp(std::string("^0x[a-fA-F0-9]{40}"))))}
        })->optional()}, 
        object::pair{std::string("amountOutMin"), z->number()->optional()}
    })->optional()}
});
any SwapSchema = z->object(object{
    object::pair{std::string("fromToken"), z->string()->regex((new RegExp(std::string("^0x[a-fA-F0-9]{40}"))))}, 
    object::pair{std::string("toToken"), z->string()->regex((new RegExp(std::string("^0x[a-fA-F0-9]{40}"))))}, 
    object::pair{std::string("amount"), z->string()->regex((new RegExp(std::string("^\d+"))))}, 
    object::pair{std::string("slippage"), z->number()->min(0)->max(0.5)->optional()}, 
    object::pair{std::string("recipient"), z->string()->regex((new RegExp(std::string("^0x[a-fA-F0-9]{40}"))))->optional()}
});
any LiquiditySchema = z->object(object{
    object::pair{std::string("tokenA"), z->string()->regex((new RegExp(std::string("^0x[a-fA-F0-9]{40}"))))}, 
    object::pair{std::string("tokenB"), z->string()->regex((new RegExp(std::string("^0x[a-fA-F0-9]{40}"))))}, 
    object::pair{std::string("amountA"), z->string()->regex((new RegExp(std::string("^\d+"))))}, 
    object::pair{std::string("amountB"), z->string()->regex((new RegExp(std::string("^\d+"))))}, 
    object::pair{std::string("slippage"), z->number()->min(0)->max(0.5)->optional()}
});
object POOL_POSITIONS = object{
    object::pair{std::string("Standard"), array<object>{ object{
        object::pair{std::string("tickLower"), -60000}, 
        object::pair{std::string("tickUpper"), -20000}, 
        object::pair{std::string("positionBps"), 8000}
    }, object{
        object::pair{std::string("tickLower"), -20000}, 
        object::pair{std::string("tickUpper"), 100000}, 
        object::pair{std::string("positionBps"), 2000}
    } }}, 
    object::pair{std::string("Project"), array<object>{ object{
        object::pair{std::string("tickLower"), -60000}, 
        object::pair{std::string("tickUpper"), -20000}, 
        object::pair{std::string("positionBps"), 6000}
    }, object{
        object::pair{std::string("tickLower"), -20000}, 
        object::pair{std::string("tickUpper"), 60000}, 
        object::pair{std::string("positionBps"), 3000}
    }, object{
        object::pair{std::string("tickLower"), 60000}, 
        object::pair{std::string("tickUpper"), 100000}, 
        object::pair{std::string("positionBps"), 1000}
    } }}
};
object FEE_CONFIGS = object{
    object::pair{std::string("StaticBasic"), object{
        object::pair{std::string("type"), as<std::shared_ptr<const>>(std::string("static"))}, 
        object::pair{std::string("clankerFee"), 100}, 
        object::pair{std::string("pairedFee"), 100}
    }}, 
    object::pair{std::string("DynamicBasic"), object{
        object::pair{std::string("type"), as<std::shared_ptr<const>>(std::string("dynamic"))}
    }}
};

void Main(void)
{
}

MAIN
