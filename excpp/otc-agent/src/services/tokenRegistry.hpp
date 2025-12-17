#pragma once
#include <algorithm>
#include <any>
#include <cstdint>
#include <functional>
#include <future>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <variant>
#include <vector>
#include "database.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



class TokenRegistryService {
public:
    std::future<Token> registerToken(std::optional<std::any> params);
    std::future<Token> getToken(const std::string& tokenId);
    std::future<std::vector<Token>> getAllTokens(std::optional<std::any> filters);
    std::future<Token> updateTokenStatus(const std::string& tokenId, bool isActive);
    std::future<Token> updateToken(const std::string& tokenId, const std::optional<Token>& updates);
    std::variant<Promise<TokenMarketData, null>> getTokenMarketData(const std::string& tokenId);
    std::future<void> setTokenMarketData(TokenMarketData data);

private:
    std::string symbol_;
    std::string name_;
    std::string contractAddress_;
    Chain chain_;
    double decimals_;
};


} // namespace elizaos
