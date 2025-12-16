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

enum Tag {
  TAGS_MADE_BY_USER = 0,
  TAGGED_USERS = 1,
}

const getTagName = (type: Tag): string => {
  switch (type) {
    case Tag.TAGS_MADE_BY_USER:
      return "Tags Sent By User";
    case Tag.TAGGED_USERS:
      return "Tags Received For User";
    default:
      return "";
  }
};

} // namespace elizaos
