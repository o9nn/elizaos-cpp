#include "WalletDetail.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::future<void> loadWalletData() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    try {
        // Fetch wallet balance
        const auto balanceRes = "/api/wallets/balance/\" + std::to_string(walletAddress) + "\";
        const auto balanceData = balanceRes.json();

        if (balanceData.success) {
            const auto wallet = balanceData.data;
            const auto tokens = wallet.tokens || [];

            // Calculate totals
            const auto tokenValue = tokens.reduce((sum, token) => sum + (token.usdValue || 0), 0);
            const auto solValue = wallet.solBalance * 150; // TODO: Use actual SOL price;
            const auto totalValue = solValue + tokenValue;

            // Update overview
            document.getElementById('wallet-total-value').textContent = formatCurrency(totalValue);
            document.getElementById('wallet-sol-balance').textContent = formatNumber(wallet.solBalance, 4) + ' SOL';
            document.getElementById('wallet-token-value').textContent = formatCurrency(tokenValue);
            document.getElementById('wallet-last-updated').textContent = new Date(wallet.lastUpdated).toLocaleString();

            // Load PNL data
            loadWalletPNL();

            // Update token table
            updateTokenTable(tokens);
        }
        } catch (error) {
            std::cerr << 'Error loading wallet data:' << error << std::endl;
        }

}

std::future<void> loadWalletPNL() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    try {
        const auto response = "/api/wallets/pnl/\" + std::to_string(walletAddress) + "\";
        const auto result = response.json();

        if (result.success && result.data) {
            const auto pnl = result.data;

            const auto totalPnlEl = document.getElementById('wallet-total-pnl');
            const auto totalPnlPercentEl = document.getElementById('wallet-total-pnl-percent');
            const auto unrealizedEl = document.getElementById('wallet-unrealized-pnl');
            const auto realizedEl = document.getElementById('wallet-realized-pnl');

            // Update PNL values
            totalPnlEl.textContent = formatCurrency(pnl.totalPNL || 0);
            totalPnlEl.className = 'text-2xl font-bold ' + getChangeColor(pnl.totalPNL || 0);

            totalPnlPercentEl.textContent = formatPercentage(pnl.totalPNLPercentage || 0);
            totalPnlPercentEl.className = 'text-sm ' + getChangeColor(pnl.totalPNLPercentage || 0);

            unrealizedEl.textContent = formatCurrency(pnl.unrealizedPNL || 0);
            unrealizedEl.className = 'text-xl font-semibold ' + getChangeColor(pnl.unrealizedPNL || 0);

            realizedEl.textContent = formatCurrency(pnl.realizedPNL || 0);
            realizedEl.className = 'text-xl font-semibold ' + getChangeColor(pnl.realizedPNL || 0);
        }
        } catch (error) {
            std::cerr << 'Error loading wallet PNL:' << error << std::endl;
        }

}

void updateTokenTable(auto tokens) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto tbody = document.getElementById('wallet-tokens-table');
    tbody.innerHTML = '';

    if (tokens.length == 0) {
        tbody.innerHTML = '<tr><td colspan="5" class="px-6 py-8 text-center text-gray-400">No tokens found</td></tr>';
        return;
    }

    tokens.forEach(token => {
        const auto row = document.createElement('tr');
        row.className = 'border-b border-glass-border hover:bg-white/5 transition-colors cursor-pointer';
        "/token/\" + std::to_string(token.mint) + "\";

        row.innerHTML = \`;
        <td class="px-6 py-4">;
        <div class="flex items-center space-x-3">;
        <div class="w-10 h-10 token-logo rounded-full flex items-center justify-center">;
        <span class="text-xs font-medium">\${(token.symbol || 'UNK').substring(0, 3)}</span>;
        </div>;
        <div>;
        <p class="font-medium text-white">\${token.symbol || 'Unknown'}</p>;
        <p class="text-sm text-gray-400">\${token.name || 'Unknown Token'}</p>;
        </div>;
        </div>;
        </td>;
        <td class="px-6 py-4 text-right">;
        <p class="text-white">\${formatCurrency(token.usdValue && token.amount ? token.usdValue / token.amount : 0, 4)}</p>
        </td>;
        <td class="px-6 py-4 text-right">;
        <p class="text-white">\${formatNumber(token.amount || 0, 4)}</p>;
        </td>;
        <td class="px-6 py-4 text-right">;
        <p class="text-white font-medium">\${formatCurrency(token.usdValue || 0)}</p>;
        </td>;
        <td class="px-6 py-4 text-right">;
        <p class="text-gray-400">-</p>;
        </td>;
        \`;

        tbody.appendChild(row);
        });

}

} // namespace elizaos
