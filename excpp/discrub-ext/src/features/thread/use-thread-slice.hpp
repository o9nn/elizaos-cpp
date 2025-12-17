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
#include "app/hooks.hpp"
#include "app/store.hpp"
#include "classes/channel.hpp"
#include "classes/message.hpp"
#include "thread-slice.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



    return getThreadsFromMessagesAction({ messages, knownThreads });

  /**
   *
   * @param {string} channelId The Id of the Thread to lift locked/archived restrictions from
   * @param {Array} noPermissionThreadIds Optional array of Thread Ids that could not be unlocked
   * @returns An updated array of Thread Ids that the User does not have permission to modify
   */


} // namespace elizaos
