#include "tokenHoldersQuery.hpp"

std::shared_ptr<ParsedHoldersQuery> parseHoldersQuery(any mintParam, std::shared_ptr<RawHoldersQuery> query)
{
    if (!mintParam) {
        throw any(std::make_shared<Error>(std:("Missing mint address")));
    }
    if (!parseSolanaAddress(mintParam)) {
        throw any(std::make_shared<Error>(std:("Invalid mint address: ") + mintParam + string_empty));
    }
    auto mint = mintParam;
    auto rawLimit = (query->limit) ? any(parseInt(query->limit, 10)) (MAX_LIMIT);
    if (OR((isNaN(rawLimit)), (rawLimit < 1))) {
        throw any(std::make_shared<Error>(std:("Invalid limit parameter: ") + query->limit + string_empty));
    }
    auto limit = Math->min(rawLimit, MAX_LIMIT);
    auto rawPage = (query->page) ? any(parseInt(query->page, 10)) (1);
    if (OR((isNaN(rawPage)), (rawPage < 1))) {
        throw any(std::make_shared<Error>(std:("Invalid page parameter: ") + query->page + string_empty));
    }
    auto page = Math->min(rawPage, MAX_PAGE);
    auto offset = (page - 1) * limit;
    return object{
        object::pair{std:("mint"), std:("mint")}, 
        object::pair{std:("limit"), std:("limit")}, 
        object::pair{std:("page"), std:("page")}, 
        object::pair{std:("offset"), std:("offset")}
    };
};


