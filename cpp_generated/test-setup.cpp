#include "/home/runner/work/elizaos-cpp/elizaos-cpp/classified/packages/agentserver/test-setup.h"

std::function<object(string)> createMockPlugin = [=](auto name) mutable
{
    return (object{
        object::pair{std::string("name"), std::string("name")}, 
        object::pair{std::string("description"), string_empty + name + std::string(" plugin mock")}, 
        object::pair{std::string("actions"), array<any>()}, 
        object::pair{std::string("providers"), array<any>()}, 
        object::pair{std::string("evaluators"), array<any>()}, 
        object::pair{std::string("services"), array<any>()}
    });
};

void Main(void)
{
    mock->module(std::string("@elizaos/core"), [=]() mutable
    {
        return (object{
            object::pair{std::string("logger"), object{
                object::pair{std::string("info"), [=]() mutable
                {
                }
                }, 
                object::pair{std::string("error"), [=]() mutable
                {
                }
                }, 
                object::pair{std::string("warn"), [=]() mutable
                {
                }
                }, 
                object::pair{std::string("debug"), [=]() mutable
                {
                }
                }
            }}, 
            object::pair{std::string("validateUuid"), [=](auto id) mutable
            {
                return (new RegExp(std::string("^[0-9a-f]{8}-[0-9a-f]{4}-[0-9a-f]{4}-[0-9a-f]{4}-[0-9a-f]{12}$")))->test(id);
            }
            }, 
            object::pair{std::string("stringToUuid"), [=](auto _str) mutable
            {
                return std::string("00000000-0000-0000-0000-000000000000");
            }
            }, 
            object::pair{std::string("ModelType"), object{
                object::pair{std::string("TEXT_LARGE"), std::string("text-large")}, 
                object::pair{std::string("TEXT_MEDIUM"), std::string("text-medium")}, 
                object::pair{std::string("TEXT_SMALL"), std::string("text-small")}
            }}, 
            object::pair{std::string("ChannelType"), object{
                object::pair{std::string("DISCORD"), std::string("discord")}, 
                object::pair{std::string("TWITTER"), std::string("twitter")}, 
                object::pair{std::string("TELEGRAM"), std::string("telegram")}
            }}
        });
    }
    );
    mock->module(std::string("@elizaos/plugin-sql"), [=]() mutable
    {
        return (object{
            object::pair{std::string("createDatabaseAdapter"), [=]() mutable
            {
                return (object{
                    object::pair{std::string("query"), [=]() mutable
                    {
                        return (object{
                            object::pair{std::string("rows"), array<any>()}
                        });
                    }
                    }, 
                    object::pair{std::string("init"), [=]() mutable
                    {
                    }
                    }
                });
            }
            }, 
            object::pair{std::string("schema"), object{
                object::pair{std::string("public"), object{
                    object::pair{std::string("Tables"), object{}}
                }}
            }}, 
            object::pair{std::string("plugin"), object{
                object::pair{std::string("name"), std::string("sql")}, 
                object::pair{std::string("description"), std::string("SQL plugin mock")}
            }}
        });
    }
    );
    mock->module(std::string("@elizaos/plugin-autonomy"), [=]() mutable
    {
        return (object{
            object::pair{std::string("autonomyPlugin"), createMockPlugin(std::string("autonomy"))}
        });
    }
    );
    mock->module(std::string("@elizaos/plugin-bootstrap"), [=]() mutable
    {
        return (object{
            object::pair{std::string("bootstrapPlugin"), createMockPlugin(std::string("bootstrap"))}
        });
    }
    );
    mock->module(std::string("@elizaos/plugin-goals"), [=]() mutable
    {
        return (object{
            object::pair{std::string("default"), createMockPlugin(std::string("goals"))}, 
            object::pair{std::string("GoalsPlugin"), createMockPlugin(std::string("goals"))}
        });
    }
    );
    mock->module(std::string("@elizaos/plugin-todo"), [=]() mutable
    {
        return (object{
            object::pair{std::string("default"), createMockPlugin(std::string("todo"))}, 
            object::pair{std::string("TodoPlugin"), createMockPlugin(std::string("todo"))}
        });
    }
    );
    mock->module(std::string("@elizaos/plugin-shell"), [=]() mutable
    {
        return (object{
            object::pair{std::string("default"), createMockPlugin(std::string("shell"))}
        });
    }
    );
    mock->module(std::string("@elizaos/plugin-knowledge"), [=]() mutable
    {
        return (object{
            object::pair{std::string("knowledgePlugin"), createMockPlugin(std::string("knowledge"))}
        });
    }
    );
    mock->module(std::string("@elizaos/plugin-inference"), [=]() mutable
    {
        return (object{
            object::pair{std::string("inferencePlugin"), createMockPlugin(std::string("inference"))}
        });
    }
    );
    mock->module(std::string("@elizaos/plugin-personality"), [=]() mutable
    {
        return (object{
            object::pair{std::string("default"), createMockPlugin(std::string("personality"))}
        });
    }
    );
    mock->module(std::string("@elizaos/plugin-experience"), [=]() mutable
    {
        return (object{
            object::pair{std::string("experiencePlugin"), createMockPlugin(std::string("experience"))}
        });
    }
    );
    mock->module(std::string("@elizaos/plugin-stagehand"), [=]() mutable
    {
        return (object{
            object::pair{std::string("stagehandPlugin"), createMockPlugin(std::string("stagehand"))}
        });
    }
    );
}

MAIN
