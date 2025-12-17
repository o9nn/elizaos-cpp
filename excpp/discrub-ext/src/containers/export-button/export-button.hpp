#pragma once
#include <algorithm>
#include <cstdint>
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include "classes/channel.hpp"
#include "components/bulk-content.hpp"
#include "components/default-content.hpp"
#include "components/export-messages.hpp"
#include "components/export-modal.hpp"
#include "enum/export-type.hpp"
#include "enum/media-type.hpp"
#include "features/app/use-app-slice.hpp"
#include "features/channel/use-channel-slice.hpp"
#include "features/dm/use-dm-slice.hpp"
#include "features/export/export-utils.hpp"
#include "features/export/use-export-slice.hpp"
#include "features/guild/use-guild-slice.hpp"
#include "features/message/use-message-slice.hpp"
#include "utils.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



using ExportButtonProps = {

      return String(selectedGuild?.name);

      // We are actively exporting, we need to send a cancel request


} // namespace elizaos
