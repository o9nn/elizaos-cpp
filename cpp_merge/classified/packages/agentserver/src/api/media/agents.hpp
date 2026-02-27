#ifndef _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_CLASSIFIED_PACKAGES_AGENTSERVER_SRC_API_MEDIA_AGENTS_H
#define _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_CLASSIFIED_PACKAGES_AGENTSERVER_SRC_API_MEDIA_AGENTS_H
#include "core.hpp"
// External dependency removed
#include "express.hpp"
// Using alias removed (invalid transpilation)
#include "../shared/response-utils.h"
#include "../shared/constants.h"
#include "multer.hpp"
// Using alias removed (invalid transpilation)
#include "fs.hpp"
// Using alias removed (invalid transpilation)
#include "path.hpp"
// Using alias removed (invalid transpilation)

extern any storage;
extern any upload;
std::shared_ptr<Promise<object>> saveUploadedFile(std::shared_ptr<Express::Multer::File> file, string agentId);

std::shared_ptr<express::Router> createAgentMediaRouter();

#endif
