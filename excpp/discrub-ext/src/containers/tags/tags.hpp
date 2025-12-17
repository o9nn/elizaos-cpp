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
#include "classes/guild.hpp"
#include "components/before-and-after-fields.hpp"
#include "components/cancel-button.hpp"
#include "components/entity-icon.hpp"
#include "components/pause-button.hpp"
#include "components/skip-replies.hpp"
#include "components/token-not-found.hpp"
#include "enum/message-type.hpp"
#include "enum/tag.hpp"
#include "features/app/use-app-slice.hpp"
#include "features/channel/use-channel-slice.hpp"
#include "features/export/export-types.hpp"
#include "features/export/use-export-slice.hpp"
#include "features/guild/use-guild-slice.hpp"
#include "features/message/use-message-slice.hpp"
#include "features/user/use-user-slice.hpp"
#include "utils.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



void Tags();


} // namespace elizaos
