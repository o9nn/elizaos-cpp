#ifndef _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_CLASSIFIED_PACKAGES_PLUGIN_KNOWLEDGE_SRC_DOCS_LOADER_H
#define _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_CLASSIFIED_PACKAGES_PLUGIN_KNOWLEDGE_SRC_DOCS_LOADER_H
#include "core.hpp"
// External dependency removed
#include "fs.hpp"
#include "path.hpp"
#include "./service.ts.h"
#include "./types.ts.h"
#include "./utils.ts.h"

string getKnowledgePath();

std::shared_ptr<Promise<object>> loadDocsFromPath(std::shared_ptr<KnowledgeService> service, std::shared_ptr<UUID> agentId, std::shared_ptr<UUID> worldId = undefined);

array<string> getAllFiles(string dirPath, array<string> files = array<string>());

any getContentType(string extension);

#endif
