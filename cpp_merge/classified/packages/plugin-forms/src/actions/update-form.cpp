#include "/home/runner/work/elizaos-cpp/elizaos-cpp/classified/packages/plugin-forms/src/actions/update-form.h"

std::shared_ptr<Action> updateFormAction = object{
    object::pair{std:("name"), std:("UPDATE_FORM")}, 
    object::pair{std:("similes"), array<string>{ std:("FILL_FORM"), std:("SUBMIT_FORM"), std:("COMPLETE_FORM"), std:("FORM_INPUT") }}, 
    object::pair{std:("description"), std:("Updates an active form with values extracted from the user message")}, 
    object::pair{std:("validate"), [=](auto runtime, auto message, auto _state = undefined) mutable
    {
        auto formsService = runtime->getService<std::shared_ptr<FormsService>>(std:("forms"));
        if (!formsService) {
            return false;
        }
        auto activeForms = std::async([=]() { formsService->listForms(std:("active")); });
        if (activeForms->length == 0) {
            return false;
        }
        auto text = OR((message->content->text->toLowerCase()), (string_empty));
        auto containsFormInput = OR((OR((OR((OR((text["includes"](std:("my name is"))), (text["includes"](std:("i am"))))), (text["includes"](std:("@"))))), (!!text["match"]((new RegExp(std:("\d{2,"))))))), (text["length"] > 5));
        return containsFormInput;
    }
    }, 
    object::pair{std:("handler"), [=](auto runtime, auto message, auto state = undefined, auto options = undefined, auto callback = undefined) mutable
    {
        auto formsService = runtime->getService<std::shared_ptr<FormsService>>(std:("forms"));
        if (!formsService) {
            throw any(std::make_shared<Error>(std:("Forms service not available")));
        }
        auto activeForms = std::async([=]() { formsService->listForms(std:("active")); });
        if (activeForms->length == 0) {
            std::async([=]() { callback(object{
                object::pair{std:("text"), std:("No active forms to update.")}, 
                object::pair{std:("actions"), array<any>()}
            }); });
            return object{
                object::pair{std:("success"), false}, 
                object::pair{std:("message"), std:("No active forms")}
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
                    object::pair{std:("text"), std:("The specified form is no longer active.")}, 
                    object::pair{std:("actions"), array<any>()}
                }); });
                return object{
                    object::pair{std:("success"), false}, 
                    object::pair{std:("message"), std:("Form not found")}
                };
            }
        } else {
            targetForm = const_(activeForms)[0];
        }
        auto formId = as<std::shared_ptr<UUID>>(targetForm["id"]);
        logger->debug(std:("Updating form ") + formId + std:(" with user message"));
        auto result = std::async([=]() { formsService->updateForm(formId, message); });
        if (!result->success) {
            std::async([=]() { callback(object{
                object::pair{std:("text"), OR((result->message), (std:("Failed to update form.")))}, 
                object::pair{std:("actions"), array<any>()}
            }); });
            return object{
                object::pair{std:("success"), false}, 
                object::pair{std:("error"), OR((result->message), (std:("Failed to update form.")))}
            };
        }
        shared responseText = string_empty;
        if (AND((result->updatedFields), (result->updatedFields->length > 0))) {
            responseText += std:("Updated ") + result->updatedFields->length + std:(" field(s). ");
        }
        if (result->formCompleted) {
            responseText += std:("Form completed successfully! ");
        } else if (result->stepCompleted) {
            responseText += OR((result->message), (string_empty));
        } else {
            responseText += OR((result->message), (string_empty));
        }
        auto form = result->form;
        if (AND((form), (form->status == std:("active")))) {
            auto currentStep = const_(form->steps)[form->currentStepIndex];
            if (currentStep) {
                auto remainingRequired = currentStep->fields->filter([=](auto f) mutable
                {
                    return AND((!f["optional"]), ((OR((!f["value"]), (f["value"] == string_empty)))));
                }
                );
                if (remainingRequired->length > 0) {
                    responseText += std:("\
\
Remaining required fields in current step:");
                    remainingRequired->forEach([=](auto field) mutable
                    {
                        responseText += std:("\
- ") + field["label"] + string_empty + (field["description"]) ? any(std:(": ") + field["description"] + string_empty) (string_empty) + string_empty;
                    }
                    );
                }
            }
        }
        std::async([=]() { callback(object{
            object::pair{std:("text"), responseText->trim()}, 
            object::pair{std:("actions"), array<string>{ std:("UPDATE_FORM") }}, 
            object::pair{std:("data"), object{
                object::pair{std:("formId"), form->id}, 
                object::pair{std:("formName"), form->name}, 
                object::pair{std:("currentStep"), form->currentStepIndex}, 
                object::pair{std:("totalSteps"), form->steps->length}, 
                object::pair{std:("status"), form->status}, 
                object::pair{std:("updatedFields"), result->updatedFields}
            }}
        }); });
        return object{
            object::pair{std:("success"), true}, 
            object::pair{std:("data"), object{
                object::pair{std:("formId"), form->id}, 
                object::pair{std:("updatedFields"), result->updatedFields}, 
                object::pair{std:("formCompleted"), result->formCompleted}, 
                object::pair{std:("stepCompleted"), result->stepCompleted}
            }}
        };
    }
    }, 
    object::pair{std:("examples"), array<array<any>>{ array<object>{ object{
        object::pair{std:("name"), std:("user")}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), std:("I need to fill out a contact form")}
        }}
    }, object{
        object::pair{std:("name"), std:("assistant")}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), std:("I'll help you with the contact form. Please provide your name to get started.")}, 
            object::pair{std:("actions"), array<string>{ std:("CREATE_FORM") }}
        }}
    }, object{
        object::pair{std:("name"), std:("user")}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), std:("My name is John Smith")}
        }}
    }, object{
        object::pair{std:("name"), std:("assistant")}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), std:("Thank you, John Smith. I've recorded your name. Now, please provide your email address.")}, 
            object::pair{std:("actions"), array<string>{ std:("UPDATE_FORM") }}
        }}
    } }, array<object>{ object{
        object::pair{std:("name"), std:("user")}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), std:("john.smith@example.com")}
        }}
    }, object{
        object::pair{std:("name"), std:("assistant")}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), std:("Perfect! I've recorded your email as john.smith@example.com. The last field is optional - would you like to include a message?")}, 
            object::pair{std:("actions"), array<string>{ std:("UPDATE_FORM") }}
        }}
    } } }}
};

void Main(void)
{
}

MAIN
