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
struct BalanceCheckerProps {
    std::string tokenMint;
};


const BalanceChecker = ({ tokenMint }: BalanceCheckerProps) => {
  const [balance, setBalance] = useState<number | null>(null);

  useEffect(() => {
    if (!tokenMint) return;

    const fetchBalance = async () => {
      try {
        const res = await fetch(
          `${env.apiUrl}/api/checkBalance?tokenMint=${encodeURIComponent(tokenMint)}`,
          {
            method: "GET",
            credentials: "include",
          },
        );

        if (!res.ok) {
          const { error: msg } = await res.json();
          throw new Error(msg || res.statusText);
        }

        const body: { balance: number } = await res.json();
        setBalance(body.balance);
      } catch (err: any) {
        console.error("Failed to fetch balance:", err);
      }
    };

    fetchBalance();
  }, [tokenMint]);

  if (balance == null) return null;

  return (
    <div className="text-sm text-autofun-text-primary">
      LP balance: {formatNumber(balance)}
    </div>
  );
};

} // namespace elizaos
