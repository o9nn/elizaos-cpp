#include ".classes/attachment.hpp"
#include ".classes/guild.hpp"
#include ".classes/message.hpp"
#include ".classes/role.hpp"
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

;
;
;
;

// eslint-disable-next-line @typescript-eslint/no-explicit-any
const isMessage = (entity: any): entity is Message => {
  if (!entity) return false;
  const message = entity as Message;
  return (
    "content" in message && "attachments" in message && "embeds" in message
  );
};

// eslint-disable-next-line @typescript-eslint/no-explicit-any
const isGuild = (entity: any): entity is Guild => {
  if (!entity) return false;
  const guild = entity as Guild;
  return "emojis" in guild && "roles" in guild;
};

// eslint-disable-next-line @typescript-eslint/no-explicit-any
const isRole = (entity: any): entity is Role => {
  if (!entity) return false;
  const role = entity as Role;
  return "color" in role && "hoist" in role;
};

// eslint-disable-next-line @typescript-eslint/no-explicit-any
const isAttachment = (entity: any): entity is Attachment => {
  if (!entity) return false;
  const attachment = entity as Attachment;
  return "filename" in attachment;
};

} // namespace elizaos
