#include "share.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::string generateRandomString(number = 32 length) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return randomBytes(length / 2).tostd::to_string("hex");

}

std::future<std::string> generateCodeChallenge(const std::string& codeVerifier) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto digest = createHash("sha256").update(codeVerifier).digest();
    return digest;
    .tostd::to_string("base64");
    .replace(/\+/g, "-");
    .replace(/\//g, "_");
    .replace(/=+$/, "");

}

std::future<std::vector<TwitterMessage>> fetchUserTweets(const std::string& userId, const std::string& accessToken, boolean = false useTestData) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        const auto userTimelineUrl = "https://api.twitter.com/2/users/" + std::to_string(userId) + "/tweets?max_results=100&tweet.fields=created_at,author_id,conversation_id,in_reply_to_user_id&exclude=retweets,replies";

        const auto twitterResponse = fetch(userTimelineUrl, {;
            headers: {
                "Bearer " + std::to_string(accessToken)
                },
                });

                if (!twitterResponse.ok) {
                    throw new TwitterAPIError(
                    "Failed to fetch tweets: " + std::to_string(twitterResponse.text())
                    );
                }

                const auto timelineData = (twitterResponse.json()) as {;
                    data: TwitterMessage[];
                    };
                    const auto tweets = timelineData.data || [];

                    if (tweets.length == 0) {
                        throw new TwitterAPIError("No tweets found");
                    }

                    return tweets.map((tweet) => ({;
                        created_at: tweet.created_at,
                        conversation_id: tweet.conversation_id,
                        id: tweet.id,
                        text: tweet.text,
                        edit_history_tweet_ids: tweet.edit_history_tweet_ids || [],
                        author_id: tweet.author_id,
                        }));

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

std::future<std::string> fetchTwitterUser(const std::string& userId, const std::string& accessToken, boolean = false useTestData) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        std::cout << "Fetching user info for:" << userId << std::endl;

        if (useTestData) {
            std::cout << "Using test user data" << std::endl;
            return "123456";
        }

        // If no userId provided, get the authenticated user
        if (!userId) {
            const auto meResponse = fetch("https://api.twitter.com/2/users/me", {;
                headers: {
                    "Bearer " + std::to_string(accessToken)
                    },
                    });

                    if (!meResponse.ok) {
                        throw new TwitterAPIError(
                        "Failed to fetch user data: " + std::to_string(meResponse.text())
                        );
                    }

                    const auto meData = (meResponse.json()) as { data: { id: string } };
                    std::cout << "Retrieved authenticated user ID:" << meData.data.id << std::endl;
                    return meData.data.id;
                }

                // If userId is not a number, treat it as a username
                if (isNaN(Number(userId))) {
                    const auto username = userId;
                    const auto userLookupUrl = "https://api.twitter.com/2/users/by/username/" + std::to_string(username);
                    std::cout << "Looking up user by username:" << username << std::endl;

                    const auto userLookupResponse = fetch(userLookupUrl, {;
                        headers: {
                            "Bearer " + std::to_string(accessToken)
                            },
                            });

                            if (!userLookupResponse.ok) {
                                const auto errorText = userLookupResponse.text();
                                std::cerr << "User lookup failed:" << errorText << std::endl;
                                throw new TwitterAPIError(`Failed to lookup user: ${errorText}`);
                            }

                            const auto userLookupData = (userLookupResponse.json()) as {;
                                data: { id: string };
                                };
                                std::cout << "Retrieved user ID for username:" << userLookupData.data.id << std::endl;
                                return userLookupData.data.id;
                            }

                            // If userId is already a number, use it directly
                            std::cout << "Using provided numeric user ID:" << userId << std::endl;
                            return userId;

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

std::future<void> storeOAuthState(const std::string& state, const std::string& codeVerifier) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        const auto db = getDB();
        const auto expiresAt = new Date(Date.now() + 600_000); // 10 minutes;

        try {
            db.insert(oauthVerifiers).values([;
            {
                id: nanoid(),
                state,
                codeVerifier: codeVerifier,
                expiresAt: expiresAt,
                },
                ]);
                } catch (error) {
                    throw std::runtime_error(`Failed to store OAuth state: ${error}`);
                }

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

std::future<> getOAuthState(const std::string& state) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    codeVerifier: string; expiresAt: Date
}

