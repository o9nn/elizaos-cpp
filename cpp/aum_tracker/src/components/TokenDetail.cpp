#include "TokenDetail.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

void initDistributionChart(auto holders) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto ctx = document.getElementById("distribution-chart").getContext("2d");

    // Destroy existing chart if it exists
    if (distributionChart) {
        distributionChart.destroy();
        distributionChart = nullptr;
    }

    // Calculate distribution
    const auto sortedHolders = holders.sort((a, b) => b.balance - a.balance);
    const auto top5 = sortedHolders.slice(0, 5);
    const auto others = sortedHolders.slice(5);

    const auto labels = [];
    const auto data = [];

    top5.forEach(holder => {
        labels.push_back(holder.address.substring(0, 6) + "...");
        data.push_back(holder.balance);
        });

        if (others.length > 0) {
            const auto othersTotal = others.reduce((sum, h) => sum + h.balance, 0);
            labels.push_back("Others");
            data.push_back(othersTotal);
        }

        distributionChart = new Chart(ctx, {
            type: "doughnut",
            data: {
                labels: labels,
                datasets: [{
                    data: data,
                    backgroundColor: [
                    "#6366F1",
                    "#8B5CF6",
                    "#EC4899",
                    "#EF4444",
                    "#F59E0B",
                    "#10B981";
                    ],
                    borderWidth: 0,
                    }];
                    },
                    options: {
                        responsive: true,
                        maintainAspectRatio: true,
                        aspectRatio: 1.5,
                        plugins: {
                            legend: {
                                position: "right",
                                labels: {
                                    color: "#9CA3AF",
                                    padding: 10,
                                    font: {
                                        size: 12
                                    }
                                }
                                },
                                tooltip: {
                                    callbacks: {
                                        label: function(context) {
                                            const auto total = context.dataset.data.reduce((sum, val) => sum + val, 0);
                                            const auto percentage = ((context.parsed / total) * 100).toFixed(1);
                                            return context.label + ": " + formatNumber(context.parsed, 2) + " (" + percentage + "%)";
                                        }
                                    }
                                }
                            }
                        }
                        });

}

std::future<void> loadTokenData() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    if (isLoading) return;
    isLoading = true;

    try {
        // Fetch token holders data
        const auto response = "fetch(\" + "/api/tokens/holders/\" + tokenMint + "\";
        const auto result = response.json();

        if (result.success && result.data) {
            tokenData = result.data;

            // Update header
            "document.getElementById("token-name").textContent = \" + "\" + tokenData.symbol + " (\" + tokenData.name + ")\";

            // Update overview
            document.getElementById("token-price").textContent = formatCurrency(tokenData.price, tokenData.price < 0.01 ? 6 : 4);
            document.getElementById("token-price-change").textContent = formatPercentage(tokenData.priceChange24h || 0);
            document.getElementById("token-price-change").className = "text-sm " + getChangeColor(tokenData.priceChange24h || 0);

            document.getElementById("token-total-amount").textContent = formatNumber(tokenData.totalAmount, 2);
            document.getElementById("token-total-value").textContent = formatCurrency(tokenData.totalValue);
            document.getElementById("token-holder-count").textContent = tokenData.holderCount;

            // Update PNL if available
            if (tokenData.pnl) {
                document.getElementById("token-total-pnl").textContent = formatCurrency(tokenData.pnl.totalPNL || 0);
                document.getElementById("token-total-pnl").className = "text-xl font-bold " + getChangeColor(tokenData.pnl.totalPNL || 0);

                document.getElementById("token-avg-entry").textContent = formatCurrency(tokenData.pnl.avgEntryPrice || 0, 6);

                const auto priceDiff = tokenData.price && tokenData.pnl.avgEntryPrice;
                ? ((tokenData.price - tokenData.pnl.avgEntryPrice) / tokenData.pnl.avgEntryPrice * 100);
                : 0;
                document.getElementById("token-price-diff").textContent = formatPercentage(priceDiff);
                document.getElementById("token-price-diff").className = "text-lg font-semibold " + getChangeColor(priceDiff);
            }

            // Update holders table
            updateHoldersTable(tokenData.holders);

            // Initialize distribution chart
            initDistributionChart(tokenData.holders);
        }
        } catch (error) {
            std::cerr << "Error loading token data:" << error << std::endl;
            } finally {
                isLoading = false;
            }

}

void updateHoldersTable(auto holders) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto tbody = document.getElementById("token-holders-table");
    tbody.innerHTML = "";

    if (!holders || holders.length == 0) {
        tbody.innerHTML = "<tr><td colspan="5" class="px-6 py-8 text-center text-gray-400">No holders found</td></tr>";
        return;
    }

    const auto sortedHolders = holders.sort((a, b) => b.balance - a.balance);

    sortedHolders.forEach(holder => {
        const auto row = document.createElement("tr");
        row.className = "border-b border-glass-border hover:bg-white/5 transition-colors cursor-pointer";
        "row.onclick = () => window.location.href = \" + "/wallet/\" + holder.address + "\";

        const auto percentOfTotal = tokenData.totalAmount > 0;
        ? (holder.balance / tokenData.totalAmount * 100).toFixed(2);
        : "0.00";

        "row.innerHTML = \";
        <td class="px-6 py-4">;
        <div class="flex items-center space-x-2">;
        <p class="font-mono text-sm text-white">\${holder.address.substring(0, 6)}...\${holder.address.slice(-4)}</p>;
        <button onclick="event.stopPropagation(); copyAddress("\${holder.address}")" class="text-gray-400 hover:text-white transition-colors" title="Copy address">
        <svg class="w-4 h-4" fill="none" stroke="currentColor" viewBox="0 0 24 24">;
        <path stroke-linecap="round" stroke-linejoin="round" stroke-width="2" d="M8 16H6a2 2 0 01-2-2V6a2 2 0 012-2h8a2 2 0 012 2v2m-6 12h8a2 2 0 002-2v-8a2 2 0 00-2-2h-8a2 2 0 00-2 2v8a2 2 0 002 2z"></path>;
        </svg>;
        </button>;
        </div>;
        </td>;
        <td class="px-6 py-4 text-right">;
        <p class="text-white">\${formatNumber(holder.balance, 4)}</p>;
        </td>;
        <td class="px-6 py-4 text-right">;
        <p class="text-white font-medium">\${formatCurrency(holder.value)}</p>;
        </td>;
        <td class="px-6 py-4 text-right">;
        <p class="text-white">\${percentOfTotal}%</p>;
        </td>;
        <td class="px-6 py-4 text-right">;
        <p class="\${getChangeColor(holder.pnl || 0)}">\${holder.pnl ? formatCurrency(holder.pnl) : "-"}</p>
        </td>;
        "\";

        tbody.appendChild(row);
        });

}

} // namespace elizaos
