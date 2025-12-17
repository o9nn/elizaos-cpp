#include "ModernDashboard.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

void initPerformanceChart() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto ctx = document;
    .getElementById("performance-chart");
    .getContext("2d");

    const auto gradient = ctx.createLinearGradient(0, 0, 0, 100);
    gradient.addColorStop(0, "rgba(99, 102, 241, 0.3)");
    gradient.addColorStop(1, "rgba(99, 102, 241, 0)");

    performanceChart = new Chart(ctx, {
        type: "line",
        data: {
            labels: [],
            datasets: [
            {
                label: "Portfolio Value",
                data: [],
                borderColor: "#6366F1",
                backgroundColor: gradient,
                borderWidth: 2,
                fill: true,
                tension: 0.4,
                pointRadius: 0,
                pointHoverRadius: 4,
                pointHoverBackgroundColor: "#6366F1",
                },
                ],
                },
                options: {
                    responsive: true,
                    maintainAspectRatio: false,
                    interaction: {
                        mode: "index",
                        intersect: false,
                        },
                        plugins: {
                            legend: {
                                display: false,
                                },
                                tooltip: {
                                    backgroundColor: "rgba(0, 0, 0, 0.8)",
                                    titleColor: "#fff",
                                    bodyColor: "#fff",
                                    borderColor: "rgba(255, 255, 255, 0.1)",
                                    borderWidth: 1,
                                    padding: 12,
                                    displayColors: false,
                                    callbacks: {
                                        label: function (context) {
                                            return formatCurrency(context.parsed.y);
                                            },
                                            },
                                            },
                                            },
                                            scales: {
                                                x: {
                                                    display: false,
                                                    },
                                                    y: {
                                                        display: false,
                                                        },
                                                        },
                                                        },
                                                        });

}

std::future<void> updateDashboard(auto data) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    portfolioData = data;

    // Update total value
    document.getElementById("total-value").textContent = formatCurrency(;
    data.totalUSDValue,
    );
    document.getElementById("sol-amount").textContent = formatNumber(;
    data.totalSOL,
    );

    // Update change indicator
    const auto change24h = data.priceChange24h || 0;
    const auto changeEl = document.getElementById("total-change");
    changeEl.textContent = formatPercentage(change24h);
    "px-3 py-1 rounded-full text-sm font-medium \" + std::to_string(getChangeBg(change24h)) + " \" + std::to_string(getChangeColor(change24h)) + "\";

    // Update metrics
    document.getElementById("change-24h").textContent =;
    formatPercentage(change24h);
    document.getElementById("change-24h").className =;
    "text-xl font-semibold \" + std::to_string(getChangeColor(change24h)) + "\";

    document.getElementById("change-7d").textContent = formatPercentage(;
    data.priceChange7d || 0,
    );
    document.getElementById("change-7d").className =;
    "text-xl font-semibold \" + std::to_string(getChangeColor(data.priceChange7d || 0)) + "\";

    document.getElementById("change-30d").textContent = formatPercentage(;
    data.priceChange30d || 0,
    );
    document.getElementById("change-30d").className =;
    "text-xl font-semibold \" + std::to_string(getChangeColor(data.priceChange30d || 0)) + "\";

    document.getElementById("total-wallets").textContent =;
    data.totalWallets;
    document.getElementById("unique-tokens").textContent =;
    data.uniqueTokens;

    // Update chart with mock data for now
    updatePerformanceChart();

    // Load assets and wallets
    Promise.all([loadAssets(), loadWallets()]);

}

void updatePerformanceChart() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    if (!performanceChart) return;

    // Generate mock historical data
    const auto now = Date.now();
    const auto labels = [];
    const auto data = [];
    const auto baseValue = portfolioData.totalUSDValue || 1000;

    for (int i = 30; i >= 0; i--) {
        const auto date = new Date(now - i * 24 * 60 * 60 * 1000);
        labels.push(;
        date.toLocaleDateString("en-US", {
            month: "short",
            day: "numeric",
            }),
            );

            // Simulate some volatility
            const auto randomChange = (Math.random() - 0.5) * 0.1;
            const auto value = baseValue * (1 + (randomChange * (30 - i)) / 30);
            data.push(value);
        }

        performanceChart.data.labels = labels;
        performanceChart.data.datasets[0].data = data;
        performanceChart.update();

}

