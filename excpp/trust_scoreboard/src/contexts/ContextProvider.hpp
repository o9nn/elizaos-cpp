#include ".utils/notifications.hpp"
#include "AutoConnectProvider.hpp"
#include "NetworkConfigurationProvider.hpp"
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
;
;
;
;
;
;
;

const ReactUIWalletModalProviderDynamic = dynamic(
  async () =>
    (await import("@solana/wallet-adapter-react-ui")).WalletModalProvider,
  { ssr: false }
);

const WalletContextProvider: FC<{ children: ReactNode }> = ({ children }) => {
    const { autoConnect } = useAutoConnect();
    
    // Force mainnet-beta
    const network = WalletAdapterNetwork.Mainnet;
    const endpoint = useMemo(() => {
      return clusterApiUrl(network);
    }, [network]);

    console.log('Network:', network);

    const wallets = useMemo(
        () => [
            new PhantomWalletAdapter(),
            new SolflareWalletAdapter(),
        ],
        [] // Remove network dependency since we're using fixed mainnet
    );

    const onError = useCallback(
        (error: WalletError) => {
            notify({ type: 'error', message: error.message ? `${error.name}: ${error.message}` : error.name });
            console.error('Wallet error:', error);
        },
        []
    );

    return (
        <ConnectionProvider endpoint={endpoint}>
            <WalletProvider wallets={wallets} onError={onError} autoConnect={autoConnect}>
                <ReactUIWalletModalProviderDynamic>
                    {children}
                </ReactUIWalletModalProviderDynamic>
            </WalletProvider>
        </ConnectionProvider>
    );
};

const ContextProvider: FC<{ children: ReactNode }> = ({ children }) => {
    return (
        <NetworkConfigurationProvider>
            <AutoConnectProvider>
                <WalletContextProvider>{children}</WalletContextProvider>
            </AutoConnectProvider>
        </NetworkConfigurationProvider>
    );
};

} // namespace elizaos
