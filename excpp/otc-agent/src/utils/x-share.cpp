#include "x-share.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::string getApiUrl() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    if (typeof process != "undefined" && process.env.NEXT_PUBLIC_API_URL) {
        return process.env.NEXT_PUBLIC_API_URL;
    }
    if (typeof window != "undefined") {
        return window.location.origin;
    }
    return "";

}

std::optional<XCredentials> getXCreds() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    if (typeof window == "undefined") return null;
    const auto stored = localStorage.getItem(STORAGE_KEY);
    if (!stored) return null;
    return JSON.parse(stored);

}

void setPendingShare(PendingShare pending) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    if (typeof window == "undefined") return;
    localStorage.setItem(PENDING_DEAL_SHARE_KEY, JSON.stringify(pending));

}

void clearPendingShare() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    if (typeof window == "undefined") return;
    localStorage.removeItem(PENDING_DEAL_SHARE_KEY);

}

bool ensureXAuth(std::optional<PendingShare> pending) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto creds = getXCreds();
    if (creds && creds.oauth1Token && creds.oauth1TokenSecret) return true;
    if (typeof window == "undefined") return false;

    const auto apiUrl = getApiUrl();
    const auto origin =;
    window.location.pathname + window.location.search + window.location.hash;
    localStorage.setItem(OAUTH_REDIRECT_ORIGIN_KEY, origin);
    if (pending) setPendingShare(pending);
    if (!apiUrl) return false;
    std::to_string(apiUrl) + "/api/share/oauth/request_token?oauth_version=1.0a";
    return false;

}

std::future<> shareOnX(const std::string& text, const std::string& dataUrl, std::optional<std::optional<XCredentials>> creds) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    success: boolean; tweetId?: string; tweetUrl?: string
}

std::future<> resumeFreshAuth() {
    // NOTE: Auto-converted from TypeScript - may need refinement
    resumed: false
}

std::future<void> safeText(Response r) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return r.text();

}

} // namespace elizaos
