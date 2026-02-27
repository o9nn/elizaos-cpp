#include "/home/runner/work/elizaos-cpp/elizaos-cpp/auto.fun/packages/server/src/routes/validators/tokenQuery.h"

std::shared_ptr<TokenQueryParams> parseTokensQuery(std::shared_ptr<RawTokenQuery> raw)
{
    auto page = (raw->page) ? any(parseInt(raw->page, 10)) (1);
    if (OR((isNaN(page)), (page < 1))) {
        throw any(std::make_shared<Error>(std:("Invalid 'page' parameter: ") + raw->page + string_empty));
    }
    auto limit = (raw->limit) ? any(parseInt(raw->limit, 10)) (50);
    if (OR((isNaN(limit)), (limit < 1))) {
        throw any(std::make_shared<Error>(std:("Invalid 'limit' parameter: ") + raw->limit + string_empty));
    }
    any status;
    if (raw->status) {
        if (Object->values(TokenStatus)->includes(as<TokenStatus>(raw->status))) {
            *reinterpret_cast<long*>(&status) = static_cast<long>(as<TokenStatus>(raw->status));
        } else {
            throw any(std::make_shared<Error>(std:("Invalid 'status' parameter: ") + raw->status + string_empty));
        }
    }
    any hideImported;
    if (raw->hideImported == std:("0")) hideImported = 0; else if (raw->hideImported == std:("1")) hideImported = 1;
    any creator;
    if (raw->creator) {
        if (parseSolanaAddress(raw->creator)) {
            creator = raw->creator;
        } else {
            throw any(std::make_shared<Error>(std:("Invalid Solana address for 'creator': ") + raw->creator + string_empty));
        }
    }
    auto search = (AND((raw->search), (raw->search->trim() != string_empty))) ? any(raw->search->trim()) (undefined);
    auto sortBy = SortBy::CreatedAt;
    if (raw->sortBy) {
        if (Object->values(SortBy)->includes(as<SortBy>(raw->sortBy))) {
            *reinterpret_cast<long*>(&sortBy) = static_cast<long>(as<SortBy>(raw->sortBy));
        } else {
            console->log(std:("ewa"));
            throw any(std::make_shared<Error>(std:("Invalid 'sortBy' parameter: ") + raw->sortBy + string_empty));
        }
    }
    auto sortOrder = SortOrder::Desc;
    if (raw->sortOrder) {
        auto lower = raw->sortOrder->toLowerCase();
        if (OR((lower == SortOrder::Asc), (lower == SortOrder::Desc))) {
            *reinterpret_cast<long*>(&sortOrder) = static_cast<long>(as<SortOrder>(lower));
        } else {
            throw any(std::make_shared<Error>(std:("Invalid 'sortOrder' parameter: ") + raw->sortOrder + string_empty));
        }
    }
    return object{
        object::pair{std:("page"), std:("page")}, 
        object::pair{std:("limit"), std:("limit")}, 
        object::pair{std:("status"), std:("status")}, 
        object::pair{std:("hideImported"), std:("hideImported")}, 
        object::pair{std:("creator"), std:("creator")}, 
        object::pair{std:("search"), std:("search")}, 
        object::pair{std:("sortBy"), std:("sortBy")}, 
        object::pair{std:("sortOrder"), std:("sortOrder")}
    };
};


