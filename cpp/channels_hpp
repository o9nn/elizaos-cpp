#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_ELIZA_PACKAGES_SERVER_SRC_API_MEDIA_CHANNELS_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_ELIZA_PACKAGES_SERVER_SRC_API_MEDIA_CHANNELS_H
#include "core.h"
#include "@elizaos/core.h"
#include "express.h"
using express = _default;
#include "express-rate-limit.h"
using rateLimit = _default;
#include "../shared/constants.h"
#include "multer.h"
using multer = _default;
#include "fs.h"
using fs = _default;
#include "path.h"
using path = _default;

extern any storage;
extern any upload;
std::shared_ptr<Promise<object>> saveUploadedFile(std::shared_ptr<Express::Multer::File> file, string channelId);

std::shared_ptr<express::Router> createChannelMediaRouter();

#endif
