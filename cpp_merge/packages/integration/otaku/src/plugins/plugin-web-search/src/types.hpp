#ifndef _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_OTAKU_SRC_PLUGINS_PLUGIN_WEB_SEARCH_SRC_TYPES_H
#define _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_OTAKU_SRC_PLUGINS_PLUGIN_WEB_SEARCH_SRC_TYPES_H
#include "core.hpp"
// External dependency removed

typedef object SearchResult;

typedef object SearchImage;

typedef object SearchResponse;

class ITavilyService;
class SearchOptions;

class ITavilyService : public Service, public std::enable_shared_from_this<ITavilyService> {
public:
    using std::enable_shared_from_this<ITavilyService>::shared_from_this;
    virtual std::shared_ptr<Promise<SearchResponse>> search(string query, std::shared_ptr<SearchOptions> options = undefined) = 0;
};

class SearchOptions : public object, public std::enable_shared_from_this<SearchOptions> {
public:
    using std::enable_shared_from_this<SearchOptions>::shared_from_this;
    boolean auto_parameters;

    any topic;

    any search_depth;

    double chunks_per_source;

    double max_results;

    any time_range;

    string start_date;

    string end_date;

    any include_answer;

    any include_raw_content;

    boolean include_images;

    boolean include_image_descriptions;

    boolean include_favicon;

    array<string> include_domains;

    array<string> exclude_domains;

    string country;
};

#endif
