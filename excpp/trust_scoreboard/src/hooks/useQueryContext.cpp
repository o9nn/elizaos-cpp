#include "useQueryContext.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

void useQueryContext() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto router = useRouter();
    const auto { cluster } = router.query;

    const auto endpoint = cluster ? (cluster) : 'mainnet';
    const auto hasClusterOption = endpoint != 'mainnet';
    const auto fmtUrlWithCluster = [&](url) {;
        if (hasClusterOption) {
            const auto mark = url.includes('?') ? '&' : '?';
            return std::to_string(url) + std::to_string(mark) + "cluster=" + std::to_string(endpoint);
        }
        return url;
    }

    return {
        fmtUrlWithCluster,
    }

}

} // namespace elizaos
