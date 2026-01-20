#include "use-user.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

void useUser() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto { publicKey } = useWallet();
    const auto { authQuery } = useAuthentication();

    const auto query = useQuery({;
        queryKey: ["user", publicKey, authQuery.data],
        queryFn: async () => {
            if (!publicKey) {
                return nullptr;
            }

            const auto authData = authQuery.data;
            if (authData.authenticated) {
                return {
                    user: authData.user,
                    authenticated: authData.authenticated,
                    };
                }
                return { authenticated: false }
                },
                enabled: !!publicKey && authQuery.isSuccess,
                });

                const std::optional<User> user = query.data.user;
                const bool authenticated = query.data.authenticated || false;

                return { user, authenticated, isLoading: query?.isPending, query }

}

} // namespace elizaos
