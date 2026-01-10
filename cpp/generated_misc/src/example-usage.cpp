#include "/home/runner/work/elizaos-cpp/elizaos-cpp/autonomous-starter/src/plugin-dynamic/examples/example-usage.h"

void createWeatherPlugin(std::shared_ptr<Agent> agent)
{
    auto weatherPluginSpec = object{
        object::pair{std::string("name"), std::string("@elizaos/plugin-weather")}, 
        object::pair{std::string("description"), std::string("Weather information and forecasting plugin")}, 
        object::pair{std::string("version"), std::string("1.0.0")}, 
        object::pair{std::string("actions"), array<object>{ object{
            object::pair{std::string("name"), std::string("getCurrentWeather")}, 
            object::pair{std::string("description"), std::string("Get current weather for a location")}, 
            object::pair{std::string("parameters"), object{
                object::pair{std::string("location"), std::string("string")}, 
                object::pair{std::string("units"), std::string("celsius | fahrenheit")}
            }}
        }, object{
            object::pair{std::string("name"), std::string("getWeatherForecast")}, 
            object::pair{std::string("description"), std::string("Get weather forecast for the next 5 days")}, 
            object::pair{std::string("parameters"), object{
                object::pair{std::string("location"), std::string("string")}, 
                object::pair{std::string("days"), std::string("number")}
            }}
        } }}, 
        object::pair{std::string("providers"), array<object>{ object{
            object::pair{std::string("name"), std::string("weatherData")}, 
            object::pair{std::string("description"), std::string("Provides current weather data in context")}, 
            object::pair{std::string("dataStructure"), object{
                object::pair{std::string("currentLocation"), std::string("string")}, 
                object::pair{std::string("temperature"), std::string("number")}, 
                object::pair{std::string("conditions"), std::string("string")}, 
                object::pair{std::string("lastUpdated"), std::string("timestamp")}
            }}
        } }}, 
        object::pair{std::string("dependencies"), object{
            object::pair{std::string("axios"), std::string("^1.6.0")}
        }}, 
        object::pair{std::string("environmentVariables"), array<object>{ object{
            object::pair{std::string("name"), std::string("WEATHER_API_KEY")}, 
            object::pair{std::string("description"), std::string("API key for weather service")}, 
            object::pair{std::string("required"), true}, 
            object::pair{std::string("sensitive"), true}
        } }}
    };
    console->log(std::string("User: Create a weather plugin with the following spec:"));
    console->log(JSON->stringify(weatherPluginSpec, nullptr, 2));
};


void createTodoPlugin(std::shared_ptr<Agent> agent)
{
    auto description = std::string("\
        I need a plugin that helps manage todo lists. It should have:\
        - An action to add new todos with title and optional due date\
        - An action to mark todos as complete\
        - An action to list all todos with filtering options\
        - A provider that shows pending todo count\
        - Store todos in memory with persistence\
    ");
    console->log(std::string("User:"), description);
};


void monitorPluginCreation(std::shared_ptr<Agent> agent)
{
    console->log(std::string("User: What's the status of my plugin creation?"));
    console->log(std::string("Agent: Plugin Creation Status: running\
Current Phase: testing\
Progress: 80%\
\
Recent logs:\
- Building plugin...\
- Build successful\
- Running tests...\
- 12 tests passed\
- Running validation..."));
};


