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

const WALLET_ADDRESS = 'AM84n1iLdxgVTAyENBcLdjXoyvjentTbu5Q6EpKV1PeG';

const connection = new Connection(
    process.env.NEXT_PUBLIC_RPC_ENDPOINT || 'https://solana-mainnet.g.alchemy.com/v2/your-api-key',
    'confirmed'
);

async ,
            });
        }

        // Fetch fresh data
        const tokenBalances = await getTokenData(connection);
        const marketData = await fetchDexScreenerData(
            tokenBalances.map(t => t.mint)
        );
        const holdings = await calculateHoldings(
            connection,
            tokenBalances,
            marketData,
            WALLET_ADDRESS
        );

        // Update cache
        setCachedData(holdings);

        return new NextResponse(JSON.stringify(holdings), {
            headers: {
                'Content-Type': 'application/json',
                'Cache-Control': 'public, max-age=60',
            },
        });
    } catch (error) {
        console.error('API Error:', error);
        // Return cached data on error if available
        const cached = getCachedData();
        if (cached) {
            return new NextResponse(JSON.stringify(cached.holdings), {
                headers: {
                    'Content-Type': 'application/json',
                    'Cache-Control': 'public, max-age=60',
                },
            });
        }
        return NextResponse.error();
    }
} 
} // namespace elizaos
