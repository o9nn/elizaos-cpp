#ifndef _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_CLASSIFIED_PACKAGES_CORE_SRC_TYPES_KNOWLEDGE_H
#define _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_CLASSIFIED_PACKAGES_CORE_SRC_TYPES_KNOWLEDGE_H
#include "core.hpp"
#include <string>
#include "./memory.h"
#include "./primitives.h"

enum struct KnowledgeScope;
enum struct CacheKeyPrefix;
class DirectoryItem;
class ChunkRow;
class KnowledgeItem;
class IKnowledgeService;

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

class KnowledgeItem : public Memory, public std::enable_shared_from_this<KnowledgeItem> {
public:
    using std::enable_shared_from_this<KnowledgeItem>::shared_from_this;
    std::shared_ptr<UUID> id;

    std::shared_ptr<Content> content;

    std::shared_ptr<DocumentMetadata> metadata;

    double createdAt;

    std::shared_ptr<UUID> agentId;

    std::shared_ptr<UUID> entityId;

    std::shared_ptr<UUID> roomId;

    std::shared_ptr<UUID> worldId;

    array<double> embedding;

    double similarity;
};

class IKnowledgeService : public object, public std::enable_shared_from_this<IKnowledgeService> {
public:
    using std::enable_shared_from_this<IKnowledgeService>::shared_from_this;
    virtual std::shared_ptr<Promise<array<std::shared_ptr<KnowledgeItem>>>> getMemories(object params) = 0;
    virtual std::shared_ptr<Promise<void>> deleteMemory(std::shared_ptr<UUID> memoryId) = 0;
    virtual std::shared_ptr<Promise<object>> addKnowledge(object options) = 0;
    virtual std::shared_ptr<Promise<array<std::shared_ptr<KnowledgeItem>>>> searchKnowledge(object params) = 0;
};

#endif