any getDatabasePluginSpec()
{
    return object{
        object::pair{std::string("name"), std::string("@elizaos/plugin-postgres")}, 
        object::pair{std::string("description"), std::string("PostgreSQL database integration")}, 
        object::pair{std::string("version"), std::string("1.0.0")}, 
        object::pair{std::string("services"), array<object>{ object{
            object::pair{std::string("name"), std::string("PostgresService")}, 
            object::pair{std::string("description"), std::string("Manages PostgreSQL connections and queries")}, 
            object::pair{std::string("methods"), array<string>{ std::string("connect"), std::string("disconnect"), std::string("query"), std::string("transaction") }}
        } }}, 
        object::pair{std::string("actions"), array<object>{ object{
            object::pair{std::string("name"), std::string("executeQuery")}, 
            object::pair{std::string("description"), std::string("Execute a SQL query")}, 
            object::pair{std::string("parameters"), object{
                object::pair{std::string("query"), std::string("string")}, 
                object::pair{std::string("params"), std::string("array")}
            }}
        }, object{
            object::pair{std::string("name"), std::string("getDatabaseStats")}, 
            object::pair{std::string("description"), std::string("Get database statistics and health")}, 
            object::pair{std::string("parameters"), object{}}
        } }}, 
        object::pair{std::string("evaluators"), array<object>{ object{
            object::pair{std::string("name"), std::string("databaseHealth")}, 
            object::pair{std::string("description"), std::string("Monitors database connection health")}, 
            object::pair{std::string("triggers"), array<string>{ std::string("*/5 * * * *") }}
        } }}, 
        object::pair{std::string("dependencies"), object{
            object::pair{std::string("pg"), std::string("^8.11.0")}, 
            object::pair{std::string("pg-pool"), std::string("^3.6.0")}
        }}, 
        object::pair{std::string("environmentVariables"), array<object>{ object{
            object::pair{std::string("name"), std::string("DATABASE_URL")}, 
            object::pair{std::string("description"), std::string("PostgreSQL connection string")}, 
            object::pair{std::string("required"), true}, 
            object::pair{std::string("sensitive"), true}
        }, object{
            object::pair{std::string("name"), std::string("DATABASE_POOL_SIZE")}, 
            object::pair{std::string("description"), std::string("Maximum connection pool size")}, 
            object::pair{std::string("required"), false}, 
            object::pair{std::string("sensitive"), false}
        } }}
    };
};


any getSocialMediaPluginSpec()
{
    return object{
        object::pair{std::string("name"), std::string("@elizaos/plugin-social")}, 
        object::pair{std::string("description"), std::string("Social media integration plugin")}, 
        object::pair{std::string("version"), std::string("1.0.0")}, 
        object::pair{std::string("actions"), array<object>{ object{
            object::pair{std::string("name"), std::string("postToTwitter")}, 
            object::pair{std::string("description"), std::string("Post a message to Twitter/X")}, 
            object::pair{std::string("parameters"), object{
                object::pair{std::string("message"), std::string("string")}, 
                object::pair{std::string("mediaUrls"), std::string("string[]")}
            }}
        }, object{
            object::pair{std::string("name"), std::string("schedulePost")}, 
            object::pair{std::string("description"), std::string("Schedule a social media post")}, 
            object::pair{std::string("parameters"), object{
                object::pair{std::string("platform"), std::string("twitter | instagram | linkedin")}, 
                object::pair{std::string("message"), std::string("string")}, 
                object::pair{std::string("scheduledTime"), std::string("timestamp")}
            }}
        } }}, 
        object::pair{std::string("providers"), array<object>{ object{
            object::pair{std::string("name"), std::string("socialStats")}, 
            object::pair{std::string("description"), std::string("Provides social media statistics")}, 
            object::pair{std::string("dataStructure"), object{
                object::pair{std::string("followers"), std::string("number")}, 
                object::pair{std::string("engagement"), std::string("number")}, 
                object::pair{std::string("recentPosts"), std::string("array")}
            }}
        } }}
    };
};


void main()
{
    auto agent = std::make_shared<Agent>(object{
        object::pair{std::string("name"), std::string("PluginCreator")}, 
        object::pair{std::string("plugins"), array<any>{ pluginDynamic }}
    });
    console->log(std::string("=== Weather Plugin Example ==="));
    std::async([=]() { createWeatherPlugin(agent); });
    console->log(std::string("\
=== Todo Plugin from Description ==="));
    std::async([=]() { createTodoPlugin(agent); });
    console->log(std::string("\
=== Database Plugin Specification ==="));
    console->log(JSON->stringify(getDatabasePluginSpec(), nullptr, 2));
    console->log(std::string("\
=== Social Media Plugin Specification ==="));
    console->log(JSON->stringify(getSocialMediaPluginSpec(), nullptr, 2));
};



void Main(void)
{
    if (require->main == module) {
        main()->_catch(console->error);
    }
}

MAIN
