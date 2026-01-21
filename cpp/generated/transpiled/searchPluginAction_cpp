#include "/home/runner/work/elizaos-cpp/elizaos-cpp/classified/packages/plugin-plugin-manager/src/actions/searchPluginAction.h"

any extractSearchQuery(string text)
{
    auto patterns = array<std::shared_ptr<RegExp>>{ (new RegExp(std::string("search\s+for\s+plugins?\s+(?:that\s+)?(?:can\s+)?(.+)"))), (new RegExp(std::string("find\s+plugins?\s+(?:for|that|to)\s+(.+)"))), (new RegExp(std::string("look\s+for\s+plugins?\s+(?:that\s+)?(.+)"))), (new RegExp(std::string("discover\s+plugins?\s+(?:for|that)\s+(.+)"))), (new RegExp(std::string("show\s+me\s+plugins?\s+(?:for|that)\s+(.+)"))), (new RegExp(std::string("need\s+(?:a\s+)?plugins?\s+(?:for|that|to)\s+(.+)"))), (new RegExp(std::string("want\s+(?:a\s+)?plugins?\s+(?:for|that|to)\s+(.+)"))), (new RegExp(std::string("plugins?\s+(?:for|that\s+can|to)\s+(.+)"))), (new RegExp(std::string("what\s+plugins?\s+(?:can|do|handle)\s+(.+)"))), (new RegExp(std::string("plugins?\s+(.+)"))) };
    auto& __array6429_6912 = patterns;
    for (auto __indx6429_6912 = 0_N; __indx6429_6912 < __array6429_6912->get_length(); __indx6429_6912++)
    {
        auto& pattern = const_(__array6429_6912)[__indx6429_6912];
        {
            auto match = text->match(pattern);
            if (AND((match), ((*const_(match))[1]))) {
                auto query = (*const_(match))[1]->trim();
                query = query->replace((new RegExp(std::string("\?+"))), string_empty);
                query = query->replace((new RegExp(std::string("^(do|handle|manage|work\s+with)\s+"))), string_empty);
                query = query->replace((new RegExp(std::string("\s+"))), std::string(" "));
                if (query->get_length() > 2) {
                    return query;
                }
            }
        }
    }
    auto techKeywords = text->match((new RegExp(std::string("\b(blockchain|ai|database|api|social|twitter|discord|telegram|solana|ethereum|trading|defi|nft|authentication|security|monitoring|analytics|file|image|video|audio|email|sms|payment)\b/"))));
    if (AND((techKeywords), (techKeywords->length > 0))) {
        return techKeywords->join(std::string(" "));
    }
    return nullptr;
};


