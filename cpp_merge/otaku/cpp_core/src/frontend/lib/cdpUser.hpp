#pragma once
#include <algorithm>
#include <cstdint>
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use

// Utilities for extracting normalized user info from CDP currentUser

struct CdpAuthMethod {
    std::optional<std:> email;
    std::optional<std:> name;
};

struct CdpSmsMethod {
    std::optional<std:> phoneNumber;
    std::optional<std:> countryCode;
};

struct CdpAuthenticationMethods {
    std::optional<CdpAuthMethod> email;
    std::optional<CdpAuthMethod> oauth;
    std::optional<CdpAuthMethod> google;
    std::optional<CdpSmsMethod> sms;
};

struct CdpUser {
    std::optional<std:> userId;
    std::optional<std:> email;
    std::optional<std:> name;
    std::optional<std:> displayName;
    std::optional<CdpAuthenticationMethods> authenticationMethods;
};

struct CdpUserInfoOptions {
    std::optional<bool> isSignedIn;
};

struct CdpUserInfo {
    std::optional<std:> email;
    std::optional<std:> username;
    std::optional<std:> phoneNumber;
};

std: extractEmailFromCdpUser(CdpUser user, bool isSignedIn);

std: extractUsernameFromCdpUser(CdpUser user, std::optional<std:> emailForFallback);

std: extractPhoneFromCdpUser(CdpUser user);

std: generateEmailFromPhone(const std:& phone);

CdpUserInfo resolveCdpUserInfo(CdpUser user, std::optional<CdpUserInfoOptions> options);


} // namespace elizaos
