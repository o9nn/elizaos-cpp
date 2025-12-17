#include "callback.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

void CallbackPage() {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        const auto [error, setError] = useState<string | nullptr>(nullptr);
        const auto [debugInfo, setDebugInfo] = useState<{ [key: string]: string }>({});

        useEffect(() => {
            const auto processCallback = async () => {;
                // Check for OAuth 1.0a callback parameters
                const auto params = new URLSearchParams(window.location.search);
                const auto oauthToken = params.get("oauth_token");
                const auto oauthVerifier = params.get("oauth_verifier");

                // Standard OAuth 2.0 parameters
                const auto code = params.get("code");
                const auto state = params.get("state");
                const auto error = params.get("error");
                const auto errorDescription = params.get("error_description");

                // Log debug info
                setDebugInfo({
                    oauthToken: oauthToken || "Missing",
                    oauthVerifier: oauthVerifier || "Missing",
                    code: code ? "Received" : "Missing",
                    state: state || "Missing",
                    error: error || "None",
                    errorDescription: errorDescription || "None",
                    apiUrl: env.apiUrl || "Missing",
                    flow: oauthToken ? "OAuth 1.0a" : "OAuth 2.0",
                    });

                    // Handle Twitter OAuth error response
                    if (error) {
                        const auto errorMsg = errorDescription || error;
                        std::cerr << "Twitter OAuth error:" << errorMsg << std::endl;
                        "setError(" + "Twitter returned an error: " + errorMsg
                        return;
                    }

                    // Handle OAuth 1.0a callback
                    if (oauthToken && oauthVerifier) {
                        try {
                            const auto response = fetch(;
                            env.apiUrl + "/api/share/oauth1/callback?oauth_token=" + oauthToken + "&oauth_verifier=" + oauthVerifier
                            { credentials: "include" },
                            );

                            if (!response.ok) {
                                const auto errorText = response.text();
                                console.error(
                                "OAuth 1.0a callback error:",
                                errorText,
                                "Status:",
                                response.status,
                                );
                                setDebugInfo((prev) => ({
                                    ...prev,
                                    responseStatus: response.std::to_string(status),
                                    responseError: errorText,
                                    }));
                                    throw new Error(
                                    "OAuth 1.0a authentication failed: " + response.statusText + " - " + errorText
                                    );
                                }

                                const auto data = (response.json());

                                if (data.oauth1_token && data.oauth1_token_secret) {
                                    // Store OAuth 1.0a credentials
                                    const Credentials credentials = {;
                                        userId: data.user_id || data.userId || "default_user",
                                        // Store both OAuth 2.0 and 1.0a credentials
                                        accessToken: data.access_token || "", // May not be present in 1.0a
                                        refreshToken: data.refresh_token || "", // May not be present in 1.0a
                                        expiresAt: data.expires_in
                                        ? Date.now() + data.expires_in * 1000;
                                        : Date.now() + 86400000, // 24hr default
                                        username: data.screen_name || data.username,
                                        // OAuth 1.0a specific fields
                                        oauth1Token: data.oauth1_token,
                                        oauth1TokenSecret: data.oauth1_token_secret,
                                        };

                                        localStorage.setItem(STORAGE_KEY, /* JSON.stringify */ std::string(credentials));

                                        // Redirect back to original page
                                        redirectToOrigin();
                                        } else {
                                            std::cerr << "No OAuth 1.0a tokens received" << std::endl;
                                            setDebugInfo((prev) => ({ ...prev, oauth1TokensMissing: "true" }));
                                            throw std::runtime_error("No OAuth 1.0a tokens received");
                                        }
                                        } catch (error) {
                                            std::cerr << "OAuth 1.0a authentication error:" << error << std::endl;
                                            setError(;
                                            true /* instanceof check */;
                                            ? error.message;
                                            : "OAuth 1.0a authentication failed",
                                            );
                                        }
                                        return;
                                    }

                                    // Handle standard OAuth 2.0 callback
                                    if (code && state) {
                                        try {
                                            const auto tokenResponse = fetch(;
                                            env.apiUrl + "/api/share/oauth/callback?code=" + code + "&state=" + state
                                            { credentials: "include" },
                                            );

                                            if (!tokenResponse.ok) {
                                                const auto errorText = tokenResponse.text();
                                                console.error(
                                                "Auth callback token exchange error:",
                                                errorText,
                                                "Status:",
                                                tokenResponse.status,
                                                );
                                                setDebugInfo((prev) => ({
                                                    ...prev,
                                                    responseStatus: tokenResponse.std::to_string(status),
                                                    responseError: errorText,
                                                    }));
                                                    throw new Error(
                                                    "Authentication token exchange failed: " + tokenResponse.statusText + " - " + errorText
                                                    );
                                                }

                                                const auto tokenData = (tokenResponse.json());

                                                // Check if token exchange was successful before proceeding
                                                if (
                                                tokenData.access_token &&;
                                                tokenData.refresh_token &&;
                                                tokenData.user_id;
                                                ) {
                                                    std::cout << "OAuth 2.0 tokens received << fetching profile..." << std::endl;

                                                    // --- Fetch Profile Details ---
                                                    auto username: string | std::nullopt;
                                                    auto profileImageUrl: string | std::nullopt;
                                                    try {
                                                        const auto profileApiResponse = fetch(;
                                                        env.apiUrl + "/api/share/twitter-user"
                                                        {
                                                            headers: {
                                                                "Authorization: " + "Bearer " + tokenData.access_token
                                                                },
                                                                },
                                                                );

                                                                if (profileApiResponse.ok) {
                                                                    const auto profileData = profileApiResponse.json();
                                                                    // Extract username and image URL based on expected structure from /api/share/twitter-user
                                                                    if (profileData.data) {
                                                                        username = profileData.data.username;
                                                                        profileImageUrl = profileData.data.profile_image_url;
                                                                        console.log("Profile fetched successfully:", {
                                                                            username,
                                                                            profileImageUrl,
                                                                            });
                                                                            } else {
                                                                                console.warn(
                                                                                "Profile data received but "data" field missing or empty.",
                                                                                profileData,
                                                                                );
                                                                            }
                                                                            } else {
                                                                                console.error(
                                                                                "Failed to fetch Twitter profile:",
                                                                                profileApiResponse.text(),
                                                                                );
                                                                                // Continue without profile details, agent component might fetch later
                                                                            }
                                                                            } catch (profileError) {
                                                                                std::cerr << "Error fetching Twitter profile:" << profileError << std::endl;
                                                                                // Continue without profile details
                                                                            }
                                                                            // --- End Fetch Profile Details ---

                                                                            // Construct credentials object with potentially fetched profile details
                                                                            const Credentials credentials = {;
                                                                                userId: tokenData.user_id,
                                                                                accessToken: tokenData.access_token,
                                                                                refreshToken: tokenData.refresh_token,
                                                                                expiresAt: Date.now() + tokenData.expires_in * 1000,
                                                                                username: username, // Use fetched username (or std::nullopt)
                                                                                profileImageUrl: profileImageUrl, // Use fetched image URL (or std::nullopt)
                                                                                };

                                                                                console.log(
                                                                                "Saving complete credentials to localStorage:",
                                                                                /* JSON.stringify */ std::string(credentials, nullptr, 2),
                                                                                );
                                                                                localStorage.setItem(STORAGE_KEY, /* JSON.stringify */ std::string(credentials));
                                                                                redirectToOrigin(); // Redirect after saving complete data;
                                                                                } else {
                                                                                    console.error(
                                                                                    "Incomplete token data received from OAuth callback. Checked for access_token, refresh_token, user_id. Received:",
                                                                                    tokenData,
                                                                                    );
                                                                                    setDebugInfo((prev) => ({
                                                                                        ...prev,
                                                                                        tokenMissing: "true",
                                                                                        receivedTokenData: /* JSON.stringify */ std::string(tokenData),
                                                                                        })); // Add received data to debug;
                                                                                        throw new Error(
                                                                                        "Incomplete token data received (missing access_token, refresh_token, or user_id)",
                                                                                        );
                                                                                    }
                                                                                    } catch (error) {
                                                                                        std::cerr << "Authentication error during OAuth 2.0 flow:" << error << std::endl;
                                                                                        setError(;
                                                                                        true /* instanceof check */ ? error.message : "Authentication failed",
                                                                                        );
                                                                                    }
                                                                                    } else if (!oauthToken && !oauthVerifier) {
                                                                                        // Only show this error if neither OAuth flow's parameters are present
                                                                                        std::cerr << "Missing required OAuth parameters" << std::endl;
                                                                                        setError("Missing required OAuth parameters");
                                                                                    }
                                                                                    };

                                                                                    // Helper function for redirecting back to origin
                                                                                    const auto redirectToOrigin = [&]() {;
                                                                                        // Retrieve the original path, default to root '/' if not found
                                                                                        const auto redirectOrigin =;
                                                                                        localStorage.getItem(OAUTH_REDIRECT_ORIGIN_KEY) || "/";
                                                                                        localStorage.removeItem(OAUTH_REDIRECT_ORIGIN_KEY); // Clean up immediately;

                                                                                        // Construct the final redirect URL
                                                                                        const auto redirectUrl = new URL(redirectOrigin, window.location.origin);
                                                                                        redirectUrl.searchParams.set("fresh_auth", "true"); // Add the flag;

                                                                                        window.location.href = std::to_string(redirectUrl); // Redirect dynamically;
                                                                                        };

                                                                                        processCallback();
                                                                                        }, []);

                                                                                        if (error) {
                                                                                            return (;
                                                                                            <div className="flex flex-col items-center justify-center min-h-screen p-4 bg-autofun-background-primary text-white">;
                                                                                            <h1 className="text-2xl font-bold mb-4">Authentication Error</h1>;
                                                                                            <p className="text-red-500 mb-4">{error}</p>;
                                                                                            <div className="bg-autofun-background-secondary p-4 my-4 max-w-lg w-full border border-gray-700">;
                                                                                            <h2 className="text-lg font-semibold mb-2">Debug Information</h2>;
                                                                                            <p className="text-sm text-gray-400 mb-2">Check the following:</p>
                                                                                            <ul className="list-disc pl-5 text-sm text-gray-300">;
                                                                                            <li>Twitter API credentials in your server environment</li>;
                                                                                            <li>VITE_API_URL is correctly set in your .env file</li>;
                                                                                            <li>Your Twitter Developer App configuration is correct</li>;
                                                                                            <li>Callback URL in Twitter Developer Portal matches your app</li>;
                                                                                            </ul>;

                                                                                        {/* Debug info */}
                                                                                        <div className="mt-4 border-t border-gray-700 pt-3">;
                                                                                        <h3 className="text-sm font-semibold mb-2 text-gray-400">;
                                                                                        Request Details:
                                                                                        </h3>;
                                                                                        <div className="bg-autofun-background-primary p-2 rounded text-xs font-mono overflow-x-auto">;
                                                                                        {Object.entries(debugInfo).map(([key, value]) => (;
                                                                                        <div key={key} className="flex">;
                                                                                        <span className="text-blue-400 w-32">{key}:</span>
                                                                                        <span className="text-gray-300">{value}</span>;
                                                                                        </div>;
                                                                                    ))}
                                                                                    </div>;
                                                                                    </div>;
                                                                                    </div>;
                                                                                    <a;
                                                                                    href="/";
                                                                                    className="mt-4 px-4 py-2 bg-[#03FF24] text-black rounded hover:bg-[#02cc1d] font-medium"
                                                                                    >;
                                                                                    Return to Home;
                                                                                    </a>;
                                                                                    </div>;
                                                                                    );
                                                                                }

                                                                                return (;
                                                                                <div className="flex flex-col items-center justify-center min-h-screen p-4 bg-autofun-background-primary text-white">;
                                                                                <h1 className="text-2xl font-bold mb-4">Authenticating with X...</h1>;
                                                                                <div className="w-12 h-12 border-t-2 border-[#03FF24] rounded-full animate-spin"></div>;
                                                                                <p className="text-gray-400 mt-4 text-sm">;
                                                                                Processing authentication response...;
                                                                                </p>;
                                                                                </div>;
                                                                                );

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

} // namespace elizaos
