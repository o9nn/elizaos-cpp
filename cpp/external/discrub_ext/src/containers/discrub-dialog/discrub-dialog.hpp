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
#include "channel-messages/channel-messages.hpp"
#include "components/about.hpp"
#include "components/announcement-component.hpp"
#include "components/close-window-button.hpp"
#include "components/donation-component.hpp"
#include "components/menu-bar.hpp"
#include "components/settings.hpp"
#include "direct-messages/direct-messages.hpp"
#include "features/app/use-app-slice.hpp"
#include "features/channel/use-channel-slice.hpp"
#include "features/dm/use-dm-slice.hpp"
#include "features/guild/use-guild-slice.hpp"
#include "features/message/use-message-slice.hpp"
#include "features/user/use-user-slice.hpp"
#include "services/chrome-service.hpp"
#include "tags/tags.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



void DiscrubDialog();


} // namespace elizaos
