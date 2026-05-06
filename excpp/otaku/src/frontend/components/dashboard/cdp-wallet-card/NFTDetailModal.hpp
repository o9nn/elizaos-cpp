#pragma once
#include <algorithm>
#include <cstdint>
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <variant>
#include <vector>
#include "contexts/LoadingPanelContext.hpp"
#include "contexts/ModalContext.hpp"
#include "lib/elizaClient.hpp"
#include "ui/button.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



// NFT interface
struct NFT {
    std::string tokenId;
    std::string name;
    std::string description;
    std::string image;
    std::string contractAddress;
    std::string contractName;
    std::string tokenType;
    std::string chain;
    std::optional<std::string> balance;
    std::string trait_type;
    std::variant<std::string, double> value;
};

struct NFTDetailModalContentProps {
    NFT nft;
    std::string userId;
};

void NFTDetailModalContent(auto userId);

} // namespace elizaos
