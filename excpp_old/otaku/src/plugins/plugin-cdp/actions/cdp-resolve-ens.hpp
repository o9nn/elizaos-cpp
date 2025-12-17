#pragma once
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#include "elizaos/core.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



struct ResolveEnsParams {
    std::string query;
    std::optional<bool> includeFarcaster;
    std::optional<bool> includeExpiry;
};

using ResolveEnsInput = {

using ResolveEnsActionResult = ActionResult & { input: ResolveEnsInput };

struct EnsDataResponse {
    std::optional<std::string> address;
    std::optional<std::string> ens;
    std::optional<std::string> ens_primary;
    std::optional<std::string> resolverAddress;
    std::optional<std::string> avatar;
    std::optional<std::string> avatar_small;
    std::optional<std::string> avatar_url;
    std::optional<std::string> contentHash;
    std::optional<std::string> description;
    std::optional<std::string> twitter;
    std::optional<std::string> github;
    std::optional<std::string> url;
    std::optional<std::string> header;
    std::optional<std::string> pgp;
    std::optional<std::string> expiry;
    std::optional<double> fid;
    std::optional<std::string> username;
    std::optional<std::string> display_name;
    std::optional<std::string> bio;
    std::optional<std::string> profile_url;
    std::optional<bool> error;
    std::optional<double> status;
    std::optional<std::string> message;
};


} // namespace elizaos
