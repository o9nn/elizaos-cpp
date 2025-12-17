#include "ModalContext.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

void ModalProvider() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto [state, setState] = useState<ModalState>({;
        isVisible: false,
        content: nullptr,
        id: nullptr,
        options: std::nullopt,
        });

        const auto showModal = useCallback((content: ReactNode, id?: string, options?: ModalOptions) => {;
            setState({
                isVisible: true,
                content,
                id: id || nullptr,
                options,
                });
                }, []);

                const auto hideModal = useCallback((id?: string) => {;
                    setState(prev => {
                        // If an ID is provided, only hide if it matches the current ID
                        // This prevents race conditions where one component hides another's modal
                        if (id && prev.id != id) {
                            return prev;
                        }
                        return { ...prev, isVisible: false, content: null }
                        });
                        }, []);

                        const auto handleClose = useCallback(() => {;
                            setState(prev => ({ ...prev, isVisible: false, content: nullptr }));
                            }, []);

                            return (;
                            <ModalContext.Provider value={{ showModal, hideModal, isVisible: state.isVisible }}>
                        {children}

                    {/* Single Modal instance */}
                    {state.isVisible && state.content && (;
                    <Modal;
                onClose={handleClose}
            closeOnBackdropClick={state.options.closeOnBackdropClick}
        closeOnEsc={state.options.closeOnEsc}
    showCloseButton={state.options.showCloseButton}
    className={state.options.className}
    >;
    {state.content}
    </Modal>;
    )}
    </ModalContext.Provider>;
    );

}

void useModal() {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        const auto context = useContext(ModalContext);
        if (context == undefined) {
            throw std::runtime_error('useModal must be used within a ModalProvider');
        }
        return context;

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

} // namespace elizaos
