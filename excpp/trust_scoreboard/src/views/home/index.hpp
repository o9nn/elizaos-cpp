#include "..components/leaderboard/LeaderboardMedals.hpp"
#include "..components/leaderboard/LeaderboardPartners.hpp"
#include "..components/leaderboard/LeaderboardTotals.hpp"
#include "index.module.css.hpp"
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









const HomeView: FC = () => {
  const { users, isLoading: isLoadingUsers } = useGetUsers({
    cursor: 1,
    limit: 100,
  })
  const topUsers = useMemo(() => {
    return users?.filter((user) => user.rank <= 3)
  }, [users])

  const otherUsers = useMemo(() => {
    return users?.filter((user) => user.rank > 3)
  }, [users])

  return (
    <div className={styles.container}>
      <PageSwitcher className="mb-6" />
      <div className={styles.content}>
        <div className="flex flex-col items-center justify-center gap-2 px-4">
          <h2 className={styles.title}>Marc’s Trust Leaderboard</h2>
          <h3 className="text-center text-lg font-normal text-white/70">
            Limited access to Marc&apos;s Cabal Chat
          </h3>
        </div>

        <LeaderboardMedals users={topUsers} isLoading={isLoadingUsers} />

        <div className={styles.leaderboardWrapper}>
          <LeaderboardPartners users={users} isLoading={isLoadingUsers} />
        </div>
      </div>
    </div>
  )
}

} // namespace elizaos
