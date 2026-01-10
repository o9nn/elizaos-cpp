#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_AUTO_FUN_PACKAGES_CLIENT_SRC_TYPES_FORM_TYPE_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_AUTO_FUN_PACKAGES_CLIENT_SRC_TYPES_FORM_TYPE_H
#include "core.h"

class TokenMetadata;

class TokenMetadata : public object, public std::enable_shared_from_this<TokenMetadata> {
public:
    using std::enable_shared_from_this<TokenMetadata>::shared_from_this;
    string name;

    string symbol;

    string description;

    double initialSol;

    object links;

    any imageBase64;

    string tokenMint;

    double decimals;

    double supply;

    string freezeAuthority;

    string mintAuthority;
};

#endif
