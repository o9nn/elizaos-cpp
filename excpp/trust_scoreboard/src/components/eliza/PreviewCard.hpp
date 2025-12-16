#include "PreviewCard.module.css.hpp"
#include "SkillsList.hpp"
#include "SocialLinks.hpp"
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

struct PreviewCardProps {
    std::string name;
    std::string description;
    std::string avatar;
};


const PreviewCard: FC<PreviewCardProps> = ({
  name = 'Default NPC',
  description = 'I will manage your community, and respond to questions.',
  avatar = '/default-profile.png'
}) => {
  return (
    <div className={styles.previewSection}>
      <div className={styles.previewCard}>
        <div className={styles.previewAvatar}>
          <Image 
            src={avatar}
            alt="NPC Avatar" 
            width={80} 
            height={80}
          />
        </div>
        <h2 className={styles.previewTitle}>{name}</h2>
        <p className={styles.previewDescription}>{description}</p>
        
        <SocialLinks />
        <SkillsList />
      </div>
    </div>
  );
}; 
} // namespace elizaos
