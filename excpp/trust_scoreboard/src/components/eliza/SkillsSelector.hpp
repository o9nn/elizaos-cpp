#include "SkillsSelector.module.css.hpp"
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

struct SkillsSelectorProps {
    std::vector<std::string> selectedSkills;
    (skills: string[]) => void onSelectSkill;
};


const SkillsSelector: FC<SkillsSelectorProps> = ({
  selectedSkills,
  onSelectSkill
}) => {
  const handleSkillToggle = (skill: string) => {
    if (selectedSkills.includes(skill)) {
      onSelectSkill(selectedSkills.filter(s => s !== skill));
    } else {
      onSelectSkill([...selectedSkills, skill]);
    }
  };

  return (
    <div className={styles.formGroup}>
      <label>X/Twitter Skills</label>
      <div className={styles.buttonGroup}>
        <button 
          className={`${styles.button} ${selectedSkills.includes('Post') ? styles.selected : ''}`}
          onClick={() => handleSkillToggle('Post')}
        >
          Post
        </button>
        <button className={styles.button} disabled>Generate Images</button>
        <button className={styles.button} disabled>Reply</button>
        <button className={styles.button} disabled>Direct Message</button>
      </div>
      <div className={styles.buttonGroup}>
        <button className={styles.button} disabled>Follow/Unfollow</button>
        <button className={styles.button} disabled>Block</button>
      </div>
      <div className={styles.accountSection}>
        <button className={styles.disconnectButton}>Disconnect</button>
      </div>
    </div>
  );
}; 
} // namespace elizaos
