#include "otc-helpers.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

ParsedOffer parseOfferStruct(RawOfferData offerRaw) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    if (Array.isArray(offerRaw)) {
        return {
            consignmentId: offerRaw[0],
            tokenId: offerRaw[1],
            beneficiary: offerRaw[2],
            tokenAmount: offerRaw[3],
            discountBps: offerRaw[4],
            createdAt: offerRaw[5],
            unlockTime: offerRaw[6],
            priceUsdPerToken: offerRaw[7],
            maxPriceDeviation: offerRaw[8],
            ethUsdPrice: offerRaw[9],
            currency: offerRaw[10],
            approved: offerRaw[11],
            paid: offerRaw[12],
            fulfilled: offerRaw[13],
            cancelled: offerRaw[14],
            payer: offerRaw[15],
            amountPaid: offerRaw[16],
            };
        }
        return offerRaw;

}

} // namespace elizaos
