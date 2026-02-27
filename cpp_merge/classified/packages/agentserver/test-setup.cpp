#include "test-setup.h"

std::function<object(string)> createMockPlugin = [=](auto name) mutable
{
    return (object{
        object::pair{std:("name"), std:("name")}, 
        object::pair{std:("description"), string_empty + name + std:(" plugin mock")}, 
        object::pair{std:("actions"), array<any>()}, 
        object::pair{std:("providers"), array<any>()}, 
        object::pair{std:("evaluators"), array<any>()}, 
        object::pair{std:("services"), array<any>()}
    });
};

void Main(void)
{
    mock->module(std:("@elizaos/core"), [=]() mutable
    {
        return (object{
            object::pair{std:("logger"), object{
                object::pair{std:("info"), [=]() mutable
                {
                }
                }, 
                object::pair{std:("error"), [=]() mutable
                {
                }
                }, 
                object::pair{std:("warn"), [=]() mutable
                {
                }
                }, 
                object::pair{std:("debug"), [=]() mutable
                {
                }
                }
            }}, 
            object::pair{std:("validateUuid"), [=](auto id) mutable
            {
                return (new RegExp(std:("^[0-9a-f]{8}-[0-9a-f]{4}-[0-9a-f]{4}-[0-9a-f]{4}-[0-9a-f]{12}$")))->test(id);
            }
            }, 
            object::pair{std:("stringToUuid"), [=](auto _str) mutable
            {
                return std:("00000000-0000-0000-0000-000000000000");
            }
            }, 
            object::pair{std:("ModelType"), object{
                object::pair{std:("TEXT_LARGE"), std:("text-large")}, 
                object::pair{std:("TEXT_MEDIUM"), std:("text-medium")}, 
                object::pair{std:("TEXT_SMALL"), std:("text-small")}
            }}, 
            object::pair{std:("ChannelType"), object{
                object::pair{std:("DISCORD"), std:("discord")}, 
                object::pair{std:("TWITTER"), std:("twitter")}, 
                object::pair{std:("TELEGRAM"), std:("telegram")}
            }}
        });
    }
    );
    mock->module(std:("@elizaos/plugin-sql"), [=]() mutable
    {
        return (object{
            object::pair{std:("createDatabaseAdapter"), [=]() mutable
            {
                return (object{
                    object::pair{std:("query"), [=]() mutable
                    {
                        return (object{
                            object::pair{std:("rows"), array<any>()}
                        });
                    }
                    }, 
                    object::pair{std:("init"), [=]() mutable
                    {
                    }
                    }
                });
            }
            }, 
            object::pair{std:("schema"), object{
                object::pair{std:("public"), object{
                    object::pair{std:("Tables"), object{}}
                }}
            }}, 
            object::pair{std:("plugin"), object{
                object::pair{std:("name"), std:("sql")}, 
                object::pair{std:("description"), std:("SQL plugin mock")}
            }}
        });
    }
    );
    mock->module(std:("@elizaos/plugin-autonomy"), [=]() mutable
    {
        return (object{
            object::pair{std:("autonomyPlugin"), createMockPlugin(std:("autonomy"))}
        });
    }
    );
    mock->module(std:("@elizaos/plugin-bootstrap"), [=]() mutable
    {
        return (object{
            object::pair{std:("bootstrapPlugin"), createMockPlugin(std:("bootstrap"))}
        });
    }
    );
    mock->module(std:("@elizaos/plugin-goals"), [=]() mutable
    {
        return (object{
            object::pair{std:("default"), createMockPlugin(std:("goals"))}, 
            object::pair{std:("GoalsPlugin"), createMockPlugin(std:("goals"))}
        });
    }
    );
    mock->module(std:("@elizaos/plugin-todo"), [=]() mutable
    {
        return (object{
            object::pair{std:("default"), createMockPlugin(std:("todo"))}, 
            object::pair{std:("TodoPlugin"), createMockPlugin(std:("todo"))}
        });
    }
    );
    mock->module(std:("@elizaos/plugin-shell"), [=]() mutable
    {
        return (object{
            object::pair{std:("default"), createMockPlugin(std:("shell"))}
        });
    }
    );
    mock->module(std:("@elizaos/plugin-knowledge"), [=]() mutable
    {
        return (object{
            object::pair{std:("knowledgePlugin"), createMockPlugin(std:("knowledge"))}
        });
    }
    );
    mock->module(std:("@elizaos/plugin-inference"), [=]() mutable
    {
        return (object{
            object::pair{std:("inferencePlugin"), createMockPlugin(std:("inference"))}
        });
    }
    );
    mock->module(std:("@elizaos/plugin-personality"), [=]() mutable
    {
        return (object{
            object::pair{std:("default"), createMockPlugin(std:("personality"))}
        });
    }
    );
    mock->module(std:("@elizaos/plugin-experience"), [=]() mutable
    {
        return (object{
            object::pair{std:("experiencePlugin"), createMockPlugin(std:("experience"))}
        });
    }
    );
    mock->module(std:("@elizaos/plugin-stagehand"), [=]() mutable
    {
        return (object{
            object::pair{std:("stagehandPlugin"), createMockPlugin(std:("stagehand"))}
        });
    }
    );
}

MAIN
