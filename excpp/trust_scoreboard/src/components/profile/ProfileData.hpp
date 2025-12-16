#include "ProfileData.module.css.hpp"
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

const ProfileData: FC = () => {
  const { data: session } = useSession();

 

  // Helper ;

  return (
    <div className={styles.container}>
      <div className={styles.profileContainer}>
        {session?.user?.image && (
          <div className={styles.imageContainer}>
            <Image
              src={session.user.image}
              alt="Profile"
              width={120}
            height={120}
              className={styles.profileImage}
              priority
              unoptimized
            />
          </div>
        )}
        <div className={styles.userInfo}>
          <h1 className={styles.userName}>
            {session?.user?.name || "Anonymous"}
          </h1>
          <span className={`hidden ${styles.userRole}`}>{getUserRole()}</span>
        </div>
      </div>
    </div>
  );
};

default ProfileData;

} // namespace elizaos
