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

const USER_NAME = 'user';
const CHAT_SOURCE = 'client_chat';
const GROUP_CHAT_SOURCE = 'client_group_chat';

const AVATAR_IMAGE_MAX_SIZE = 300;

enum FIELD_REQUIREMENT_TYPE {
  REQUIRED = 'required',
  OPTIONAL = 'optional',
}

const FIELD_REQUIREMENTS = {
  name: FIELD_REQUIREMENT_TYPE.REQUIRED,
  username: FIELD_REQUIREMENT_TYPE.OPTIONAL,
  system: FIELD_REQUIREMENT_TYPE.REQUIRED,
  'settings.voice.model': FIELD_REQUIREMENT_TYPE.OPTIONAL,
  bio: FIELD_REQUIREMENT_TYPE.OPTIONAL,
  topics: FIELD_REQUIREMENT_TYPE.OPTIONAL,
  adjectives: FIELD_REQUIREMENT_TYPE.OPTIONAL,
  'style.all': FIELD_REQUIREMENT_TYPE.OPTIONAL,
  'style.chat': FIELD_REQUIREMENT_TYPE.OPTIONAL,
  'style.post': FIELD_REQUIREMENT_TYPE.OPTIONAL,
};

} // namespace elizaos
