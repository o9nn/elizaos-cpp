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

struct AutoConnectContextState {
    bool autoConnect;
};


const AutoConnectContext = createContext<AutoConnectContextState>({} as AutoConnectContextState);



const AutoConnectProvider: FC<{ children: ReactNode }> = ({ children }) => {
    // TODO: fix auto connect to actual reconnect on refresh/other.
    // TODO: make switch/slider settings
    // const [autoConnect, setAutoConnect] = useLocalStorage('autoConnect', false);
    const [autoConnect, setAutoConnect] = useLocalStorage('autoConnect', true);

    return (
        <AutoConnectContext.Provider value={{ autoConnect, setAutoConnect }}>{children}</AutoConnectContext.Provider>
    );
};

} // namespace elizaos
