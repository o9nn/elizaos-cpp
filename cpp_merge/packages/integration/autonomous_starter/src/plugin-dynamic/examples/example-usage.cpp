#include "example-usage.h"

void createWeatherPlugin(std::shared_ptr<Agent> agent)
{
    auto weatherPluginSpec = object{
        object::pair{std:("name"), std:("@elizaos/plugin-weather")}, 
        object::pair{std:("description"), std:("Weather information and forecasting plugin")}, 
        object::pair{std:("version"), std:("1.0.0")}, 
        object::pair{std:("actions"), array<object>{ object{
            object::pair{std:("name"), std:("getCurrentWeather")}, 
            object::pair{std:("description"), std:("Get current weather for a location")}, 
            object::pair{std:("parameters"), object{
                object::pair{std:("location"), std:("string")}, 
                object::pair{std:("units"), std:("celsius | fahrenheit")}
            }}
        }, object{
            object::pair{std:("name"), std:("getWeatherForecast")}, 
            object::pair{std:("description"), std:("Get weather forecast for the next 5 days")}, 
            object::pair{std:("parameters"), object{
                object::pair{std:("location"), std:("string")}, 
                object::pair{std:("days"), std:("number")}
            }}
        } }}, 
        object::pair{std:("providers"), array<object>{ object{
            object::pair{std:("name"), std:("weatherData")}, 
            object::pair{std:("description"), std:("Provides current weather data in context")}, 
            object::pair{std:("dataStructure"), object{
                object::pair{std:("currentLocation"), std:("string")}, 
                object::pair{std:("temperature"), std:("number")}, 
                object::pair{std:("conditions"), std:("string")}, 
                object::pair{std:("lastUpdated"), std:("timestamp")}
            }}
        } }}, 
        object::pair{std:("dependencies"), object{
            object::pair{std:("axios"), std:("^1.6.0")}
        }}, 
        object::pair{std:("environmentVariables"), array<object>{ object{
            object::pair{std:("name"), std:("WEATHER_API_KEY")}, 
            object::pair{std:("description"), std:("API key for weather service")}, 
            object::pair{std:("required"), true}, 
            object::pair{std:("sensitive"), true}
        } }}
    };
    console->log(std:("User: Create a weather plugin with the following spec:"));
    console->log(JSON->stringify(weatherPluginSpec, nullptr, 2));
};


void createTodoPlugin(std::shared_ptr<Agent> agent)
{
    auto description = std:("\
        I need a plugin that helps manage todo lists. It should have:\
        - An action to add new todos with title and optional due date\
        - An action to mark todos as complete\
        - An action to list all todos with filtering options\
        - A provider that shows pending todo count\
        - Store todos in memory with persistence\
    ");
    console->log(std:("User:"), description);
};


void monitorPluginCreation(std::shared_ptr<Agent> agent)
{
    console->log(std:("User: What's the status of my plugin creation?"));
    console->log(std:("Agent: Plugin Creation Status: running\
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
        object::pair{std:("name"), std:("@elizaos/plugin-postgres")}, 
        object::pair{std:("description"), std:("PostgreSQL database integration")}, 
        object::pair{std:("version"), std:("1.0.0")}, 
        object::pair{std:("services"), array<object>{ object{
            object::pair{std:("name"), std:("PostgresService")}, 
            object::pair{std:("description"), std:("Manages PostgreSQL connections and queries")}, 
            object::pair{std:("methods"), array<string>{ std:("connect"), std:("disconnect"), std:("query"), std:("transaction") }}
        } }}, 
        object::pair{std:("actions"), array<object>{ object{
            object::pair{std:("name"), std:("executeQuery")}, 
            object::pair{std:("description"), std:("Execute a SQL query")}, 
            object::pair{std:("parameters"), object{
                object::pair{std:("query"), std:("string")}, 
                object::pair{std:("params"), std:("array")}
            }}
        }, object{
            object::pair{std:("name"), std:("getDatabaseStats")}, 
            object::pair{std:("description"), std:("Get database statistics and health")}, 
            object::pair{std:("parameters"), object{}}
        } }}, 
        object::pair{std:("evaluators"), array<object>{ object{
            object::pair{std:("name"), std:("databaseHealth")}, 
            object::pair{std:("description"), std:("Monitors database connection health")}, 
            object::pair{std:("triggers"), array<string>{ std:("*/5 * * * *") }}
        } }}, 
        object::pair{std:("dependencies"), object{
            object::pair{std:("pg"), std:("^8.11.0")}, 
            object::pair{std:("pg-pool"), std:("^3.6.0")}
        }}, 
        object::pair{std:("environmentVariables"), array<object>{ object{
            object::pair{std:("name"), std:("DATABASE_URL")}, 
            object::pair{std:("description"), std:("PostgreSQL connection string")}, 
            object::pair{std:("required"), true}, 
            object::pair{std:("sensitive"), true}
        }, object{
            object::pair{std:("name"), std:("DATABASE_POOL_SIZE")}, 
            object::pair{std:("description"), std:("Maximum connection pool size")}, 
            object::pair{std:("required"), false}, 
            object::pair{std:("sensitive"), false}
        } }}
    };
};


any getSocialMediaPluginSpec()
{
    return object{
        object::pair{std:("name"), std:("@elizaos/plugin-social")}, 
        object::pair{std:("description"), std:("Social media integration plugin")}, 
        object::pair{std:("version"), std:("1.0.0")}, 
        object::pair{std:("actions"), array<object>{ object{
            object::pair{std:("name"), std:("postToTwitter")}, 
            object::pair{std:("description"), std:("Post a message to Twitter/X")}, 
            object::pair{std:("parameters"), object{
                object::pair{std:("message"), std:("string")}, 
                object::pair{std:("mediaUrls"), std:("string[]")}
            }}
        }, object{
            object::pair{std:("name"), std:("schedulePost")}, 
            object::pair{std:("description"), std:("Schedule a social media post")}, 
            object::pair{std:("parameters"), object{
                object::pair{std:("platform"), std:("twitter | instagram | linkedin")}, 
                object::pair{std:("message"), std:("string")}, 
                object::pair{std:("scheduledTime"), std:("timestamp")}
            }}
        } }}, 
        object::pair{std:("providers"), array<object>{ object{
            object::pair{std:("name"), std:("socialStats")}, 
            object::pair{std:("description"), std:("Provides social media statistics")}, 
            object::pair{std:("dataStructure"), object{
                object::pair{std:("followers"), std:("number")}, 
                object::pair{std:("engagement"), std:("number")}, 
                object::pair{std:("recentPosts"), std:("array")}
            }}
        } }}
    };
};


void main()
{
    auto agent = std::make_shared<Agent>(object{
        object::pair{std:("name"), std:("PluginCreator")}, 
        object::pair{std:("plugins"), array<any>{ pluginDynamic }}
    });
    console->log(std:("== Weather Plugin Example =="));
    std::async([=]() { createWeatherPlugin(agent); });
    console->log(std:("\
== Todo Plugin from Description =="));
    std::async([=]() { createTodoPlugin(agent); });
    console->log(std:("\
== Database Plugin Specification =="));
    console->log(JSON->stringify(getDatabasePluginSpec(), nullptr, 2));
    console->log(std:("\
== Social Media Plugin Specification =="));
    console->log(JSON->stringify(getSocialMediaPluginSpec(), nullptr, 2));
};



void Main(void)
{
    if (require->main == module) {
        main()->_catch(console->error);
    }
}

MAIN
