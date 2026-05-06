#pragma once
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#include "..app/store.hpp"
#include "..classes/channel.hpp"
#include "..classes/guild.hpp"
#include "..classes/message.hpp"
#include "..enum/export-type.hpp"
#include "..enum/media-type.hpp"
#include "..enum/message-regex.hpp"
#include "..services/discord-service.hpp"
#include "..utils.hpp"
#include ".app/app-slice.hpp"
#include ".channel/channel-slice.hpp"
#include ".message/message-slice.hpp"
#include "export-types.hpp"
#include "export-utils.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



/**
 *
 * @param {String} content String content to parse Discord special formatting
 * @returns An Object of special formatting
 */

/**
 *
 * @param {String} content String content to get formatted html from
 * @returns Html in String format
 */

  // TODO: Do we still need to reference messages in case of error?
  // HTML Exports actually are using ExportMessages component ref, NOT the messages passed to _exportHtml

              // We are currently only parsing User mentions, using username, in JSON exports.


} // namespace elizaos
