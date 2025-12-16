#include "PersonalitySelector.module.css.hpp"
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

struct PersonalitySelectorProps {
    std::string selected;
    (personality: string) => void onSelect;
};


const PersonalitySelector: FC<PersonalitySelectorProps> = ({
  selected,
  onSelect
}) => {
  return (
    <div className={styles.formGroup}>
      <label>Personality</label>
      <div className={styles.buttonGroup}>
        <button 
          className={`${styles.button} ${selected === 'Manager' ? styles.selected : ''}`}
          onClick={() => onSelect('Manager')}
        >
          Manager
        </button>
        <button className={styles.button} disabled>Member</button>
        <button className={styles.button} disabled>Moderator</button>
      </div>
      <span className={styles.helperText}>Custom personalities coming soon.</span>
    </div>
  );
}; 
} // namespace elizaos
