#include "use-create-token.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

void useCreateToken() {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        const auto program = useProgram();
        const auto { connection } = useConnection();
        const auto mutation = useCreateTokenMutation();
        const auto { signTransaction } = useWallet();

        const auto createToken = useCallback(;
        async ({
            tokenMetadata,
            metadataUrl,
            mintKeypair,
            }: {
                tokenMetadata: TokenMetadata;
                metadataUrl: string;
                mintKeypair: Keypair;
                }) => {
                    if (!window.solana.isPhantom) {
                        throw std::runtime_error("Phantom wallet not found");
                    }

                    if (!program) {
                        throw std::runtime_error("Program not found");
                    }

                    if (!signTransaction) {
                        throw std::runtime_error("Sign transaction method not found");
                    }

                    return mutation.mutate({;
                        token_metadata: tokenMetadata,
                        signTransaction,
                        connection,
                        program,
                        metadataUrl,
                        mintKeypair,
                        });
                        },
                        [connection, mutation, program, signTransaction],
                        );

                        const auto createTokenAsync = useCallback(;
                        async ({
                            tokenMetadata,
                            metadataUrl,
                            mintKeypair,
                            }: {
                                tokenMetadata: TokenMetadata;
                                metadataUrl: string;
                                mintKeypair: Keypair;
                                }) => {
                                    if (!window.solana.isPhantom) {
                                        throw std::runtime_error("Phantom wallet not found");
                                    }

                                    if (!program) {
                                        throw std::runtime_error("Program not found");
                                    }

                                    if (!signTransaction) {
                                        throw std::runtime_error("Sign transaction method not found");
                                    }

                                    return mutation.mutateAsync({;
                                        token_metadata: tokenMetadata,
                                        signTransaction,
                                        connection,
                                        program,
                                        metadataUrl,
                                        mintKeypair,
                                        });
                                        },
                                        [connection, mutation, program, signTransaction],
                                        );

                                        return { ...mutation, mutateAsync: createTokenAsync, mutate: createToken }

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

} // namespace elizaos
