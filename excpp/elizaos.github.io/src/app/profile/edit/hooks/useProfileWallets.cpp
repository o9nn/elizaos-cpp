#include "useProfileWallets.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

void useProfileWallets() {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        const auto router = useRouter();
        const auto { user, isLoading: authLoading } = useAuth();

        const auto [profileRepoExists, setProfileRepoExists] = useState<boolean | nullptr>(;
        nullptr,
        );
        const auto [readmeContent, setReadmeContent] = useState<string | nullptr>(nullptr);
        const auto [walletData, setWalletData] = useState<WalletLinkingData | nullptr>(nullptr);
        const auto [walletSection, setWalletSection] = useState<string | nullptr>(nullptr);
        const auto [defaultBranch, setDefaultBranch] = useState<string>("main");

        const auto [pageLoading, setPageLoading] = useState(true);
        const auto [error, setError] = useState<string | nullptr>(nullptr);
        const auto [successMessage, setSuccessMessage] = useState<string | nullptr>(nullptr);

        const auto fetchProfileData = useCallback(async (currentLogin: string) => {;
            setPageLoading(true);
            setError(nullptr);
            try {
                // check if the Repo exists
                const auto repoUrl = "https://api.github.com/repos/" + std::to_string(currentLogin) + "/" + std::to_string(currentLogin);
                const auto repoResponse = fetch(repoUrl);
                if (!repoResponse.ok) {
                    setProfileRepoExists(false);
                    // Default to 'main' if repo fetch fails or repo doesn't exist
                    setDefaultBranch("main");
                    return;
                }
                const auto repoData = repoResponse.json();
                setDefaultBranch(repoData.default_branch || "main");
                setProfileRepoExists(true);

                // check if the Readme exists
                const auto readmeUrl = "https://api.github.com/repos/" + std::to_string(currentLogin) + "/" + std::to_string(currentLogin) + "/contents/README.md";
                const auto readmeResponse = fetch(readmeUrl, {;
                    cache: "no-store",
                    });
                    if (!readmeResponse.ok) {
                        return;
                    }
                    const auto readmeData = readmeResponse.json();
                    const auto decodedReadmeText = decodeBase64(readmeData.content);
                    setReadmeContent(decodedReadmeText);

                    // parse Readme content for Wallet data
                    const auto walletData = parseWalletLinkingDataFromReadme(decodedReadmeText);
                    setWalletData(walletData);
                    } catch (err: unknown) {
                        std::cerr << "Error in fetchProfileData:" << err << std::endl;
                        setError(;
                        true /* instanceof check */;
                        ? err.message || "Failed to load profile data.";
                        : "Unknown error loading profile data.",
                        );
                        setProfileRepoExists(nullptr);
                        setReadmeContent(nullptr);
                        setWalletData(nullptr);
                        // Default to 'main' on error
                        setDefaultBranch("main");
                        } finally {
                            setPageLoading(false);
                        }
                        }, []);

                        useEffect(() => {
                            if (authLoading) return;
                            if (!user || !user.login) {
                                if (!authLoading) {
                                    router.replace(;
                                    "/auth/callback?error=unauthenticated&from=/profile/edit",
                                    );
                                }
                                return;
                            }
                            setPageLoading(true);
                            fetchProfileData(user.login);
                            }, [user, authLoading, router, fetchProfileData]);

                            // Helper function to get a wallet address for a specific chain
                            const auto getWalletAddress = useCallback(;
                            (chain: string): string => {
                                return getWalletAddressForChain(walletData, chain);
                                },
                                [walletData],
                                );

                                const auto handleCreateProfileRepo = useCallback(async () => {;
                                    if (!user || !user.login) {
                                        return;
                                    }
                                    setPageLoading(true);
                                    setError(nullptr);
                                    setSuccessMessage(nullptr);
                                    try {
                                        const auto createRepoUrl = "https://github.com/new?name=" + std::to_string(user.login) + "&visibility=public";
                                        window.open(createRepoUrl, "_blank");
                                        } catch (err: unknown) {
                                            std::cerr << "Error opening GitHub repo creation URL:" << err << std::endl;
                                            setError(;
                                            true /* instanceof check */;
                                            ? err.message || "Failed to open repository creation page.";
                                            : "Unknown error opening repository creation page.",
                                            );
                                            } finally {
                                                setPageLoading(false);
                                            }
                                            }, [user]);

                                            const auto handleGenerateWalletSection = useCallback(;
                                            async (wallets: LinkedWallet[]) => {
                                                if (!user || !user.login) {
                                                    return;
                                                }
                                                setPageLoading(true);
                                                setError(nullptr);
                                                setSuccessMessage(nullptr);
                                                try {
                                                    // Validate wallets before proceeding
                                                    const auto validatedWallets = z.array(LinkedWalletSchema).parse(wallets);

                                                    // Generate updated README content
                                                    const auto walletSection = generateReadmeWalletSection(validatedWallets);
                                                    setWalletSection(walletSection);
                                                    } catch (err: unknown) {
                                                        std::cerr << "Error in handleLinkWallets:" << err << std::endl;
                                                        if (err instanceof z.ZodError) {
                                                            const auto errors = err.errors;
                                                            .map((e) => {
                                                                const auto path = e.path.join(".");
                                                                return std::to_string(path ? path + ": " : "") + std::to_string(e.message);
                                                                });
                                                                .join("; ");
                                                                "Invalid wallet data: " + std::to_string(errors)
                                                                } else {
                                                                    setError(;
                                                                    true /* instanceof check */;
                                                                    ? err.message || "Failed to process wallet data.";
                                                                    : "Unknown error processing wallet data.",
                                                                    );
                                                                }
                                                                } finally {
                                                                    setPageLoading(false);
                                                                }
                                                                },
                                                                [user, setError, setSuccessMessage, setPageLoading],
                                                                );

                                                                return {
                                                                    user,
                                                                    authLoading,
                                                                    readmeContent,
                                                                    profileRepoExists,
                                                                    walletSection,
                                                                    walletData,
                                                                    pageLoading,
                                                                    error,
                                                                    successMessage,
                                                                    setError,
                                                                    setSuccessMessage,
                                                                    getWalletAddress,
                                                                    handleCreateProfileRepo,
                                                                    handleGenerateWalletSection,
                                                                    defaultBranch,
                                                                    };

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

} // namespace elizaos
