#include "/home/runner/work/elizaos-cpp/elizaos-cpp/classified/packages/plugin-forms/src/actions/cancel-form.h"

std::shared_ptr<Action> cancelFormAction = object{
    object::pair{std::string("name"), std::string("CANCEL_FORM")}, 
    object::pair{std::string("similes"), array<string>{ std::string("ABORT_FORM"), std::string("STOP_FORM"), std::string("QUIT_FORM"), std::string("EXIT_FORM") }}, 
    object::pair{std::string("description"), std::string("Cancels an active form")}, 
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
        auto wantsCancel = OR((OR((OR((OR((OR((OR((OR((text["includes"](std::string("cancel"))), (text["includes"](std::string("stop"))))), (text["includes"](std::string("abort"))))), (text["includes"](std::string("quit"))))), (text["includes"](std::string("exit"))))), (text["includes"](std::string("nevermind"))))), (text["includes"](std::string("never mind"))))), ((AND((text["includes"](std::string("don't"))), (text["includes"](std::string("want")))))));
        return wantsCancel;
    }
    }, 
    object::pair{std::string("handler"), [=](auto runtime, auto _message, auto state = undefined, auto options = undefined, auto callback = undefined) mutable
    {
        auto formsService = runtime->getService<std::shared_ptr<FormsService>>(std::string("forms"));
        if (!formsService) {
            throw any(std::make_shared<Error>(std::string("Forms service not available")));
        }
        auto activeForms = std::async([=]() { formsService->listForms(std::string("active")); });
        if (activeForms->length == 0) {
            std::async([=]() { callback(object{
                object::pair{std::string("text"), std::string("No active forms to cancel.")}, 
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
        auto formId = targetForm["id"];
        logger->debug(std::string("Cancelling form ") + formId + string_empty);
        auto success = std::async([=]() { formsService->cancelForm(formId); });
        if (success) {
            std::async([=]() { callback(object{
                object::pair{std::string("text"), std::string("I've cancelled the form. Is there anything else I can help you with?")}, 
                object::pair{std::string("actions"), array<string>{ std::string("CANCEL_FORM") }}, 
                object::pair{std::string("data"), object{
                    object::pair{std::string("formId"), std::string("formId")}
                }}
            }); });
            return object{
                object::pair{std::string("success"), true}, 
                object::pair{std::string("data"), object{
                    object::pair{std::string("formId"), std::string("formId")}
                }}
            };
        } else {
            std::async([=]() { callback(object{
                object::pair{std::string("text"), std::string("I was unable to cancel the form. It may have already been completed or cancelled.")}, 
                object::pair{std::string("actions"), array<any>()}
            }); });
            return object{
                object::pair{std::string("success"), false}, 
                object::pair{std::string("message"), std::string("Failed to cancel form")}
            };
        }
    }
    }, 
    object::pair{std::string("examples"), as<std::shared_ptr<const>>(array<std::shared_ptr<const>>{ array<object>{ object{
        object::pair{std::string("name"), std::string("user")}, 
        object::pair{std::string("content"), object{
            object::pair{std::string("text"), std::string("Actually, cancel the form")}
        }}
    }, object{
        object::pair{std::string("name"), std::string("assistant")}, 
        object::pair{std::string("content"), object{
            object::pair{std::string("text"), std::string("I've cancelled the contact form. Is there anything else I can help you with?")}, 
            object::pair{std::string("actions"), array<string>{ std::string("CANCEL_FORM") }}
        }}
    } }, array<object>{ object{
        object::pair{std::string("name"), std::string("user")}, 
        object::pair{std::string("content"), object{
            object::pair{std::string("text"), std::string("Never mind, I don't want to fill this out")}
        }}
    }, object{
        object::pair{std::string("name"), std::string("assistant")}, 
        object::pair{std::string("content"), object{
            object::pair{std::string("text"), std::string("I've cancelled the form. Is there anything else I can help you with?")}, 
            object::pair{std::string("actions"), array<string>{ std::string("CANCEL_FORM") }}
        }}
    } } })}
};

void Main(void)
{
}

MAIN