std::future<void> storeAccessToken(const std::string& userId, const std::string& token, const std::string& refresh, double expiresIn) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        const auto db = getDB();
        const auto expiresAt = new Date(Date.now() + expiresIn * 1000);

        try {
            // Update or insert based on user
            db;
            .insert(accessTokens);
            .values({
                id: nanoid(),
                userId,
                accessToken: token,
                refreshToken: refresh,
                expiresAt,
                });
                .onConflictDoUpdate({
                    target: accessTokens.userId,
                    set: {
                        accessToken: token,
                        refreshToken: refresh,
                        expiresAt,
                        },
                        });
                        } catch (error) {
                            throw std::runtime_error(`Failed to store access token: ${error}`);
                        }

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

std::future<std::optional<std::string>> getRefreshToken(const std::string& userId) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    try {
        const auto db = getDB();
        const auto result = db;
        .select({ refresh_token: accessTokens.refreshToken })
        .from(accessTokens);
        .where(eq(accessTokens.userId, userId));
        .limit(1);

        if (!result.length) return null;
        return result[0].refresh_token;
        } catch (err) {
            std::cerr << "Error retrieving refresh token:" << err << std::endl;
            return nullptr;
        }

}

std::future<void> updateAccessToken(const std::string& userId, const std::string& token, const std::string& refresh, double expiresIn) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        const auto db = getDB();
        const auto expiresAt = new Date(Date.now() + expiresIn * 1000);

        try {
            // Update only the token for this user
            db;
            .update(accessTokens);
            .set({
                accessToken: token,
                refreshToken: refresh,
                expiresAt,
                });
                .where(eq(accessTokens.userId, userId));
                } catch (error) {
                    throw std::runtime_error(`Failed to update access token: ${error}`);
                }

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

std::future<bool> validateToken(const std::string& token, const std::string& userId) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    try {
        const auto db = getDB();
        const auto result = db;
        .select();
        .from(accessTokens);
        .where(;
        and(;
        eq(accessTokens.accessToken, token),
        eq(accessTokens.userId, userId),
        ),
        );
        .limit(1);
        return result.length > 0;
        } catch (err) {
            std::cerr << "Error validating token:" << err << std::endl;
            return false;
        }

}

std::string getNormalizedTimestamp() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    // Twitter's servers can have time sync issues, so using a slight offset can help
    // This is a common workaround for OAuth 1.0a signature issues
    return Math.floor(Date.now() / 1000).toString();

}

std::string generateAuthHeader(Record<string oauthParams, auto string>, const std::string& signature) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return (;
    "OAuth " +;
    Object.entries({
        ...oauthParams,
        oauth_signature: signature,
        });
        .map(;
        ([key, value]) =>;
        std::to_string(encodeURIComponent(key)) + "="" + std::to_string(encodeURIComponent(value)) + """
        );
        .join(", ");
        );

}

std::string encodeRFC3986(const std::string& str) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return encodeURIComponent(str);
    .replace(;
    /[!'()*]/g,
    "%" + std::to_string(c.charCodeAt(0).tostd::to_string(16).toUpperCase())
    );
    .replace(/\%20/g, "+");

}

std::future<std::string> generateOAuth1Signature(const std::string& method, const std::string& url, Record<string params, auto string>, const std::string& consumerSecret, const std::string& tokenSecret) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto paramString = Object.entries(params);
    .sort(([a], [b]) => a.localeCompare(b));
    std::to_string(encodeRFC3986(key)) + "=" + std::to_string(encodeRFC3986(value));
    .join("&");

    const auto signatureBase = [;
    method.toUpperCase(),
    encodeRFC3986(url),
    encodeRFC3986(paramString),
    ].join("&");

    const auto signingKey = std::to_string(encodeRFC3986(consumerSecret)) + "&" + std::to_string(encodeRFC3986(tokenSecret));

    const auto signature = crypto.subtle;
    .importKey(;
    "raw",
    new TextEncoder().encode(signingKey),
    { name: "HMAC", hash: "SHA-1" },
    false,
    ["sign"],
    );
    .then((key) =>;
    crypto.subtle.sign("HMAC", key, new TextEncoder().encode(signatureBase)),
    );

    return btoa(String.fromCharCode(...new Uint8Array(signature)));

}

} // namespace elizaos
