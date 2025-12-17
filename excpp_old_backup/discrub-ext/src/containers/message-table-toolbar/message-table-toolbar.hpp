#include "..common-components/tooltip/tooltip.hpp"
#include "..features/app/use-app-slice.hpp"
#include "..features/dm/use-dm-slice.hpp"
#include "..features/message/message-types.hpp"
#include "..features/message/use-message-slice.hpp"
#include "..features/thread/use-thread-slice.hpp"
#include ".export-button/export-button.hpp"
#include "components/delete-modal.hpp"
#include "components/edit-modal.hpp"
#include "components/filter-modal.hpp"
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



using MessageTableToolbarProps = std::vector<{

      // We are actively deleting, we need to send a cancel request

      // We are actively editing, we need to send a cancel request


} // namespace elizaos
