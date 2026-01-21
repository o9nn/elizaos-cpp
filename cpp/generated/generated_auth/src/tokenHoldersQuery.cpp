#include "/home/runner/work/elizaos-cpp/elizaos-cpp/auto.fun/packages/server/src/routes/validators/tokenHoldersQuery.h"

std::shared_ptr<ParsedHoldersQuery> parseHoldersQuery(any mintParam, std::shared_ptr<RawHoldersQuery> query)
{
    if (!mintParam) {
        throw any(std::make_shared<Error>(std::string("Missing mint address")));
    }
    if (!parseSolanaAddress(mintParam)) {
        throw any(std::make_shared<Error>(std::string("Invalid mint address: ") + mintParam + string_empty));
    }
    auto mint = mintParam;
    auto rawLimit = (query->limit) ? any(parseInt(query->limit, 10)) : any(MAX_LIMIT);
    if (OR((isNaN(rawLimit)), (rawLimit < 1))) {
        throw any(std::make_shared<Error>(std::string("Invalid limit parameter: ") + query->limit + string_empty));
    }
    auto limit = Math->min(rawLimit, MAX_LIMIT);
    auto rawPage = (query->page) ? any(parseInt(query->page, 10)) : any(1);
    if (OR((isNaN(rawPage)), (rawPage < 1))) {
        throw any(std::make_shared<Error>(std::string("Invalid page parameter: ") + query->page + string_empty));
    }
    auto page = Math->min(rawPage, MAX_PAGE);
    auto offset = (page - 1) * limit;
    return object{
        object::pair{std::string("mint"), std::string("mint")}, 
        object::pair{std::string("limit"), std::string("limit")}, 
        object::pair{std::string("page"), std::string("page")}, 
        object::pair{std::string("offset"), std::string("offset")}
    };
};


