#include "SocialLinks.module.css.hpp"
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

const SocialLinks: FC = () => {
  return (
    <div className={styles.socialSection}>
      <h3 className={styles.sectionTitle}>Socials</h3>
      <div className={styles.socialButtons}>
        <button className={styles.socialButton}>
          <Image src="/twitter.svg" alt="Twitter" width={16} height={16} />
          @npc
        </button>
        <button className={styles.socialButton}>
          <Image src="/twitter.svg" alt="Twitter" width={16} height={16} />
          @npc
        </button>
        <button className={styles.socialButton}>
          <Image src="/twitter.svg" alt="Twitter" width={16} height={16} />
          @npc
        </button>
      </div>
    </div>
  );
}; 
} // namespace elizaos
