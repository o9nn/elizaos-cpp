#include "/home/runner/work/elizaos-cpp/elizaos-cpp/classified/packages/plugin-forms/src/__tests__/e2e/forms-plugin.test.h"

void setupModelHandlers(std::shared_ptr<IAgentRuntime> runtime)
{
    console->log(std::string("Setting up model handlers..."));
    shared originalUseModel = runtime->useModel->bind(runtime);
    (as<any>(runtime))["useModel"] = [=](auto modelType, auto params) mutable
    {
        console->log(std::string("Mock useModel called:"), modelType, params);
        if (OR((modelType == ModelType->TEXT_SMALL), (modelType == std::string("TEXT_SMALL")))) {
            auto messages = OR((params["messages"]), (array<any>()));
            auto userMessageObj = messages["find"]([=](auto m) mutable
            {
                return m["role"] == std::string("user");
            }
            );
            auto userMessage = OR((userMessageObj["content"]), (string_empty));
            auto actualMessageMatch = userMessage["match"]((new RegExp(std::string("User message:\s*"([^"]+)"))));
            if (actualMessageMatch) {
                userMessage = const_(actualMessageMatch)[1];
            }
            console->log(std::string("Extracting from actual user message:"), userMessage);
            auto values = object{};
            auto nameMatch = userMessage["match"]((new RegExp(std::string("(?:name is|my name is|i'm|i am)\s+([A-Za-z\s]+?)(?:\s+and|\s*,|\s*$)"))));
            if (nameMatch) {
                values->name = const_(nameMatch)[1]["trim"]();
            }
            auto emailMatch = userMessage["match"]((new RegExp(std::string("(?:email is|my email is)\s+([^\s]+@[^\s]+)"))));
            if (emailMatch) {
                values->email = const_(emailMatch)[1];
            }
            if (OR((OR((userMessage["toLowerCase"]()["includes"](std::string("learn"))), (userMessage["toLowerCase"]()["includes"](std::string("services"))))), (userMessage["toLowerCase"]()["includes"](std::string("about"))))) {
                values->message = userMessage;
            }
            auto projectMatch = userMessage["match"]((new RegExp(std::string("(?:project name is|name is)\s+([A-Za-z0-9\s]+?)(?:\s+and|\s*,|\s*$)"))));
            if (projectMatch) {
                values->projectName = const_(projectMatch)[1]["trim"]();
            }
            auto apiKeyMatch = userMessage["match"]((new RegExp(std::string("(?:api key is|key is)\s+([^\s]+)"))));
            if (apiKeyMatch) {
                values->apiKey = const_(apiKeyMatch)[1];
            }
            auto endpointMatch = userMessage["match"]((new RegExp(std::string("(?:endpoint is)\s+(https?:\/\/[^\s]+)"))));
            if (endpointMatch) {
                values->endpoint = const_(endpointMatch)[1];
            }
            if (userMessage["toLowerCase"]()["includes"](std::string("typescript"))) {
                values->language = std::string("TypeScript");
            }
            auto frameworkMatch = userMessage["match"]((new RegExp(std::string("(?:use|with)\s+([A-Za-z.]+)\s+framework"))));
            if (frameworkMatch) {
                values->framework = const_(frameworkMatch)[1];
            }
            console->log(std::string("Extracted values:"), values);
            return JSON->stringify(values);
        }
        return originalUseModel(modelType, params);
    };
};


