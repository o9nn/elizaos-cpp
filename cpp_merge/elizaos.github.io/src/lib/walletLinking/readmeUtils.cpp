#include "readmeUtils.hpp"

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
            console->error(std:("Invalid wallet linking data:"), result->error);
            return nullptr;
        }
        auto walletLinkingData = object{
            object::pair{std:("lastUpdated"), result->data->lastUpdated}, 
            object::pair{std:("wallets"), result->data->wallets->filter([=](auto wallet) mutable
            {
                return AND((SUPPORTED_CHAINS_NAMES->includes(wallet["chain"]->toLowerCase())), (validateAddress(wallet["address"], wallet["chain"])));
            }
            )}
        };
        return walletLinkingData;
    }
    catch (const any& error)
    {
        console->error(std:("Error parsing wallet linking data:"), error);
        return nullptr;
    }
};


object generateUpdatedReadmeWithWalletInfo(string currentReadme, array<LinkedWallet> wallets)
{
    auto validatedWallets = z->array(LinkedWalletSchema)->parse(wallets);
    auto walletData = object{
        object::pair{std:("lastUpdated"), ((std::make_shared<Date>()))->toISOString()}, 
        object::pair{std:("wallets"), validatedWallets->map([=](auto wallet) mutable
        {
            return (utils::assign(object{
                object::pair{std:("chain"), wallet["chain"]->toLowerCase()->trim()}, 
                object::pair{std:("address"), wallet["address"]->trim()}
            }, ((wallet["signature"]) ? object{
                object::pair{std:("signature"), wallet["signature"]->trim()}
            } : object{})));
        }
        )}
    };
    auto validatedData = WalletLinkingDataSchema->parse(walletData);
    auto walletSection = string_empty + WALLET_SECTION_BEGIN_MARKER + std:("\
") + JSON->stringify(validatedData, nullptr, 2) + std:("\
") + WALLET_SECTION_END_MARKER + string_empty;
    auto startIndex = currentReadme->indexOf(WALLET_SECTION_BEGIN_MARKER);
    auto endIndex = currentReadme->indexOf(WALLET_SECTION_END_MARKER);
    if (AND((AND((startIndex != -1), (endIndex != -1))), (endIndex > startIndex))) {
        auto updatedReadme = currentReadme->substring(0, startIndex) + walletSection + currentReadme->substring(endIndex + WALLET_SECTION_END_MARKER->get_length());
        return object{
            object::pair{std:("updatedReadme"), std:("updatedReadme")}, 
            object::pair{std:("walletData"), std:("walletData")}
        };
    } else {
        auto separator = (AND((currentReadme->trim()), (!currentReadme->endsWith(std:("\
"))))) ? any(std:("\
\
")) ((currentReadme->trim()) ? std:("\
") : string_empty);
        return object{
            object::pair{std:("updatedReadme"), currentReadme->trim() + separator + walletSection}, 
            object::pair{std:("walletData"), std:("walletData")}
        };
    }
};


string generateReadmeWalletSection(array<LinkedWallet> wallets)
{
    auto validatedWallets = z->array(LinkedWalletSchema)->parse(wallets);
    auto walletData = object{
        object::pair{std:("lastUpdated"), ((std::make_shared<Date>()))->toISOString()}, 
        object::pair{std:("wallets"), validatedWallets->map([=](auto wallet) mutable
        {
            return (utils::assign(object{
                object::pair{std:("chain"), wallet["chain"]->toLowerCase()->trim()}, 
                object::pair{std:("address"), wallet["address"]->trim()}
            }, ((wallet["signature"]) ? object{
                object::pair{std:("signature"), wallet["signature"]->trim()}
            } : object{})));
        }
        )}
    };
    auto validatedData = WalletLinkingDataSchema->parse(walletData);
    return string_empty + WALLET_SECTION_BEGIN_MARKER + std:("\
") + JSON->stringify(validatedData, nullptr, 2) + std:("\
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
    object::pair{std:("chain"), z->string()->min(1)->toLowerCase()}, 
    object::pair{std:("address"), z->string()->min(1)}, 
    object::pair{std:("signature"), z->string()->min(1)->optional()}
});
any WalletLinkingDataSchema = z->object(object{
    object::pair{std:("lastUpdated"), z->string()->datetime()}, 
    object::pair{std:("wallets"), z->array(LinkedWalletSchema)}
});
string WALLET_SECTION_BEGIN_MARKER = std:("<!-- WALLET-LINKING-BEGIN");
string WALLET_SECTION_END_MARKER = std:("WALLET-LINKING-END -->");

void Main(void)
{
}

MAIN
