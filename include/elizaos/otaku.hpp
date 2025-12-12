#pragma once

#include "elizaos/core.hpp"
#include "elizaos/agentlogger.hpp"
#include <string>
#include <memory>

namespace elizaos {

/**
 * @brief Otaku AI Agent - DeFi-focused AI agent
 * 
 * Features:
 * - CDP wallet integration (Coinbase Developer Platform)
 * - Multi-chain DeFi operations (Ethereum, Base, Polygon, Arbitrum)
 * - Token swaps, transfers, bridging, NFT operations
 * - Real-time market data and analytics
 */

class OtakuAgent {
public:
    OtakuAgent(const std::string& agentId);
    ~OtakuAgent() = default;
    
    // Wallet operations
    bool connectWallet(const std::string& walletAddress);
    std::string getBalance(const std::string& token);
    
    // DeFi operations
    bool executeSwap(const std::string& fromToken, const std::string& toToken, float amount);
    bool executeBridge(const std::string& fromChain, const std::string& toChain, float amount);
    bool executeTransfer(const std::string& to, const std::string& token, float amount);
    
    // Market data
    float getTokenPrice(const std::string& token);
    std::vector<std::string> getTrendingTokens();
    
private:
    std::string agentId_;
    std::string walletAddress_;
    std::shared_ptr<AgentLogger> logger_;
};

} // namespace elizaos
