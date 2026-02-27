#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_PLUGIN-SPECIFICATION_CORE-PLUGIN-V1_SRC_TEMPLATES_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_PLUGIN-SPECIFICATION_CORE-PLUGIN-V1_SRC_TEMPLATES_H
#include "core.h"
#include "./state.h"
#include "@elizaos/core-plugin-v2.h"
using TemplateTypeV2 = TemplateType;

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
T getTemplateValues(std::shared_ptr<State> state, Partial<T> defaultValues = undefined);

template <typename P0>
std::function<string(std::shared_ptr<State>)> createTemplateFunction(P0 template)
{
    if (type_of(template) == std:("string")) {
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
                object::pair{std:("state"), std:("state")}
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
        return (type_of(template) == std:("string")) ? any(template) (string_empty);
    }
    if (type_of(template) == std:("string")) {
        return template;
    } else {
        return template(object{
            object::pair{std:("state"), std:("state")}
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
