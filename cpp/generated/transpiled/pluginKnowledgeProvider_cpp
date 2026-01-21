#include "/home/runner/work/elizaos-cpp/elizaos-cpp/classified/packages/plugin-plugin-manager/src/providers/pluginKnowledgeProvider.h"

std::shared_ptr<Provider> pluginKnowledgeProvider = object{
    object::pair{std::string("name"), std::string("pluginKnowledge")}, 
    object::pair{std::string("get"), [=](auto runtime, auto message, auto state = undefined) mutable
    {
        elizaLogger->info(std::string("[pluginKnowledgeProvider] Fetching plugin knowledge"));
        try
        {
            auto plugins = std::async([=]() { getAllPlugins(); });
            if (plugins->get_length() == 0) {
                return object{
                    object::pair{std::string("values"), object{}}, 
                    object::pair{std::string("text"), std::string("No plugins available in the registry.")}
                };
            }
            shared knowledge = std::string("Available ElizaOS Plugins (") + plugins->get_length() + std::string(" total):\
\
");
            plugins->forEach([=](auto plugin) mutable
            {
                knowledge += std::string("**") + plugin->name + std::string("**\
");
                if (plugin->description) {
                    knowledge += std::string("Description: ") + plugin->description + std::string("\
");
                }
                if (AND((plugin->tags), (plugin->tags->get_length() > 0))) {
                    knowledge += std::string("Tags: ") + plugin->tags->join(std::string(", ")) + std::string("\
");
                }
                knowledge += std::string("\
");
            }
            );
            return object{
                object::pair{std::string("values"), object{}}, 
                object::pair{std::string("text"), knowledge}
            };
        }
        catch (const any& error)
        {
            elizaLogger->error(std::string("[pluginKnowledgeProvider] Failed to fetch plugin knowledge:"), error);
            return object{
                object::pair{std::string("values"), object{}}, 
                object::pair{std::string("text"), std::string("Unable to fetch plugin knowledge at this time.")}
            };
        }
    }
    }
};

void Main(void)
{
}

MAIN
