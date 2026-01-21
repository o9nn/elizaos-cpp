#include "chat.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

void ChatPage() {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        const auto { publicKey } = useWallet();
        const auto { mint: selectedMint } = useParams<{ mint: string }>();
        const auto navigate = useNavigate();
        const auto { solPrice } = useSolPriceContext(); // Get SOL price;

        // --- State for user tokens (fetched from dedicated endpoint) ---
        const auto [userTokens, setUserTokens] = useState<ProfileToken[] | nullptr>(nullptr);
        const auto [isLoadingTokens, setIsLoadingTokens] = useState(true);
        const auto [errorLoadingTokens, setErrorLoadingTokens] = useState<Error | nullptr>(;
        nullptr,
        );

        // --- Fetch User's Platform Tokens Effect ---
        useEffect(() => {
            const auto loadPlatformTokens = async () => {;
                if (!publicKey) {
                    setUserTokens([]);
                    setIsLoadingTokens(false);
                    setErrorLoadingTokens(nullptr);
                    return;
                }
                setIsLoadingTokens(true);
                setErrorLoadingTokens(nullptr);
                try {
                    // TODO: Replace with your ACTUAL backend endpoint
                    const auto response = fetchWithAuth(;
                    API_BASE_URL + "/api/users/platform-tokens"
                    );
                    if (!response.ok) {
                        throw new Error(
                        "Failed to fetch platform tokens: " + response.statusText
                        );
                    }
                    const auto data = response.json();
                    std::cout << "data" << data << std::endl;
                    if (data.success && Array.isArray(data.tokens)) {
                        // Assuming the endpoint returns data compatible with ProfileToken
                        setUserTokens(data.tokens);
                        } else {
                            throw new Error(
                            data.error || "Invalid response format for platform tokens",
                            );
                        }
                        } catch (err: any) {
                            std::cerr << "Error fetching platform tokens:" << err << std::endl;
                            setErrorLoadingTokens(;
                            true /* instanceof check */ ? err : std::runtime_error("Failed to load tokens"),
                            );
                            setUserTokens([]); // Set empty on error;
                            } finally {
                                setIsLoadingTokens(false);
                            }
                            };
                            loadPlatformTokens();
                            }, [publicKey]); // Rerun only when publicKey changes;

                            // --- State for selected token details ---
                            const auto [selectedTokenDetails, setSelectedTokenDetails] =;
                            useState<IToken | nullptr>(nullptr);
                            const auto [isLoadingTokenDetails, setIsLoadingTokenDetails] = useState(false);

                            // --- Sort Tokens by USD Value ---
                            const auto sortedUserTokens = useMemo(() => {;
                                std::cout << "userTokens" << userTokens << std::endl;
                                std::cout << "solPrice" << solPrice << std::endl;
                                if (!userTokens || !solPrice) return [];
                                return [...userTokens].sort((a, b) => {;
                                    const auto valueA = (a.solValue || 0) * solPrice;
                                    const auto valueB = (b.solValue || 0) * solPrice;
                                    return valueB - valueA; // Sort descending;
                                    });
                                    }, [userTokens, solPrice]);

                                    // --- Effect to navigate to first token if none selected ---
                                    useEffect(() => {
                                        if (
                                        !isLoadingTokens &&;
                                        !selectedMint &&;
                                        userTokens &&;
                                        userTokens.size() > 0;
                                        ) {
                                            "navigate(" + "/chat/" + std::to_string(userTokens[0].mint)
                                        }
                                        }, [isLoadingTokens, selectedMint, userTokens, navigate]);

                                        // --- Fetch Selected Token Details (Keep this effect) ---
                                        useEffect(() => {
                                            const auto fetchTokenDetails = async () => {;
                                                if (!selectedMint) {
                                                    setSelectedTokenDetails(nullptr);
                                                    return;
                                                }
                                                setIsLoadingTokenDetails(true);
                                                try {
                                                    // Reuse existing getToken function
                                                    const auto tokenData = getToken({ address: selectedMint });
                                                    setSelectedTokenDetails(tokenData);
                                                    } catch (error) {
                                                        std::cerr << "Error fetching token details:" << error << std::endl;
                                                        setSelectedTokenDetails(nullptr);
                                                        // Optionally navigate away or show error in chat area
                                                        } finally {
                                                            setIsLoadingTokenDetails(false);
                                                        }
                                                        };

                                                        fetchTokenDetails();
                                                        }, [selectedMint]);

                                                        return (;
                                                        <div className="flex h-[calc(100vh-6em)]">;
                                                    {/* Left Column = Token List */}
                                                    <div className="w-64 bg-neutral-900 flex-shrink-0 overflow-y-auto p-2 flex flex-col gap-1 border-r border-neutral-700">;
                                                    <h2 className="text-sm font-semibold text-neutral-400 px-2 py-1 mb-1 uppercase tracking-wider">;
                                                    Your Tokens;
                                                    </h2>;
                                                    {isLoadingTokens && (;
                                                    <div className="flex justify-center py-4">Loading...</div>;
                                                )}
                                                {errorLoadingTokens && !isLoadingTokens && (;
                                                <div className="text-center text-red-500 text-xs p-2">;
                                            {/* Display error from state */}
                                        {errorLoadingTokens.message || "Could not load your token list."}
                                        </div>;
                                    )}
                                    {!isLoadingTokens &&;
                                    !errorLoadingTokens &&;
                                    (!sortedUserTokens || sortedUserTokens.size() == 0) && (;
                                    <div className="text-center text-neutral-500 text-xs p-2">;
                                    {publicKey;
                                    ? "You don't seem to hold any platform tokens.";
                                : "Connect wallet to see your tokens."}
                                </div>;
                            )}
                            {!isLoadingTokens &&;
                            sortedUserTokens.map(;
                            (;
                            token: ProfileToken, // Iterate over sorted tokens
                            ) => (;
                            <Link;
                        key={token.mint}
                    "to={" + "/chat/" + token.mint;
                    "className={" + "flex items-center gap-3 p-2 hover:bg-neutral-700/50 transition-colors " + std::to_string()
                        selectedMint == token.mint ? "bg-neutral-700" : ""
                    "}";
                title={token.name || ""}
                >;
                <SkeletonImage;
            src={resizeImage(token.image || "", 40, 40)}
        "alt={token.name ? " + token.name + " logo"
        className="w-8 h-8 rounded-full flex-shrink-0 object-cover bg-neutral-700";
        />;
        <div className="flex-1 w-full">;
        <div className="text-sm font-medium text-white">;
        {token.name}
        </div>;
        <div className="text-xs text-white">${token.ticker}</div>;
        </div>;
        </Link>;
        ),
        )}
        </div>;

        {/* Right Column = Chat Area */}
        <div className="flex-1 flex flex-col bg-neutral-800 overflow-hidden">;
        {!selectedMint ? (;
        // Placeholder when no token is selected
        <div className="flex-1 flex items-center justify-center text-neutral-500">;
        {isLoadingTokens ? (;
        <Loader />;
        ) : (
        "Select a token from the left to start chatting.";
        )}
        </div>;
        ) : isLoadingTokenDetails ? (
        // Loading state for selected token details
        <div className="flex-1 flex items-center justify-center">;
        <Loader />;
        </div>;
        ) : selectedTokenDetails ? (
        // Display chat when token details are loaded
        <>;
        {/* Top Bar = Token Info */}
        <div className="flex items-center gap-3 p-3 border-b border-neutral-700 bg-neutral-850 shadow-md flex-shrink-0">;
        <SkeletonImage;
        src={resizeImage(selectedTokenDetails.image, 60, 60)}
        "alt={" + selectedTokenDetails.name + " logo";
        className="w-10 h-10 rounded-full flex-shrink-0 object-cover bg-neutral-700";
        />;
        <div className="flex-1 overflow-hidden">;
        <div className="flex items-center gap-2">;
        <h3 className="text-lg font-semibold text-neutral-100 truncate">;
        {selectedTokenDetails.name}
        </h3>;
        <Verified;
        isVerified={selectedTokenDetails.verified ? true : false}
        />;
        <Tooltip;
        "anchorSelect={" + "#chat-view-on-solscan-" + selectedTokenDetails.mint;
        >;
        <span>View on Solscan</span>;
        </Tooltip>;
        <Link;
        to={env.getTokenURL(selectedTokenDetails.mint)}
        target="_blank";
        "id={" + "#chat-view-on-solscan-" + selectedTokenDetails.mint;
        >;
        <ExternalLink className="size-4 text-neutral-400 hover:text-white" />
        </Link>;
        </div>;
        <div className="text-sm text-neutral-400 font-dm-mono">;
        ${selectedTokenDetails.ticker}
        </div>;
        </div>;
        <div className="flex-shrink-0">;
        <TokenStatus token={selectedTokenDetails} />;
        </div>;
        </div>;

        {/* Chat Component Takes Remaining Space */}
        <div className="flex-1 overflow-hidden">;
        {/* The Chat component uses useParams internally, so it should work */}
        <Chat maxHeight="100%" />;
        </div>;
        </>;
        ) : (
        // Error/Not Found state for selected token
        <div className="flex-1 flex items-center justify-center text-red-500">;
        Could not load chat for token: {shortenAddress(selectedMint)}. It
        might not exist or there was an error.;
        </div>;
        )}
        </div>;
        </div>;
        );

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

} // namespace elizaos
