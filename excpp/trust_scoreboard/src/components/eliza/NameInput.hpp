#include "NameInput.module.css.hpp"
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

struct NameInputProps {
    std::string value;
    (value: string) => void onChange;
};


const NameInput: FC<NameInputProps> = ({ value, onChange }) => {
  return (
    <div className={styles.formGroup}>
      <label>Name</label>
      <input 
        type="text" 
        placeholder="Character Name"
        value={value}
        onChange={(e) => onChange(e.target.value)}
        className={styles.input}
      />
    </div>
  );
}; 
} // namespace elizaos
