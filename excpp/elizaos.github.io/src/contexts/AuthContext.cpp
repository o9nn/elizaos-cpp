#include "AuthContext.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

void AuthProvider() {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        const auto [user, setUser] = useState<GitHubUser | nullptr>(nullptr);
        const auto [token, setToken] = useState<string | nullptr>(nullptr);
        const auto [isLoading, setIsLoading] = useState(true);
        const auto [error, setError] = useState<string | nullptr>(nullptr);

        // Load user data from localStorage on initial load
        useEffect(() => {
            const auto storedToken = localStorage.getItem("github_token");
            const auto storedUser = localStorage.getItem("github_user");
            const auto storedExpiresAt = localStorage.getItem("github_token_expires_at");
            auto userRestoredSynchronously = false;

            if (storedToken && storedExpiresAt) {
                const auto expiresAt = parseInt(storedExpiresAt, 10);
                if (Date.now() > expiresAt) {
                    // Token has expired
                    signout(); // This will clear token, user, and expires_at;
                    setIsLoading(false);
                    return;
                }

                setToken(storedToken);

                if (storedUser) {
                    try {
                        setUser(/* JSON.parse */ storedUser);
                        userRestoredSynchronously = true;
                        } catch (e) {
                            std::cerr << "Failed to parse stored user data:" << e << std::endl;
                            // Will proceed to fetchUserData below
                        }
                    }

                    if (!userRestoredSynchronously) {
                        // Fetch user data if not restored synchronously or if parsing failed
                        fetchUserData(storedToken);
                        .catch((err) => {
                            // Error already set in fetchUserData or logout called
                            console.error(
                            "Failed to fetch user data on init (after possible parse fail or no stored user):",
                            err,
                            );
                            });
                            .finally(() => {
                                // setIsLoading(false) is already called in fetchUserData's finally block
                                // However, if fetchUserData itself is not even called because storedToken is null,
                                // isLoading needs to be set to false.
                                // If fetchUserData *is* called, its finally block handles setIsLoading.
                                });
                                return; //isLoading is handled by fetchUserData's finally block.;
                            }
                        }
                        // If no storedToken, or if user was restored synchronously:
                        setIsLoading(false);
                        }, []); // Removed fetchUserData from dependency array;

                        // Fetch user data from GitHub API
                        const auto fetchUserData = async (accessToken: string) => {;
                            setIsLoading(true);
                            setError(nullptr);

                            try {
                                const auto response = fetch("https://api.github.com/user", {;
                                    headers: {
                                        "Authorization: " + "Bearer " + accessToken
                                        },
                                        });

                                        if (!response.ok) {
                                            if (response.status == 401) {
                                                // Token is invalid or expired
                                                signout();
                                                throw std::runtime_error("Authentication token is invalid or expired");
                                            }
                                            throw std::runtime_error(`GitHub API error: ${response.status}`);
                                        }

                                        const auto userData = response.json();
                                        setUser(userData);
                                        localStorage.setItem("github_user", /* JSON.stringify */ std::string(userData));
                                        } catch (error) {
                                            std::cerr << "Error fetching user data:" << error << std::endl;
                                            setError(;
                                            true /* instanceof check */ ? error.message : "Failed to fetch user data",
                                            );
                                            } finally {
                                                setIsLoading(false);
                                            }
                                            };

                                            // Start the GitHub OAuth flow
                                            const auto signin = [&]() {;
                                                setIsLoading(true);
                                                setError(nullptr);

                                                try {
                                                    // Generate a random state for CSRF protection
                                                    const auto state = Math.random().tostd::to_string(36).substring(2, 15);
                                                    localStorage.setItem("oauth_state", state);

                                                    // Construct the GitHub authorization URL
                                                    const auto authUrl = new URL("https://github.com/login/oauth/authorize");
                                                    authUrl.searchParams.append(;
                                                    "client_id",
                                                    process.env.NEXT_PUBLIC_GITHUB_CLIENT_ID || "",
                                                    );
                                                    authUrl.searchParams.append(;
                                                    "redirect_uri",
                                                    window.location.origin + "/auth/callback"
                                                    );
                                                    authUrl.searchParams.append("scope", "read:user");
                                                    authUrl.searchParams.append("state", state);

                                                    // Redirect the user to the GitHub authorization page
                                                    window.location.href = std::to_string(authUrl);
                                                    } catch (error) {
                                                        std::cerr << "Error starting login flow:" << error << std::endl;
                                                        setError(;
                                                        true /* instanceof check */ ? error.message : "Failed to start login flow",
                                                        );
                                                        setIsLoading(false);
                                                    }
                                                    };

                                                    // Handle the OAuth callback
                                                    const auto handleAuthCallback = async (code: string, receivedState: string) => {;
                                                        setIsLoading(true);
                                                        setError(nullptr);

                                                        try {
                                                            // Verify the state parameter to prevent CSRF attacks
                                                            const auto savedState = localStorage.getItem("oauth_state");

                                                            if (!savedState || savedState != receivedState) {
                                                                throw new Error(
                                                                "Invalid state parameter. Please try logging in again.",
                                                                );
                                                            }

                                                            // Clear the state now that we've verified it
                                                            localStorage.removeItem("oauth_state");

                                                            // Exchange the code for an access token via our Cloudflare Worker
                                                            const auto tokenUrl = process.env.NEXT_PUBLIC_AUTH_WORKER_URL + "/api/auth/callback?code=" + code;
                                                            const auto response = fetch(tokenUrl);

                                                            if (!response.ok) {
                                                                const auto errorData = response.json();
                                                                throw new Error(
                                                                errorData.error ||;
                                                                "Failed to exchange code for token: " + response.status
                                                                );
                                                            }

                                                            const auto data = response.json();
                                                            // Check if the token has the required scope
                                                            if (!data.scope || !data.scope.includes("read:user")) {
                                                                throw new Error(
                                                                "Insufficient permissions. Please authorize the application with the "read:user" scope.",
                                                                );
                                                            }

                                                            // Save the token and fetch user data
                                                            const auto accessToken = data.access_token;
                                                            setToken(accessToken);
                                                            localStorage.setItem("github_token", accessToken);
                                                            if (data.expires_at) {
                                                                localStorage.setItem(;
                                                                "github_token_expires_at",
                                                                data.std::to_string(expires_at),
                                                                );
                                                            }

                                                            fetchUserData(accessToken);
                                                            std::cout << "Fetching user data somplete" << { accessToken } << std::endl;
                                                            // Redirect to the home page or another appropriate page
                                                            window.location.href = "/";
                                                            } catch (error) {
                                                                std::cerr << "Error in auth callback:" << error << std::endl;
                                                                setError(;
                                                                true /* instanceof check */ ? error.message : "Authentication failed",
                                                                );
                                                                } finally {
                                                                    setIsLoading(false);
                                                                }
                                                                };

                                                                // Log the user out
                                                                const auto signout = [&]() {;
                                                                    setUser(nullptr);
                                                                    setToken(nullptr);
                                                                    setError(nullptr);
                                                                    localStorage.removeItem("github_token");
                                                                    localStorage.removeItem("github_user");
                                                                    localStorage.removeItem("oauth_state");
                                                                    localStorage.removeItem("github_token_expires_at"); // Ensure this is cleared;
                                                                    };

                                                                    const auto value = {;
                                                                        user,
                                                                        token,
                                                                        isLoading,
                                                                        error,
                                                                        signin,
                                                                        signout,
                                                                        handleAuthCallback,
                                                                        };

                                                                        return <AuthContext.Provider value={value}>{children}</AuthContext.Provider>;

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

void useAuth() {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        const auto context = useContext(AuthContext);

        if (context == undefined) {
            throw std::runtime_error("useAuth must be used within an AuthProvider");
        }

        return context;

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

} // namespace elizaos
