#include "searchPluginAction.hpp"

any extractSearchQuery(string text)
{
    auto patterns = array<std::shared_ptr<RegExp>>{ (new RegExp(std:("search\s+for\s+plugins?\s+(?:that\s+)?(?:can\s+)?(.+)"))), (new RegExp(std:("find\s+plugins?\s+(?:for|that|to)\s+(.+)"))), (new RegExp(std:("look\s+for\s+plugins?\s+(?:that\s+)?(.+)"))), (new RegExp(std:("discover\s+plugins?\s+(?:for|that)\s+(.+)"))), (new RegExp(std:("show\s+me\s+plugins?\s+(?:for|that)\s+(.+)"))), (new RegExp(std:("need\s+(?:a\s+)?plugins?\s+(?:for|that|to)\s+(.+)"))), (new RegExp(std:("want\s+(?:a\s+)?plugins?\s+(?:for|that|to)\s+(.+)"))), (new RegExp(std:("plugins?\s+(?:for|that\s+can|to)\s+(.+)"))), (new RegExp(std:("what\s+plugins?\s+(?:can|do|handle)\s+(.+)"))), (new RegExp(std:("plugins?\s+(.+)"))) };
    auto& __array6429_6912 = patterns;
    for (auto __indx6429_6912 = 0_N; __indx6429_6912 < __array6429_6912->get_length(); __indx6429_6912++)
    {
        auto& pattern = const_(__array6429_6912)[__indx6429_6912];
        {
            auto match = text->match(pattern);
            if (AND((match), ((*const_(match))[1]))) {
                auto query = (*const_(match))[1]->trim();
                query = query->replace((new RegExp(std:("\?+"))), string_empty);
                query = query->replace((new RegExp(std:("^(do|handle|manage|work\s+with)\s+"))), string_empty);
                query = query->replace((new RegExp(std:("\s+"))), std:(" "));
                if (query->get_length() > 2) {
                    return query;
                }
            }
        }
    }
    auto techKeywords = text->match((new RegExp(std:("\b(blockchain|ai|database|api|social|twitter|discord|telegram|solana|ethereum|trading|defi|nft|authentication|security|monitoring|analytics|file|image|video|audio|email|sms|payment)\b/"))));
    if (AND((techKeywords), (techKeywords->length > 0))) {
        return techKeywords->join(std:(" "));
    }
    return nullptr;
};


