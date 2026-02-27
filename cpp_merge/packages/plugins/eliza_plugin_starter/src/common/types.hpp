#ifndef _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_ELIZA_PLUGIN_STARTER_SRC_COMMON_TYPES_H
#define _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_ELIZA_PLUGIN_STARTER_SRC_COMMON_TYPES_H
#include "core.hpp"
// External dependency removed

class SearchResult;
class SearchOptions;
class SearchProvider;
class SearchPluginConfig;
class SearchAction;
class SearchPlugin;

class SearchResult : public object, public std::enable_shared_from_this<SearchResult> {
public:
    using std::enable_shared_from_this<SearchResult>::shared_from_this;
    string title;

    string url;

    string snippet;

    double score;

    any source;

    Record<string, any> metadata;
};

class SearchOptions : public object, public std::enable_shared_from_this<SearchOptions> {
public:
    using std::enable_shared_from_this<SearchOptions>::shared_from_this;
    double maxResults;

    string searchType;

    Record<string, any> filters;
};

class SearchProvider : public object, public std::enable_shared_from_this<SearchProvider> {
public:
    using std::enable_shared_from_this<SearchProvider>::shared_from_this;
    virtual std::shared_ptr<Promise<array<std::shared_ptr<SearchResult>>>> search(string query, std::shared_ptr<SearchOptions> options = undefined) = 0;
};

class SearchPluginConfig : public object, public std::enable_shared_from_this<SearchPluginConfig> {
public:
    using std::enable_shared_from_this<SearchPluginConfig>::shared_from_this;
    string apiKey;

    double maxResults;

    string searchType;

    Record<string, any> filters;
};

class SearchAction : public Action, public std::enable_shared_from_this<SearchAction> {
public:
    using std::enable_shared_from_this<SearchAction>::shared_from_this;
    string name;

    string description;

    array<array<std::shared_ptr<ActionExample>>> examples;

    array<string> similes;

    std::shared_ptr<Handler> handler;

    std::shared_ptr<Validator> validate;
};

class SearchPlugin : public Plugin, public std::enable_shared_from_this<SearchPlugin> {
public:
    using std::enable_shared_from_this<SearchPlugin>::shared_from_this;
    string name;

    string description;

    array<std::shared_ptr<SearchAction>> actions;

    std::shared_ptr<SearchPluginConfig> config;
};

#endif
