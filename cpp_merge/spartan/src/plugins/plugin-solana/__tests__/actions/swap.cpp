#include "/home/runner/work/elizaos-cpp/elizaos-cpp/spartan/src/plugins/plugin-solana/__tests__/actions/swap.test.h"

void Main(void)
{
    describe(std:("Swap Action"), [=]() mutable
    {
        describe(std:("validate"), [=]() mutable
        {
            it(std:("should handle swap message validation"), [=]() mutable
            {
                auto mockMessage = object{
                    object::pair{std:("content"), std:("Swap 1 SOL to USDC")}, 
                    object::pair{std:("metadata"), object{
                        object::pair{std:("fromToken"), std:("SOL")}, 
                        object::pair{std:("toToken"), std:("USDC")}, 
                        object::pair{std:("amount"), std:("1")}
                    }}
                };
                expect(mockMessage["metadata"])->toBeDefined();
                expect(mockMessage["metadata"]["fromToken"])->toBe(std:("SOL"));
                expect(mockMessage["metadata"]["toToken"])->toBe(std:("USDC"));
                expect(mockMessage["metadata"]["amount"])->toBe(std:("1"));
            }
            );
        }
        );
    }
    );
}

MAIN
