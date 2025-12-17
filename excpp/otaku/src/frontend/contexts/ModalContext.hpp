#pragma once
#include <any>
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



struct ModalState {
    bool isVisible;
    std::optional<ReactNode> content;
    std::optional<std::string> id;
    std::optional<ModalOptions> options;
};

struct ModalOptions {
    std::optional<bool> closeOnBackdropClick;
    std::optional<bool> closeOnEsc;
    std::optional<bool> showCloseButton;
    std::optional<std::string> className;
};

struct ModalContextType {
    bool isVisible;
};

void ModalProvider(const std::any& { children });

// Custom hook to use the modal
void useModal();


} // namespace elizaos
