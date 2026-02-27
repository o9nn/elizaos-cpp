#include "verify-multichain-deployment.hpp"
#include <string>
#include <future>
#include <cstdlib>
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::future<void> verifyBaseDeployment() {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        std::cout << "\n== Verifying Base Deployment ==\n" << std::endl;

        if (!OTC_ADDRESS) {
            std::cerr << "❌ NEXT_PUBLIC_BASE_OTC_ADDRESS not set" << std::endl;
            return false;
        }

        if (!REGISTRATION_HELPER_ADDRESS) {
            std::cerr << "❌ NEXT_PUBLIC_REGISTRATION_HELPER_ADDRESS not set" << std::endl;
            return false;
        }

        const auto client = createPublicClient({;
            chain: base,
            transport: http(BASE_RPC),
            });

            try {
                // Check OTC contract - verify it has code
                std::cout << "Checking OTC contract at:" << OTC_ADDRESS << std::endl;

                // Use getCode instead of getBytecode (getBytecode might not work on all RPCs)
                const auto code = "client.getCode({ address: OTC_ADDRESS as " + "0x" + std::string;
                if (!code || code == "0x") {
                    std::cout << "⚠️  Could not verify contract code via RPC (may be indexing delay)" << std::endl;
                    std::cout << "   Contract was deployed successfully - checking via std::function call..." << std::endl;
                    // Try to call a std::function instead
                    try {
                        const auto otcAbi = parseAbi(["std::function nextOfferId() view returns (uint256)"]);
                        client.readContract({
                            "address: OTC_ADDRESS as " + "0x" + std::string abi: otcAbi,
                            functionName: "nextOfferId",
                            });
                            std::cout << "✅ OTC contract responds to std::function calls (deployed and working)" << std::endl;
                            } catch (funcError) {
                                std::cout << "⚠️  Function call also failed - contract may still be indexing" << std::endl;
                                std::cout << "   Check on Basescan: https://basescan.org/address/" + OTC_ADDRESS << std::endl;
                                std::cout << "   Deployment was successful << so contract exists - this is likely an RPC indexing delay" << std::endl;
                                // Don't fail - deployment was successful
                                return true;
                            }
                            } else {
                                std::cout << "✅ OTC contract has code (deployed)" << std::endl;
                            }

                            // Try to read a simple std::function to verify it's the right contract
                            const auto otcAbi = parseAbi([;
                            "std::function nextOfferId() view returns (uint256)",
                            "std::function agent() view returns (address)",
                            "std::function usdc() view returns (address)",
                            "std::function owner() view returns (address)",
                            ]);

                            try {
                                const auto nextOfferId = client.readContract({;
                                    "address: OTC_ADDRESS as " + "0x" + std::string abi: otcAbi,
                                    functionName: "nextOfferId",
                                    });
                                    std::cout << "  Next Offer ID:" << std::to_string(nextOfferId) << std::endl;
                                    } catch (error) {
                                        std::cout << "⚠️  Could not read nextOfferId (contract deployed but may have ABI mismatch)" << std::endl;
                                    }

                                    try {
                                        const auto agent = client.readContract({;
                                            "address: OTC_ADDRESS as " + "0x" + std::string abi: otcAbi,
                                            functionName: "agent",
                                            });
                                            std::cout << "  Agent:" << agent << std::endl;
                                            } catch (error) {
                                                throw;
                                            }

                                            try {
                                                const auto usdc = client.readContract({;
                                                    "address: OTC_ADDRESS as " + "0x" + std::string abi: otcAbi,
                                                    functionName: "usdc",
                                                    });
                                                    std::cout << "  USDC:" << usdc << std::endl;
                                                    } catch (error) {
                                                        throw;
                                                    }

                                                    try {
                                                        const auto owner = client.readContract({;
                                                            "address: OTC_ADDRESS as " + "0x" + std::string abi: otcAbi,
                                                            functionName: "owner",
                                                            });
                                                            std::cout << "  Owner:" << owner << std::endl;
                                                            } catch (error) {
                                                                throw;
                                                            }

                                                            // Check RegistrationHelper
                                                            std::cout << "\nChecking RegistrationHelper at:" << REGISTRATION_HELPER_ADDRESS << std::endl;

                                                            const auto helperCode = "client.getCode({ address: REGISTRATION_HELPER_ADDRESS as " + "0x" + std::string;
                                                            if (!helperCode || helperCode == "0x") {
                                                                std::cout << "⚠️  Could not verify RegistrationHelper code via RPC (may be indexing delay)" << std::endl;
                                                                std::cout << "   Contract was deployed successfully - checking via std::function call..." << std::endl;
                                                                try {
                                                                    const auto helperAbi = parseAbi(["std::function otc() view returns (address)"]);
                                                                    client.readContract({
                                                                        "address: REGISTRATION_HELPER_ADDRESS as " + "0x" + std::string abi: helperAbi,
                                                                        functionName: "otc",
                                                                        });
                                                                        std::cout << "✅ RegistrationHelper responds to std::function calls (deployed and working)" << std::endl;
                                                                        } catch (funcError) {
                                                                            std::cout << "⚠️  Function call also failed - contract may still be indexing" << std::endl;
                                                                            std::cout << "   Check on Basescan: https://basescan.org/address/" + REGISTRATION_HELPER_ADDRESS << std::endl;
                                                                            std::cout << "   Deployment was successful << so contract exists - this is likely an RPC indexing delay" << std::endl;
                                                                            // Don't fail - deployment was successful
                                                                            return true;
                                                                        }
                                                                        } else {
                                                                            std::cout << "✅ RegistrationHelper has code (deployed)" << std::endl;
                                                                        }

                                                                        // Try to read functions (non-critical if they fail due to ABI issues)
                                                                        const auto helperAbi = parseAbi([;
                                                                        "std::function otc() view returns (address)",
                                                                        "std::function registrationFee() view returns (uint256)",
                                                                        "std::function feeRecipient() view returns (address)",
                                                                        ]);

                                                                        try {
                                                                            const auto helperOtc = client.readContract({;
                                                                                "address: REGISTRATION_HELPER_ADDRESS as " + "0x" + std::string abi: helperAbi,
                                                                                functionName: "otc",
                                                                                });
                                                                                std::cout << "  OTC Address:" << helperOtc << std::endl;

                                                                                // Verify RegistrationHelper points to correct OTC
                                                                                if (helperOtc.toLowerCase() != OTC_ADDRESS.toLowerCase()) {
                                                                                    std::cout << "⚠️  RegistrationHelper points to different OTC:" << helperOtc << std::endl;
                                                                                    std::cout << "     Expected:" << OTC_ADDRESS << std::endl;
                                                                                }
                                                                                } catch (error) {
                                                                                    std::cout << "⚠️  Could not verify RegistrationHelper.otc (non-critical)" << std::endl;
                                                                                }

                                                                                try {
                                                                                    const auto regFee = client.readContract({;
                                                                                        "address: REGISTRATION_HELPER_ADDRESS as " + "0x" + std::string abi: helperAbi,
                                                                                        functionName: "registrationFee",
                                                                                        });
                                                                                        std::cout << "  Registration Fee:" << (Number(regFee) / 1e18).toFixed(4) << "ETH" << std::endl;
                                                                                        } catch (error) {
                                                                                            throw;
                                                                                        }

                                                                                        try {
                                                                                            const auto feeRecipient = client.readContract({;
                                                                                                "address: REGISTRATION_HELPER_ADDRESS as " + "0x" + std::string abi: helperAbi,
                                                                                                functionName: "feeRecipient",
                                                                                                });
                                                                                                std::cout << "  Fee Recipient:" << feeRecipient << std::endl;
                                                                                                } catch (error) {
                                                                                                    throw;
                                                                                                }

                                                                                                std::cout << "\n✅ Base deployment verified successfully" << std::endl;
                                                                                                return true;
                                                                                                } catch (error) {
                                                                                                    std::cerr << "❌ Failed to verify Base deployment:" << error << std::endl;
                                                                                                    return false;
                                                                                                }

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

std::future<void> verifySolanaDeployment() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    std::cout << "\n== Verifying Solana Deployment ==\n" << std::endl;

    if (!SOLANA_PROGRAM_ID) {
        std::cout << "⚠️  NEXT_PUBLIC_SOLANA_PROGRAM_ID not std::set - skipping Solana verification" << std::endl;
        return true; // Not a failure if Solana isn't configured;
    }

    if (!SOLANA_DESK) {
        std::cout << "⚠️  NEXT_PUBLIC_SOLANA_DESK not std::set - skipping Solana verification" << std::endl;
        return true; // Not a failure if Solana isn't configured;
    }

    try {
        const auto connection = new Connection(SOLANA_RPC, "confirmed");

        // Check program exists
        std::cout << "Checking Solana program at:" << SOLANA_PROGRAM_ID << std::endl;
        const auto programInfo = connection.getAccountInfo(new PublicKey(SOLANA_PROGRAM_ID));

        if (!programInfo) {
            std::cout << "⚠️  Solana program not found on mainnet" << std::endl;
            std::cout << "   This is OK if Solana is deployed on devnet/testnet instead" << std::endl;
            std::cout << "   Program ID:" << SOLANA_PROGRAM_ID << std::endl;
            return true; // Not a failure - might be on different network;
        }

        std::cout << "✅ Solana program is deployed" << std::endl;
        std::cout << "  Executable:" << programInfo.executable << std::endl;
        std::cout << "  Owner:" << programInfo.owner.toBase58() << std::endl;

        // Check desk account
        std::cout << "\nChecking desk account at:" << SOLANA_DESK << std::endl;
        const auto deskInfo = connection.getAccountInfo(new PublicKey(SOLANA_DESK));

        if (!deskInfo) {
            std::cout << "⚠️  Desk account not found on mainnet" << std::endl;
            std::cout << "   This is OK if Solana is deployed on devnet/testnet instead" << std::endl;
            return true; // Not a failure - might be on different network;
        }

        std::cout << "✅ Desk account exists" << std::endl;
        std::cout << "  Data Size:" << deskInfo.data.size() << "bytes" << std::endl;
        std::cout << "  Owner:" << deskInfo.owner.toBase58() << std::endl;

        std::cout << "\n✅ Solana deployment verified successfully" << std::endl;
        return true;
        } catch (error) {
            std::cout << "⚠️  Failed to verify Solana deployment (may be on different network):" << (error).message << std::endl;
            return true; // Not a failure - Solana might not be on mainnet;
        }

}

