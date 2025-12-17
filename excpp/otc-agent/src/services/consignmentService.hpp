#pragma once
#include <algorithm>
#include <any>
#include <cstdint>
#include <functional>
#include <future>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include "database.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



struct ConsignmentParams {
    std::string tokenId;
    std::string consignerAddress;
    std::string amount;
    bool isNegotiable;
    std::optional<double> fixedDiscountBps;
    std::optional<double> fixedLockupDays;
    double minDiscountBps;
    double maxDiscountBps;
    double minLockupDays;
    double maxLockupDays;
    std::string minDealAmount;
    std::string maxDealAmount;
    bool isFractionalized;
    bool isPrivate;
    std::optional<std::vector<std::string>> allowedBuyers;
    double maxPriceVolatilityBps;
    double maxTimeToExecuteSeconds;
    Chain chain;
    std::optional<std::string> contractConsignmentId;
};

class ConsignmentService {
public:
    std::future<OTCConsignment> createConsignment(ConsignmentParams params);
    std::future<OTCConsignment> updateConsignment(const std::string& consignmentId, const std::optional<OTCConsignment>& updates);
    std::future<void> withdrawConsignment(const std::string& consignmentId);
    std::future<OTCConsignment> getConsignment(const std::string& consignmentId);
    std::future<std::vector<OTCConsignment>> getConsignmentsByToken(const std::string& tokenId, std::optional<std::any> filters);
    std::future<std::vector<OTCConsignment>> getConsignerConsignments(const std::string& consignerAddress, std::optional<Chain> chain);
    std::future<std::vector<OTCConsignment>> getAllConsignments(std::optional<std::any> filters);
    std::future<void> reserveAmount(const std::string& consignmentId, const std::string& amount);
    std::future<void> releaseReservation(const std::string& consignmentId, const std::string& amount);
    std::future<ConsignmentDeal> recordDeal(std::optional<std::any> params);
    std::future<std::vector<ConsignmentDeal>> getConsignmentDeals(const std::string& consignmentId);
    std::optional<OTCConsignment> findSuitableConsignment(const std::vector<OTCConsignment>& consignments, const std::string& amount, double discountBps, double lockupDays);
};


} // namespace elizaos
