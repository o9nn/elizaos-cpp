#include "/home/runner/work/elizaos-cpp/elizaos-cpp/spartan/src/plugins/plugin-solana/__tests__/actions/swap.test.h"

void Main(void)
{
    describe(std::string("Swap Action"), [=]() mutable
    {
        describe(std::string("validate"), [=]() mutable
        {
            it(std::string("should handle swap message validation"), [=]() mutable
            {
                auto mockMessage = object{
                    object::pair{std::string("content"), std::string("Swap 1 SOL to USDC")}, 
                    object::pair{std::string("metadata"), object{
                        object::pair{std::string("fromToken"), std::string("SOL")}, 
                        object::pair{std::string("toToken"), std::string("USDC")}, 
                        object::pair{std::string("amount"), std::string("1")}
                    }}
                };
                expect(mockMessage["metadata"])->toBeDefined();
                expect(mockMessage["metadata"]["fromToken"])->toBe(std::string("SOL"));
                expect(mockMessage["metadata"]["toToken"])->toBe(std::string("USDC"));
                expect(mockMessage["metadata"]["amount"])->toBe(std::string("1"));
            }
            );
        }
        );
    }
    );
}

MAIN
