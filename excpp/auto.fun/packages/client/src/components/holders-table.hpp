#include "loader.hpp"
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



  const percentage = (value / total) * 100;
  return percentage?.toFixed(2);
}

default : { token: IToken }) {
  const query = useQuery({
    queryKey: ["token", token.mint, "holders"],
    queryFn: async () => {
      const holders = await getHolders({ address: token.mint });

      return holders?.holders;
    },
    refetchInterval: 30_000,
  });

  const isLoading = query.isLoading;
  const supply = token?.tokenSupplyUiAmount;
  const data = query?.data || [];

  if (isLoading) {
    return <Loader className="h-40" />;
  }

  if ((data || [])?.length === 0) {
    return (
      <div className="flex flex-col items-center gap-2">
        <img
          className="w-auto grayscale size-16 select-none"
          src="/dice.svg"
          alt="logo"
        />
        <p className="text-sm font-dm-mono text-autofun-text-secondary">
          No holders were found.
        </p>
      </div>
    );
  }

  return (
    <Table className="border-0 !rounded-0 !border-spacing-y-0">
      <TableHeader className="relative">
        {/* <PausedIndicator show={paused} /> */}
        <TableRow className="bg-transparent">
          <TableHead className="text-left w-[120px]">Account</TableHead>
          <TableHead className="text-right">Amount</TableHead>
          <TableHead className="text-right w-[80px]">%</TableHead>
          <TableHead className="text-right w-[50px]" />
        </TableRow>
      </TableHeader>
      <TableBody>
        {data?.length > 0
          ? data.map((holder) => {
              const formattedAmount: number =
                (Number(holder?.balance) ? Number(holder.balance) : 0) /
                10 ** (token.tokenDecimals || 6);
              return (
                <TableRow className="hover:bg-white/5" key={holder?.address}>
                  <TableCell className="text-left text-sm">
                    <Link
                      to={env.getWalletUrl(holder.address)}
                      target="_blank"
                      className="hover:text-autofun-text-highlight"
                    >
                      {holder?.address ===
                      import.meta.env.VITE_BONDING_CURVE_ADDRESS
                        ? "Bonding Curve"
                        : shortenAddress(holder?.address)}
                    </Link>
                  </TableCell>
                  <TableCell className="text-right text-sm">
                    {formattedAmount.toLocaleString()}
                  </TableCell>
                  <TableCell className="text-right text-sm">
                    {getPercentageOfTotal(formattedAmount, supply)}%
                  </TableCell>
                  <TableCell className="text-sm">
                    <Link to={env.getWalletUrl(holder.address)} target="_blank">
                      <ExternalLink className="ml-auto size-4 text-autofun-icon-secondary" />
                    </Link>
                  </TableCell>
                </TableRow>
              );
            })
          : null}
      </TableBody>
    </Table>
  );
}

} // namespace elizaos
