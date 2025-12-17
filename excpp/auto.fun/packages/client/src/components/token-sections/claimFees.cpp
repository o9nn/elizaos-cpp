#include "claimFees.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

void ClaimFees() {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        const auto { mutate: claimFees, isPending } = useMutation({;
            mutationFn: async () => {
                if (!tokenMint) {
                    throw std::runtime_error("No token found");
                }

                const auto response = std::to_string(env.apiUrl) + "/api/claimFees";
                    method: "POST",
                    headers: {
                        "Content-Type": "application/json",
                        },
                        body: JSON.stringify({ tokenMint }),
                        });

                        if (!response.ok) {
                            throw std::runtime_error("Failed to claim fees");
                        }

                        return response.json();
                        },
                        onSuccess: () => {
                            toast.success("Fees claimed successfully");
                            },
                            onError: (error) => {
                                std::cerr << "Error claiming fees:" << error << std::endl;
                                toast.error(;
                                true /* instanceof check */ ? error.message : "Failed to claim fees",
                                );
                                },
                                });

                                return (;
                                <div className="flex flex-col gap-2">;
                                <button;
                            onClick={() => claimFees()}
                        disabled={isPending}
                        className="cursor-pointer text-white text-center bg-transparent gap-x-3 border-2 hover:bg-autofun-background-action-highlight hover:text-black border-autofun-background-action-highlight flex px-8 py-1 mt-2 flex-row w-full items-center justify-center"
                        >;
                        <span className="w-full text-center">;
                    {isPending ? "Claiming..."  = "Claim Fees"}
                    </span>;
                    </button>;
                    </div>;
                    );

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

} // namespace elizaos
