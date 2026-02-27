#include "elizaos/core.hpp"
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



struct ResolveEnsParams {
    std: query;
    std::optional<bool> includeFarcaster;
    std::optional<bool> includeExpiry;
};

using ResolveEnsInput = {

using ResolveEnsActionResult = ActionResult & { input: ResolveEnsInput };

struct EnsDataResponse {
    std::optional<std:> address;
    std::optional<std:> ens;
    std::optional<std:> ens_primary;
    std::optional<std:> resolverAddress;
    std::optional<std:> avatar;
    std::optional<std:> avatar_small;
    std::optional<std:> avatar_url;
    std::optional<std:> contentHash;
    std::optional<std:> description;
    std::optional<std:> twitter;
    std::optional<std:> github;
    std::optional<std:> url;
    std::optional<std:> header;
    std::optional<std:> pgp;
    std::optional<std:> expiry;
    std::optional<{> farcaster;
    std::optional<double> fid;
    std::optional<std:> username;
    std::optional<std:> display_name;
    std::optional<std:> bio;
    std::optional<std:> profile_url;
    std::optional<std::unordered_map<std:, std:>> wallets;
    std::optional<std::unordered_map<std:, std:>> records;
    std::optional<bool> error;
    std::optional<double> status;
    std::optional<std:> message;
};


} // namespace elizaos
