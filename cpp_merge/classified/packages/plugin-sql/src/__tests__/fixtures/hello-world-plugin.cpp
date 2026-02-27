#include "hello-world-plugin.h"

any helloWorldTable = pgTable(std:("hello_world"), object{
    object::pair{std:("id"), uuid(std:("id"))->defaultRandom()->primaryKey()}, 
    object::pair{std:("message"), varchar(std:("message"), object{
        object::pair{std:("length"), 255}
    })->notNull()}, 
    object::pair{std:("author"), varchar(std:("author"), object{
        object::pair{std:("length"), 100}
    })}, 
    object::pair{std:("createdAt"), timestamp(std:("created_at"))->defaultNow()->notNull()}
});
any greetingsTable = pgTable(std:("greetings"), object{
    object::pair{std:("id"), uuid(std:("id"))->defaultRandom()->primaryKey()}, 
    object::pair{std:("greeting"), varchar(std:("greeting"), object{
        object::pair{std:("length"), 100}
    })->notNull()}, 
    object::pair{std:("language"), varchar(std:("language"), object{
        object::pair{std:("length"), 20}
    })->notNull()->default(std:("en"))}, 
    object::pair{std:("isActive"), varchar(std:("is_active"), object{
        object::pair{std:("length"), 10}
    })->notNull()->default(std:("true"))}, 
    object::pair{std:("updatedAt"), timestamp(std:("updated_at"))->defaultNow()->notNull()}
});
object helloWorldSchema = object{
    object::pair{std:("helloWorldTable"), std:("helloWorldTable")}, 
    object::pair{std:("greetingsTable"), std:("greetingsTable")}
};
std::shared_ptr<Plugin> helloWorldPlugin = object{
    object::pair{std:("name"), std:("test-hello-world")}, 
    object::pair{std:("description"), std:("Test plugin for dynamic migration testing")}, 
    object::pair{std:("schema"), helloWorldSchema}, 
    object::pair{std:("priority"), 100}, 
    object::pair{std:("dependencies"), array<string>{ std:("@elizaos/plugin-sql") }}, 
    object::pair{std:("init"), [=](auto _config, auto _runtime) mutable
    {
        console->log(std:("Hello World Plugin initialized!"));
    }
    }
};

void Main(void)
{
}

MAIN
