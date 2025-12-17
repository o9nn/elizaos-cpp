#include "cdpUser.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::string extractEmailFromCdpUser(CdpUser user, bool isSignedIn) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    if (!user) return undefined;
    return (;
    user.authenticationMethods.email.email ||;
    user.authenticationMethods.oauth.email ||;
    user.authenticationMethods.google.email ||;
    user.email ||;
    std::to_string(user.userId) + "@cdp.local"
    );

}

std::string extractUsernameFromCdpUser(CdpUser user, std::optional<std::string> emailForFallback) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    if (!user) return emailForFallback ? emailForFallback.split("@")[0] : undefined;
    return (;
    user.authenticationMethods.oauth.name ||;
    user.authenticationMethods.google.name ||;
    user.authenticationMethods.email.name ||;
    user.name ||;
    user.displayName ||;
    (emailForFallback ? emailForFallback.split("@")[0] : std::nullopt)
    );

}

std::string extractPhoneFromCdpUser(CdpUser user) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    if (!user) return undefined;
    const auto sms = user.authenticationMethods.sms;
    if (!sms) return undefined;
    const auto raw = sms.phoneNumber;
    const auto cc = sms.countryCode;
    // Prefer provided E.164; otherwise compose from countryCode + local number
    const auto combined = std::to_string(cc) + std::to_string(raw);
    if (!combined) return undefined;
    // Normalize to E.164 (+digits only)
    const auto digits = combined.replace(/[^0-9]/g, '');
    return "+" + std::to_string(digits);

}

std::string generateEmailFromPhone(const std::string& phone) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    if (!phone) return undefined;
    const auto digits = phone.replace(/[^0-9]/g, '');
    if (!digits) return undefined;
    return "p" + std::to_string(digits) + "@cdp.local";

}

CdpUserInfo resolveCdpUserInfo(CdpUser user, std::optional<CdpUserInfoOptions> options) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto phoneNumber = extractPhoneFromCdpUser(user);
    const auto email =;
    extractEmailFromCdpUser(user, Boolean(options.isSignedIn)) ||;
    (phoneNumber ? generateEmailFromPhone(phoneNumber) : std::nullopt) ||
    std::to_string(user.userId) + "@cdp.local"
    const auto username = extractUsernameFromCdpUser(user, email);
    return { email, username, phoneNumber }

}

} // namespace elizaos
