#pragma once
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



struct WalletModalProviderProps {
    ReactNode children;
    std::optional<std::string> className;
    std::optional<std::string> container;
};

struct WalletModalProps {
    std::optional<std::string> className;
    std::optional<std::string> container;
};

        // Store selection in localStorage

        // Connect - use a direct approach for Phantom wallet

        // Try direct connection for Phantom wallet
            // Force disconnect first to ensure a clean connection

              // Wait a moment for connection to register

        // If direct connection failed or this isn't Phantom, try adapter approach
          // Select and connect via adapter

            // If we're using Phantom and direct connection was successful, ignore adapter errors

        // Wait for the public key to be available with timeout

        // Use direct Phantom publicKey if available, otherwise use adapter

          // Check for direct Phantom publicKey first

          // Check adapter publicKey

        // Convert window.solana.publicKey to string if needed

        /** Nonce generation */

        // For signing, prefer direct Phantom signMessage when available

            // Use direct Phantom signing
            // Handle different signature formats - Phantom may return the signature directly or in an object
              // Use a type assertion to handle signature property access
              using PhantomSignatureResponse = {

              // Check if it has a signature property

            // If adapter signing is available, try that as fallback
          // Use adapter signing

        // Encode the signature for sending to the server

        // Prepare the authentication payload

        // Use token-based authentication with Authorization header instead of cookies

        // Define the expected response type
        struct AuthResponse {
    std::string token;
    std::string message;
    std::string address;
};

        // Store the token using the new handler
          // Generate a fallback token for compatibility


} // namespace elizaos
