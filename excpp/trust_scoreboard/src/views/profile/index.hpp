#include "..components/profile/ApiSection.hpp"
#include "..components/profile/ProfileData.hpp"
#include "..components/profile/ProfileHoldings.hpp"
#include "..components/profile/ProfileTotals.hpp"
#include "..components/profile/ProfileWallets.hpp"
#include "..components/profile/Socials.hpp"
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

;
;
;
;
;
;
;
;
;

using View = std::variant<"profile", "holdings">;

const ProfileView: FC = () => {
  const { data: session } = useSession();
  const [currentView, setCurrentView] = useState<View>("profile");

  const handleViewChange = (view: View) => {
    setCurrentView(view);
  };

  const handleSignOut = () => {
    signOut({ callbackUrl: "/" });
  };

  return (
    <div className={styles.container}>
      <div className={styles.contentWrapper}>
        <div className={styles.profileDataWrapper}>
          <ProfileData />
        </div>
        <div className={styles.profileTotalsWrapper}>
          <ProfileTotals onViewChange={handleViewChange} />

        </div>
        {
          session?.user &&
          (<div className="flex items-center justify-center w-full px-[24px]">
            <button onClick={handleSignOut} className={`rounded-2xl ${styles.signOutButton}`}>
              Logout
            </button>
          </div>)
        }
        <div className={styles.contentArea}>
          {currentView === "profile" ? (
            <div className={styles.profileContent}>
              <div className={styles.rowContainer}>
                <div className={`hidden px-10 ${styles.column}`}>
                  <Socials />
                </div>
                <div className={`hidden ${styles.column}`}>
                  <ProfileWallets />
                </div>
              </div>
            </div>
          ) : (
            <ProfileHoldings />
          )}

        </div>
      </div>
    </div>
  );
};

default ProfileView;

} // namespace elizaos
