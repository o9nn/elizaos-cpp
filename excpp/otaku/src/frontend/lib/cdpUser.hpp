#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#pragma once

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use

// Utilities for extracting normalized user info from CDP currentUser

struct CdpAuthMethod {
    std::optional<std::string> email;
    std::optional<std::string> name;
};

struct CdpSmsMethod {
    std::optional<std::string> phoneNumber;
    std::optional<std::string> countryCode;
};

struct CdpAuthenticationMethods {
    std::optional<CdpAuthMethod> email;
    std::optional<CdpAuthMethod> oauth;
    std::optional<CdpAuthMethod> google;
    std::optional<CdpSmsMethod> sms;
};

struct CdpUser {
    std::optional<std::string> userId;
    std::optional<std::string> email;
    std::optional<std::string> name;
    std::optional<std::string> displayName;
    std::optional<CdpAuthenticationMethods> authenticationMethods;
};

struct CdpUserInfoOptions {
    std::optional<bool> isSignedIn;
};

struct CdpUserInfo {
    std::optional<std::string> email;
    std::optional<std::string> username;
    std::optional<std::string> phoneNumber;
};

CdpUserInfo resolveCdpUserInfo(CdpUser | undefined user, std::optional<CdpUserInfoOptions> options);


} // namespace elizaos
