#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_SWEAGENT_TOOLS_SRC_IMAGE-TOOLS_VIEW-IMAGE_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_SWEAGENT_TOOLS_SRC_IMAGE-TOOLS_VIEW-IMAGE_H
#include "core.h"
#include "fs.h"
#include "path.h"
#include "commander.h"

extern std::shared_ptr<Set<string>> VALID_MIME_TYPES;
any getMimeType(string filename);

void viewImage(string imagePath);

void setupCLI();

#endif
