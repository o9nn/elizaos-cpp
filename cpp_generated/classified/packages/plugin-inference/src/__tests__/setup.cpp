#include "/home/runner/work/elizaos-cpp/elizaos-cpp/classified/packages/plugin-inference/src/__tests__/setup.h"

void Main(void)
{
    vi->mock(std::string("@elizaos/plugin-openai"), [=]() mutable
    {
        return (object{
            object::pair{std::string("openaiPlugin"), object{
                object::pair{std::string("name"), std::string("openai")}, 
                object::pair{std::string("description"), std::string("OpenAI plugin")}, 
                , 
                , 
                object::pair{std::string("models"), object{
                    object::pair{ModelType->TEXT_SMALL, vi->fn()->mockResolvedValue(std::string("OpenAI response"))}, 
                    object::pair{ModelType->TEXT_LARGE, vi->fn()->mockResolvedValue(std::string("OpenAI response"))}, 
                    object::pair{ModelType->TEXT_EMBEDDING, vi->fn()->mockResolvedValue(array<double>{ 0.1, 0.2, 0.3 })}
                }}
            }}
        });
    }
    );
    vi->mock(std::string("@elizaos/plugin-anthropic"), [=]() mutable
    {
        return (object{
            object::pair{std::string("anthropicPlugin"), object{
                object::pair{std::string("name"), std::string("anthropic")}, 
                object::pair{std::string("description"), std::string("Anthropic plugin")}, 
                , 
                , 
                object::pair{std::string("models"), object{
                    object::pair{ModelType->TEXT_SMALL, vi->fn()->mockResolvedValue(std::string("Anthropic response"))}, 
                    object::pair{ModelType->TEXT_LARGE, vi->fn()->mockResolvedValue(std::string("Anthropic response"))}
                }}
            }}
        });
    }
    );
    vi->mock(std::string("@elizaos/plugin-groq"), [=]() mutable
    {
        return (object{
            object::pair{std::string("groqPlugin"), object{
                object::pair{std::string("name"), std::string("groq")}, 
                object::pair{std::string("description"), std::string("Groq plugin")}, 
                , 
                , 
                object::pair{std::string("models"), object{
                    object::pair{ModelType->TEXT_SMALL, vi->fn()->mockResolvedValue(std::string("Groq response"))}, 
                    object::pair{ModelType->TEXT_LARGE, vi->fn()->mockResolvedValue(std::string("Groq response"))}
                }}
            }}
        });
    }
    );
    vi->mock(std::string("@elizaos/plugin-ollama"), [=]() mutable
    {
        return (object{
            object::pair{std::string("ollamaPlugin"), object{
                object::pair{std::string("name"), std::string("ollama")}, 
                object::pair{std::string("description"), std::string("Ollama plugin")}, 
                , 
                , 
                object::pair{std::string("models"), object{
                    object::pair{ModelType->TEXT_SMALL, vi->fn()->mockImplementation([=](auto runtime) mutable
                    {
                        auto available = runtime->getSetting(std::string("OLLAMA_AVAILABLE"));
                        if (available == std::string("false")) {
                            throw any(std::make_shared<Error>(std::string("Ollama is not available")));
                        }
                        return std::string("Ollama response");
                    }
                    )}, 
                    object::pair{ModelType->TEXT_LARGE, vi->fn()->mockImplementation([=](auto runtime) mutable
                    {
                        auto available = runtime->getSetting(std::string("OLLAMA_AVAILABLE"));
                        if (available == std::string("false")) {
                            throw any(std::make_shared<Error>(std::string("Ollama is not available")));
                        }
                        return std::string("Ollama response");
                    }
                    )}, 
                    object::pair{ModelType->TEXT_EMBEDDING, vi->fn()->mockImplementation([=](auto runtime) mutable
                    {
                        auto available = runtime->getSetting(std::string("OLLAMA_AVAILABLE"));
                        if (available == std::string("false")) {
                            throw any(std::make_shared<Error>(std::string("Ollama is not available")));
                        }
                        return array<double>{ 0.4, 0.5, 0.6 };
                    }
                    )}
                }}
            }}
        });
    }
    );
    vi->mock(std::string("@elizaos/plugin-elizaos-services"), [=]() mutable
    {
        return (object{
            object::pair{std::string("elizaOSServicesPlugin"), object{
                object::pair{std::string("name"), std::string("elizaos")}, 
                object::pair{std::string("description"), std::string("ElizaOS Services plugin")}, 
                , 
                , 
                object::pair{std::string("models"), object{
                    object::pair{ModelType->TEXT_SMALL, vi->fn()->mockResolvedValue(std::string("ElizaOS response"))}, 
                    object::pair{ModelType->TEXT_LARGE, vi->fn()->mockResolvedValue(std::string("ElizaOS response"))}
                }}
            }}
        });
    }
    );
    vi->mock(std::string("@elizaos/plugin-local-embedding"), [=]() mutable
    {
        return (object{
            object::pair{std::string("localEmbeddingPlugin"), object{
                object::pair{std::string("name"), std::string("local-embedding")}, 
                object::pair{std::string("description"), std::string("Local embedding plugin")}, 
                , 
                , 
                object::pair{std::string("models"), object{
                    object::pair{ModelType->TEXT_EMBEDDING, vi->fn()->mockResolvedValue(array<double>{ 0.1, 0.2, 0.3, 0.4, 0.5 })}
                }}
            }}
        });
    }
    );
    global->fetch = as<fetch>(as<any>(vi->fn()->mockImplementation([=]() mutable
    {
        return Promise->resolve(object{
            object::pair{std::string("ok"), false}, 
            object::pair{std::string("json"), [=]() mutable
            {
                return Promise->resolve(object{});
            }
            }
        });
    }
    )));
}

MAIN
