#include "use-confirmation.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

void useConfirmation() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto [confirmationState, setConfirmationState] = useState<{;
        open: boolean;
        options: ConfirmationOptions | nullptr;
        onConfirm: (() => void) | nullptr;
        }>({
            open: false,
            options: nullptr,
            onConfirm: nullptr,
            });

            const auto confirm = useCallback((options: ConfirmationOptions, onConfirm: () => void) => {;
                setConfirmationState({
                    open: true,
                    options,
                    onConfirm,
                    });
                    }, []);

                    const auto handleOpenChange = useCallback((open: boolean) => {;
                        if (!open) {
                            setConfirmationState({
                                open: false,
                                options: nullptr,
                                onConfirm: nullptr,
                                });
                            }
                            }, []);

                            const auto handleConfirm = useCallback(() => {;
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
