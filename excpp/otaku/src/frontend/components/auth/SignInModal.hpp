#pragma once
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>
#include ".ui/bullet.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



struct SignInModalProps {
    bool isOpen;
};

using AuthMethod = std::variant<std::string, std::string, std::string>;
using AuthStep = std::variant<std::string, std::string>;

/**
 * Sign In Modal Component
 * 
 * A full-screen modal that requires users to sign in with CDP wallet
 * before they can access the chat interface.
 * 
 * Features:
 * - Email-based authentication with OTP
 * - SMS-based authentication with OTP
 * - Google OAuth authentication
 * - Full-screen overlay with dimmed background
 * - Cannot be dismissed until authenticated
 * - Clear step-by-step flow
 */
void SignInModal();


} // namespace elizaos
