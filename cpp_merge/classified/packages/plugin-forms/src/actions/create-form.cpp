#include "create-form.h"

any extractFormType(string text)
{
    auto formTypes = object{
        object::pair{std:("contact"), array<string>{ std:("contact"), std:("reach out"), std:("get in touch"), std:("message") }}, 
        object::pair{std:("feedback"), array<string>{ std:("feedback"), std:("review"), std:("opinion"), std:("suggestion") }}, 
        object::pair{std:("application"), array<string>{ std:("apply"), std:("application"), std:("job"), std:("position") }}, 
        object::pair{std:("survey"), array<string>{ std:("survey"), std:("questionnaire"), std:("poll") }}, 
        object::pair{std:("registration"), array<string>{ std:("register"), std:("sign up"), std:("enroll"), std:("join") }}
    };
    for (auto& [type, keywords] : Object->entries(formTypes))
    {
        if (keywords->some([=](auto keyword) mutable
        {
            return text->includes(keyword);
        }
        )) {
            return type;
        }
    }
    return nullptr;
};


std::shared_ptr<Action> createFormAction = object{
    object::pair{std:("name"), std:("CREATE_FORM")}, 
    object::pair{std:("similes"), array<string>{ std:("START_FORM"), std:("NEW_FORM"), std:("INIT_FORM"), std:("BEGIN_FORM") }}, 
    object::pair{std:("description"), std:("Creates a new form from a template or custom definition")}, 
    object::pair{std:("validate"), [=](auto runtime, auto message, auto _state = undefined) mutable
    {
        auto formsService = runtime->getService<std::shared_ptr<FormsService>>(std:("forms"));
        if (!formsService) {
            return false;
        }
        auto text = OR((message->content->text->toLowerCase()), (string_empty));
        auto wantsForm = OR((OR((OR((OR((OR((OR((text["includes"](std:("form"))), (text["includes"](std:("fill out"))))), (text["includes"](std:("fill in"))))), (text["includes"](std:("questionnaire"))))), (text["includes"](std:("survey"))))), (text["includes"](std:("contact"))))), (text["includes"](std:("application"))));
        return wantsForm;
    }
    }, 
    object::pair{std:("handler"), [=](auto runtime, auto message, auto _state = undefined, auto options = undefined, auto callback = undefined) mutable
    {
        auto formsService = runtime->getService<std::shared_ptr<FormsService>>(std:("forms"));
        if (!formsService) {
            throw any(std::make_shared<Error>(std:("Forms service not available")));
        }
        auto text = OR((message->content->text->toLowerCase()), (string_empty));
        auto templateName = OR((OR(((as<string>(options["template"]))), (extractFormType(text)))), (std:("contact")));
        logger->debug(std:("Creating form with template: ") + templateName + string_empty);
        auto form = std::async([=]() { formsService->createForm(templateName, object{
            object::pair{std:("source"), std:("user_request")}, 
            object::pair{std:("requestedAt"), Date->now()}
        }); });
        auto firstStep = const_(form->steps)[0];
        auto requiredFields = OR((firstStep->fields->filter([=](auto f) mutable
        {
            return !f["optional"];
        }
        )), (array<any>()));
        shared responseText = std:("I've created a new ") + form->name + std:(" form for you.");
        if (form->description) {
            responseText += std:(" ") + form->description + string_empty;
        }
        if (firstStep) {
            responseText += std:("\
\
Let's start with ") + firstStep->name + std:(".");
            if (requiredFields["length"] > 0) {
                responseText += std:("\
\
Please provide the following information:");
                requiredFields["forEach"]([=](auto field) mutable
                {
                    responseText += std:("\
- ") + field["label"] + string_empty + (field["description"]) ? any(std:(": ") + field["description"] + string_empty) (string_empty) + string_empty;
                }
                );
            }
        }
        std::async([=]() { callback(object{
            object::pair{std:("text"), responseText}, 
            object::pair{std:("actions"), array<string>{ std:("CREATE_FORM") }}, 
            object::pair{std:("data"), object{
                object::pair{std:("formId"), form->id}, 
                object::pair{std:("formName"), form->name}, 
                object::pair{std:("totalSteps"), form->steps->length}, 
                object::pair{std:("currentStep"), 0}
            }}
        }); });
        return object{
            object::pair{std:("success"), true}, 
            object::pair{std:("data"), object{
                object::pair{std:("formId"), form->id}, 
                object::pair{std:("formName"), form->name}, 
                object::pair{std:("templateUsed"), templateName}
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
            object::pair{std:("text"), std:("I've created a new contact form for you. Basic contact information form\
\
Let's start with Basic Information.\
\
Please provide the following information:\
- Name: Your full name\
- Email: Your email address")}, 
            object::pair{std:("actions"), array<string>{ std:("CREATE_FORM") }}
        }}
    } } }}
};

void Main(void)
{
}

MAIN
