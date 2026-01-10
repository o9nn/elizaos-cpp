#include "/home/runner/work/elizaos-cpp/elizaos-cpp/classified/packages/plugin-forms/src/actions/update-form.h"

std::shared_ptr<Action> updateFormAction = object{
    object::pair{std::string("name"), std::string("UPDATE_FORM")}, 
    object::pair{std::string("similes"), array<string>{ std::string("FILL_FORM"), std::string("SUBMIT_FORM"), std::string("COMPLETE_FORM"), std::string("FORM_INPUT") }}, 
    object::pair{std::string("description"), std::string("Updates an active form with values extracted from the user message")}, 
    object::pair{std::string("validate"), [=](auto runtime, auto message, auto _state = undefined) mutable
    {
        auto formsService = runtime->getService<std::shared_ptr<FormsService>>(std::string("forms"));
        if (!formsService) {
            return false;
        }
        auto activeForms = std::async([=]() { formsService->listForms(std::string("active")); });
        if (activeForms->length == 0) {
            return false;
        }
        auto text = OR((message->content->text->toLowerCase()), (string_empty));
        auto containsFormInput = OR((OR((OR((OR((text["includes"](std::string("my name is"))), (text["includes"](std::string("i am"))))), (text["includes"](std::string("@"))))), (!!text["match"]((new RegExp(std::string("\d{2,"))))))), (text["length"] > 5));
        return containsFormInput;
    }
    }, 
    object::pair{std::string("handler"), [=](auto runtime, auto message, auto state = undefined, auto options = undefined, auto callback = undefined) mutable
    {
        auto formsService = runtime->getService<std::shared_ptr<FormsService>>(std::string("forms"));
        if (!formsService) {
            throw any(std::make_shared<Error>(std::string("Forms service not available")));
        }
        auto activeForms = std::async([=]() { formsService->listForms(std::string("active")); });
        if (activeForms->length == 0) {
            std::async([=]() { callback(object{
                object::pair{std::string("text"), std::string("No active forms to update.")}, 
                object::pair{std::string("actions"), array<any>()}
            }); });
            return object{
                object::pair{std::string("success"), false}, 
                object::pair{std::string("message"), std::string("No active forms")}
            };
        }
        any targetForm;
        shared specifiedFormId = OR((OR((options["formId"]), (state->data->activeFormId))), (state->values->activeFormId));
        if (specifiedFormId) {
            targetForm = activeForms->find([=](auto f) mutable
            {
                return f["id"] == specifiedFormId;
            }
            );
            if (!targetForm) {
                std::async([=]() { callback(object{
                    object::pair{std::string("text"), std::string("The specified form is no longer active.")}, 
                    object::pair{std::string("actions"), array<any>()}
                }); });
                return object{
                    object::pair{std::string("success"), false}, 
                    object::pair{std::string("message"), std::string("Form not found")}
                };
            }
        } else {
            targetForm = const_(activeForms)[0];
        }
        auto formId = as<std::shared_ptr<UUID>>(targetForm["id"]);
        logger->debug(std::string("Updating form ") + formId + std::string(" with user message"));
        auto result = std::async([=]() { formsService->updateForm(formId, message); });
        if (!result->success) {
            std::async([=]() { callback(object{
                object::pair{std::string("text"), OR((result->message), (std::string("Failed to update form.")))}, 
                object::pair{std::string("actions"), array<any>()}
            }); });
            return object{
                object::pair{std::string("success"), false}, 
                object::pair{std::string("error"), OR((result->message), (std::string("Failed to update form.")))}
            };
        }
        shared responseText = string_empty;
        if (AND((result->updatedFields), (result->updatedFields->length > 0))) {
            responseText += std::string("Updated ") + result->updatedFields->length + std::string(" field(s). ");
        }
        if (result->formCompleted) {
            responseText += std::string("Form completed successfully! ");
        } else if (result->stepCompleted) {
            responseText += OR((result->message), (string_empty));
        } else {
            responseText += OR((result->message), (string_empty));
        }
        auto form = result->form;
        if (AND((form), (form->status == std::string("active")))) {
            auto currentStep = const_(form->steps)[form->currentStepIndex];
            if (currentStep) {
                auto remainingRequired = currentStep->fields->filter([=](auto f) mutable
                {
                    return AND((!f["optional"]), ((OR((!f["value"]), (f["value"] == string_empty)))));
                }
                );
                if (remainingRequired->length > 0) {
                    responseText += std::string("\
\
Remaining required fields in current step:");
                    remainingRequired->forEach([=](auto field) mutable
                    {
                        responseText += std::string("\
- ") + field["label"] + string_empty + (field["description"]) ? any(std::string(": ") + field["description"] + string_empty) : any(string_empty) + string_empty;
                    }
                    );
                }
            }
        }
        std::async([=]() { callback(object{
            object::pair{std::string("text"), responseText->trim()}, 
            object::pair{std::string("actions"), array<string>{ std::string("UPDATE_FORM") }}, 
            object::pair{std::string("data"), object{
                object::pair{std::string("formId"), form->id}, 
                object::pair{std::string("formName"), form->name}, 
                object::pair{std::string("currentStep"), form->currentStepIndex}, 
                object::pair{std::string("totalSteps"), form->steps->length}, 
                object::pair{std::string("status"), form->status}, 
                object::pair{std::string("updatedFields"), result->updatedFields}
            }}
        }); });
        return object{
            object::pair{std::string("success"), true}, 
            object::pair{std::string("data"), object{
                object::pair{std::string("formId"), form->id}, 
                object::pair{std::string("updatedFields"), result->updatedFields}, 
                object::pair{std::string("formCompleted"), result->formCompleted}, 
                object::pair{std::string("stepCompleted"), result->stepCompleted}
            }}
        };
    }
    }, 
    object::pair{std::string("examples"), array<array<any>>{ array<object>{ object{
        object::pair{std::string("name"), std::string("user")}, 
        object::pair{std::string("content"), object{
            object::pair{std::string("text"), std::string("I need to fill out a contact form")}
        }}
    }, object{
        object::pair{std::string("name"), std::string("assistant")}, 
        object::pair{std::string("content"), object{
            object::pair{std::string("text"), std::string("I'll help you with the contact form. Please provide your name to get started.")}, 
            object::pair{std::string("actions"), array<string>{ std::string("CREATE_FORM") }}
        }}
    }, object{
        object::pair{std::string("name"), std::string("user")}, 
        object::pair{std::string("content"), object{
            object::pair{std::string("text"), std::string("My name is John Smith")}
        }}
    }, object{
        object::pair{std::string("name"), std::string("assistant")}, 
        object::pair{std::string("content"), object{
            object::pair{std::string("text"), std::string("Thank you, John Smith. I've recorded your name. Now, please provide your email address.")}, 
            object::pair{std::string("actions"), array<string>{ std::string("UPDATE_FORM") }}
        }}
    } }, array<object>{ object{
        object::pair{std::string("name"), std::string("user")}, 
        object::pair{std::string("content"), object{
            object::pair{std::string("text"), std::string("john.smith@example.com")}
        }}
    }, object{
        object::pair{std::string("name"), std::string("assistant")}, 
        object::pair{std::string("content"), object{
            object::pair{std::string("text"), std::string("Perfect! I've recorded your email as john.smith@example.com. The last field is optional - would you like to include a message?")}, 
            object::pair{std::string("actions"), array<string>{ std::string("UPDATE_FORM") }}
        }}
    } } }}
};

void Main(void)
{
}

MAIN
