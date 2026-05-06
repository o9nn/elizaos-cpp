#include "database.hpp"
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
  async createConsignment(params: ConsignmentParams): Promise<OTCConsignment> {
    if (!params.isNegotiable) {
      if (
        params.fixedDiscountBps === undefined ||
        params.fixedLockupDays === undefined
      ) {
        throw new Error(
          "Fixed consignments must specify fixedDiscountBps and fixedLockupDays",
        );
      }
    }

    if (BigInt(params.minDealAmount) > BigInt(params.maxDealAmount)) {
      throw new Error("minDealAmount cannot exceed maxDealAmount");
    }

    if (BigInt(params.amount) < BigInt(params.minDealAmount)) {
      throw new Error(
        `Total amount (${params.amount}) must be at least minDealAmount (${params.minDealAmount})`,
      );
    }

    if (params.minDiscountBps > params.maxDiscountBps) {
      throw new Error("minDiscountBps cannot exceed maxDiscountBps");
    }

    if (params.minLockupDays > params.maxLockupDays) {
      throw new Error("minLockupDays cannot exceed maxLockupDays");
    }

    // Solana addresses are Base58 and case-sensitive, EVM addresses are case-insensitive
    const normalizeAddress = (addr: string) =>
      params.chain === "solana" ? addr : addr.toLowerCase();

    const consignment = await ConsignmentDB.createConsignment({
      tokenId: params.tokenId,
      consignerAddress: normalizeAddress(params.consignerAddress),
      consignerEntityId: walletToEntityId(params.consignerAddress),
      totalAmount: params.amount,
      remainingAmount: params.amount,
      isNegotiable: params.isNegotiable,
      fixedDiscountBps: params.fixedDiscountBps,
      fixedLockupDays: params.fixedLockupDays,
      minDiscountBps: params.minDiscountBps,
      maxDiscountBps: params.maxDiscountBps,
      minLockupDays: params.minLockupDays,
      maxLockupDays: params.maxLockupDays,
      minDealAmount: params.minDealAmount,
      maxDealAmount: params.maxDealAmount,
      isFractionalized: params.isFractionalized,
      isPrivate: params.isPrivate,
      allowedBuyers: params.allowedBuyers?.map((a) => normalizeAddress(a)),
      maxPriceVolatilityBps: params.maxPriceVolatilityBps,
      maxTimeToExecuteSeconds: params.maxTimeToExecuteSeconds,
      status: "active",
      chain: params.chain,
      contractConsignmentId: params.contractConsignmentId,
    });

    return consignment;
  }

        // Compare addresses - Solana is case-sensitive, EVM is case-insensitive

    // Normalize address based on chain - Solana is case-sensitive, EVM is case-insensitive

    // Normalize address based on chain - Solana is case-sensitive, EVM is case-insensitive


} // namespace elizaos
