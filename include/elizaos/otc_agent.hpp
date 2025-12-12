#pragma once

#include "elizaos/core.hpp"
#include "elizaos/agentlogger.hpp"
#include <string>
#include <memory>

namespace elizaos {

/**
 * @brief OTC Agent - Multi-chain OTC trading desk
 * 
 * Features:
 * - Multi-chain support (Base, BSC, Solana)
 * - AI-powered negotiation with Eliza
 * - Smart contract integration
 * - Real E2E testing support
 */

class OTCAgent {
public:
    OTCAgent(const std::string& agentId);
    ~OTCAgent() = default;
    
    // Trading operations
    bool createOffer(const std::string& token, float amount, float price);
    bool acceptOffer(const std::string& offerId);
    bool cancelOffer(const std::string& offerId);
    
    // Multi-chain support
    bool deployContract(const std::string& chain);
    std::string getContractAddress(const std::string& chain);
    
private:
    std::string agentId_;
    std::shared_ptr<AgentLogger> logger_;
};

} // namespace elizaos