std::shared_ptr<Action> searchPluginAction = object{
    object::pair{std::string("name"), std::string("SEARCH_PLUGINS")}, 
    object::pair{std::string("similes"), array<string>{ std::string("search for plugins"), std::string("find plugins"), std::string("look for plugins"), std::string("discover plugins"), std::string("search registry") }}, 
    object::pair{std::string("description"), std::string("Search for plugins in the official ElizaOS registry using vectorized similarity search. Finds plugins by functionality, features, and natural language descriptions.")}, 
    object::pair{std::string("examples"), array<array<object>>{ array<object>{ object{
        object::pair{std::string("name"), std::string("{{user1}}")}, 
        object::pair{std::string("content"), object{
            object::pair{std::string("text"), std::string("Search for plugins that can handle blockchain transactions")}, 
            object::pair{std::string("actions"), array<string>{ std::string("SEARCH_PLUGINS") }}
        }}
    }, object{
        object::pair{std::string("name"), std::string("{{agentName}}")}, 
        object::pair{std::string("content"), object{
            object::pair{std::string("text"), std::string("I'll search for blockchain-related plugins that can handle transactions.")}, 
            object::pair{std::string("actions"), array<string>{ std::string("SEARCH_PLUGINS") }}
        }}
    }, object{
        object::pair{std::string("name"), std::string("{{agentName}}")}, 
        object::pair{std::string("content"), object{
            object::pair{std::string("text"), std::string("🔍 Found 5 plugins related to blockchain transactions:\
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
            object::pair{std::string("actions"), array<string>{ std::string("SEARCH_PLUGINS") }}
        }}
    } } }}, 
    object::pair{std::string("validate"), [=](auto runtime, auto message) mutable
    {
        shared text = OR((message->content->text->toLowerCase()), (string_empty));
        auto searchPatterns = array<std::shared_ptr<RegExp>>{ (new RegExp(std::string("search.*plugins?"))), (new RegExp(std::string("find.*plugins?"))), (new RegExp(std::string("look.*for.*plugins?"))), (new RegExp(std::string("discover.*plugins?"))), (new RegExp(std::string("plugins?.*(for|that|to)"))), (new RegExp(std::string("need.*plugins?"))), (new RegExp(std::string("show.*plugins?"))), (new RegExp(std::string("list.*plugins?"))) };
        return searchPatterns->some([=](auto pattern) mutable
        {
            return pattern->test(text);
        }
        );
    }
    }, 
    object::pair{std::string("handler"), [=](auto runtime, auto message, auto state = undefined, auto options = undefined, auto callback = undefined) mutable
    {
        elizaLogger->info(std::string("[searchPluginAction] Starting plugin search"));
        auto query = extractSearchQuery(OR((message->content->text), (string_empty)));
        if (!query) {
            if (callback) {
                std::async([=]() { callback(object{
                    object::pair{std::string("text"), std::string("🤔 Please specify what kind of functionality or features you're looking for in a plugin.\
\
For example:\
• "Search for plugins that handle blockchain transactions"\
• "Find plugins for social media integration"\
• "Look for plugins that can process images"")}, 
                    object::pair{std::string("actions"), array<string>{ std::string("SEARCH_PLUGINS") }}
                }); });
            }
            return std::shared_ptr<Promise<void>>();
        }
        elizaLogger->info(std::string("[searchPluginAction] Searching for: "") + query + std::string("""));
        try
        {
            auto results = std::async([=]() { searchPluginsByContent(query); });
            if (results->get_length() == 0) {
                if (callback) {
                    std::async([=]() { callback(object{
                        object::pair{std::string("text"), std::string("🔍 No plugins found matching "") + query + std::string("".\
\
💡 Try using different keywords like:\
• Functionality: "database", "api", "blockchain", "ai"\
• Technology: "twitter", "discord", "solana", "ethereum"\
• Purpose: "authentication", "monitoring", "trading"")}, 
                        object::pair{std::string("actions"), array<string>{ std::string("SEARCH_PLUGINS") }}
                    }); });
                }
                return std::shared_ptr<Promise<void>>();
            }
            shared responseText = std::string("🔍 Found ") + results->get_length() + std::string(" plugin") + (results->get_length() > 1) ? std::string("s") : string_empty + std::string(" matching "") + query + std::string("":\
\
");
            results->forEach([=](auto plugin, auto index) mutable
            {
                auto score = (plugin->score) ? any((plugin->score * 100)->toFixed(0)) : any(string_empty);
                responseText += string_empty + (index + 1) + std::string(". **") + plugin->name + std::string("**") + (score) ? any(std::string(" (Score: ") + score + std::string("%)")) : any(string_empty) + std::string("\
");
                if (plugin->description) {
                    responseText += std::string("   💡 ") + plugin->description + std::string("\
");
                }
                if (AND((plugin->tags), (plugin->tags->get_length() > 0))) {
                    auto displayTags = plugin->tags->slice(0, 5);
                    responseText += std::string("   🏷️ Tags: ") + displayTags->join(std::string(", ")) + std::string("\
");
                }
                if (plugin->relevantSection) {
                    responseText += std::string("   📄 "") + plugin->relevantSection + std::string(""\
");
                }
                if (plugin->version) {
                    responseText += std::string("   📌 Version: ") + plugin->version + std::string("\
");
                }
                responseText += std::string("\
");
            }
            );
            responseText += std::string("💡 **Next steps:**\
");
            responseText += std::string("• Say "tell me more about [plugin-name]" for detailed info\
");
            responseText += std::string("• Say "install [plugin-name]" to install a plugin\
");
            responseText += std::string("• Say "clone [plugin-name]" to clone for development");
            if (callback) {
                std::async([=]() { callback(object{
                    object::pair{std::string("text"), responseText}, 
                    object::pair{std::string("actions"), array<string>{ std::string("SEARCH_PLUGINS") }}
                }); });
            }
        }
        catch (const any& error)
        {
            elizaLogger->error(std::string("[searchPluginAction] Search failed:"), error);
            if (callback) {
                std::async([=]() { callback(object{
                    object::pair{std::string("text"), std::string("❌ Failed to search plugins. Please try again later.")}, 
                    object::pair{std::string("actions"), array<string>{ std::string("SEARCH_PLUGINS") }}
                }); });
            }
        }
        return std::shared_ptr<Promise<void>>();
    }
    }
};
std::shared_ptr<Action> getPluginDetailsAction = object{
    object::pair{std::string("name"), std::string("GET_PLUGIN_DETAILS")}, 
    object::pair{std::string("similes"), array<string>{ std::string("tell me more about"), std::string("show details for"), std::string("plugin info"), std::string("plugin details") }}, 
    object::pair{std::string("description"), std::string("Get detailed information about a specific plugin including features, dependencies, and usage.")}, 
    object::pair{std::string("examples"), array<array<object>>{ array<object>{ object{
        object::pair{std::string("name"), std::string("{{user1}}")}, 
        object::pair{std::string("content"), object{
            object::pair{std::string("text"), std::string("Tell me more about @elizaos/plugin-solana")}
        }}
    }, object{
        object::pair{std::string("name"), std::string("{{agentName}}")}, 
        object::pair{std::string("content"), object{
            object::pair{std::string("text"), std::string("📋 **@elizaos/plugin-solana** Details:\
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
    object::pair{std::string("validate"), [=](auto runtime, auto message) mutable
    {
        auto text = OR((message->content->text->toLowerCase()), (string_empty));
        return (AND(((new RegExp(std::string("tell\s+me\s+more|show\s+details|plugin\s+info|more\s+abou")))->test(text)), ((new RegExp(std::string("@?[\w-]+\/plugin-[\w-]+|plugin-[\w-]")))->test(text))));
    }
    }, 
    object::pair{std::string("handler"), [=](auto runtime, auto message, auto state = undefined, auto options = undefined, auto callback = undefined) mutable
    {
        auto text = OR((message->content->text), (string_empty));
        auto pluginMatch = text["match"]((new RegExp(std::string("@?([\w-]+\/plugin-[\w-]+|plugin-[\w-]+)"))));
        if (!pluginMatch) {
            if (callback) {
                std::async([=]() { callback(object{
                    object::pair{std::string("text"), std::string("🤔 Please specify which plugin you'd like to know more about.\
\
Example: "Tell me more about @elizaos/plugin-solana"")}
                }); });
            }
            return std::shared_ptr<Promise<void>>();
        }
        auto pluginName = const_(pluginMatch)[1];
        if (AND((!pluginName["startsWith"](std::string("@"))), (!pluginName["includes"](std::string("/"))))) {
            pluginName = std::string("@elizaos/") + pluginName + string_empty;
        }
        try
        {
            auto details = std::async([=]() { getPluginDetails(pluginName); });
            if (!details) {
                if (callback) {
                    std::async([=]() { callback(object{
                        object::pair{std::string("text"), std::string("❌ Plugin "") + pluginName + std::string("" not found in the registry.\
\
Try searching for plugins first: "search for [functionality]"")}
                    }); });
                }
                return std::shared_ptr<Promise<void>>();
            }
            auto responseText = std::string("📋 **") + details->name + std::string("** Details:\
\
");
            if (details->description) {
                responseText += std::string("💡 **Description:** ") + details->description + std::string("\
\
");
            }
            if (AND((details->tags), (details->tags->get_length() > 0))) {
                responseText += std::string("🏷️ **Tags:** ") + details->tags->join(std::string(", ")) + std::string("\
\
");
            }
            if (details->latestVersion) {
                responseText += std::string("📌 **Version:** ") + details->latestVersion + std::string("\
");
            }
            if (details->repository) {
                responseText += std::string("📍 **Repository:** ") + details->repository + std::string("\
");
            }
            responseText += std::string("\
\
To install: "install ") + details->name + std::string(""");
            if (callback) {
                std::async([=]() { callback(object{
                    object::pair{std::string("text"), responseText}
                }); });
            }
        }
        catch (const any& error)
        {
            elizaLogger->error(std::string("[getPluginDetailsAction] Failed to get plugin details:"), error);
            if (callback) {
                std::async([=]() { callback(object{
                    object::pair{std::string("text"), std::string("❌ Failed to get plugin details. Please try again later.")}
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
