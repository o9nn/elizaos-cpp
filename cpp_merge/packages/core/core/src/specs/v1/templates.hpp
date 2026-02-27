#ifndef _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_ELIZA_PACKAGES_CORE_SRC_SPECS_V1_TEMPLATES_H
#define _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_ELIZA_PACKAGES_CORE_SRC_SPECS_V1_TEMPLATES_H
#include "core.hpp"
#include <string>
#include "./state.h"

typedef any TemplateType;

class TemplateValues;

class TemplateValues : public object, public std::enable_shared_from_this<TemplateValues> {
public:
    using std::enable_shared_from_this<TemplateValues>::shared_from_this;
};

template <typename P0>
std::function<string(std::shared_ptr<State>)> createTemplateFunction(P0 template);

template <typename P0>
string processTemplate(P0 template, std::shared_ptr<State> state);

template <typename T>
T getTemplateValues(std::shared_ptr<State> state, Partial<T> defaultValues = std::nullopt);

template <typename P0>
std::function<string(std::shared_ptr<State>)> createTemplateFunction(P0 template)
{
    if (type_of(template) == std::string("string")) {
        return [=]() mutable
        {
            return template;
        };
    } else {
        return [=](auto state) mutable
        {
            if (!state) {
                return string_empty;
            }
            return template(object{
                object::pair{std::string("state"), std::string("state")}
            });
        };
    }
};


template <typename P0>
string processTemplate(P0 template, std::shared_ptr<State> state)
{
    if (!template) {
        return string_empty;
    }
    if (!state) {
        return (type_of(template) == std::string("string")) ? any(template) (string_empty);
    }
    if (type_of(template) == std::string("string")) {
        return template;
    } else {
        return template(object{
            object::pair{std::string("state"), std::string("state")}
        });
    }
};


template <typename T>
T getTemplateValues(std::shared_ptr<State> state, Partial<T> defaultValues)
{
    if (OR((!state), (!state->values))) {
        return as<T>((OR((defaultValues), (object{}))));
    }
    auto stateValues = as<Record<string, any>>(state->values);
    auto defaults = OR((defaultValues), ((as<Partial<T>>(object{}))));
    return as<T>(utils::assign(object{
    }, defaults, stateValues));
};


#endif