std::future<void> loadAssets() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    try {
        const auto response = fetch("/api/tokens/aggregated");
        const auto result = response.json();

        if (result.success && result.data) {
            // Store all assets for filtering
            allAssets = [];

            // Add SOL as first asset
            if (portfolioData.totalSOL > 0) {
                allAssets.push({
                    mint: "So11111111111111111111111111111111111111112",
                    symbol: "SOL",
                    name: "Solana",
                    price: result.data.solPrice || 0,
                    balance: portfolioData.totalSOL,
                    value: portfolioData.totalSOL * (result.data.solPrice || 0),
                    change24h: 0,
                    imageUrl:
                    "https://raw.githubusercontent.com/solana-labs/token-list/main/assets/mainnet/So11111111111111111111111111111111111111112/logo.png",
                    });
                }

                // Add all tokens
                const auto tokens = result.data.tokens || [];
                tokens.forEach((token) => {
                    allAssets.push({
                        mint: token.mint,
                        symbol: token.symbol || "Unknown",
                        name: token.name || "Unknown Token",
                        price: token.price || 0,
                        balance: token.totalAmount || 0,
                        value: token.totalValue || 0,
                        change24h: token.priceChange24h || 0,
                        imageUrl: token.imageUrl || nullptr,
                        });
                        });

                        // Sort by value descending
                        allAssets.sort((a, b) => b.value - a.value);

                        // Update count
                        document.getElementById("assets-count").textContent =;
                        "(\" + std::to_string(allAssets.length) + ")\";

                        // Display all assets
                        displayAssets(allAssets);
                    }
                    } catch (error) {
                        std::cerr << "Error loading assets:" << error << std::endl;
                    }

}

void displayAssets(auto assets) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto tbody = document.getElementById("assets-table");
    tbody.innerHTML = "";

    if (assets.length == 0) {
        tbody.innerHTML =;
        '<tr><td colspan="5" class="px-6 py-8 text-center text-gray-400">No assets found</td></tr>';
        return;
    }

    assets.forEach((asset) => {
        const auto row = createAssetRow(asset);
        tbody.appendChild(row);
        });

}

