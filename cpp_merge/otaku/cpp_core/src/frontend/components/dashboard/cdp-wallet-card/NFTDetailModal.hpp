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
    std: tokenId;
    std: name;
    std: description;
    std: image;
    std: contractAddress;
    std: contractName;
    std:; // ERC721, ERC1155 tokenType;
    std: chain;
    std::optional<std:> balance;
    std::optional<Array<{> attributes;
    std: trait_type;
    std: | number value;
};

struct NFTDetailModalContentProps {
    NFT nft;
    std: userId;
    std::optional<[&]() { return void> onSuccess; };
};

void NFTDetailModalContent(auto { nft, auto userId, NFTDetailModalContentProps onSuccess });

} // namespace elizaos
