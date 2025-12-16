#include "Socials.module.css.hpp"
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

struct SocialButtonProps {
    std::string provider;
    std::optional<bool> isActive;
};


const SocialButton: FC<SocialButtonProps> = ({ provider }) => {
  const { data: session, status } = useSession();

  useEffect(() => {
    console.log("Session updated:", {
      session,
      status,
      provider,
      account: session?.user?.connections?.[provider.toLowerCase()],
    });
  }, [session, status, provider]);

  // Check if this provider is the active one
  const isThisProviderActive =
    !!session?.user?.connections?.[provider.toLowerCase()];

  const handleClick = () => {
    if (session) {
      signOut();
    } else {
      signIn(provider.toLowerCase());
    }
  };

  return (
    <div
      className={`${styles.button} ${
        isThisProviderActive ? styles.active : ""
      }`}
      onClick={handleClick}
    >
      <div className={styles.text}>
        {isThisProviderActive ? `${provider}` : `Connect with ${provider}`}
      </div>
      {isThisProviderActive && session?.user?.name && (
        <span className={styles.userName}>({session.user.name})</span>
      )}
    </div>
  );
};

const Socials: FC = () => {
  // Commented out GitHub and Twitter, only using Discord for now
  const providers = ["Discord"]; // ['GitHub', 'Discord', 'Twitter'];

  return (
    <div>
      <h2 className={styles.title}></h2>
      <div className={styles.buttonParent}>
        {providers.map((provider) => (
          <SocialButton key={provider} provider={provider} />
        ))}
      </div>
    </div>
  );
};

default Socials;

} // namespace elizaos
