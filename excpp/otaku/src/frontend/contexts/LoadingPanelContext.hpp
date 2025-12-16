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

struct LoadingPanelState {
    bool isVisible;
    'loading' | 'success' | 'error' type;
    std::string title;
    std::vector<string | string> messages;
    string | null; // Track which component is showing the panel id;
};


struct LoadingPanelContextType {
    (title: string, messages: string | string[], id?: string) => void showLoading;
    (title: string, messages: string | string[], id?: string, autoClose?: boolean) => void showSuccess;
    (title: string, messages: string | string[], id?: string) => void showError;
    (id?: string) => void hide;
    bool isVisible;
};


const LoadingPanelContext = createContext<LoadingPanelContextType | undefined>(undefined);

: { children: ReactNode }) {
  const [state, setState] = useState<LoadingPanelState>({
    isVisible: false,
    type: 'loading',
    title: '',
    messages: '',
    id: null,
  });

  const showLoading = useCallback((title: string, messages: string | string[], id?: string) => {
    setState({
      isVisible: true,
      type: 'loading',
      title,
      messages,
      id: id || null,
    });
  }, []);

  const showSuccess = useCallback((
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
      id: id || null,
    });

    // Auto-close success messages after 2 seconds
    if (autoClose) {
      setTimeout(() => {
        setState(prev => {
          // Only hide if this is still the same success message
          if (prev.id === (id || null) && prev.type === 'success') {
            return { ...prev, isVisible: false };
          }
          return prev;
        });
      }, 2000);
    }
  }, []);

  const showError = useCallback((title: string, messages: string | string[], id?: string) => {
    setState({
      isVisible: true,
      type: 'error',
      title,
      messages,
      id: id || null,
    });
  }, []);

  const hide = useCallback((id?: string) => {
    setState(prev => {
      // If an ID is provided, only hide if it matches the current ID
      // This prevents race conditions where one component hides another's panel
      if (id && prev.id !== id) {
        return prev;
      }
      return { ...prev, isVisible: false };
    });
  }, []);

  const handleClose = useCallback(() => {
    setState(prev => ({ ...prev, isVisible: false }));
  }, []);

  return (
    <LoadingPanelContext.Provider value={{ showLoading, showSuccess, showError, hide, isVisible: state.isVisible }}>
      {children}
      
      {/* Single LoadingPanel instance */}
      {state.isVisible && (
        <LoadingPanel
          title={state.title}
          messages={state.messages}
          type={state.type}
          onClose={state.type !== 'loading' ? handleClose : undefined}
        />
      )}
    </LoadingPanelContext.Provider>
  );
}

// Custom hook to use the loading panel

  return context;
}


} // namespace elizaos
