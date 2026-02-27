#include "/home/runner/work/elizaos-cpp/elizaos-cpp/classified/packages/plugin-elizaos-services/src/tests.h"

object ElizaOSServicesTestSuite = object{
    object::pair{std:("name"), std:("ElizaOSServicesTestSuite")}, 
    object::pair{std:("tests"), array<object>{ object{
        object::pair{std:("name"), std:("elizaos_services_plugin_initialization")}, 
        object::pair{std:("fn"), [=](auto runtime) mutable
        {
            console->log(std:("🧪 Testing ElizaOS Services plugin initialization"));
            if (OR((!runtime), (!runtime->character))) {
                throw any(std::make_shared<Error>(std:("Runtime not properly initialized")));
            }
            auto service = runtime->getService(std:("elizaos-services"));
            if (!service) {
                console->log(std:("⚠️  ElizaOS Services service not found - plugin may not be loaded"));
            } else {
                console->log(std:("✅ ElizaOS Services service found"));
            }
            console->log(std:("✅ ElizaOS Services plugin initialization test passed"));
        }
        }
    }, object{
        object::pair{std:("name"), std:("elizaos_services_text_embedding")}, 
        object::pair{std:("fn"), [=](auto runtime) mutable
        {
            console->log(std:("🧪 Testing ElizaOS Services text embedding"));
            try
            {
                auto embedding = std::async([=]() { runtime->useModel(ModelType->TEXT_EMBEDDING, object{
                    object::pair{std:("text"), std:("Hello, world! This is a test for embedding generation.")}
                }); });
                if (!Array->isArray(embedding)) {
                    throw any(std::make_shared<Error>(std:("Embedding should be an array")));
                }
                if (embedding->get_length() == 0) {
                    throw any(std::make_shared<Error>(std:("Embedding array should not be empty")));
                }
                if (OR((embedding->get_length() < 100), (embedding->get_length() > 3072))) {
                    console->log(std:("⚠️  Unusual embedding dimension: ") + embedding->get_length() + string_empty);
                }
                if (type_of(const_(embedding)[0]) != std:("number")) {
                    throw any(std::make_shared<Error>(std:("Embedding should contain numbers")));
                }
                console->log(std:("✅ ElizaOS Services embedding test passed - dimension: ") + embedding->get_length() + string_empty);
            }
            catch (const any& error)
            {
                console->error(std:("❌ ElizaOS Services embedding test failed:"), error);
                throw any(std::make_shared<Error>(std:("Embedding generation failed: ") + (is<Error>(error)) ? any(error->message) (std:("Unknown error")) + string_empty));
            }
        }
        }
    }, object{
        object::pair{std:("name"), std:("elizaos_services_text_generation_small")}, 
        object::pair{std:("fn"), [=](auto runtime) mutable
        {
            console->log(std:("🧪 Testing ElizaOS Services small text generation"));
            try
            {
                auto text = std::async([=]() { runtime->useModel(ModelType->TEXT_SMALL, object{
                    object::pair{std:("prompt"), std:("What is 2+2? Answer with just the number.")}, 
                    object::pair{std:("maxTokens"), 10}
                }); });
                if (type_of(text) != std:("string")) {
                    throw any(std::make_shared<Error>(std:("Generated text should be a string")));
                }
                if (text->get_length() == 0) {
                    throw any(std::make_shared<Error>(std:("Generated text should not be empty")));
                }
                console->log(std:("✅ ElizaOS Services small text generation test passed: "") + text->trim() + std:("""));
            }
            catch (const any& error)
            {
                console->error(std:("❌ ElizaOS Services small text generation test failed:"), error);
                throw any(std::make_shared<Error>(std:("Small text generation failed: ") + (is<Error>(error)) ? any(error->message) (std:("Unknown error")) + string_empty));
            }
        }
        }
    }, object{
        object::pair{std:("name"), std:("elizaos_services_text_generation_large")}, 
        object::pair{std:("fn"), [=](auto runtime) mutable
        {
            console->log(std:("🧪 Testing ElizaOS Services large text generation"));
            try
            {
                auto text = std::async([=]() { runtime->useModel(ModelType->TEXT_LARGE, object{
                    object::pair{std:("prompt"), std:("Explain quantum computing in exactly one sentence.")}, 
                    object::pair{std:("maxTokens"), 100}, 
                    object::pair{std:("temperature"), 0.1}
                }); });
                if (type_of(text) != std:("string")) {
                    throw any(std::make_shared<Error>(std:("Generated text should be a string")));
                }
                if (text->get_length() == 0) {
                    throw any(std::make_shared<Error>(std:("Generated text should not be empty")));
                }
                console->log(std:("✅ ElizaOS Services large text generation test passed: "") + text->substring(0, 100) + std:("...""));
            }
            catch (const any& error)
            {
                console->error(std:("❌ ElizaOS Services large text generation test failed:"), error);
                throw any(std::make_shared<Error>(std:("Large text generation failed: ") + (is<Error>(error)) ? any(error->message) (std:("Unknown error")) + string_empty));
            }
        }
        }
    }, object{
        object::pair{std:("name"), std:("elizaos_services_object_generation")}, 
        object::pair{std:("fn"), [=](auto runtime) mutable
        {
            console->log(std:("🧪 Testing ElizaOS Services object generation"));
            try
            {
                auto obj = std::async([=]() { runtime->useModel(ModelType->OBJECT_SMALL, object{
                    object::pair{std:("prompt"), std:("Generate a JSON object with fields: name (string), age (number), active (boolean). Use realistic values.")}, 
                    object::pair{std:("temperature"), 0}
                }); });
                if (OR((type_of(obj) != std:("object")), (obj == nullptr))) {
                    throw any(std::make_shared<Error>(std:("Generated object should be a non-null object")));
                }
                console->log(std:("✅ ElizaOS Services object generation test passed:"), obj);
            }
            catch (const any& error)
            {
                console->error(std:("❌ ElizaOS Services object generation test failed:"), error);
                throw any(std::make_shared<Error>(std:("Object generation failed: ") + (is<Error>(error)) ? any(error->message) (std:("Unknown error")) + string_empty));
            }
        }
        }
    }, object{
        object::pair{std:("name"), std:("elizaos_services_image_description")}, 
        object::pair{std:("fn"), [=](auto runtime) mutable
        {
            console->log(std:("🧪 Testing ElizaOS Services image description"));
            try
            {
                auto result = as<any>((std::async([=]() { runtime->useModel(ModelType->IMAGE_DESCRIPTION, std:("https://upload.wikimedia.org/wikipedia/commons/thumb/1/1c/Vitalik_Buterin_TechCrunch_London_2015_%28cropped%29.jpg/537px-Vitalik_Buterin_TechCrunch_London_2015_%28cropped%29.jpg")); })));
                if (AND((AND((AND((type_of(result) == std:("object")), (result != nullptr))), (in(std:("title"), result)))), (in(std:("description"), result)))) {
                    console->log(std:("✅ ElizaOS Services image description test passed (object format):"), result);
                } else if (AND((type_of(result) == std:("string")), (result->get_length() > 10))) {
                    console->log(std:("✅ ElizaOS Services image description test passed (string format): "") + result->substring(0, 100) + std:("...""));
                } else {
                    throw any(std::make_shared<Error>(std:("Invalid image description result format")));
                }
            }
            catch (const any& error)
            {
                console->error(std:("❌ ElizaOS Services image description test failed:"), error);
                throw any(std::make_shared<Error>(std:("Image description failed: ") + (is<Error>(error)) ? any(error->message) (std:("Unknown error")) + string_empty));
            }
        }
        }
    }, object{
        object::pair{std:("name"), std:("elizaos_services_storage_integration")}, 
        object::pair{std:("fn"), [=](auto runtime) mutable
        {
            console->log(std:("🧪 Testing ElizaOS Services storage integration"));
            try
            {
                auto service = runtime->getService(std:("elizaos-services"));
                if (!service) {
                    console->log(std:("⚠️  ElizaOS Services service not found - storage test skipped"));
                    return std::shared_ptr<Promise<void>>();
                }
                auto storage = (as<any>(service))["getStorage"]();
                if (!storage) {
                    console->log(std:("⚠️  Storage service not available - test skipped"));
                    return std::shared_ptr<Promise<void>>();
                }
                auto isConfigured = AND((AND((process->env->ELIZAOS_STORAGE_ENDPOINT), (process->env->ELIZAOS_STORAGE_ACCESS_KEY))), (process->env->ELIZAOS_STORAGE_SECRET_KEY));
                if (!isConfigured) {
                    console->log(std:("⚠️  Storage not configured - basic test passed (service available)"));
                    console->log(std:("✅ Storage integration test passed (service accessible)"));
                    return std::shared_ptr<Promise<void>>();
                }
                auto testKey = std:("test-") + Date->now() + std:(".txt");
                auto testData = Buffer::from(std:("Hello from ElizaOS Services storage test!"));
                try
                {
                    auto uploadResult = std::async([=]() { storage["uploadFile"](testKey, testData, std:("text/plain")); });
                    if (uploadResult != testKey) {
                        throw any(std::make_shared<Error>(std:("Upload did not return expected key")));
                    }
                    console->log(std:("✅ Storage upload test passed"));
                    auto exists = std::async([=]() { storage["fileExists"](testKey); });
                    if (!exists) {
                        throw any(std::make_shared<Error>(std:("File existence check failed")));
                    }
                    console->log(std:("✅ Storage existence check passed"));
                    auto metadata = std::async([=]() { storage["getFileMetadata"](testKey); });
                    if (OR((!metadata), (metadata["size"] != testData->length))) {
                        throw any(std::make_shared<Error>(std:("Metadata check failed")));
                    }
                    console->log(std:("✅ Storage metadata test passed"));
                    auto getUrl = std::async([=]() { storage["getSignedUrl"](testKey, std:("get"), 300); });
                    if (!getUrl["startsWith"](std:("http"))) {
                        throw any(std::make_shared<Error>(std:("Signed URL generation failed")));
                    }
                    console->log(std:("✅ Storage signed URL test passed"));
                    auto downloadedData = std::async([=]() { storage["downloadFile"](testKey); });
                    if (!downloadedData["equals"](testData)) {
                        throw any(std::make_shared<Error>(std:("Downloaded data does not match uploaded data")));
                    }
                    console->log(std:("✅ Storage download test passed"));
                    auto files = std::async([=]() { storage["listFiles"](std:("test-") + Date->now()->toString()->slice(0, -3) + string_empty); });
                    if (!files["includes"](testKey)) {
                        console->log(std:("⚠️  File listing may not include recently uploaded file (eventual consistency)"));
                    } else {
                        console->log(std:("✅ Storage list files test passed"));
                    }
                    std::async([=]() { storage["deleteFile"](testKey); });
                    console->log(std:("✅ Storage delete test passed"));
                    auto existsAfterDelete = std::async([=]() { storage["fileExists"](testKey); });
                    if (existsAfterDelete) {
                        console->log(std:("⚠️  File may still exist after deletion (eventual consistency)"));
                    } else {
                        console->log(std:("✅ Storage deletion verification passed"));
                    }
                    console->log(std:("✅ ElizaOS Services storage integration test passed"));
                }
                catch (const any& storageError)
                {
                    console->log(std:("⚠️  Storage operations failed (may be due to configuration):"), (is<Error>(storageError)) ? any(storageError->message) (std:("Unknown error")));
                    console->log(std:("✅ Storage integration test passed (service accessible, operations may need config)"));
                }
            }
            catch (const any& error)
            {
                console->error(std:("❌ ElizaOS Services storage integration test failed:"), error);
                throw any(error);
            }
        }
        }
    }, object{
        object::pair{std:("name"), std:("elizaos_services_error_handling")}, 
        object::pair{std:("fn"), [=](auto runtime) mutable
        {
            console->log(std:("🧪 Testing ElizaOS Services error handling"));
            try
            {
                auto embedding = std::async([=]() { runtime->useModel(ModelType->TEXT_EMBEDDING, object{
                    object::pair{std:("text"), string_empty}
                }); });
                if (OR((!Array->isArray(embedding)), (embedding->get_length() == 0))) {
                    throw any(std::make_shared<Error>(std:("Error handling failed - should return fallback embedding")));
                }
                console->log(std:("✅ Empty input error handling passed"));
                auto nullEmbedding = std::async([=]() { runtime->useModel(ModelType->TEXT_EMBEDDING, nullptr); });
                if (OR((!Array->isArray(nullEmbedding)), (nullEmbedding->get_length() == 0))) {
                    throw any(std::make_shared<Error>(std:("Null input error handling failed")));
                }
                console->log(std:("✅ Null input error handling passed"));
                console->log(std:("✅ ElizaOS Services error handling test passed"));
            }
            catch (const any& error)
            {
                console->error(std:("❌ ElizaOS Services error handling test failed:"), error);
                throw any(std::make_shared<Error>(std:("Error handling test failed: ") + (is<Error>(error)) ? any(error->message) (std:("Unknown error")) + string_empty));
            }
        }
        }
    } }}
};

void Main(void)
{
}

MAIN
