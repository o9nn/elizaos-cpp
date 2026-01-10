#include "/home/runner/work/elizaos-cpp/elizaos-cpp/eliza/packages/plugin-sql/src/__tests__/fixtures/hello-world-plugin.h"

any helloWorldTable = pgTable(std::string("hello_world"), object{
    object::pair{std::string("id"), uuid(std::string("id"))->defaultRandom()->primaryKey()}, 
    object::pair{std::string("message"), varchar(std::string("message"), object{
        object::pair{std::string("length"), 255}
    })->notNull()}, 
    object::pair{std::string("author"), varchar(std::string("author"), object{
        object::pair{std::string("length"), 100}
    })}, 
    object::pair{std::string("createdAt"), timestamp(std::string("created_at"))->defaultNow()->notNull()}
});
any greetingsTable = pgTable(std::string("greetings"), object{
    object::pair{std::string("id"), uuid(std::string("id"))->defaultRandom()->primaryKey()}, 
    object::pair{std::string("greeting"), varchar(std::string("greeting"), object{
        object::pair{std::string("length"), 100}
    })->notNull()}, 
    object::pair{std::string("language"), varchar(std::string("language"), object{
        object::pair{std::string("length"), 20}
    })->notNull()->default(std::string("en"))}, 
    object::pair{std::string("isActive"), varchar(std::string("is_active"), object{
        object::pair{std::string("length"), 10}
    })->notNull()->default(std::string("true"))}, 
    object::pair{std::string("updatedAt"), timestamp(std::string("updated_at"))->defaultNow()->notNull()}
});
object helloWorldSchema = object{
    object::pair{std::string("helloWorldTable"), std::string("helloWorldTable")}, 
    object::pair{std::string("greetingsTable"), std::string("greetingsTable")}
};
std::shared_ptr<Plugin> helloWorldPlugin = object{
    object::pair{std::string("name"), std::string("test-hello-world")}, 
    object::pair{std::string("description"), std::string("Test plugin for dynamic migration testing")}, 
    object::pair{std::string("schema"), helloWorldSchema}, 
    object::pair{std::string("priority"), 100}, 
    object::pair{std::string("dependencies"), array<string>{ std::string("@elizaos/plugin-sql") }}, 
    object::pair{std::string("init"), [=](auto _config, auto _runtime) mutable
    {
        console->log(std::string("Hello World Plugin initialized!"));
    }
    }
};

void Main(void)
{
}

MAIN
