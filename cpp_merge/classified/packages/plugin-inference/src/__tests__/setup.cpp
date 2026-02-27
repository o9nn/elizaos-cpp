#include "setup.hpp"

void Main(void)
{
    vi->mock(std:("@elizaos/plugin-openai"), [=]() mutable
    {
        return (object{
            object::pair{std:("openaiPlugin"), object{
                object::pair{std:("name"), std:("openai")}, 
                object::pair{std:("description"), std:("OpenAI plugin")}, 
                , 
                , 
                object::pair{std:("models"), object{
                    object::pair{ModelType->TEXT_SMALL, vi->fn()->mockResolvedValue(std:("OpenAI response"))}, 
                    object::pair{ModelType->TEXT_LARGE, vi->fn()->mockResolvedValue(std:("OpenAI response"))}, 
                    object::pair{ModelType->TEXT_EMBEDDING, vi->fn()->mockResolvedValue(array<double>{ 0.1, 0.2, 0.3 })}
                }}
            }}
        });
    }
    );
    vi->mock(std:("@elizaos/plugin-anthropic"), [=]() mutable
    {
        return (object{
            object::pair{std:("anthropicPlugin"), object{
                object::pair{std:("name"), std:("anthropic")}, 
                object::pair{std:("description"), std:("Anthropic plugin")}, 
                , 
                , 
                object::pair{std:("models"), object{
                    object::pair{ModelType->TEXT_SMALL, vi->fn()->mockResolvedValue(std:("Anthropic response"))}, 
                    object::pair{ModelType->TEXT_LARGE, vi->fn()->mockResolvedValue(std:("Anthropic response"))}
                }}
            }}
        });
    }
    );
    vi->mock(std:("@elizaos/plugin-groq"), [=]() mutable
    {
        return (object{
            object::pair{std:("groqPlugin"), object{
                object::pair{std:("name"), std:("groq")}, 
                object::pair{std:("description"), std:("Groq plugin")}, 
                , 
                , 
                object::pair{std:("models"), object{
                    object::pair{ModelType->TEXT_SMALL, vi->fn()->mockResolvedValue(std:("Groq response"))}, 
                    object::pair{ModelType->TEXT_LARGE, vi->fn()->mockResolvedValue(std:("Groq response"))}
                }}
            }}
        });
    }
    );
    vi->mock(std:("@elizaos/plugin-ollama"), [=]() mutable
    {
        return (object{
            object::pair{std:("ollamaPlugin"), object{
                object::pair{std:("name"), std:("ollama")}, 
                object::pair{std:("description"), std:("Ollama plugin")}, 
                , 
                , 
                object::pair{std:("models"), object{
                    object::pair{ModelType->TEXT_SMALL, vi->fn()->mockImplementation([=](auto runtime) mutable
                    {
                        auto available = runtime->getSetting(std:("OLLAMA_AVAILABLE"));
                        if (available == std:("false")) {
                            throw any(std::make_shared<Error>(std:("Ollama is not available")));
                        }
                        return std:("Ollama response");
                    }
                    )}, 
                    object::pair{ModelType->TEXT_LARGE, vi->fn()->mockImplementation([=](auto runtime) mutable
                    {
                        auto available = runtime->getSetting(std:("OLLAMA_AVAILABLE"));
                        if (available == std:("false")) {
                            throw any(std::make_shared<Error>(std:("Ollama is not available")));
                        }
                        return std:("Ollama response");
                    }
                    )}, 
                    object::pair{ModelType->TEXT_EMBEDDING, vi->fn()->mockImplementation([=](auto runtime) mutable
                    {
                        auto available = runtime->getSetting(std:("OLLAMA_AVAILABLE"));
                        if (available == std:("false")) {
                            throw any(std::make_shared<Error>(std:("Ollama is not available")));
                        }
                        return array<double>{ 0.4, 0.5, 0.6 };
                    }
                    )}
                }}
            }}
        });
    }
    );
    vi->mock(std:("@elizaos/plugin-elizaos-services"), [=]() mutable
    {
        return (object{
            object::pair{std:("elizaOSServicesPlugin"), object{
                object::pair{std:("name"), std:("elizaos")}, 
                object::pair{std:("description"), std:("ElizaOS Services plugin")}, 
                , 
                , 
                object::pair{std:("models"), object{
                    object::pair{ModelType->TEXT_SMALL, vi->fn()->mockResolvedValue(std:("ElizaOS response"))}, 
                    object::pair{ModelType->TEXT_LARGE, vi->fn()->mockResolvedValue(std:("ElizaOS response"))}
                }}
            }}
        });
    }
    );
    vi->mock(std:("@elizaos/plugin-local-embedding"), [=]() mutable
    {
        return (object{
            object::pair{std:("localEmbeddingPlugin"), object{
                object::pair{std:("name"), std:("local-embedding")}, 
                object::pair{std:("description"), std:("Local embedding plugin")}, 
                , 
                , 
                object::pair{std:("models"), object{
                    object::pair{ModelType->TEXT_EMBEDDING, vi->fn()->mockResolvedValue(array<double>{ 0.1, 0.2, 0.3, 0.4, 0.5 })}
                }}
            }}
        });
    }
    );
    global->fetch = as<fetch>(as<any>(vi->fn()->mockImplementation([=]() mutable
    {
        return Promise->resolve(object{
            object::pair{std:("ok"), false}, 
            object::pair{std:("json"), [=]() mutable
            {
                return Promise->resolve(object{});
            }
            }
        });
    }
    )));
}

MAIN
