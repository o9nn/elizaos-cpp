#ifndef _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_SPARTAN_SRC_PLUGINS_PLUGIN_APP_SRC_FORMS_BASE_H
#define _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_SPARTAN_SRC_PLUGINS_PLUGIN_APP_SRC_FORMS_BASE_H
#include "core.hpp"

class form;

class form : public object, public std::enable_shared_from_this<form> {
public:
    using std::enable_shared_from_this<form>::shared_from_this;
    any fields;

    form();
    virtual void addField(any type, any name, any value, any label, any validation, any options);
};

#endif
