#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_SPARTAN_SRC_PLUGINS_PLUGIN-APP_SRC_FORMS_BASE_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_SPARTAN_SRC_PLUGINS_PLUGIN-APP_SRC_FORMS_BASE_H
#include "core.h"

class form;

class form : public object, public std::enable_shared_from_this<form> {
public:
    using std::enable_shared_from_this<form>::shared_from_this;
    std::any fields;

    form();
    virtual void addField(std::any type, std::any name, std::any value, std::any label, std::any validation, std::any options);
};

#endif
