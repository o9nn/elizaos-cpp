#include "/home/runner/work/elizaos-cpp/elizaos-cpp/eliza/packages/cli/src/commands/create/utils/selection.h"

std::shared_ptr<Promise<array<string>>> getLocalAvailableDatabases()
{
    return array<string>{ std::string("pglite"), std::string("postgres") };
};


array<std::shared_ptr<AIModelOption>> getAvailableAIModels()
{
    return array<object>{ object{
        object::pair{std::string("title"), std::string("Local AI")}, 
        object::pair{std::string("value"), std::string("local")}, 
        object::pair{std::string("description"), std::string("Local models, no API required")}
    }, object{
        object::pair{std::string("title"), std::string("OpenAI")}, 
        object::pair{std::string("value"), std::string("openai")}, 
        object::pair{std::string("description"), std::string("GPT-4 models")}
    }, object{
        object::pair{std::string("title"), std::string("Anthropic")}, 
        object::pair{std::string("value"), std::string("claude")}, 
        object::pair{std::string("description"), std::string("Claude models")}
    }, object{
        object::pair{std::string("title"), std::string("OpenRouter")}, 
        object::pair{std::string("value"), std::string("openrouter")}, 
        object::pair{std::string("description"), std::string("Access multiple AI models")}
    }, object{
        object::pair{std::string("title"), std::string("Ollama")}, 
        object::pair{std::string("value"), std::string("ollama")}, 
        object::pair{std::string("description"), std::string("Self-hosted models")}
    }, object{
        object::pair{std::string("title"), std::string("Google Generative AI")}, 
        object::pair{std::string("value"), std::string("google")}, 
        object::pair{std::string("description"), std::string("Gemini models")}
    } };
};


array<std::shared_ptr<DatabaseOption>> getAvailableDatabases()
{
    return array<object>{ object{
        object::pair{std::string("title"), std::string("Pglite (Pglite)")}, 
        object::pair{std::string("value"), std::string("pglite")}, 
        object::pair{std::string("description"), std::string("Local development")}
    }, object{
        object::pair{std::string("title"), std::string("PostgreSQL")}, 
        object::pair{std::string("value"), std::string("postgres")}, 
        object::pair{std::string("description"), std::string("Production database")}
    } };
};


std::shared_ptr<Promise<string>> selectDatabase()
{
    auto availableDatabases = getAvailableDatabases();
    auto database = std::async([=]() { clack->select(object{
        object::pair{std::string("message"), std::string("Which database would you like to use?")}, 
        object::pair{std::string("options"), availableDatabases->map([=](auto db) mutable
        {
            return (object{
                object::pair{std::string("label"), db->title}, 
                object::pair{std::string("value"), db->value}, 
                object::pair{std::string("hint"), db->description}
            });
        }
        )}, 
        object::pair{std::string("initialValue"), std::string("pglite")}
    }); });
    if (clack->isCancel(database)) {
        clack->cancel(std::string("Operation cancelled."));
        process->exit(0);
    }
    return as<string>(database);
};


std::shared_ptr<Promise<string>> selectAIModel()
{
    auto availableModels = getAvailableAIModels();
    auto aiModel = std::async([=]() { clack->select(object{
        object::pair{std::string("message"), std::string("Which AI model would you like to use?")}, 
        object::pair{std::string("options"), availableModels->map([=](auto model) mutable
        {
            return (object{
                object::pair{std::string("label"), model->title}, 
                object::pair{std::string("value"), model->value}, 
                object::pair{std::string("hint"), model->description}
            });
        }
        )}, 
        object::pair{std::string("initialValue"), std::string("local")}
    }); });
    if (clack->isCancel(aiModel)) {
        clack->cancel(std::string("Operation cancelled."));
        process->exit(0);
    }
    return as<string>(aiModel);
};


array<std::shared_ptr<AIModelOption>> getAvailableEmbeddingModels()
{
    return array<object>{ object{
        object::pair{std::string("title"), std::string("Local AI")}, 
        object::pair{std::string("value"), std::string("local")}, 
        object::pair{std::string("description"), std::string("Local embeddings, no API required")}
    }, object{
        object::pair{std::string("title"), std::string("OpenAI")}, 
        object::pair{std::string("value"), std::string("openai")}, 
        object::pair{std::string("description"), std::string("OpenAI text-embedding-ada-002")}
    }, object{
        object::pair{std::string("title"), std::string("Ollama")}, 
        object::pair{std::string("value"), std::string("ollama")}, 
        object::pair{std::string("description"), std::string("Self-hosted embedding models")}
    }, object{
        object::pair{std::string("title"), std::string("Google Generative AI")}, 
        object::pair{std::string("value"), std::string("google")}, 
        object::pair{std::string("description"), std::string("Google embedding models")}
    } };
};


std::shared_ptr<Promise<string>> selectEmbeddingModel()
{
    auto availableModels = getAvailableEmbeddingModels();
    auto embeddingModel = std::async([=]() { clack->select(object{
        object::pair{std::string("message"), std::string("Select an embedding model (required since your AI model doesn't support embeddings):")}, 
        object::pair{std::string("options"), availableModels->map([=](auto model) mutable
        {
            return (object{
                object::pair{std::string("label"), model->title}, 
                object::pair{std::string("value"), model->value}, 
                object::pair{std::string("hint"), model->description}
            });
        }
        )}, 
        object::pair{std::string("initialValue"), std::string("local")}
    }); });
    if (clack->isCancel(embeddingModel)) {
        clack->cancel(std::string("Operation cancelled."));
        process->exit(0);
    }
    return as<string>(embeddingModel);
};


