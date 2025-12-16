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


struct NetworkConfigurationState {
    std::string networkConfiguration;
};


const NetworkConfigurationContext = createContext<NetworkConfigurationState>({} as NetworkConfigurationState);



const NetworkConfigurationProvider: FC<{ children: ReactNode }> = ({ children }) => {
    const [networkConfiguration, setNetworkConfiguration] = useLocalStorage("network", "devnet");

    return (
        <NetworkConfigurationContext.Provider value={{ networkConfiguration, setNetworkConfiguration }}>{children}</NetworkConfigurationContext.Provider>
    );
};
} // namespace elizaos
