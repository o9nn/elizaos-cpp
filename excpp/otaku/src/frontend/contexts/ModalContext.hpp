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

struct ModalState {
    bool isVisible;
    ReactNode | null content;
    string | null id;
    std::optional<ModalOptions> options;
};


struct ModalOptions {
    std::optional<bool> closeOnBackdropClick;
    std::optional<bool> closeOnEsc;
    std::optional<bool> showCloseButton;
    std::optional<std::string> className;
};


struct ModalContextType {
    (content: ReactNode, id?: string, options?: ModalOptions) => void showModal;
    (id?: string) => void hideModal;
    bool isVisible;
};


const ModalContext = createContext<ModalContextType | undefined>(undefined);

: { children: ReactNode }) {
  const [state, setState] = useState<ModalState>({
    isVisible: false,
    content: null,
    id: null,
    options: undefined,
  });

  const showModal = useCallback((content: ReactNode, id?: string, options?: ModalOptions) => {
    setState({
      isVisible: true,
      content,
      id: id || null,
      options,
    });
  }, []);

  const hideModal = useCallback((id?: string) => {
    setState(prev => {
      // If an ID is provided, only hide if it matches the current ID
      // This prevents race conditions where one component hides another's modal
      if (id && prev.id !== id) {
        return prev;
      }
      return { ...prev, isVisible: false, content: null };
    });
  }, []);

  const handleClose = useCallback(() => {
    setState(prev => ({ ...prev, isVisible: false, content: null }));
  }, []);

  return (
    <ModalContext.Provider value={{ showModal, hideModal, isVisible: state.isVisible }}>
      {children}
      
      {/* Single Modal instance */}
      {state.isVisible && state.content && (
        <Modal
          onClose={handleClose}
          closeOnBackdropClick={state.options?.closeOnBackdropClick}
          closeOnEsc={state.options?.closeOnEsc}
          showCloseButton={state.options?.showCloseButton}
          className={state.options?.className}
        >
          {state.content}
        </Modal>
      )}
    </ModalContext.Provider>
  );
}

// Custom hook to use the modal

  return context;
}


} // namespace elizaos
