#ifndef _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_SWEAGENT_TOOLS_SRC_IMAGE_TOOLS_VIEW_IMAGE_H
#define _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_SWEAGENT_TOOLS_SRC_IMAGE_TOOLS_VIEW_IMAGE_H
#include "core.hpp"
#include "fs.hpp"
#include "path.hpp"
#include "commander.hpp"

extern std::shared_ptr<Set<string>> VALID_MIME_TYPES;
any getMimeType(string filename);

void viewImage(string imagePath);

void setupCLI();

#endif
