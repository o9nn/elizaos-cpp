#include "use-authentication.hpp"
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

;
;
;

struct User {
    std::string address;
    double points;
    std::optional<double> solBalance;
};


 = useWallet();
  const { authQuery } = useAuthentication();

  const query = useQuery({
    queryKey: ["user", publicKey, authQuery.data],
    queryFn: async () => {
      if (!publicKey) {
        return null;
      }

      const authData = authQuery.data;
      if (authData?.authenticated) {
        return {
          user: authData.user,
          authenticated: authData.authenticated,
        };
      }
      return { authenticated: false };
    },
    enabled: !!publicKey && authQuery.isSuccess,
  });

  const user: User | null | undefined = query?.data?.user;
  const authenticated: boolean = query?.data?.authenticated || false;

  return { user, authenticated, isLoading: query?.isPending, query };
}

} // namespace elizaos
