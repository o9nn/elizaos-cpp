#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_ELIZAS-LIST_SRC_TYPES_PROJECT_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_ELIZAS-LIST_SRC_TYPES_PROJECT_H
#include "core.h"

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
