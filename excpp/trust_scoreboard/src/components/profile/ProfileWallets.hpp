#include "ProfileWallets.module.css.hpp"
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

struct WalletDisplayProps {
    std::optional<double> truncateLength;
};


const truncateAddress = (address: string, length: number = 4): string => {
  if (!address) return '';
  return `${address.slice(0, length)}...${address.slice(-length)}`;
};

const ProfileWallets: FC<WalletDisplayProps> = ({ truncateLength = 4 }) => {
  const { publicKey, disconnect, connect, wallet } = useWallet();

  const handleWalletClick = async () => {
    if (publicKey) {
      try {
        await disconnect();
      } catch (error) {
        console.error('Failed to disconnect wallet:', error);
      }
    } else if (wallet) {
      try {
        await connect();
      } catch (error) {
        console.error('Failed to connect wallet:', error);
      }
    }
  };

  return (
    <div>
      <h2 className={styles.title}></h2>
      <div className={styles.buttonParent}>
        {wallet ? (
          <div 
            className={`${styles.button} ${publicKey ? styles.active : ''}`}
            onClick={handleWalletClick}
            role="button"
            tabIndex={0}
          >
            <div className={styles.text}>
              {publicKey 
                ? truncateAddress(publicKey.toString(), truncateLength)
                : 'Connect Wallet'}
            </div>
          </div>
        ) : (
          <WalletMultiButton className={styles.button}>
            <div className={styles.text}>Select Wallet</div>
          </WalletMultiButton>
        )}
      </div>
    </div>
  );
};

default ProfileWallets;
} // namespace elizaos
