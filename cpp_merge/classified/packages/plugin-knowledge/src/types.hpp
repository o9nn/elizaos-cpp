#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_CLASSIFIED_PACKAGES_PLUGIN-KNOWLEDGE_SRC_TYPES_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_CLASSIFIED_PACKAGES_PLUGIN-KNOWLEDGE_SRC_TYPES_H
#include "core.h"
#include "@elizaos/core.h"
#include "zod.h"
using z = _default;

typedef z::infer<ModelConfigSchema> ModelConfig;

class ProviderRateLimits;
class TextGenerationOptions;
class AddKnowledgeOptions;
namespace @elizaos/core {
    class ServiceTypeRegistry;
}
class KnowledgeConfig;
class LoadResult;

extern any ModelConfigSchema;
class ProviderRateLimits : public object, public std::enable_shared_from_this<ProviderRateLimits> {
public:
    using std::enable_shared_from_this<ProviderRateLimits>::shared_from_this;
    double maxConcurrentRequests;

    double requestsPerMinute;

    double tokensPerMinute;

    string provider;
};

class TextGenerationOptions : public object, public std::enable_shared_from_this<TextGenerationOptions> {
public:
    using std::enable_shared_from_this<TextGenerationOptions>::shared_from_this;
    any provider;

    string modelName;

    double maxTokens;

    string cacheDocument;

    object cacheOptions;

    boolean autoCacheContextualRetrieval;
};

class AddKnowledgeOptions : public object, public std::enable_shared_from_this<AddKnowledgeOptions> {
public:
    using std::enable_shared_from_this<AddKnowledgeOptions>::shared_from_this;
    std::shared_ptr<UUID> agentId;

    std::shared_ptr<UUID> worldId;

    std::shared_ptr<UUID> roomId;

    std::shared_ptr<UUID> entityId;

    std::shared_ptr<UUID> clientDocumentId;

    string contentType;

    string originalFilename;

    string content;

    Record<string, any> metadata;
};

namespace std:("@elizaos/core") {
    class ServiceTypeRegistry : public object, public std::enable_shared_from_this<ServiceTypeRegistry> {
    public:
        using std::enable_shared_from_this<ServiceTypeRegistry>::shared_from_this;
        string KNOWLEDGE;
    };

}
extern object KnowledgeServiceType, satisfies, Partial;
class KnowledgeConfig : public object, public std::enable_shared_from_this<KnowledgeConfig> {
public:
    using std::enable_shared_from_this<KnowledgeConfig>::shared_from_this;
    boolean CTX_KNOWLEDGE_ENABLED;

    boolean LOAD_DOCS_ON_STARTUP;

    any MAX_INPUT_TOKENS;

    any MAX_OUTPUT_TOKENS;

    string EMBEDDING_PROVIDER;

    string TEXT_PROVIDER;

    string TEXT_EMBEDDING_MODEL;
};

class LoadResult : public object, public std::enable_shared_from_this<LoadResult> {
public:
    using std::enable_shared_from_this<LoadResult>::shared_from_this;
    double successful;

    double failed;

    array<object> errors;
};

namespace @elizaos/core {
}
#endif
