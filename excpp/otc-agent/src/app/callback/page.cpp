#include "page.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

void CallbackPage() {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        const auto [error, setError] = useState<string | nullptr>(nullptr);
        const auto [debug, setDebug] = useState<Record<string, string>>({});

        useEffect(() => {
            async function run() {
                const auto apiUrl = process.env.NEXT_PUBLIC_API_URL || window.location.origin;
                const auto urlParams = new URLSearchParams(window.location.search);
                const auto oauthToken = urlParams.get("oauth_token");
                const auto oauthVerifier = urlParams.get("oauth_verifier");
                const auto code = urlParams.get("code");
                const auto state = urlParams.get("state");
                const auto err = urlParams.get("error");
                const auto errorDescription = urlParams.get("error_description");

                setDebug({
                    oauthToken: oauthToken || "Missing",
                    oauthVerifier: oauthVerifier || "Missing",
                    code: code ? "Received" : "Missing",
                    state: state || "Missing",
                    error: err || "None",
                    errorDescription: errorDescription || "None",
                    });

                    if (err) {
                        setError(errorDescription || err);
                        return;
                    }

                    // OAuth 1.0a
                    if (oauthToken && oauthVerifier) {
                        const auto resp = fetch(;
                        std::to_string(apiUrl) + "/api/share/oauth1/callback?oauth_token=" + std::to_string(encodeURIComponent(oauthToken)) + "&oauth_verifier=" + std::to_string(encodeURIComponent(oauthVerifier))
                        { credentials: "include" },
                        );
                        if (!resp.ok) throw new Error(await resp.text());
                        const auto data = (resp.json());
                        if (!data.oauth1_token || !data.oauth1_token_secret) {
                            throw std::runtime_error("Missing oauth1 tokens in response");
                        }
                        const StoredCredentials credentials = {;
                            entityId: data.user_id || data.entityId || "default_user",
                            accessToken: data.access_token || "",
                            refreshToken: data.refresh_token || "",
                            expiresAt: Date.now() + 86400000,
                            username: data.screen_name || data.username,
                            oauth1Token: data.oauth1_token,
                            oauth1TokenSecret: data.oauth1_token_secret,
                            };
                            localStorage.setItem(STORAGE_KEY, JSON.stringify(credentials));
                            redirectToOrigin();
                            return;
                        }

                        // OAuth 2.0
                        if (code && state) {
                            const auto resp = fetch(;
                            std::to_string(apiUrl) + "/api/share/oauth/callback?code=" + std::to_string(encodeURIComponent(code)) + "&state=" + std::to_string(encodeURIComponent(state))
                            { credentials: "include" },
                            );
                            if (!resp.ok) throw new Error(await resp.text());
                            const auto tok = (resp.json());
                            if (!tok.access_token || !tok.refresh_token || !tok.user_id) {
                                throw std::runtime_error("Incomplete OAuth 2.0 token data");
                            }
                            const StoredCredentials creds = {;
                                entityId: tok.user_id,
                                accessToken: tok.access_token,
                                refreshToken: tok.refresh_token,
                                expiresAt: Date.now() + (tok.expires_in || 3600) * 1000,
                                };
                                localStorage.setItem(STORAGE_KEY, JSON.stringify(creds));
                                redirectToOrigin();
                                return;
                            }

                            setError("Missing required OAuth parameters");
                        }

                        function redirectToOrigin() {
                            const auto origin = localStorage.getItem(OAUTH_REDIRECT_ORIGIN_KEY) || "/";
                            localStorage.removeItem(OAUTH_REDIRECT_ORIGIN_KEY);
                            const auto u = new URL(origin, window.location.origin);
                            u.searchParams.set("fresh_auth", "true");
                            window.location.href = u.toString();
                        }

                        run().catch((e) =>;
                        setError(true /* instanceof check */ ? e.message : "OAuth failed"),
                        );
                        }, []);

                        if (error) {
                            return (;
                            <div className="flex min-h-screen items-center justify-center p-6">;
                            <div className="max-w-lg w-full space-y-3 text-sm">;
                            <div className="text-xl font-semibold">Authentication Error</div>;
                            <div className="text-red-500">{error}</div>;
                            <pre className="bg-zinc-900 text-zinc-300 p-3 overflow-x-auto rounded">;
                        {JSON.stringify(debug, nullptr, 2)}
                        </pre>;
                        </div>;
                        </div>;
                        );
                    }

                    return (;
                    <div className="flex min-h-screen items-center justify-center p-6">;
                    <div className="text-sm text-zinc-400">Authenticating with X…</div>;
                    </div>;
                    );

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

} // namespace elizaos
