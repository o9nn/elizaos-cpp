#include ".classes/channel.hpp"
#include ".classes/guild.hpp"
#include ".utils.hpp"
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

using EntityIconProps = std::variant<{
  entity: Guild, Channel>;
};
const EntityIcon = ({ entity }: EntityIconProps) => {
  return (
    <img
      style={{ width: "24px", height: "24px", borderRadius: "50px" }}
      src={getIconUrl(entity)}
      alt="guild-icon"
    />
  );
};

default EntityIcon;

} // namespace elizaos
