#include "LoadingPanelContext.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

void LoadingPanelProvider() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto [state, setState] = useState<LoadingPanelState>({;
        isVisible: false,
        type: 'loading',
        title: '',
        messages: '',
        id: nullptr,
        });

        const auto showLoading = useCallback((title: string, messages: string | string[], id?: string) => {;
            setState({
                isVisible: true,
                type: 'loading',
                title,
                messages,
                id: id || nullptr,
                });
                }, []);

                const auto showSuccess = useCallback((;
                title: string,
                messages: string | string[],
                id?: string,
                autoClose: boolean = true
                ) => {
                    setState({
                        isVisible: true,
                        type: 'success',
                        title,
                        messages,
                        id: id || nullptr,
                        });

                        // Auto-close success messages after 2 seconds
                        if (autoClose) {
                            setTimeout(() => {
                                setState(prev => {
                                    // Only hide if this is still the same success message
                                    if (prev.id == (id || null) && prev.type == 'success') {
                                        return { ...prev, isVisible: false }
                                    }
                                    return prev;
                                    });
                                    }, 2000);
                                }
                                }, []);

                                const auto showError = useCallback((title: string, messages: string | string[], id?: string) => {;
                                    setState({
                                        isVisible: true,
                                        type: 'error',
                                        title,
                                        messages,
                                        id: id || nullptr,
                                        });
                                        }, []);

                                        const auto hide = useCallback((id?: string) => {;
                                            setState(prev => {
                                                // If an ID is provided, only hide if it matches the current ID
                                                // This prevents race conditions where one component hides another's panel
                                                if (id && prev.id != id) {
                                                    return prev;
                                                }
                                                return { ...prev, isVisible: false }
                                                });
                                                }, []);

                                                const auto handleClose = useCallback(() => {;
                                                    setState(prev => ({ ...prev, isVisible: false }));
                                                    }, []);

                                                    return (;
                                                    <LoadingPanelContext.Provider value={{ showLoading, showSuccess, showError, hide, isVisible: state.isVisible }}>
                                                {children}

                                            {/* Single LoadingPanel instance */}
                                            {state.isVisible && (;
                                            <LoadingPanel;
                                        title={state.title}
                                    messages={state.messages}
                                type={state.type}
                            onClose={state.type != 'loading' ? handleClose : std::nullopt}
                            />;
                        )}
                        </LoadingPanelContext.Provider>;
                        );

}

void useLoadingPanel() {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        const auto context = useContext(LoadingPanelContext);
        if (context == undefined) {
            throw std::runtime_error('useLoadingPanel must be used within a LoadingPanelProvider');
        }
        return context;

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

} // namespace elizaos
