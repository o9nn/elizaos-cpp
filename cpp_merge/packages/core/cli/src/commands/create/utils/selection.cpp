#include "selection.hpp"

std::shared_ptr<Promise<array<string>>> getLocalAvailableDatabases()
{
    return array<string>{ std:("pglite"), std:("postgres") };
};


array<std::shared_ptr<AIModelOption>> getAvailableAIModels()
{
    return array<object>{ object{
        object::pair{std:("title"), std:("Local AI")}, 
        object::pair{std:("value"), std:("local")}, 
        object::pair{std:("description"), std:("Local models, no API required")}
    }, object{
        object::pair{std:("title"), std:("OpenAI")}, 
        object::pair{std:("value"), std:("openai")}, 
        object::pair{std:("description"), std:("GPT-4 models")}
    }, object{
        object::pair{std:("title"), std:("Anthropic")}, 
        object::pair{std:("value"), std:("claude")}, 
        object::pair{std:("description"), std:("Claude models")}
    }, object{
        object::pair{std:("title"), std:("OpenRouter")}, 
        object::pair{std:("value"), std:("openrouter")}, 
        object::pair{std:("description"), std:("Access multiple AI models")}
    }, object{
        object::pair{std:("title"), std:("Ollama")}, 
        object::pair{std:("value"), std:("ollama")}, 
        object::pair{std:("description"), std:("Self-hosted models")}
    }, object{
        object::pair{std:("title"), std:("Google Generative AI")}, 
        object::pair{std:("value"), std:("google")}, 
        object::pair{std:("description"), std:("Gemini models")}
    } };
};


array<std::shared_ptr<DatabaseOption>> getAvailableDatabases()
{
    return array<object>{ object{
        object::pair{std:("title"), std:("Pglite (Pglite)")}, 
        object::pair{std:("value"), std:("pglite")}, 
        object::pair{std:("description"), std:("Local development")}
    }, object{
        object::pair{std:("title"), std:("PostgreSQL")}, 
        object::pair{std:("value"), std:("postgres")}, 
        object::pair{std:("description"), std:("Production database")}
    } };
};


std::shared_ptr<Promise<string>> selectDatabase()
{
    auto availableDatabases = getAvailableDatabases();
    auto database = std::async([=]() { clack->select(object{
        object::pair{std:("message"), std:("Which database would you like to use?")}, 
        object::pair{std:("options"), availableDatabases->map([=](auto db) mutable
        {
            return (object{
                object::pair{std:("label"), db->title}, 
                object::pair{std:("value"), db->value}, 
                object::pair{std:("hint"), db->description}
            });
        }
        )}, 
        object::pair{std:("initialValue"), std:("pglite")}
    }); });
    if (clack->isCancel(database)) {
        clack->cancel(std:("Operation cancelled."));
        process->exit(0);
    }
    return as<string>(database);
};


std::shared_ptr<Promise<string>> selectAIModel()
{
    auto availableModels = getAvailableAIModels();
    auto aiModel = std::async([=]() { clack->select(object{
        object::pair{std:("message"), std:("Which AI model would you like to use?")}, 
        object::pair{std:("options"), availableModels->map([=](auto model) mutable
        {
            return (object{
                object::pair{std:("label"), model->title}, 
                object::pair{std:("value"), model->value}, 
                object::pair{std:("hint"), model->description}
            });
        }
        )}, 
        object::pair{std:("initialValue"), std:("local")}
    }); });
    if (clack->isCancel(aiModel)) {
        clack->cancel(std:("Operation cancelled."));
        process->exit(0);
    }
    return as<string>(aiModel);
};


array<std::shared_ptr<AIModelOption>> getAvailableEmbeddingModels()
{
    return array<object>{ object{
        object::pair{std:("title"), std:("Local AI")}, 
        object::pair{std:("value"), std:("local")}, 
        object::pair{std:("description"), std:("Local embeddings, no API required")}
    }, object{
        object::pair{std:("title"), std:("OpenAI")}, 
        object::pair{std:("value"), std:("openai")}, 
        object::pair{std:("description"), std:("OpenAI text-embedding-ada-002")}
    }, object{
        object::pair{std:("title"), std:("Ollama")}, 
        object::pair{std:("value"), std:("ollama")}, 
        object::pair{std:("description"), std:("Self-hosted embedding models")}
    }, object{
        object::pair{std:("title"), std:("Google Generative AI")}, 
        object::pair{std:("value"), std:("google")}, 
        object::pair{std:("description"), std:("Google embedding models")}
    } };
};


std::shared_ptr<Promise<string>> selectEmbeddingModel()
{
    auto availableModels = getAvailableEmbeddingModels();
    auto embeddingModel = std::async([=]() { clack->select(object{
        object::pair{std:("message"), std:("Select an embedding model (required since your AI model doesn't support embeddings):")}, 
        object::pair{std:("options"), availableModels->map([=](auto model) mutable
        {
            return (object{
                object::pair{std:("label"), model->title}, 
                object::pair{std:("value"), model->value}, 
                object::pair{std:("hint"), model->description}
            });
        }
        )}, 
        object::pair{std:("initialValue"), std:("local")}
    }); });
    if (clack->isCancel(embeddingModel)) {
        clack->cancel(std:("Operation cancelled."));
        process->exit(0);
    }
    return as<string>(embeddingModel);
};


