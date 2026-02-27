#ifndef _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_ELIZAS_LIST_SRC_TYPES_TAGS_H
#define _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_ELIZAS_LIST_SRC_TYPES_TAGS_H
#include "core.hpp"
#include "react-icons.h"

class TagMetadata;
class TagAnalytics;
class TagCategory;

class TagMetadata : public object, public std::enable_shared_from_this<TagMetadata> {
public:
    using std::enable_shared_from_this<TagMetadata>::shared_from_this;
    std::shared_ptr<IconType> icon;

    string color;

    boolean trending;

    string description;

    double popularity;

    string category;

    array<string> relatedTags;
};

class TagAnalytics : public object, public std::enable_shared_from_this<TagAnalytics> {
public:
    using std::enable_shared_from_this<TagAnalytics>::shared_from_this;
    double usageCount;

    double trendingScore;

    double projectCount;

    double recentGrowth;

    double averageStars;
};

class TagCategory : public object, public std::enable_shared_from_this<TagCategory> {
public:
    using std::enable_shared_from_this<TagCategory>::shared_from_this;
    string id;

    string name;

    string description;

    std::shared_ptr<IconType> icon;

    string color;

    array<string> tags;
};

#endif
