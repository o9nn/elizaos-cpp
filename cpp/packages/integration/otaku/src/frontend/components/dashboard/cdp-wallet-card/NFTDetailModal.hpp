#include "...contexts/LoadingPanelContext.hpp"
#include "...contexts/ModalContext.hpp"
#include "...lib/elizaClient.hpp"
#include "..ui/button.hpp"
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#pragma once

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
    string; // ERC721, ERC1155 tokenType;
    std::string chain;
    std::optional<std::string> balance;
    std::optional<Array<{> attributes;
    std::string trait_type;
    string | number value;
};

struct NFTDetailModalContentProps {
    NFT nft;
    std::string userId;
    std::optional<() => void> onSuccess;
};

void NFTDetailModalContent(auto { nft, auto userId, NFTDetailModalContentProps onSuccess });

} // namespace elizaos
