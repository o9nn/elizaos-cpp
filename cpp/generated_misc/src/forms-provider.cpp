#include "/home/runner/work/elizaos-cpp/elizaos-cpp/classified/packages/plugin-forms/src/providers/forms-provider.h"

std::shared_ptr<Provider> formsProvider = object{
    object::pair{std::string("name"), std::string("FORMS_CONTEXT")}, 
    object::pair{std::string("description"), std::string("Provides context about active forms and their current state")}, 
    object::pair{std::string("dynamic"), true}, 
    object::pair{std::string("position"), 50}, 
    object::pair{std::string("get"), [=](auto runtime, auto _message, auto _state = undefined) mutable
    {
        auto formsService = runtime->getService<std::shared_ptr<FormsService>>(std::string("forms"));
        if (!formsService) {
            return object{
                object::pair{std::string("text"), std::string("Forms service is not available.")}, 
                object::pair{std::string("values"), object{}}, 
                object::pair{std::string("data"), object{}}
            };
        }
        try
        {
            auto activeForms = std::async([=]() { formsService->listForms(std::string("active")); });
            if (activeForms->length == 0) {
                return object{
                    object::pair{std::string("text"), string_empty}, 
                    object::pair{std::string("values"), object{}}, 
                    object::pair{std::string("data"), object{}}
                };
            }
            shared contextText = std::string("[FORMS]\
");
            auto formsData = array<std::shared_ptr<Form>>();
            auto& __array1179_2866 = activeForms;
            for (auto __indx1179_2866 = 0_N; __indx1179_2866 < __array1179_2866->get_length(); __indx1179_2866++)
            {
                auto& form = const_(__array1179_2866)[__indx1179_2866];
                {
                    auto currentStep = const_(form->steps)[form->currentStepIndex];
                    contextText += std::string("\
Active Form: ") + form->name + std::string(" (ID: ") + form->id + std::string(")\
");
                    contextText += std::string("Current Step: ") + (OR((currentStep->name), (currentStep->id))) + std::string("\
");
                    auto completedFields = currentStep->fields->filter([=](auto f) mutable
                    {
                        return f["value"] != undefined;
                    }
                    );
                    if (completedFields->length > 0) {
                        contextText += std::string("Completed fields:\
");
                        completedFields->forEach([=](auto field) mutable
                        {
                            auto displayValue = (field["secret"]) ? any(std::string("[SECRET]")) : any(field["value"]);
                            contextText += std::string("  - ") + field["label"] + std::string(": ") + displayValue + std::string("\
");
                        }
                        );
                    }
                    auto remainingRequired = currentStep->fields->filter([=](auto f) mutable
                    {
                        return AND((!f["optional"]), (f["value"] == undefined));
                    }
                    );
                    if (remainingRequired->length > 0) {
                        contextText += std::string("Required fields:\
");
                        remainingRequired->forEach([=](auto field) mutable
                        {
                            contextText += std::string("  - ") + field["label"] + string_empty + (field["description"]) ? any(std::string(" (") + field["description"] + std::string(")")) : any(string_empty) + std::string("\
");
                        }
                        );
                    }
                    auto optionalFields = currentStep->fields->filter([=](auto f) mutable
                    {
                        return AND((f["optional"]), (f["value"] == undefined));
                    }
                    );
                    if (optionalFields->length > 0) {
                        contextText += std::string("Optional fields:\
");
                        optionalFields->forEach([=](auto field) mutable
                        {
                            contextText += std::string("  - ") + field["label"] + string_empty + (field["description"]) ? any(std::string(" (") + field["description"] + std::string(")")) : any(string_empty) + std::string("\
");
                        }
                        );
                    }
                    contextText += std::string("Progress: Step ") + (form->currentStepIndex + 1) + std::string(" of ") + form->steps->length + std::string("\
");
                    formsData->push(form);
                }
            }
            return object{
                object::pair{std::string("text"), contextText}, 
                object::pair{std::string("values"), object{
                    object::pair{std::string("activeFormsCount"), activeForms->length}
                }}, 
                object::pair{std::string("data"), object{
                    object::pair{std::string("forms"), formsData}
                }}
            };
        }
        catch (const any& error)
        {
            auto errorMessage = (is<Error>(error)) ? any(error->message) : any(std::string("Unknown error"));
            return object{
                object::pair{std::string("text"), std::string("Error retrieving forms context: ") + errorMessage + string_empty}, 
                object::pair{std::string("values"), object{}}, 
                object::pair{std::string("data"), object{}}
            };
        }
    }
    }
};

void Main(void)
{
}

MAIN
