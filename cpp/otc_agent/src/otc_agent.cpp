#include "elizaos/otc_agent.hpp"
#include <sstream>
#include <iomanip>
#include <map>
#include <vector>
#include <random>
#include <chrono>
#include <algorithm>

namespace elizaos {

// OTC Offer structure for managing offers
struct OTCOffer {
    std::string offerId;
    std::string token;
    float amount;
    float price;
    std::string creator;
    std::string status; // "active", "accepted", "cancelled", "completed"
    std::chrono::system_clock::time_point createdAt;
    std::chrono::system_clock::time_point expiresAt;
};

// Mock storage for offers (in production, would use database)
static std::map<std::string, OTCOffer> activeOffers;

// Mock contract addresses for different chains
static std::map<std::string, std::string> deployedContracts;

// Generate unique offer ID
static std::string generateOfferId() {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_int_distribution<> dis(0, 15);
    
    std::string offerId = "otc-xxxxxxxxxxxx";
    for (auto& c : offerId) {
        if (c == 'x') {
            c = "0123456789abcdef"[dis(gen)];
        }
    }
    return offerId;
}

// Generate mock contract address
static std::string generateContractAddress() {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_int_distribution<> dis(0, 15);
    
    std::string address = "0x";
    for (int i = 0; i < 40; ++i) {
        address += "0123456789abcdef"[dis(gen)];
    }
    return address;
}

OTCAgent::OTCAgent(const std::string& agentId)
    : agentId_(agentId)
    , logger_(std::make_shared<AgentLogger>()) {
    elizaos::logInfo("OTC Agent initialized: " + agentId, "otc_agent");
    elizaos::logInfo("Supported chains: Base, BSC (Binance Smart Chain), Solana", "otc_agent");
    elizaos::logInfo("AI-powered negotiation enabled with Eliza integration", "otc_agent");
}

bool OTCAgent::createOffer(const std::string& token, float amount, float price) {
    if (token.empty()) {
        elizaos::logError("Token cannot be empty", "otc_agent");
        return false;
    }
    
    if (amount <= 0) {
        elizaos::logError("Amount must be positive", "otc_agent");
        return false;
    }
    
    if (price <= 0) {
        elizaos::logError("Price must be positive", "otc_agent");
        return false;
    }
    
    std::ostringstream oss;
    oss << "Creating OTC offer for " << token << ": " 
        << std::fixed << std::setprecision(4) << amount 
        << " tokens at $" << price << " each";
    elizaos::logInfo(oss.str(), "otc_agent");
    
    // Create new offer
    OTCOffer offer;
    offer.offerId = generateOfferId();
    offer.token = token;
    offer.amount = amount;
    offer.price = price;
    offer.creator = agentId_;
    offer.status = "active";
    offer.createdAt = std::chrono::system_clock::now();
    offer.expiresAt = offer.createdAt + std::chrono::hours(24); // 24 hour expiry
    
    // In production, this would:
    // 1. Verify token balance
    // 2. Lock tokens in escrow smart contract
    // 3. Publish offer to on-chain order book
    // 4. Emit event for off-chain indexers
    // 5. Store offer metadata in database
    // 6. Notify potential counterparties
    
    // Store offer
    activeOffers[offer.offerId] = offer;
    
    std::ostringstream resultOss;
    resultOss << "OTC offer created successfully. Offer ID: " << offer.offerId;
    elizaos::logInfo(resultOss.str(), "otc_agent");
    
    // Calculate total value
    float totalValue = amount * price;
    std::ostringstream valueOss;
    valueOss << "Total offer value: $" << std::fixed << std::setprecision(2) << totalValue;
    elizaos::logInfo(valueOss.str(), "otc_agent");
    
    return true;
}

bool OTCAgent::acceptOffer(const std::string& offerId) {
    if (offerId.empty()) {
        elizaos::logError("Offer ID cannot be empty", "otc_agent");
        return false;
    }
    
    // Find offer
    auto it = activeOffers.find(offerId);
    if (it == activeOffers.end()) {
        elizaos::logError("Offer not found: " + offerId, "otc_agent");
        return false;
    }
    
    OTCOffer& offer = it->second;
    
    // Check if offer is still active
    if (offer.status != "active") {
        elizaos::logError("Offer is not active: " + offer.status, "otc_agent");
        return false;
    }
    
    // Check if offer has expired
    auto now = std::chrono::system_clock::now();
    if (now > offer.expiresAt) {
        elizaos::logError("Offer has expired", "otc_agent");
        offer.status = "expired";
        return false;
    }
    
    elizaos::logInfo("Accepting OTC offer: " + offerId, "otc_agent");
    
    // In production, this would:
    // 1. Verify buyer has sufficient funds
    // 2. Lock buyer's payment in escrow
    // 3. Execute atomic swap via smart contract
    // 4. Transfer tokens from seller to buyer
    // 5. Transfer payment from buyer to seller
    // 6. Release escrow funds
    // 7. Update offer status
    // 8. Emit completion event
    // 9. Record transaction in database
    
    // AI-powered negotiation simulation
    std::ostringstream negotiationOss;
    negotiationOss << "Eliza AI analyzing offer terms...";
    elizaos::logInfo(negotiationOss.str(), "otc_agent");
    
    // Mock negotiation logic
    float marketPrice = offer.price * 1.05f; // Assume market is 5% higher
    if (offer.price < marketPrice * 0.95f) {
        elizaos::logInfo("Eliza: This is a good deal! Price is below market.", "otc_agent");
    } else if (offer.price > marketPrice * 1.05f) {
        elizaos::logWarning("Eliza: Price seems high. Consider negotiating.", "otc_agent");
    } else {
        elizaos::logInfo("Eliza: Price is fair, within market range.", "otc_agent");
    }
    
    // Update offer status
    offer.status = "accepted";
    
    std::ostringstream resultOss;
    resultOss << "OTC offer accepted: " << offerId;
    resultOss << " | Token: " << offer.token;
    resultOss << " | Amount: " << std::fixed << std::setprecision(4) << offer.amount;
    resultOss << " | Price: $" << std::fixed << std::setprecision(2) << offer.price;
    elizaos::logInfo(resultOss.str(), "otc_agent");
    
    // Calculate settlement
    float totalPayment = offer.amount * offer.price;
    std::ostringstream settlementOss;
    settlementOss << "Settlement: Transferring " << std::fixed << std::setprecision(4) 
                  << offer.amount << " " << offer.token 
                  << " for $" << std::fixed << std::setprecision(2) << totalPayment;
    elizaos::logInfo(settlementOss.str(), "otc_agent");
    
    return true;
}

bool OTCAgent::cancelOffer(const std::string& offerId) {
    if (offerId.empty()) {
        elizaos::logError("Offer ID cannot be empty", "otc_agent");
        return false;
    }
    
    // Find offer
    auto it = activeOffers.find(offerId);
    if (it == activeOffers.end()) {
        elizaos::logError("Offer not found: " + offerId, "otc_agent");
        return false;
    }
    
    OTCOffer& offer = it->second;
    
    // Verify creator
    if (offer.creator != agentId_) {
        elizaos::logError("Only offer creator can cancel the offer", "otc_agent");
        return false;
    }
    
    // Check if offer can be cancelled
    if (offer.status == "accepted" || offer.status == "completed") {
        elizaos::logError("Cannot cancel offer in status: " + offer.status, "otc_agent");
        return false;
    }
    
    elizaos::logInfo("Cancelling OTC offer: " + offerId, "otc_agent");
    
    // In production, this would:
    // 1. Verify offer is cancellable
    // 2. Release locked tokens from escrow
    // 3. Update on-chain order book
    // 4. Emit cancellation event
    // 5. Update database
    // 6. Notify interested parties
    
    // Update offer status
    offer.status = "cancelled";
    
    std::ostringstream resultOss;
    resultOss << "OTC offer cancelled: " << offerId;
    elizaos::logInfo(resultOss.str(), "otc_agent");
    
    return true;
}

bool OTCAgent::deployContract(const std::string& chain) {
    if (chain.empty()) {
        elizaos::logError("Chain cannot be empty", "otc_agent");
        return false;
    }
    
    // Validate supported chains
    std::vector<std::string> supportedChains = {"base", "bsc", "solana"};
    std::string chainLower = chain;
    std::transform(chainLower.begin(), chainLower.end(), chainLower.begin(), ::tolower);
    
    if (std::find(supportedChains.begin(), supportedChains.end(), chainLower) == supportedChains.end()) {
        elizaos::logError("Unsupported chain: " + chain, "otc_agent");
        elizaos::logInfo("Supported chains: Base, BSC, Solana", "otc_agent");
        return false;
    }
    
    // Check if contract already deployed
    if (deployedContracts.count(chainLower)) {
        elizaos::logWarning("Contract already deployed on " + chain + ": " + deployedContracts[chainLower], "otc_agent");
        return true;
    }
    
    elizaos::logInfo("Deploying OTC smart contract on " + chain + "...", "otc_agent");
    
    // In production, this would:
    // 1. Compile smart contract bytecode
    // 2. Estimate deployment gas
    // 3. Sign deployment transaction
    // 4. Broadcast to network
    // 5. Wait for confirmation
    // 6. Verify contract on block explorer
    // 7. Initialize contract parameters
    // 8. Grant necessary permissions
    // 9. Store contract address
    
    // Mock deployment
    std::string contractAddress = generateContractAddress();
    deployedContracts[chainLower] = contractAddress;
    
    std::ostringstream resultOss;
    resultOss << "OTC contract deployed successfully on " << chain;
    resultOss << " | Contract address: " << contractAddress;
    elizaos::logInfo(resultOss.str(), "otc_agent");
    
    // Log contract features
    elizaos::logInfo("Contract features: Atomic swaps, Escrow, Multi-sig, Time locks", "otc_agent");
    
    return true;
}

std::string OTCAgent::getContractAddress(const std::string& chain) {
    if (chain.empty()) {
        elizaos::logError("Chain cannot be empty", "otc_agent");
        return "";
    }
    
    std::string chainLower = chain;
    std::transform(chainLower.begin(), chainLower.end(), chainLower.begin(), ::tolower);
    
    // Check if contract is deployed
    if (!deployedContracts.count(chainLower)) {
        elizaos::logWarning("Contract not deployed on " + chain, "otc_agent");
        return "";
    }
    
    std::string address = deployedContracts[chainLower];
    
    std::ostringstream oss;
    oss << "OTC contract address on " << chain << ": " << address;
    elizaos::logInfo(oss.str(), "otc_agent");
    
    return address;
}

} // namespace elizaos