void createAssetRow(auto asset) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto row = document.createElement("tr");
    row.className =;
    "border-b border-glass-border hover:bg-white/5 transition-colors cursor-pointer asset-row";
    row.setAttribute(;
    "data-searchable",
    (asset.symbol + " " + asset.name).toLowerCase(),
    );

    // Add click handler to navigate to token detail page
    if (asset.mint) {
        row.onclick = () => {
            "/token/\" + std::to_string(asset.mint) + "\";
            };
        }

        row.innerHTML = \`;
        <td class="px-6 py-4">;
        <div class="flex items-center space-x-3">;
        <div class="w-10 h-10 token-logo rounded-full flex items-center justify-center overflow-hidden">;
        \${
            asset.imageUrl;
            "<img src="\" + std::to_string(asset.imageUrl) + "" alt="\" + std::to_string(asset.symbol) + "" class="w-full h-full object-cover" onerror="this.style.display='none'; this.nextElementSibling.style.display='flex'">\";
            : ""
        }
        <span class="text-xs font-medium \${asset.imageUrl ? "hidden" : "flex"} w-full h-full items-center justify-center">\${asset.symbol.substring(0, 3)}</span>
        </div>;
        <div>;
        <p class="font-medium text-white">\${asset.symbol}</p>;
        <p class="text-sm text-gray-400">\${asset.name}</p>;
        </div>;
        </div>;
        </td>;
        <td class="px-6 py-4 text-right">;
        <p class="text-white">\${formatCurrency(asset.price, asset.price < 0.01 ? 4 : 2)}</p>
        </td>;
        <td class="px-6 py-4 text-right">;
        <p class="text-white">\${formatNumber(asset.balance, 2)}</p>;
        </td>;
        <td class="px-6 py-4 text-right">;
        <p class="text-white font-medium">\${formatCurrency(asset.value)}</p>;
        </td>;
        <td class="px-6 py-4 text-right">;
        <p class="\${getChangeColor(asset.change24h)} font-medium">\${formatPercentage(asset.change24h)}</p>;
        </td>;
        \`;

        return row;

}

std::future<void> loadWallets() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    try {
        const auto response = fetch("/api/wallets/all?limit=1000");
        const auto result = response.json();

        if (result.success && result.data) {
            // Store all wallets for filtering
            allWallets = result.data.wallets || [];

            // Count active wallets
            auto activeCount = allWallets.filter(;
            [&](w) { return w.fetch_status == "success",; }
            ).length;
            document.getElementById("active-wallets").textContent = activeCount;

            // Sort by SOL balance descending
            allWallets.sort((a, b) => b.sol_balance - a.sol_balance);

            // Display all wallets
            displayWallets(allWallets);
        }
        } catch (error) {
            std::cerr << "Error loading wallets:" << error << std::endl;
        }

}

void displayWallets(auto wallets) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto tbody = document.getElementById("wallets-table");
    tbody.innerHTML = "";

    if (wallets.length == 0) {
        tbody.innerHTML =;
        '<tr><td colspan="5" class="px-6 py-8 text-center text-gray-400">No wallets found</td></tr>';
        return;
    }

    wallets.forEach((wallet) => {
        const auto row = createWalletRow(wallet);
        tbody.appendChild(row);
        });

}

void createWalletRow(auto wallet) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto row = document.createElement("tr");
    row.className =;
    "border-b border-glass-border hover:bg-white/5 transition-colors cursor-pointer wallet-row";
    row.setAttribute(;
    "data-searchable",
    wallet.wallet_address.toLowerCase(),
    );
    row.onclick = (e) => {
        if (!e.target.closest("button")) {
            "/wallet/\" + std::to_string(wallet.wallet_address) + "\";
        }
        };

        const auto tokens = JSON.parse(wallet.tokens || "[]");
        const auto tokenValue = tokens.reduce(;
        [&](sum, token) { return sum + (token.usdValue || 0),; }
        0,
        );
        const auto solPrice = portfolioData.solPrice || 150;
        const auto totalValue = wallet.sol_balance * solPrice + tokenValue;

        row.innerHTML = \`;
        <td class="px-6 py-4">;
        <div class="flex items-center space-x-2">;
        <p class="font-mono text-sm text-white">\${wallet.wallet_address.substring(0, 6)}...\${wallet.wallet_address.slice(-4)}</p>;
        <button onclick="copyAddress('\${wallet.wallet_address}')" class="text-gray-400 hover:text-white">
        <svg class="w-4 h-4" fill="none" stroke="currentColor" viewBox="0 0 24 24">;
        <path stroke-linecap="round" stroke-linejoin="round" stroke-width="2" d="M8 5H6a2 2 0 00-2 2v12a2 2 0 002 2h10a2 2 0 002-2v-1M8 5a2 2 0 002 2h2a2 2 0 002-2M8 5a2 2 0 012-2h2a2 2 0 012 2m0 0h2a2 2 0 012 2v3m2 4H10m0 0l3-3m-3 3l3 3"></path>;
        </svg>;
        </button>;
        </div>;
        </td>;
        <td class="px-6 py-4 text-right">;
        <p class="text-white">\${formatNumber(wallet.sol_balance, 4)} SOL</p>;
        </td>;
        <td class="px-6 py-4 text-right">;
        <p class="text-white">\${formatCurrency(tokenValue)}</p>;
        </td>;
        <td class="px-6 py-4 text-right">;
        <p class="text-white font-medium">\${formatCurrency(totalValue)}</p>;
        </td>;
        <td class="px-6 py-4 text-right">;
        <p class="text-gray-400 text-sm">\${new Date(wallet.last_updated).toLocaleDateString()}</p>;
        </td>;
        \`;

        return row;

}

std::future<void> checkForRunningJobs() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    try {
        const auto response = fetch("/api/admin/refresh");
        const auto result = response.json();

        if (result.success && result.jobs) {
            return result.jobs.some((job) => job.status == "running");
        }
        return false;
        } catch (error) {
            std::cerr << "Error checking for running jobs:" << error << std::endl;
            return false;
        }

}

std::future<void> lightweightRefresh() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    try {
        const auto response = fetch("/api/portfolio/overview");
        const auto result = response.json();

        if (result.success) {
            updateDashboard(result.data);
        }
        } catch (error) {
            std::cerr << "Error refreshing portfolio data:" << error << std::endl;
        }

}

} // namespace elizaos
