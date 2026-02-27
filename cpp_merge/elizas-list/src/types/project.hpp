#ifndef _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_ELIZAS_LIST_SRC_TYPES_PROJECT_H
#define _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_ELIZAS_LIST_SRC_TYPES_PROJECT_H
#include "core.hpp"

class Project;
class Collection;

class Project : public object, public std::enable_shared_from_this<Project> {
public:
    using std::enable_shared_from_this<Project>::shared_from_this;
    string id;

    string name;

    string description;

    string projectUrl;

    string github;

    string image;

    object author;

    object donation;

    array<string> tags;

    string addedOn;

    object metrics;
};

class Collection : public object, public std::enable_shared_from_this<Collection> {
public:
    using std::enable_shared_from_this<Collection>::shared_from_this;
    string id;

    string name;

    string description;

    array<string> projects;

    object curator;

    boolean featured;
};

#endif
