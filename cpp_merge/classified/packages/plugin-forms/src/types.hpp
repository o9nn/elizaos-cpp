#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_CLASSIFIED_PACKAGES_PLUGIN-FORMS_SRC_TYPES_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_CLASSIFIED_PACKAGES_PLUGIN-FORMS_SRC_TYPES_H
#include "core.h"
#include "@elizaos/core.h"

typedef any FormFieldType;

typedef any FormStatus;

class FormField;
class FormStep;
class Form;
class FormTemplate;
class FormUpdateResult;
namespace @elizaos/core {
    class ServiceTypeRegistry;
}

class FormField : public object, public std::enable_shared_from_this<FormField> {
public:
    using std::enable_shared_from_this<FormField>::shared_from_this;
    string id;

    string label;

    FormFieldType type;

    string description;

    string criteria;

    boolean optional;

    boolean secret;

    any value;

    string error;

    Record<string, any> metadata;
};

class FormStep : public object, public std::enable_shared_from_this<FormStep> {
public:
    using std::enable_shared_from_this<FormStep>::shared_from_this;
    string id;

    string name;

    array<std::shared_ptr<FormField>> fields;

    boolean completed;

    std::function<std::shared_ptr<Promise<void>>(std::shared_ptr<Form>, string)> onComplete;
};

class Form : public object, public std::enable_shared_from_this<Form> {
public:
    using std::enable_shared_from_this<Form>::shared_from_this;
    std::shared_ptr<UUID> id;

    string name;

    string description;

    array<std::shared_ptr<FormStep>> steps;

    double currentStepIndex;

    FormStatus status;

    double createdAt;

    double updatedAt;

    double completedAt;

    std::shared_ptr<UUID> agentId;

    std::function<std::shared_ptr<Promise<void>>(std::shared_ptr<Form>)> onComplete;

    Record<string, any> metadata;
};

class FormTemplate : public object, public std::enable_shared_from_this<FormTemplate> {
public:
    using std::enable_shared_from_this<FormTemplate>::shared_from_this;
    string name;

    string description;

    array<std::shared_ptr<FormStep>> steps;

    Record<string, any> metadata;
};

class FormUpdateResult : public object, public std::enable_shared_from_this<FormUpdateResult> {
public:
    using std::enable_shared_from_this<FormUpdateResult>::shared_from_this;
    boolean success;

    std::shared_ptr<Form> form;

    array<string> updatedFields;

    array<object> errors;

    boolean stepCompleted;

    boolean formCompleted;

    string currentStep;

    string message;
};

namespace std:("@elizaos/core") {
    class ServiceTypeRegistry : public object, public std::enable_shared_from_this<ServiceTypeRegistry> {
    public:
        using std::enable_shared_from_this<ServiceTypeRegistry>::shared_from_this;
        string FORMS;
    };

}
extern object FormsServiceType, satisfies, Partial;
namespace @elizaos/core {
}
#endif
