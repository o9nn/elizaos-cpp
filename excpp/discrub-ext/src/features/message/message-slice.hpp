#pragma once
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#include "..app/guards.hpp"
#include "..app/store.hpp"
#include "..classes/attachment.hpp"
#include "..classes/channel.hpp"
#include "..classes/message.hpp"
#include "..classes/reaction.hpp"
#include "..enum/filter-name.hpp"
#include "..enum/filter-type.hpp"
#include "..enum/has-type.hpp"
#include "..enum/message-category.hpp"
#include "..enum/message-regex.hpp"
#include "..enum/message-type.hpp"
#include "..enum/query-string-param.hpp"
#include "..enum/reaction-type.hpp"
#include "..enum/sort-direction.hpp"
#include "..services/discord-service.hpp"
#include "..utils.hpp"
#include ".app/app-slice.hpp"
#include ".export/export-slice.hpp"
#include ".export/export-types.hpp"
#include ".guild/guild-slice.hpp"
#include ".thread/thread-slice.hpp"
#include "message-types.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



          // Add the toggle to filters
          // Remove the toggle from filters
          // Remove filter from list

        // If we are purging or exporting, we need to allow those respective slices to handle this.

          // Max offset is 5000, need to reset offset and update/set searchBeforeDate


} // namespace elizaos
