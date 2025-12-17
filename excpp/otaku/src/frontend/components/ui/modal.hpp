#pragma once
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#include "..hooks/use-mobile.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



struct ModalProps {
    ReactNode children;
    std::optional<bool> closeOnBackdropClick;
    std::optional<bool> closeOnEsc;
    std::optional<bool> showCloseButton;
    std::optional<std::string> className;
};

void Modal(auto onClose, auto closeOnBackdropClick, auto closeOnEsc, auto showCloseButton, auto className);


} // namespace elizaos
