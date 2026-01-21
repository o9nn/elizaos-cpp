#include "/home/runner/work/elizaos-cpp/elizaos-cpp/classified/packages/plugin-forms/src/actions/create-form.h"

any extractFormType(string text)
{
    auto formTypes = object{
        object::pair{std::string("contact"), array<string>{ std::string("contact"), std::string("reach out"), std::string("get in touch"), std::string("message") }}, 
        object::pair{std::string("feedback"), array<string>{ std::string("feedback"), std::string("review"), std::string("opinion"), std::string("suggestion") }}, 
        object::pair{std::string("application"), array<string>{ std::string("apply"), std::string("application"), std::string("job"), std::string("position") }}, 
        object::pair{std::string("survey"), array<string>{ std::string("survey"), std::string("questionnaire"), std::string("poll") }}, 
        object::pair{std::string("registration"), array<string>{ std::string("register"), std::string("sign up"), std::string("enroll"), std::string("join") }}
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
    object::pair{std::string("name"), std::string("CREATE_FORM")}, 
    object::pair{std::string("similes"), array<string>{ std::string("START_FORM"), std::string("NEW_FORM"), std::string("INIT_FORM"), std::string("BEGIN_FORM") }}, 
    object::pair{std::string("description"), std::string("Creates a new form from a template or custom definition")}, 
    object::pair{std::string("validate"), [=](auto runtime, auto message, auto _state = undefined) mutable
    {
        auto formsService = runtime->getService<std::shared_ptr<FormsService>>(std::string("forms"));
        if (!formsService) {
            return false;
        }
        auto text = OR((message->content->text->toLowerCase()), (string_empty));
        auto wantsForm = OR((OR((OR((OR((OR((OR((text["includes"](std::string("form"))), (text["includes"](std::string("fill out"))))), (text["includes"](std::string("fill in"))))), (text["includes"](std::string("questionnaire"))))), (text["includes"](std::string("survey"))))), (text["includes"](std::string("contact"))))), (text["includes"](std::string("application"))));
        return wantsForm;
    }
    }, 
    object::pair{std::string("handler"), [=](auto runtime, auto message, auto _state = undefined, auto options = undefined, auto callback = undefined) mutable
    {
        auto formsService = runtime->getService<std::shared_ptr<FormsService>>(std::string("forms"));
        if (!formsService) {
            throw any(std::make_shared<Error>(std::string("Forms service not available")));
        }
        auto text = OR((message->content->text->toLowerCase()), (string_empty));
        auto templateName = OR((OR(((as<string>(options["template"]))), (extractFormType(text)))), (std::string("contact")));
        logger->debug(std::string("Creating form with template: ") + templateName + string_empty);
        auto form = std::async([=]() { formsService->createForm(templateName, object{
            object::pair{std::string("source"), std::string("user_request")}, 
            object::pair{std::string("requestedAt"), Date->now()}
        }); });
        auto firstStep = const_(form->steps)[0];
        auto requiredFields = OR((firstStep->fields->filter([=](auto f) mutable
        {
            return !f["optional"];
        }
        )), (array<any>()));
        shared responseText = std::string("I've created a new ") + form->name + std::string(" form for you.");
        if (form->description) {
            responseText += std::string(" ") + form->description + string_empty;
        }
        if (firstStep) {
            responseText += std::string("\
\
Let's start with ") + firstStep->name + std::string(".");
            if (requiredFields["length"] > 0) {
                responseText += std::string("\
\
Please provide the following information:");
                requiredFields["forEach"]([=](auto field) mutable
                {
                    responseText += std::string("\
- ") + field["label"] + string_empty + (field["description"]) ? any(std::string(": ") + field["description"] + string_empty) : any(string_empty) + string_empty;
                }
                );
            }
        }
        std::async([=]() { callback(object{
            object::pair{std::string("text"), responseText}, 
            object::pair{std::string("actions"), array<string>{ std::string("CREATE_FORM") }}, 
            object::pair{std::string("data"), object{
                object::pair{std::string("formId"), form->id}, 
                object::pair{std::string("formName"), form->name}, 
                object::pair{std::string("totalSteps"), form->steps->length}, 
                object::pair{std::string("currentStep"), 0}
            }}
        }); });
        return object{
            object::pair{std::string("success"), true}, 
            object::pair{std::string("data"), object{
                object::pair{std::string("formId"), form->id}, 
                object::pair{std::string("formName"), form->name}, 
                object::pair{std::string("templateUsed"), templateName}
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
            object::pair{std::string("text"), std::string("I've created a new contact form for you. Basic contact information form\
\
Let's start with Basic Information.\
\
Please provide the following information:\
- Name: Your full name\
- Email: Your email address")}, 
            object::pair{std::string("actions"), array<string>{ std::string("CREATE_FORM") }}
        }}
    } } }}
};

void Main(void)
{
}

MAIN