std::shared_ptr<Action> searchPluginAction = object{
    object::pair{std:("name"), std:("SEARCH_PLUGINS")}, 
    object::pair{std:("similes"), array<string>{ std:("search for plugins"), std:("find plugins"), std:("look for plugins"), std:("discover plugins"), std:("search registry") }}, 
    object::pair{std:("description"), std:("Search for plugins in the official ElizaOS registry using vectorized similarity search. Finds plugins by functionality, features, and natural language descriptions.")}, 
    object::pair{std:("examples"), array<array<object>>{ array<object>{ object{
        object::pair{std:("name"), std:("{{user1}}")}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), std:("Search for plugins that can handle blockchain transactions")}, 
            object::pair{std:("actions"), array<string>{ std:("SEARCH_PLUGINS") }}
        }}
    }, object{
        object::pair{std:("name"), std:("{{agentName}}")}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), std:("I'll search for blockchain-related plugins that can handle transactions.")}, 
            object::pair{std:("actions"), array<string>{ std:("SEARCH_PLUGINS") }}
        }}
    }, object{
        object::pair{std:("name"), std:("{{agentName}}")}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), std:("🔍 Found 5 plugins related to blockchain transactions:\
\
1. **@elizaos/plugin-solana** (Score: 0.87)\
   💡 Solana blockchain integration with transaction handling\
   🏷️ Tags: blockchain, solana, transaction, defi\
   📦 Features: Send transactions, Query balances, Deploy contracts\
\
2. **@elizaos/plugin-ethereum** (Score: 0.82)\
   💡 Ethereum blockchain operations and smart contracts\
   🏷️ Tags: blockchain, ethereum, web3, smart-contracts\
   📦 Features: ERC-20 operations, Gas estimation, Contract deployment\
\
3. **@elizaos/plugin-wallet** (Score: 0.75)\
   💡 Multi-chain wallet operations and management\
   🏷️ Tags: wallet, multi-chain, transaction, security\
   📦 Features: Wallet creation, Transaction signing, Balance tracking\
\
Would you like me to show details for any of these plugins or help you install one?")}, 
            object::pair{std:("actions"), array<string>{ std:("SEARCH_PLUGINS") }}
        }}
    } } }}, 
    object::pair{std:("validate"), [=](auto runtime, auto message) mutable
    {
        shared text = OR((message->content->text->toLowerCase()), (string_empty));
        auto searchPatterns = array<std::shared_ptr<RegExp>>{ (new RegExp(std:("search.*plugins?"))), (new RegExp(std:("find.*plugins?"))), (new RegExp(std:("look.*for.*plugins?"))), (new RegExp(std:("discover.*plugins?"))), (new RegExp(std:("plugins.*(for|that|to)"))), (new RegExp(std:("need.*plugins?"))), (new RegExp(std:("show.*plugins?"))), (new RegExp(std:("list.*plugins?"))) };
        return searchPatterns->some([=](auto pattern) mutable
        {
            return pattern->test(text);
        }
        );
    }
    }, 
    object::pair{std:("handler"), [=](auto runtime, auto message, auto state = undefined, auto options = undefined, auto callback = undefined) mutable
    {
        elizaLogger->info(std:("[searchPluginAction] Starting plugin search"));
        auto query = extractSearchQuery(OR((message->content->text), (string_empty)));
        if (!query) {
            if (callback) {
                std::async([=]() { callback(object{
                    object::pair{std:("text"), std:("🤔 Please specify what kind of functionality or features you're looking for in a plugin.\
\
For example:\
• "Search for plugins that handle blockchain transactions"\
• "Find plugins for social media integration"\
• "Look for plugins that can process images"")}, 
                    object::pair{std:("actions"), array<string>{ std:("SEARCH_PLUGINS") }}
                }); });
            }
            return std::shared_ptr<Promise<void>>();
        }
        elizaLogger->info(std:("[searchPluginAction] Searching for: "") + query + std:("""));
        try
        {
            auto results = std::async([=]() { searchPluginsByContent(query); });
            if (results->get_length() == 0) {
                if (callback) {
                    std::async([=]() { callback(object{
                        object::pair{std:("text"), std:("🔍 No plugins found matching "") + query + std:("".\
\
💡 Try using different keywords like:\
• Functionality: "database", "api", "blockchain", "ai"\
• Technology: "twitter", "discord", "solana", "ethereum"\
• Purpose: "authentication", "monitoring", "trading"")}, 
                        object::pair{std:("actions"), array<string>{ std:("SEARCH_PLUGINS") }}
                    }); });
                }
                return std::shared_ptr<Promise<void>>();
            }
            shared responseText = std:("🔍 Found ") + results->get_length() + std:(" plugin") + (results->get_length() > 1) ? std:("s") : string_empty + std:(" matching "") + query + std:("":\
\
");
            results->forEach([=](auto plugin, auto index) mutable
            {
                auto score = (plugin->score) ? any((plugin->score * 100)->toFixed(0)) (string_empty);
                responseText += string_empty + (index + 1) + std:(". **") + plugin->name + std:("**") + (score) ? any(std:(" (Score: ") + score + std:("%)")) (string_empty) + std:("\
");
                if (plugin->description) {
                    responseText += std:("   💡 ") + plugin->description + std:("\
");
                }
                if (AND((plugin->tags), (plugin->tags->get_length() > 0))) {
                    auto displayTags = plugin->tags->slice(0, 5);
                    responseText += std:("   🏷️ Tags: ") + displayTags->join(std:(", ")) + std:("\
");
                }
                if (plugin->relevantSection) {
                    responseText += std:("   📄 "") + plugin->relevantSection + std:(""\
");
                }
                if (plugin->version) {
                    responseText += std:("   📌 Version: ") + plugin->version + std:("\
");
                }
                responseText += std:("\
");
            }
            );
            responseText += std:("💡 **Next steps:**\
");
            responseText += std:("• Say "tell me more about [plugin-name]" for detailed info\
");
            responseText += std:("• Say "install [plugin-name]" to install a plugin\
");
            responseText += std:("• Say "clone [plugin-name]" to clone for development");
            if (callback) {
                std::async([=]() { callback(object{
                    object::pair{std:("text"), responseText}, 
                    object::pair{std:("actions"), array<string>{ std:("SEARCH_PLUGINS") }}
                }); });
            }
        }
        catch (const any& error)
        {
            elizaLogger->error(std:("[searchPluginAction] Search failed:"), error);
            if (callback) {
                std::async([=]() { callback(object{
                    object::pair{std:("text"), std:("❌ Failed to search plugins. Please try again later.")}, 
                    object::pair{std:("actions"), array<string>{ std:("SEARCH_PLUGINS") }}
                }); });
            }
        }
        return std::shared_ptr<Promise<void>>();
    }
    }
};
std::shared_ptr<Action> getPluginDetailsAction = object{
    object::pair{std:("name"), std:("GET_PLUGIN_DETAILS")}, 
    object::pair{std:("similes"), array<string>{ std:("tell me more about"), std:("show details for"), std:("plugin info"), std:("plugin details") }}, 
    object::pair{std:("description"), std:("Get detailed information about a specific plugin including features, dependencies, and usage.")}, 
    object::pair{std:("examples"), array<array<object>>{ array<object>{ object{
        object::pair{std:("name"), std:("{{user1}}")}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), std:("Tell me more about @elizaos/plugin-solana")}
        }}
    }, object{
        object::pair{std:("name"), std:("{{agentName}}")}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), std:("📋 **@elizaos/plugin-solana** Details:\
\
💡 **Description:** Comprehensive Solana blockchain integration\
\
🏷️ **Tags:** blockchain, solana, defi, transaction\
\
📦 **Features:**\
• Send and receive SOL transactions\
• Query wallet balances and transaction history\
• Deploy and interact with programs\
• Handle SPL token operations\
\
🔗 **Dependencies:** None\
\
📌 **Version:** 1.2.0\
📍 **Repository:** https://github.com/elizaos-plugins/plugin-solana\
📦 **NPM:** @elizaos/plugin-solana\
\
💡 **Related Plugins:**\
• @elizaos/plugin-wallet (complementary)\
• @elizaos/plugin-defi (similar)\
\
To install: "install @elizaos/plugin-solana"")}
        }}
    } } }}, 
    object::pair{std:("validate"), [=](auto runtime, auto message) mutable
    {
        auto text = OR((message->content->text->toLowerCase()), (string_empty));
        return (AND(((new RegExp(std:("tell\s+me\s+more|show\s+details|plugin\s+info|more\s+abou")))->test(text)), ((new RegExp(std:("@?[\w-]+\/plugin-[\w-]+|plugin-[\w-]")))->test(text))));
    }
    }, 
    object::pair{std:("handler"), [=](auto runtime, auto message, auto state = undefined, auto options = undefined, auto callback = undefined) mutable
    {
        auto text = OR((message->content->text), (string_empty));
        auto pluginMatch = text["match"]((new RegExp(std:("@?([\w-]+\/plugin-[\w-]+|plugin-[\w-]+)"))));
        if (!pluginMatch) {
            if (callback) {
                std::async([=]() { callback(object{
                    object::pair{std:("text"), std:("🤔 Please specify which plugin you'd like to know more about.\
\
Example: "Tell me more about @elizaos/plugin-solana"")}
                }); });
            }
            return std::shared_ptr<Promise<void>>();
        }
        auto pluginName = const_(pluginMatch)[1];
        if (AND((!pluginName["startsWith"](std:("@"))), (!pluginName["includes"](std:("/"))))) {
            pluginName = std:("@elizaos/") + pluginName + string_empty;
        }
        try
        {
            auto details = std::async([=]() { getPluginDetails(pluginName); });
            if (!details) {
                if (callback) {
                    std::async([=]() { callback(object{
                        object::pair{std:("text"), std:("❌ Plugin "") + pluginName + std:("" not found in the registry.\
\
Try searching for plugins first: "search for [functionality]"")}
                    }); });
                }
                return std::shared_ptr<Promise<void>>();
            }
            auto responseText = std:("📋 **") + details->name + std:("** Details:\
\
");
            if (details->description) {
                responseText += std:("💡 **Description:** ") + details->description + std:("\
\
");
            }
            if (AND((details->tags), (details->tags->get_length() > 0))) {
                responseText += std:("🏷️ **Tags:** ") + details->tags->join(std:(", ")) + std:("\
\
");
            }
            if (details->latestVersion) {
                responseText += std:("📌 **Version:** ") + details->latestVersion + std:("\
");
            }
            if (details->repository) {
                responseText += std:("📍 **Repository:** ") + details->repository + std:("\
");
            }
            responseText += std:("\
\
To install: "install ") + details->name + std:(""");
            if (callback) {
                std::async([=]() { callback(object{
                    object::pair{std:("text"), responseText}
                }); });
            }
        }
        catch (const any& error)
        {
            elizaLogger->error(std:("[getPluginDetailsAction] Failed to get plugin details:"), error);
            if (callback) {
                std::async([=]() { callback(object{
                    object::pair{std:("text"), std:("❌ Failed to get plugin details. Please try again later.")}
                }); });
            }
        }
    }
    }
};

void Main(void)
{
}

MAIN
