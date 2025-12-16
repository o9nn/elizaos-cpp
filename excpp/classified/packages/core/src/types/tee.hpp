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


// Represents an agent in the TeeAgent table, containing details about the agent.
/**
 * Represents an agent's registration details within a Trusted Execution Environment (TEE) context.
 * This is typically stored in a database table (e.g., `TeeAgent`) to manage agents operating in a TEE.
 * It allows for multiple registrations of the same `agentId` to support scenarios where an agent might restart,
 * generating a new keypair and attestation each time.
 */
struct TeeAgent {
    string; // Primary key id;
    std::string agentId;
    std::string agentName;
    double createdAt;
    std::string publicKey;
    std::string attestation;
};

/**
 * Defines the operational modes for a Trusted Execution Environment (TEE).
 * This enum is used to configure how TEE functionalities are engaged, allowing for
 * different setups for local development, Docker-based development, and production.
 */
enum TEEMode {
  /** TEE functionality is completely disabled. */
  OFF = 'OFF',
  /** For local development, potentially using a TEE simulator. */
  LOCAL = 'LOCAL', // For local development with simulator
  /** For Docker-based development environments, possibly with a TEE simulator. */
  DOCKER = 'DOCKER', // For docker development with simulator
  /** For production deployments, using actual TEE hardware without a simulator. */
  PRODUCTION = 'PRODUCTION', // For production without simulator
}

/**
 * Represents a quote obtained during remote attestation for a Trusted Execution Environment (TEE).
 * This quote is a piece of evidence provided by the TEE, cryptographically signed, which can be
 * verified by a relying party to ensure the TEE's integrity and authenticity.
 */
struct RemoteAttestationQuote {
    std::string quote;
    double timestamp;
};

/**
 * Data structure used in the attestation process for deriving a key within a Trusted Execution Environment (TEE).
 * This information helps establish a secure channel or verify the identity of the agent instance
 * requesting key derivation.
 */
struct DeriveKeyAttestationData {
    std::string agentId;
    std::string publicKey;
    std::optional<std::string> subject;
};

/**
 * Represents a message that has been attested by a Trusted Execution Environment (TEE).
 * This structure binds a message to an agent's identity and a timestamp, all within the
 * context of a remote attestation process, ensuring the message originated from a trusted TEE instance.
 */
struct RemoteAttestationMessage {
    std::string agentId;
    double timestamp;
    { message;
    std::string entityId;
    std::string roomId;
    std::string content;
};

/**
 * Enumerates different types or vendors of Trusted Execution Environments (TEEs).
 * This allows the system to adapt to specific TEE technologies, like Intel TDX on DSTACK.
 */
enum TeeType {
  /** Represents Intel Trusted Domain Extensions (TDX) running on DSTACK infrastructure. */
  TDX_DSTACK = 'tdx_dstack',
}

/**
 * Configuration for a TEE (Trusted Execution Environment) plugin.
 * This allows specifying the TEE vendor and any vendor-specific configurations.
 * It's used to initialize and configure TEE-related functionalities within the agent system.
 */
struct TeePluginConfig {
    std::optional<std::string> vendor;
    std::optional<Metadata> vendorConfig;
};


} // namespace elizaos
