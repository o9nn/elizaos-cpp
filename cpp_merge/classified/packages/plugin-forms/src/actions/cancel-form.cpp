#include "cancel-form.h"

std::shared_ptr<Action> cancelFormAction = object{
    object::pair{std:("name"), std:("CANCEL_FORM")}, 
    object::pair{std:("similes"), array<string>{ std:("ABORT_FORM"), std:("STOP_FORM"), std:("QUIT_FORM"), std:("EXIT_FORM") }}, 
    object::pair{std:("description"), std:("Cancels an active form")}, 
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
        auto wantsCancel = OR((OR((OR((OR((OR((OR((OR((text["includes"](std:("cancel"))), (text["includes"](std:("stop"))))), (text["includes"](std:("abort"))))), (text["includes"](std:("quit"))))), (text["includes"](std:("exit"))))), (text["includes"](std:("nevermind"))))), (text["includes"](std:("never mind"))))), ((AND((text["includes"](std:("don't"))), (text["includes"](std:("want")))))));
        return wantsCancel;
    }
    }, 
    object::pair{std:("handler"), [=](auto runtime, auto _message, auto state = undefined, auto options = undefined, auto callback = undefined) mutable
    {
        auto formsService = runtime->getService<std::shared_ptr<FormsService>>(std:("forms"));
        if (!formsService) {
            throw any(std::make_shared<Error>(std:("Forms service not available")));
        }
        auto activeForms = std::async([=]() { formsService->listForms(std:("active")); });
        if (activeForms->length == 0) {
            std::async([=]() { callback(object{
                object::pair{std:("text"), std:("No active forms to cancel.")}, 
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
        auto formId = targetForm["id"];
        logger->debug(std:("Cancelling form ") + formId + string_empty);
        auto success = std::async([=]() { formsService->cancelForm(formId); });
        if (success) {
            std::async([=]() { callback(object{
                object::pair{std:("text"), std:("I've cancelled the form. Is there anything else I can help you with?")}, 
                object::pair{std:("actions"), array<string>{ std:("CANCEL_FORM") }}, 
                object::pair{std:("data"), object{
                    object::pair{std:("formId"), std:("formId")}
                }}
            }); });
            return object{
                object::pair{std:("success"), true}, 
                object::pair{std:("data"), object{
                    object::pair{std:("formId"), std:("formId")}
                }}
            };
        } else {
            std::async([=]() { callback(object{
                object::pair{std:("text"), std:("I was unable to cancel the form. It may have already been completed or cancelled.")}, 
                object::pair{std:("actions"), array<any>()}
            }); });
            return object{
                object::pair{std:("success"), false}, 
                object::pair{std:("message"), std:("Failed to cancel form")}
            };
        }
    }
    }, 
    object::pair{std:("examples"), as<std::shared_ptr<const>>(array<std::shared_ptr<const>>{ array<object>{ object{
        object::pair{std:("name"), std:("user")}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), std:("Actually, cancel the form")}
        }}
    }, object{
        object::pair{std:("name"), std:("assistant")}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), std:("I've cancelled the contact form. Is there anything else I can help you with?")}, 
            object::pair{std:("actions"), array<string>{ std:("CANCEL_FORM") }}
        }}
    } }, array<object>{ object{
        object::pair{std:("name"), std:("user")}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), std:("Never mind, I don't want to fill this out")}
        }}
    }, object{
        object::pair{std:("name"), std:("assistant")}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), std:("I've cancelled the form. Is there anything else I can help you with?")}, 
            object::pair{std:("actions"), array<string>{ std:("CANCEL_FORM") }}
        }}
    } } })}
};

void Main(void)
{
}

MAIN
