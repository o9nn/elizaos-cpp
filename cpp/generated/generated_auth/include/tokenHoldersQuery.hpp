#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_AUTO_FUN_PACKAGES_SERVER_SRC_ROUTES_VALIDATORS_TOKENHOLDERSQUERY_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_AUTO_FUN_PACKAGES_SERVER_SRC_ROUTES_VALIDATORS_TOKENHOLDERSQUERY_H
#include "core.h"
#include "./global.h"

class RawHoldersQuery;
class ParsedHoldersQuery;

class RawHoldersQuery : public object, public std::enable_shared_from_this<RawHoldersQuery> {
public:
    using std::enable_shared_from_this<RawHoldersQuery>::shared_from_this;
    string limit;

    string page;
};

class ParsedHoldersQuery : public object, public std::enable_shared_from_this<ParsedHoldersQuery> {
public:
    using std::enable_shared_from_this<ParsedHoldersQuery>::shared_from_this;
    string mint;

    double limit;

    double page;

    double offset;
};

std::shared_ptr<ParsedHoldersQuery> parseHoldersQuery(any mintParam, std::shared_ptr<RawHoldersQuery> query);

#endif
