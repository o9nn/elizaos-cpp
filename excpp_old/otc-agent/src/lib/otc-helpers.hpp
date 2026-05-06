#pragma once
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use

/**
 * OTC Contract helpers for parsing struct responses from viem
 */

struct ParsedOffer {
    bigint consignmentId;
    std::string tokenId;
    std::string beneficiary;
    bigint tokenAmount;
    bigint discountBps;
    bigint createdAt;
    bigint unlockTime;
    bigint priceUsdPerToken;
    bigint maxPriceDeviation;
    bigint ethUsdPrice;
    double currency;
    bool approved;
    bool paid;
    bool fulfilled;
    bool cancelled;
    std::string payer;
    bigint amountPaid;
};

/**
 * Raw offer data as returned from viem contract read - can be array or object.
 * Array format is the standard Solidity struct tuple return.
 */
using RawOfferData = std::variant<, readonly [

/**
 * Parse an Offer struct from viem contract read.
 * Viem may return structs as arrays or objects depending on version/config.
 *
 * Struct order from OTC.sol (UPDATED with multi-token support):
 * 0. consignmentId (uint256)
 * 1. tokenId (bytes32)
 * 2. beneficiary (address)
 * 3. tokenAmount (uint256)
 * 4. discountBps (uint256)
 * 5. createdAt (uint256)
 * 6. unlockTime (uint256)
 * 7. priceUsdPerToken (uint256)
 * 8. maxPriceDeviation (uint256)
 * 9. ethUsdPrice (uint256)
 * 10. currency (uint8)
 * 11. approved (bool)
 * 12. paid (bool)
 * 13. fulfilled (bool)
 * 14. cancelled (bool)
 * 15. payer (address)
 * 16. amountPaid (uint256)
 */
ParsedOffer parseOfferStruct(RawOfferData offerRaw);

} // namespace elizaos
