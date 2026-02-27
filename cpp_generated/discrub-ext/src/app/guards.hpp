#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_DISCRUB-EXT_SRC_APP_GUARDS_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_DISCRUB-EXT_SRC_APP_GUARDS_H
#include "core.h"
#include "../classes/attachment.h"
using Attachment = _default;
#include "../classes/guild.h"
using Guild = _default;
#include "../classes/message.h"
using Message = _default;
#include "../classes/role.h"
using Role = _default;

extern std::function<any(any)> isMessage;
extern std::function<any(any)> isGuild;
extern std::function<any(any)> isRole;
extern std::function<any(any)> isAttachment;
#endif
