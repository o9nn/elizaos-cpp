#include "useQueryContext.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

void useQueryContext() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto router = useRouter();
    const auto { cluster } = router.query;

    const auto endpoint = cluster ? (cluster) : "mainnet";
    const auto hasClusterOption = endpoint != "mainnet";
    const auto fmtUrlWithCluster = [&](url) {;
        if (hasClusterOption) {
            const auto mark = (std::find(url.begin(), url.end(), "?") != url.end()) ? "&" : "?";
            return "decodeURIComponent(" + url + mark + "cluster=" + endpoint;
        }
        return url;
    }

    return {
        fmtUrlWithCluster,
    }

}

} // namespace elizaos
