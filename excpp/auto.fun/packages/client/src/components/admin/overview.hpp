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
;

struct AdminStats {
    double userCount;
    double tokenCount;
    double volume24h;
};


default ;
      return response.stats;
    },
    refetchInterval: 60000, // Refetch every minute
  });

  if (statsQuery.isLoading) {
    return <Loader />;
  }

  const stats = statsQuery.data || {
    userCount: 0,
    tokenCount: 0,
    volume24h: 0,
  };

  return (
    <div className="p-4 bg-autofun-background-input ">
      <h2 className="text-xl font-bold mb-4">Admin Overview</h2>
      <div className="grid grid-cols-1 md:grid-cols-3 gap-4">
        <div className="p-4 bg-autofun-background-primary ">
          <h3 className="text-lg font-medium mb-2">Users</h3>
          <p className="text-2xl font-bold text-autofun-text-highlight">
            {stats.userCount.toLocaleString()}
          </p>
          <p className="text-sm text-autofun-text-secondary">Total users</p>
        </div>
        <div className="p-4 bg-autofun-background-primary ">
          <h3 className="text-lg font-medium mb-2">Tokens</h3>
          <p className="text-2xl font-bold text-autofun-text-highlight">
            {stats.tokenCount.toLocaleString()}
          </p>
          <p className="text-sm text-autofun-text-secondary">Total tokens</p>
        </div>
        <div className="p-4 bg-autofun-background-primary ">
          <h3 className="text-lg font-medium mb-2">Volume</h3>
          <p className="text-2xl font-bold text-autofun-text-highlight">
            {formatNumber(stats.volume24h)}
          </p>
          <p className="text-sm text-autofun-text-secondary">24h volume</p>
        </div>
      </div>
    </div>
  );
}

} // namespace elizaos
