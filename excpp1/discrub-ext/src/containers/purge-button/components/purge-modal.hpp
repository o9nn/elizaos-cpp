#pragma once
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#include "...app/guards.hpp"
#include "...components/cancel-button.hpp"
#include "...components/modal-debug-message.hpp"
#include "...components/pause-button.hpp"
#include "...components/prefilter-user.hpp"
#include "...features/app/use-app-slice.hpp"
#include "...features/channel/use-channel-slice.hpp"
#include "...features/dm/use-dm-slice.hpp"
#include "...features/guild/use-guild-slice.hpp"
#include "...features/message/use-message-slice.hpp"
#include "...features/purge/use-purge-slice.hpp"
#include "...features/user/use-user-slice.hpp"
#include "..message-mock/message-mock.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



using PurgeModalProps = {

    // eslint-disable-next-line react-hooks/exhaustive-deps

      // We are actively deleting, we need to send a cancel request


} // namespace elizaos
