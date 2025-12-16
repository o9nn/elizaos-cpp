#include "..data/users.hpp"
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

// Hook for managing selected tags
, []);

  const toggleOperator = useCallback(() => {
    setOperator((op) => (op === 'OR' ? 'AND' : 'OR'));
  }, []);

  // Filter users based on selected tags
  const filteredUsers =
    selectedTags.length === 0
      ? sortedUsers
      : sortedUsers.filter((user) => {
          if (operator === 'AND') {
            return selectedTags.every((tag) => user.tags.includes(tag));
          } else {
            return selectedTags.some((tag) => user.tags.includes(tag));
          }
        });

  return {
    selectedTags,
    toggleTag,
    operator,
    toggleOperator,
    filteredUsers,
  };
}

} // namespace elizaos