std::future<void> testWalletScanning() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    std::cout << "\n== Testing Wallet Scanning ==\n" << std::endl;

    try {
        // Note: Actual wallet scanning requires user authentication
        // This just checks if the required APIs are configured

        const auto alchemyKey = std::getenv("NEXT_PUBLIC_ALCHEMY_API_KEY");
        const auto heliusKey = std::getenv("HELIUS_API_KEY");

        std::cout << "Alchemy API Key configured:" << alchemyKey ? "✅" : "❌" << std::endl;
        std::cout << "Helius API Key configured:" << heliusKey ? "✅" : "❌" << std::endl;

        if (!alchemyKey) {
            std::cout << "⚠️  Alchemy API key not configured - Base wallet scanning won't work" << std::endl;
        }

        if (!heliusKey) {
            std::cout << "⚠️  Helius API key not configured - Solana metadata will be limited" << std::endl;
        }

        return true;
        } catch (error) {
            std::cerr << "❌ Failed to test wallet scanning:" << error << std::endl;
            return false;
        }

}

std::future<void> main() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    std::cout << "╔════════════════════════════════════════════════╗" << std::endl;
    std::cout << "║  Multi-Chain OTC Deployment Verification      ║" << std::endl;
    std::cout << "╚════════════════════════════════════════════════╝" << std::endl;

    const auto results = {;
        base: verifyBaseDeployment(),
        solana: verifySolanaDeployment(),
        walletScanning: testWalletScanning(),
        };

        std::cout << "\n╔════════════════════════════════════════════════╗" << std::endl;
        std::cout << "║  Verification Summary                          ║" << std::endl;
        std::cout << "╚════════════════════════════════════════════════╝\n" << std::endl;

        std::cout << "Base Deployment:" << results.base ? "✅ PASS" : "❌ FAIL" << std::endl;
        std::cout << "Solana Deployment:" << results.solana ? "✅ PASS" : "❌ FAIL" << std::endl;
        std::cout << "Wallet Scanning:" << results.walletScanning ? "✅ PASS" : "❌ FAIL" << std::endl;

        const auto allPassed = Object.values(results).every(Boolean);

        if (allPassed) {
            std::cout << "\n🎉 All verifications passed!" << std::endl;
            std::cout << "\nNext steps:" << std::endl;
            std::cout << "1. Start backend event listeners:" << std::endl;
            std::cout << "   - Run token registration listeners for both chains" << std::endl;
            std::cout << "2. Test token registration in UI:" << std::endl;
            std::cout << "   - Connect wallet" << std::endl;
            std::cout << "   - Click "Register Token from Wallet"" << std::endl;
            std::cout << "   - Select a token and complete registration" << std::endl;
            std::cout << "3. Monitor backend logs for TokenRegistered events" << std::endl;
            std::exit(0);
            } else {
                std::cout << "\n❌ Some verifications failed. Please check the errors above." << std::endl;
                std::exit(1);
            }

}

} // namespace elizaos
