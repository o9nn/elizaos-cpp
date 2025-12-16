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

// pages/api/daoHoldings.ts
import type { NextApiRequest, NextApiResponse } from 'next';

struct TokenInfo {
    std::string name;
    std::string symbol;
    double balance;
    double decimals;
    std::optional<{> price_info;
    double total_price;
};


struct HeliusResponse {
    { result;
    { items;
    TokenInfo token_info;
    std::optional<double> amount;
    { nativeBalance;
    double total_price;
};


struct DAOHolding {
    double rank;
    std::string name;
    std::string holdings;
    std::string value;
    std::string percentage;
    std::string imageUrl;
};


default async );
  }

  const url = `https://mainnet.helius-rpc.com/?api-key=${process.env.NEXT_PUBLIC_SOLANA_API}`;
  const DAO_WALLET = 'AM84n1iLdxgVTAyENBcLdjXoyvjentTbu5Q6EpKV1PeG';

  try {
    const response = await fetch(url, {
      method: 'POST',
      headers: {
        'Content-Type': 'application/json',
      },
      body: JSON.stringify({
        jsonrpc: '2.0',
        id: 'my-id',
        method: 'searchAssets',
        params: {
          ownerAddress: DAO_WALLET,
          tokenType: 'fungible',
          displayOptions: {
            showNativeBalance: true,
          },
        },
      }),
    });

    if (!response.ok) {
      throw new Error('Failed to fetch from Helius API');
    }

    const data: HeliusResponse = await response.json();

    if (!data?.result?.items) {
      throw new Error('Invalid data format received from API');
    }

    const totalValue = data.result.nativeBalance.total_price;
    
    const holdings = data.result.items.map((item, index) => {
      const tokenInfo = item.token_info;
      const tokenValue = tokenInfo.price_info?.total_price || 0;
      const tokenAmount = item.amount || 0; // Get raw token amount
      const formattedAmount = formatTokenAmount(tokenAmount, tokenInfo.decimals); // Need to implement this

      return {
        rank: index + 1,
        name: tokenInfo.symbol || tokenInfo.name,
        holdings: `${formattedAmount} ${tokenInfo.symbol}`, // Show amount with symbol
        value: formatCurrency(tokenValue), // Keep monetary value
        percentage: calculatePercentage(tokenValue, totalValue),
        imageUrl: `/images/tokens/${tokenInfo.symbol || 'default'}.png`,
      };
    });

    return res.status(200).json({ holdings });

  } catch (error) {
    console.error('Error:', error);
    return res.status(500).json({ error: 'Failed to fetch holdings' });
  }
}

).format(amount);
}

%`;
}

// Helper 
} // namespace elizaos
