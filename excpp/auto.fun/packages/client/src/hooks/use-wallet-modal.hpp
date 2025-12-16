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

;

struct WalletModalState {
    bool visible;
    (visible: boolean) => void setVisible;
};


const useWalletModal = create<WalletModalState>(
  (set: (partial: Partial<WalletModalState>) => void) => ({
    visible: false,
    setVisible: (visible: boolean) => set({ visible }),
  }),
);

} // namespace elizaos
