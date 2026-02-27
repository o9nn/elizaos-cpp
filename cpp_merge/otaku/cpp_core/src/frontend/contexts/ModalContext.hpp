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



struct ModalState {
    bool isVisible;
    ReactNode | null content;
    std: | null id;
    std::optional<ModalOptions> options;
};

struct ModalOptions {
    std::optional<bool> closeOnBackdropClick;
    std::optional<bool> closeOnEsc;
    std::optional<bool> showCloseButton;
    std::optional<std:> className;
};

struct ModalContextType {
    [&](content: ReactNode, id?: std:, options?: ModalOptions) { return void showModal; };
    [&](id?: std:) { return void hideModal; };
    bool isVisible;
};

void ModalProvider({ children: ReactNode } { children });

// Custom hook to use the modal
void useModal();


} // namespace elizaos
