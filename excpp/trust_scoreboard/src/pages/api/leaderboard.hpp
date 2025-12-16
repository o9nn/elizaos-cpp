#include "partners.hpp"
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

// pages/api/leaderboard.ts
import type { NextApiRequest, NextApiResponse } from 'next';
;

const HELIUS_API = `https://mainnet.helius-rpc.com/?api-key=${process.env.NEXT_PUBLIC_SOLANA_API}`;
const TOKEN_ADDRESSES = [
  'HeLp6NuQkmYB4pYWo2zYs22mESHXPQYzXbB8n4V98jwC',
  'Gu3LDkn7Vx3bmCzLafYNKcDxv2mH7YN44NJZFXnyai16z'
];

struct TokenPrice {
    std::string address;
    double usd;
};


struct TokenPrices {
    double usd;
};


struct ErrorResponse {
    std::string message;
    double statusCode;
};


std::future<double> getTokenPrice(const std::string& address);,
      body: JSON.stringify({
        jsonrpc: '2.0',
        id: 'price-fetch',
        method: 'getAsset',
        params: {
          id: address,
          displayOptions: { showFungible: true }
        }
      })
    });

    const { result } = await response.json();
    return result?.token_info?.price_info?.price_per_token || 0;
  } catch (error) {
    console.error(`Error fetching price for ${address}:`, error);
    return 0;
  }
}

default async >) {
  try {
    if (req.method !== 'GET') {
      return res.status(405).json({ message: 'Method not allowed', statusCode: 405 });
    }

    // Get partners first with error handling
    const partners = await getAllPartners();
    if (!partners) {
      return res.status(500).json({ message: 'Failed to fetch partners', statusCode: 500 });
    }

    // Fetch token prices from CoinGecko
    const prices: TokenPrice[] = await Promise.all(
      TOKEN_ADDRESSES.map(async (address) => ({
        address,
        usd: await getTokenPrice(address),
      }))
    );

    return res.status(200).json({ partners, prices });
  } catch (error) {
    return res.status(500).json({ message: 'Internal server error', statusCode: 500 });
  }
}

} // namespace elizaos
