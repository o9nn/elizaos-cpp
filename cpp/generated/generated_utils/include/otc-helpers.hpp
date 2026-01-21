#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_OTC-AGENT_SRC_LIB_OTC-HELPERS_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_OTC-AGENT_SRC_LIB_OTC-HELPERS_H
#include "core.h"

typedef any RawOfferData;

class ParsedOffer;

class ParsedOffer : public object, public std::enable_shared_from_this<ParsedOffer> {
public:
    using std::enable_shared_from_this<ParsedOffer>::shared_from_this;
    any consignmentId;

    string tokenId;

    string beneficiary;

    any tokenAmount;

    any discountBps;

    any createdAt;

    any unlockTime;

    any priceUsdPerToken;

    any maxPriceDeviation;

    any ethUsdPrice;

    double currency;

    boolean approved;

    boolean paid;

    boolean fulfilled;

    boolean cancelled;

    string payer;

    any amountPaid;
};

template <typename P0>
std::shared_ptr<ParsedOffer> parseOfferStruct(P0 offerRaw);

template <typename P0>
std::shared_ptr<ParsedOffer> parseOfferStruct(P0 offerRaw)
{
    if (Array->isArray(offerRaw)) {
        return object{
            object::pair{std::string("consignmentId"), const_(offerRaw)[0]}, 
            object::pair{std::string("tokenId"), const_(offerRaw)[1]}, 
            object::pair{std::string("beneficiary"), const_(offerRaw)[2]}, 
            object::pair{std::string("tokenAmount"), const_(offerRaw)[3]}, 
            object::pair{std::string("discountBps"), const_(offerRaw)[4]}, 
            object::pair{std::string("createdAt"), const_(offerRaw)[5]}, 
            object::pair{std::string("unlockTime"), const_(offerRaw)[6]}, 
            object::pair{std::string("priceUsdPerToken"), const_(offerRaw)[7]}, 
            object::pair{std::string("maxPriceDeviation"), const_(offerRaw)[8]}, 
            object::pair{std::string("ethUsdPrice"), const_(offerRaw)[9]}, 
            object::pair{std::string("currency"), const_(offerRaw)[10]}, 
            object::pair{std::string("approved"), const_(offerRaw)[11]}, 
            object::pair{std::string("paid"), const_(offerRaw)[12]}, 
            object::pair{std::string("fulfilled"), const_(offerRaw)[13]}, 
            object::pair{std::string("cancelled"), const_(offerRaw)[14]}, 
            object::pair{std::string("payer"), const_(offerRaw)[15]}, 
            object::pair{std::string("amountPaid"), const_(offerRaw)[16]}
        };
    }
    return as<std::shared_ptr<ParsedOffer>>(offerRaw);
};


#endif
