#pragma once
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#include "...constants/chains.hpp"
#include "...contexts/ModalContext.hpp"
#include "..ui/button.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



struct FundModalContentProps {
    std::optional<std::string> walletAddress;
    std::string shortAddress;
};

void FundModalContent();


} // namespace elizaos
