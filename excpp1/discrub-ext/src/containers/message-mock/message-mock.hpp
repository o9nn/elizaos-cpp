#pragma once
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#include "..classes/message.hpp"
#include "..components/author-avatar.hpp"
#include "..enum/message-type.hpp"
#include "..features/app/use-app-slice.hpp"
#include "..features/channel/use-channel-slice.hpp"
#include "..features/export/use-export-slice.hpp"
#include "..features/guild/use-guild-slice.hpp"
#include "..features/message/use-message-slice.hpp"
#include "..features/thread/use-thread-slice.hpp"
#include "..features/user/use-user-slice.hpp"
#include "..utils.hpp"
#include "components/attachments.hpp"
#include "components/author-name.hpp"
#include "components/call-message.hpp"
#include "components/chained-date.hpp"
#include "components/channel-name.hpp"
#include "components/date.hpp"
#include "components/message-content.hpp"
#include "components/pin-message.hpp"
#include "components/reactions.hpp"
#include "components/replied-to-content.hpp"
#include "components/thread-name.hpp"
#include "components/webhook-embeds.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



using MessageMockProps = {

    return resolveRoleUrl(id, icon, roleMap);


} // namespace elizaos
