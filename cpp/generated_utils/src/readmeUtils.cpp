#include "/home/runner/work/elizaos-cpp/elizaos-cpp/elizaos.github.io/src/lib/walletLinking/readmeUtils.h"

any parseWalletLinkingDataFromReadme(string readmeContent)
{
    auto startIndex = readmeContent->indexOf(WALLET_SECTION_BEGIN_MARKER);
    auto endIndex = readmeContent->indexOf(WALLET_SECTION_END_MARKER);
    if (OR((OR((startIndex == -1), (endIndex == -1))), (endIndex <= startIndex))) {
        return nullptr;
    }
    auto walletSectionContent = readmeContent->substring(startIndex + WALLET_SECTION_BEGIN_MARKER->get_length(), endIndex)->trim();
    try
    {
        auto rawData = JSON->parse(walletSectionContent);
        auto result = WalletLinkingDataSchema->safeParse(rawData);
        if (!result->success) {
            console->error(std::string("Invalid wallet linking data:"), result->error);
            return nullptr;
        }
        auto walletLinkingData = object{
            object::pair{std::string("lastUpdated"), result->data->lastUpdated}, 
            object::pair{std::string("wallets"), result->data->wallets->filter([=](auto wallet) mutable
            {
                return AND((SUPPORTED_CHAINS_NAMES->includes(wallet["chain"]->toLowerCase())), (validateAddress(wallet["address"], wallet["chain"])));
            }
            )}
        };
        return walletLinkingData;
    }
    catch (const any& error)
    {
        console->error(std::string("Error parsing wallet linking data:"), error);
        return nullptr;
    }
};


object generateUpdatedReadmeWithWalletInfo(string currentReadme, array<LinkedWallet> wallets)
{
    auto validatedWallets = z->array(LinkedWalletSchema)->parse(wallets);
    auto walletData = object{
        object::pair{std::string("lastUpdated"), ((std::make_shared<Date>()))->toISOString()}, 
        object::pair{std::string("wallets"), validatedWallets->map([=](auto wallet) mutable
        {
            return (utils::assign(object{
                object::pair{std::string("chain"), wallet["chain"]->toLowerCase()->trim()}, 
                object::pair{std::string("address"), wallet["address"]->trim()}
            }, ((wallet["signature"]) ? object{
                object::pair{std::string("signature"), wallet["signature"]->trim()}
            } : object{})));
        }
        )}
    };
    auto validatedData = WalletLinkingDataSchema->parse(walletData);
    auto walletSection = string_empty + WALLET_SECTION_BEGIN_MARKER + std::string("\
") + JSON->stringify(validatedData, nullptr, 2) + std::string("\
") + WALLET_SECTION_END_MARKER + string_empty;
    auto startIndex = currentReadme->indexOf(WALLET_SECTION_BEGIN_MARKER);
    auto endIndex = currentReadme->indexOf(WALLET_SECTION_END_MARKER);
    if (AND((AND((startIndex != -1), (endIndex != -1))), (endIndex > startIndex))) {
        auto updatedReadme = currentReadme->substring(0, startIndex) + walletSection + currentReadme->substring(endIndex + WALLET_SECTION_END_MARKER->get_length());
        return object{
            object::pair{std::string("updatedReadme"), std::string("updatedReadme")}, 
            object::pair{std::string("walletData"), std::string("walletData")}
        };
    } else {
        auto separator = (AND((currentReadme->trim()), (!currentReadme->endsWith(std::string("\
"))))) ? any(std::string("\
\
")) : any((currentReadme->trim()) ? std::string("\
") : string_empty);
        return object{
            object::pair{std::string("updatedReadme"), currentReadme->trim() + separator + walletSection}, 
            object::pair{std::string("walletData"), std::string("walletData")}
        };
    }
};


string generateReadmeWalletSection(array<LinkedWallet> wallets)
{
    auto validatedWallets = z->array(LinkedWalletSchema)->parse(wallets);
    auto walletData = object{
        object::pair{std::string("lastUpdated"), ((std::make_shared<Date>()))->toISOString()}, 
        object::pair{std::string("wallets"), validatedWallets->map([=](auto wallet) mutable
        {
            return (utils::assign(object{
                object::pair{std::string("chain"), wallet["chain"]->toLowerCase()->trim()}, 
                object::pair{std::string("address"), wallet["address"]->trim()}
            }, ((wallet["signature"]) ? object{
                object::pair{std::string("signature"), wallet["signature"]->trim()}
            } : object{})));
        }
        )}
    };
    auto validatedData = WalletLinkingDataSchema->parse(walletData);
    return string_empty + WALLET_SECTION_BEGIN_MARKER + std::string("\
") + JSON->stringify(validatedData, nullptr, 2) + std::string("\
") + WALLET_SECTION_END_MARKER + string_empty;
};


string getWalletAddressForChain(any data, string chain)
{
    if (!data) return string_empty;
    auto wallet = data["wallets"]["find"]([=](auto w) mutable
    {
        return w["chain"]->toLowerCase() == chain->toLowerCase();
    }
    );
    return OR((wallet["address"]), (string_empty));
};


any LinkedWalletSchema = z->object(object{
    object::pair{std::string("chain"), z->string()->min(1)->toLowerCase()}, 
    object::pair{std::string("address"), z->string()->min(1)}, 
    object::pair{std::string("signature"), z->string()->min(1)->optional()}
});
any WalletLinkingDataSchema = z->object(object{
    object::pair{std::string("lastUpdated"), z->string()->datetime()}, 
    object::pair{std::string("wallets"), z->array(LinkedWalletSchema)}
});
string WALLET_SECTION_BEGIN_MARKER = std::string("<!-- WALLET-LINKING-BEGIN");
string WALLET_SECTION_END_MARKER = std::string("WALLET-LINKING-END -->");

void Main(void)
{
}

MAIN
