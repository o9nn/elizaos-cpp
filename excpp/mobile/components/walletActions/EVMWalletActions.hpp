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

default  = useEmbeddedEthereumWallet();
  const wallet = wallets?.[0];

  const signMessage = async () => {
    const provider = await wallet?.getProvider?.();
    if (!provider) return;

    const message = "Hello World";
    const signature = await provider.request({
      method: "personal_sign",
      params: [message, wallet?.address],
    });
    setResult(signature);
  };
  const signTransaction = async () => {
    const provider = await wallet?.getProvider?.();
    if (!provider) return;

    // Sign transaction (will be signed and populated)
    try {
      const { signature } = await provider.request({
        method: "eth_signTransaction",
        params: [
          {
            from: wallet.address,
            to: "0x0000000000000000000000000000000000000000",
            value: "1",
          },
        ],
      });
      setResult(signature);
    } catch (error) {
      setResult(JSON.stringify(error));
    }
  };
  const signAndSendTransaction = async () => {
    const provider = await wallet?.getProvider?.();
    if (!provider) return;
    const response = await provider.request({
      method: "eth_sendTransaction",
      params: [
        {
          from: wallet.address,
          to: "0x0000000000000000000000000000000000000000",
          value: "1",
        },
      ],
    });
    setResult(JSON.stringify(response));
  };
  return (
    <View>
      <Text>EVM Wallet Actions</Text>
      <Button title="Sign Message" onPress={signMessage} />
      <Button title="Sign Transaction" onPress={signTransaction} />
      <Button
        title="Sign And Send Transaction"
        onPress={signAndSendTransaction}
      />
      {result && <Text>{result}</Text>}
    </View>
  );
}

} // namespace elizaos
