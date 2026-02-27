#include "forms-provider.h"

std::shared_ptr<Provider> formsProvider = object{
    object::pair{std:("name"), std:("FORMS_CONTEXT")}, 
    object::pair{std:("description"), std:("Provides context about active forms and their current state")}, 
    object::pair{std:("dynamic"), true}, 
    object::pair{std:("position"), 50}, 
    object::pair{std:("get"), [=](auto runtime, auto _message, auto _state = undefined) mutable
    {
        auto formsService = runtime->getService<std::shared_ptr<FormsService>>(std:("forms"));
        if (!formsService) {
            return object{
                object::pair{std:("text"), std:("Forms service is not available.")}, 
                object::pair{std:("values"), object{}}, 
                object::pair{std:("data"), object{}}
            };
        }
        try
        {
            auto activeForms = std::async([=]() { formsService->listForms(std:("active")); });
            if (activeForms->length == 0) {
                return object{
                    object::pair{std:("text"), string_empty}, 
                    object::pair{std:("values"), object{}}, 
                    object::pair{std:("data"), object{}}
                };
            }
            shared contextText = std:("[FORMS]\
");
            auto formsData = array<std::shared_ptr<Form>>();
            auto& __array1179_2866 = activeForms;
            for (auto __indx1179_2866 = 0_N; __indx1179_2866 < __array1179_2866->get_length(); __indx1179_2866++)
            {
                auto& form = const_(__array1179_2866)[__indx1179_2866];
                {
                    auto currentStep = const_(form->steps)[form->currentStepIndex];
                    contextText += std:("\
Active Form: ") + form->name + std:(" (ID: ") + form->id + std:(")\
");
                    contextText += std:("Current Step: ") + (OR((currentStep->name), (currentStep->id))) + std:("\
");
                    auto completedFields = currentStep->fields->filter([=](auto f) mutable
                    {
                        return f["value"] != undefined;
                    }
                    );
                    if (completedFields->length > 0) {
                        contextText += std:("Completed fields:\
");
                        completedFields->forEach([=](auto field) mutable
                        {
                            auto displayValue = (field["secret"]) ? any(std:("[SECRET]")) (field["value"]);
                            contextText += std:("  - ") + field["label"] + std:(": ") + displayValue + std:("\
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
                        contextText += std:("Required fields:\
");
                        remainingRequired->forEach([=](auto field) mutable
                        {
                            contextText += std:("  - ") + field["label"] + string_empty + (field["description"]) ? any(std:(" (") + field["description"] + std:(")")) (string_empty) + std:("\
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
                        contextText += std:("Optional fields:\
");
                        optionalFields->forEach([=](auto field) mutable
                        {
                            contextText += std:("  - ") + field["label"] + string_empty + (field["description"]) ? any(std:(" (") + field["description"] + std:(")")) (string_empty) + std:("\
");
                        }
                        );
                    }
                    contextText += std:("Progress: Step ") + (form->currentStepIndex + 1) + std:(" of ") + form->steps->length + std:("\
");
                    formsData->push(form);
                }
            }
            return object{
                object::pair{std:("text"), contextText}, 
                object::pair{std:("values"), object{
                    object::pair{std:("activeFormsCount"), activeForms->length}
                }}, 
                object::pair{std:("data"), object{
                    object::pair{std:("forms"), formsData}
                }}
            };
        }
        catch (const any& error)
        {
            auto errorMessage = (is<Error>(error)) ? any(error->message) (std:("Unknown error"));
            return object{
                object::pair{std:("text"), std:("Error retrieving forms context: ") + errorMessage + string_empty}, 
                object::pair{std:("values"), object{}}, 
                object::pair{std:("data"), object{}}
            };
        }
    }
    }
};

void Main(void)
{
}

MAIN
