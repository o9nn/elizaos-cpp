#include "pluginKnowledgeProvider.hpp"

std::shared_ptr<Provider> pluginKnowledgeProvider = object{
    object::pair{std:("name"), std:("pluginKnowledge")}, 
    object::pair{std:("get"), [=](auto runtime, auto message, auto state = undefined) mutable
    {
        elizaLogger->info(std:("[pluginKnowledgeProvider] Fetching plugin knowledge"));
        try
        {
            auto plugins = std::async([=]() { getAllPlugins(); });
            if (plugins->get_length() == 0) {
                return object{
                    object::pair{std:("values"), object{}}, 
                    object::pair{std:("text"), std:("No plugins available in the registry.")}
                };
            }
            shared knowledge = std:("Available ElizaOS Plugins (") + plugins->get_length() + std:(" total):\
\
");
            plugins->forEach([=](auto plugin) mutable
            {
                knowledge += std:("**") + plugin->name + std:("**\
");
                if (plugin->description) {
                    knowledge += std:("Description: ") + plugin->description + std:("\
");
                }
                if (AND((plugin->tags), (plugin->tags->get_length() > 0))) {
                    knowledge += std:("Tags: ") + plugin->tags->join(std:(", ")) + std:("\
");
                }
                knowledge += std:("\
");
            }
            );
            return object{
                object::pair{std:("values"), object{}}, 
                object::pair{std:("text"), knowledge}
            };
        }
        catch (const any& error)
        {
            elizaLogger->error(std:("[pluginKnowledgeProvider] Failed to fetch plugin knowledge:"), error);
            return object{
                object::pair{std:("values"), object{}}, 
                object::pair{std:("text"), std:("Unable to fetch plugin knowledge at this time.")}
            };
        }
    }
    }
};

void Main(void)
{
}

MAIN
