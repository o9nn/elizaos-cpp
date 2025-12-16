#include ".features/app/app-slice.hpp"
#include ".features/channel/channel-slice.hpp"
#include ".features/dm/dm-slice.hpp"
#include ".features/export/export-slice.hpp"
#include ".features/guild/guild-slice.hpp"
#include ".features/message/message-slice.hpp"
#include ".features/purge/purge-slice.hpp"
#include ".features/relationship/relationship-slice.hpp"
#include ".features/thread/thread-slice.hpp"
#include ".features/user/user-slice.hpp"
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



// Infer the `RootState` and `AppDispatch` types from the store itself
using RootState = ReturnType<typeof store.getState>;
// Inferred type: {posts: PostsState, comments: CommentsState, users: UsersState}
using AppDispatch = typeof store.dispatch;

} // namespace elizaos
