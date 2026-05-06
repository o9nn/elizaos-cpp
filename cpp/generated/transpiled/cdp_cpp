#include "/home/runner/work/elizaos-cpp/elizaos-cpp/otaku/src/packages/api-client/src/services/cdp.h"

std::shared_ptr<Promise<std::shared_ptr<WalletInfo>>> CdpService::getOrCreateWallet(string name)
{
    auto response = std::async([=]() { this->post<std::shared_ptr<WalletInfo>>(std::string("/api/cdp/wallet"), object{
        object::pair{std::string("name"), std::string("name")}
    }); });
    return response;
}

std::shared_ptr<Promise<std::shared_ptr<TokensResponse>>> CdpService::getTokens(string chain)
{
    auto params = (chain) ? any(std::string("?chain=") + encodeURIComponent(chain) + string_empty) : any(string_empty);
    auto response = std::async([=]() { this->get<std::shared_ptr<TokensResponse>>(std::string("/api/cdp/wallet/tokens") + params + string_empty); });
    return response;
}

std::shared_ptr<Promise<std::shared_ptr<TokensResponse>>> CdpService::syncTokens(string chain)
{
    auto body = (chain) ? object{
        object::pair{std::string("chain"), std::string("chain")}
    } : object{};
    auto response = std::async([=]() { this->post<std::shared_ptr<TokensResponse>>(std::string("/api/cdp/wallet/tokens/sync"), body); });
    return response;
}

std::shared_ptr<Promise<std::shared_ptr<NFTsResponse>>> CdpService::getNFTs(string chain)
{
    auto params = (chain) ? any(std::string("?chain=") + encodeURIComponent(chain) + string_empty) : any(string_empty);
    auto response = std::async([=]() { this->get<std::shared_ptr<NFTsResponse>>(std::string("/api/cdp/wallet/nfts") + params + string_empty); });
    return response;
}

std::shared_ptr<Promise<std::shared_ptr<NFTsResponse>>> CdpService::syncNFTs(string chain)
{
    auto body = (chain) ? object{
        object::pair{std::string("chain"), std::string("chain")}
    } : object{};
    auto response = std::async([=]() { this->post<std::shared_ptr<NFTsResponse>>(std::string("/api/cdp/wallet/nfts/sync"), body); });
    return response;
}

std::shared_ptr<Promise<std::shared_ptr<TransactionHistoryResponse>>> CdpService::getHistory()
{
    auto response = std::async([=]() { this->get<std::shared_ptr<TransactionHistoryResponse>>(std::string("/api/cdp/wallet/history")); });
    return response;
}

std::shared_ptr<Promise<std::shared_ptr<SendTokenResponse>>> CdpService::sendToken(std::shared_ptr<SendTokenRequest> request)
{
    auto response = std::async([=]() { this->post<std::shared_ptr<SendTokenResponse>>(std::string("/api/cdp/wallet/send"), request); });
    return response;
}

std::shared_ptr<Promise<std::shared_ptr<SendNFTResponse>>> CdpService::sendNFT(std::shared_ptr<SendNFTRequest> request)
{
    auto response = std::async([=]() { this->post<std::shared_ptr<SendNFTResponse>>(std::string("/api/cdp/wallet/send-nft"), request); });
    return response;
}

std::shared_ptr<Promise<std::shared_ptr<SwapPriceResponse>>> CdpService::getSwapPrice(std::shared_ptr<SwapPriceRequest> request)
{
    auto response = std::async([=]() { this->post<std::shared_ptr<SwapPriceResponse>>(std::string("/api/cdp/wallet/swap-price"), request); });
    return response;
}

std::shared_ptr<Promise<std::shared_ptr<SwapResponse>>> CdpService::swap(std::shared_ptr<SwapRequest> request)
{
    auto response = std::async([=]() { this->post<std::shared_ptr<SwapResponse>>(std::string("/api/cdp/wallet/swap"), request); });
    return response;
}

std::shared_ptr<Promise<std::shared_ptr<SearchTokenResponse>>> CdpService::searchTokens(std::shared_ptr<SearchTokenRequest> request)
{
    auto params = std::make_shared<URLSearchParams>();
    params->append(std::string("query"), request->query);
    if (request->chain) {
        params->append(std::string("chain"), request->chain);
    }
    auto response = std::async([=]() { this->get<std::shared_ptr<SearchTokenResponse>>(std::string("/api/cdp/tokens/search?") + params->toString() + string_empty); });
    return response;
}

std::shared_ptr<Promise<std::shared_ptr<TopAndTrendingTokensResponse>>> CdpService::getTopAndTrendingTokens(std::shared_ptr<TopAndTrendingTokensRequest> request)
{
    auto params = std::make_shared<URLSearchParams>();
    params->append(std::string("chain"), request->chain);
    if (request->limit) {
        params->append(std::string("limit"), request->limit->toString());
    }
    auto response = std::async([=]() { this->get<std::shared_ptr<TopAndTrendingTokensResponse>>(std::string("/api/cdp/tokens/top-and-trending?") + params->toString() + string_empty); });
    return response;
}

CdpService::CdpService(std::shared_ptr<ApiClientConfig> config) : BaseApiClient(config) {
}

string NATIVE_TOKEN_ADDRESS = std::string("0xEeeeeEeeeEeEeeEeEeEeeEEEeeeeEeeeeeeeEEeE");

void Main(void)
{
}

MAIN
