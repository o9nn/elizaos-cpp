#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_CLASSIFIED_PACKAGES_PLUGIN-KNOWLEDGE_SRC_DOCS-LOADER_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_CLASSIFIED_PACKAGES_PLUGIN-KNOWLEDGE_SRC_DOCS-LOADER_H
#include "core.h"
#include "@elizaos/core.h"
#include "fs.h"
#include "path.h"
#include "./service.ts.h"
#include "./types.ts.h"
#include "./utils.ts.h"

string getKnowledgePath();

std::shared_ptr<Promise<object>> loadDocsFromPath(std::shared_ptr<KnowledgeService> service, std::shared_ptr<UUID> agentId, std::shared_ptr<UUID> worldId = undefined);

array<string> getAllFiles(string dirPath, array<string> files = array<string>());

any getContentType(string extension);

#endif
