#ifndef _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_ELIZA_PACKAGES_CORE_SRC_TYPES_KNOWLEDGE_H
#define _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_ELIZA_PACKAGES_CORE_SRC_TYPES_KNOWLEDGE_H
#include "core.hpp"
#include <string>
#include "./memory.h"
#include "./primitives.h"

typedef object KnowledgeItem;

enum struct KnowledgeScope;
enum struct CacheKeyPrefix;
class DirectoryItem;
class ChunkRow;

enum struct KnowledgeScope {
    SHARED = std::string("shared"), PRIVATE = std::string("private")
};
enum struct CacheKeyPrefix {
    KNOWLEDGE = std::string("knowledge")
};
class DirectoryItem : public object, public std::enable_shared_from_this<DirectoryItem> {
public:
    using std::enable_shared_from_this<DirectoryItem>::shared_from_this;
    string directory;

    boolean shared;
};

class ChunkRow : public object, public std::enable_shared_from_this<ChunkRow> {
public:
    using std::enable_shared_from_this<ChunkRow>::shared_from_this;
    string id;
};

#endif
