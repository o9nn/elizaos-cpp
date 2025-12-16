#include "GetStarted.module.css.hpp"
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

struct GetStartedProps {
    std::string tier;
    double price;
    std::optional<bool> isHighlighted;
};


// Move this to a safe initialization
const getTreasuryWallet = (): PublicKey | null => {
  try {
    const walletAddress = process.env.NEXT_PUBLIC_TREASURY_WALLET_SOL;
    if (!walletAddress) {
      console.error('Treasury wallet address not found in environment variables');
      return null;
    }
    return new PublicKey(walletAddress);
  } catch (e) {
    console.error('Invalid treasury wallet address:', e);
    return null;
  }
};

const TREASURY_WALLET = getTreasuryWallet();
const HELIUS_RPC = process.env.NEXT_PUBLIC_SOLANA_API ? 
  `https://rpc.helius.xyz/?api-key=${process.env.NEXT_PUBLIC_SOLANA_API}` : 
  null;

const GetStarted: FC<GetStartedProps> = ({ tier, price, isHighlighted }) => {
  const { publicKey, sendTransaction } = useWallet();
  const { connection } = useConnection();
  const [isLoading, setIsLoading] = useState(false);

  const handlePayment = async () => {
    if (!publicKey || !TREASURY_WALLET || !HELIUS_RPC) {
      console.error('Missing required configuration');
      return;
    }
    
    setIsLoading(true);
    try {
      // ... rest of the payment logic
    } catch (error) {
      console.error('Payment failed:', error);
    } finally {
      setIsLoading(false);
    }
  };

  return (
    <button 
      onClick={handlePayment}
      className={`${styles.button} ${isHighlighted ? styles.highlighted : ''}`}
      disabled={!publicKey || isLoading}
    >
      {isLoading ? 'Processing...' : 'Get Started'}
    </button>
  );
};

default GetStarted;
} // namespace elizaos
