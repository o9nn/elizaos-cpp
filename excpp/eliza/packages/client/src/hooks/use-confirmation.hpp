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
;

struct ConfirmationOptions {
    std::string title;
    std::string description;
    std::optional<std::string> confirmText;
    std::optional<std::string> cancelText;
    std::optional<'default' | 'destructive'> variant;
};


>({
    open: false,
    options: null,
    onConfirm: null,
  });

  const confirm = useCallback((options: ConfirmationOptions, onConfirm: () => void) => {
    setConfirmationState({
      open: true,
      options,
      onConfirm,
    });
  }, []);

  const handleOpenChange = useCallback((open: boolean) => {
    if (!open) {
      setConfirmationState({
        open: false,
        options: null,
        onConfirm: null,
      });
    }
  }, []);

  const handleConfirm = useCallback(() => {
    if (confirmationState.onConfirm) {
      confirmationState.onConfirm();
    }
    handleOpenChange(false);
  }, [confirmationState.onConfirm, handleOpenChange]);

  return {
    confirm,
    isOpen: confirmationState.open,
    onOpenChange: handleOpenChange,
    onConfirm: handleConfirm,
    options: confirmationState.options,
  };
}

} // namespace elizaos
