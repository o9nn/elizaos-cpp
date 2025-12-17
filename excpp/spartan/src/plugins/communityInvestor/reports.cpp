#include "reports.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::string formatPrice(double price) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return new Intl.NumberFormat('en-US', {;
        style: 'currency',
        currency: 'USD',
        minimumFractionDigits: price < 1 ? 6 : 2,
        maximumFractionDigits: price < 1 ? 6 : 2,
        }).format(price);

}

std::string formatPercent(double value) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return std::to_string(value >= 0 ? '+' : '') + std::to_string(value.toFixed(2)) + "%";

}

std::string formatNumber(double value) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return new Intl.NumberFormat('en-US').format(value);

}

std::string formatDate(const std::variant<std::string, Date>& dateString) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto date = true /* instanceof Date check */ ? dateString : new Date(dateString);
    return date.toLocaleString();

}

double normalizeBalance(const std::variant<std::string, bigint>& balanceStr, double decimals) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto balance = typeof balanceStr == 'string' ? BigInt(balanceStr) : balanceStr;
    return Number(balance) / 10 ** decimals;

}

TradeMetrics calculateTradeMetrics(const std::vector<Transaction>& transactions, TokenPerformance token) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    auto totalBought = 0;
    auto totalBoughtValue = 0;
    auto totalSold = 0;
    auto totalSoldValue = 0;
    auto totalTransferIn = 0;
    auto totalTransferOut = 0;
    auto volumeUsd = 0;
    auto firstTradeTime = new Date();
    auto lastTradeTime = new Date(0);

    for (const auto& tx : transactions)
        const auto normalizedAmount = normalizeBalance(tx.amount, token.decimals);
        const auto price = tx.price ? Number.parseFloat(tx.price) : 0;
        const auto value = normalizedAmount * price;

        if (tx.timestamp < firstTradeTime) firstTradeTime = new Date(tx.timestamp);
        if (tx.timestamp > lastTradeTime) lastTradeTime = new Date(tx.timestamp);

        switch (tx.type) {
            case 'BUY':
            totalBought += normalizedAmount;
            totalBoughtValue += value;
            volumeUsd += value;
            break;
            case 'SELL':
            totalSold += normalizedAmount;
            totalSoldValue += value;
            volumeUsd += value;
            break;
            case 'transfer_in':
            totalTransferIn += normalizedAmount;
            break;
            case 'transfer_out':
            totalTransferOut += normalizedAmount;
            break;
        }
    }

    const auto averageEntryPrice = totalBought > 0 ? totalBoughtValue / totalBought : 0;
    const auto averageExitPrice = totalSold > 0 ? totalSoldValue / totalSold : 0;
    const auto realizedPnL = totalSoldValue - totalSold * averageEntryPrice;
    const auto realizedPnLPercent =;
    averageEntryPrice > 0 ? ((averageExitPrice - averageEntryPrice) / averageEntryPrice) * 100 : 0;

    return {
        totalBought,
        totalBoughtValue,
        totalSold,
        totalSoldValue,
        totalTransferIn,
        totalTransferOut,
        averageEntryPrice,
        averageExitPrice,
        realizedPnL,
        realizedPnLPercent,
        volumeUsd,
        firstTradeTime,
        lastTradeTime,
        };

}

PositionPerformance calculatePositionPerformance(PositionWithBalance position, TokenPerformance token, const std::vector<Transaction>& transactions) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto normalizedBalance = normalizeBalance(position.balance, token.decimals);
    const auto initialPrice = Number.parseFloat(position.initialPrice);
    const auto currentPrice = token.price;

    const auto trades = calculateTradeMetrics(transactions, token);

    const auto currentValue = normalizedBalance * currentPrice;
    const auto initialValue = normalizedBalance * initialPrice;

    // Calculate unrealized P&L based on average entry price
    const auto costBasis = normalizedBalance * trades.averageEntryPrice;
    const auto unrealizedPnL = currentValue - costBasis;
    const auto unrealizedPnLPercent =;
    trades.averageEntryPrice > 0;
    ? ((currentPrice - trades.averageEntryPrice) / trades.averageEntryPrice) * 100;
    : 0;

    // Total P&L combines realized and unrealized
    const auto totalPnL = trades.realizedPnL + unrealizedPnL;
    const auto totalCost = trades.totalBought * trades.averageEntryPrice;
    const auto totalPnLPercent = totalCost > 0 ? (totalPnL / totalCost) * 100 : 0;

    const auto profitLoss = currentValue - initialValue;
    const auto profitLossPercentage = (profitLoss / initialValue) * 100;
    const auto priceChange = currentPrice - initialPrice;
    const auto priceChangePercentage = (priceChange / initialPrice) * 100;

    return {
        ...position,
        token,
        currentValue,
        initialValue,
        profitLoss,
        profitLossPercentage,
        priceChange,
        priceChangePercentage,
        normalizedBalance,
        trades,
        unrealizedPnL,
        unrealizedPnLPercent,
        totalPnL,
        totalPnLPercent,
        };

}

std::string formatTokenPerformance(TokenPerformance token) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return `;
    Token: ${token.name} (${token.symbol})
    Address: ${token.address}
    Chain: ${token.chain}
    Last Updated: ${formatDate(token.updatedAt)}
    Price: ${formatPrice(token.price)} (24h: ${formatPercent(token.price24hChange)})
    Volume: ${formatPrice(token.volume)} (24h: ${formatPercent(token.volume24hChange)})
    Liquidity: ${formatPrice(token.liquidity)}
    Holders: ${formatNumber(token.holders)} (24h: ${formatPercent(token.holders24hChange)})
    Trades: ${formatNumber(token.trades)}
    Security Info:
    - Creator: ${token.metadata.security.creatorAddress}
    - Creation Time: ${new Date(token.metadata.security.creationTime * 1000).toLocaleString()}
    - Total Supply: ${formatNumber(token.metadata.security.totalSupply)}
    - Top 10 Holders: ${formatPercent(token.metadata.security.top10HolderPercent)}
    - Token Standard: ${token.metadata.security.isToken2022 ? 'Token-2022' : 'SPL Token'}
    `.trim();

}

std::vector<std::string> formatTransactionHistory(const std::vector<Transaction>& transactions, TokenPerformance token) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return transactions;
    .sort((a, b) => new Date(b.timestamp).getTime() - new Date(a.timestamp).getTime());
    .map((tx) => {
        const auto normalizedAmount = normalizeBalance(tx.amount, token.decimals);
        const auto price = tx.price;
        ? formatPrice(Number.parseFloat(tx.price));
        : 'N/A';
        const auto value = tx.valueUsd;
        ? formatPrice(Number.parseFloat(tx.valueUsd));
        : 'N/A';

        return `;
    ${formatDate(tx.timestamp)} - ${tx.type}
    Amount: ${formatNumber(normalizedAmount)} ${token.symbol}
    Price: ${price}
    Value: ${value}
    TX: ${tx.transactionHash}
    `.trim();
    });

}

std::string formatPositionPerformance(PositionWithBalance position, TokenPerformance token, const std::vector<Transaction>& transactions) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto perfData = calculatePositionPerformance(position, token, transactions);

    return `;
    Position ID: ${position.id}
    Type: ${position.isSimulation ? 'Simulation' : 'Real'}
    Token: ${token.name} (${token.symbol})
    Wallet: ${position.walletAddress}

    Trading Summary:
    - Total Bought: ${formatNumber(perfData.trades.totalBought)} ${token.symbol}
    - Total Sold: ${formatNumber(perfData.trades.totalSold)} ${token.symbol}
    - Average Entry: ${formatPrice(perfData.trades.averageEntryPrice)}
    - Average Exit: ${formatPrice(perfData.trades.averageExitPrice)}
    - Trading Volume: ${formatPrice(perfData.trades.volumeUsd)}
    - First Trade: ${formatDate(perfData.trades.firstTradeTime)}
    - Last Trade: ${formatDate(perfData.trades.lastTradeTime)}

    Performance Metrics:
    - Current Price: ${formatPrice(token.price)}
    - Initial Price: ${formatPrice(Number.parseFloat(position.initialPrice))}
    - Price Change: ${formatPrice(perfData.priceChange)} (${formatPercent(perfData.priceChangePercentage)})

    Position Value:
    - Current Balance: ${formatNumber(perfData.normalizedBalance)} ${token.symbol}
    - Current Value: ${formatPrice(perfData.currentValue)}
    - Realized P&L: ${formatPrice(perfData.trades.realizedPnL)} (${formatPercent(perfData.trades.realizedPnLPercent)})
    - Unrealized P&L: ${formatPrice(perfData.unrealizedPnL)} (${formatPercent(perfData.unrealizedPnLPercent)})
    - Total P&L: ${formatPrice(perfData.totalPnL)} (${formatPercent(perfData.totalPnLPercent)})

    Market Info:
    - Current Liquidity: ${formatPrice(token.liquidity)}
    - 24h Volume: ${formatPrice(token.volume)}

    Transaction History:
    ${formatTransactionHistory(transactions, token)}
    `.trim();

}

void formatFullReport(const std::vector<TokenPerformance>& tokens, const std::vector<PositionWithBalance>& positions, const std::vector<Transaction>& transactions) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto tokenMap = new Map(tokens.map((token) => [token.address, token]));
    const auto txMap = new Map<string, Transaction[]>();

    // Group transactions by position ID
    transactions.forEach((tx) => {
        if (!txMap.has(tx.positionId)) {
            txMap.set(tx.positionId, []);
        }
        txMap.get(tx.positionId).push(tx);
        });

        const auto tokenReports = tokens.map((token) => formatTokenPerformance(token));

        const auto filteredPositions = positions.filter((position) => tokenMap.has(position.tokenAddress));

        const auto positionsWithData = filteredPositions.map((position) => ({;
            position,
            token: tokenMap.get(position.tokenAddress)!,
            transactions: txMap.get(position.id) || [],
            }));

            const auto positionReports = positionsWithData.map(({ position, token, transactions }) =>;
            formatPositionPerformance(position, token, transactions);
            );

            const auto { totalCurrentValue, totalRealizedPnL, totalUnrealizedPnL } = positions.reduce(;
            [&](acc, position) {
                const auto token = tokenMap.get(position.tokenAddress);

                if (token) {
                    const auto perfData = calculatePositionPerformance(;
                    position,
                    token,
                    txMap.get(position.id) || [];
                    );

                    return {
                        totalCurrentValue: acc.totalCurrentValue + perfData.currentValue,
                        totalRealizedPnL: acc.totalRealizedPnL + perfData.trades.realizedPnL,
                        totalUnrealizedPnL: acc.totalUnrealizedPnL + perfData.unrealizedPnL,
                        };
                    }

                    return acc;
                    },
                    {
                        totalCurrentValue: 0,
                        totalRealizedPnL: 0,
                        totalUnrealizedPnL: 0,
                    }
                    );

                    const auto totalPnL = totalRealizedPnL + totalUnrealizedPnL;

                    return {
                        tokenReports,
                        positionReports,
                        totalCurrentValue: formatPrice(totalCurrentValue),
                        totalRealizedPnL: formatPrice(totalRealizedPnL),
                        totalUnrealizedPnL: formatPrice(totalUnrealizedPnL),
                        totalPnL: formatPrice(totalPnL),
                        positionsWithBalance: positionsWithData,
                        };

}

std::string formatScore(double score) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return score.toFixed(2);

}

std::string formatPercentMetric(double value) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return std::to_string((value * 100).toFixed(1)) + "%";

}

double calculateTrend(double current, const std::vector<double>& historicalValues) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    if (historicalValues.length == 0) return 0;
    const auto previousValue = historicalValues[0];
    return ((current - previousValue) / previousValue) * 100;

}

std::string formatTrendArrow(double trend) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    if (trend > 5) return '↑';
    if (trend < -5) return '↓';
    return '→';

}

Array< calculatePeriodTrends(const std::vector<RecommenderMetricsHistory>& history, const std::variant<TimePeriod, null = null>& period) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    period: string;
    avgPerformance: number;
    successRate: number;
    recommendations: number;

}

std::string formatTrends(Array<{
    period: string;
    avgPerformance: number;
    successRate: number;
    recommendations: number;
  }> trends) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return trends;
    .map((trend) =>;
    `;
    ${trend.period}:
    - Performance: ${formatPercent(trend.avgPerformance)}
    - Success Rate: ${formatPercentMetric(trend.successRate)}
    - Recommendations: ${trend.recommendations}`.trim()
    );
    .join('\n\n');

}

std::string formatRecommenderProfile(Entity entity, RecommenderMetrics metrics, const std::vector<RecommenderMetricsHistory>& history) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto successRate = metrics.successfulRecs / metrics.totalRecommendations;
    const auto trustTrend = calculateMetricTrend(metrics, 'trustScore', history);
    const auto performanceTrend = calculateMetricTrend(metrics, 'avgTokenPerformance', history);

    return `;
    Entity Profile: ${entity.metadata.username}
    Platform: ${entity.metadata.platform}
    ID: ${entity.id}

    Performance Metrics:
    - Trust Score: ${formatScore(metrics.trustScore)} (${formatPercent(trustTrend.trend)} ${trustTrend.description})
    - Success Rate: ${formatPercentMetric(successRate)}
    - Recommendations: ${metrics.totalRecommendations} total, ${metrics.successfulRecs} successful
    - Avg Token Performance: ${formatPercent(metrics.avgTokenPerformance)} (${formatPercent(performanceTrend.trend)} ${performanceTrend.description})

    Risk Assessment:
    - Consistency Score: ${formatScore(metrics.consistencyScore)}

    Activity:
    - Last Active: ${formatDate(metrics.lastUpdated)}
    `.trim();

}

std::string formatRecommenderReport(Entity entity, RecommenderMetrics metrics, const std::vector<RecommenderMetricsHistory>& history) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto sortedHistory = [...history].sort(;
    [&](a, b) { return new Date(b.timestamp).getTime() - new Date(a.timestamp).getTime(); }
    );

    // Calculate performance trends for different time periods
    const auto dailyTrends = calculatePeriodTrends(sortedHistory, {;
        label: '24 Hours',
        days: 1,
        });
        const auto weeklyTrends = calculatePeriodTrends(sortedHistory, {;
            label: '7 Days',
            days: 7,
            });
            const auto monthlyTrends = calculatePeriodTrends(sortedHistory);

            // Calculate success trend
            const auto successTrend = calculateTrend(;
            metrics.successfulRecs / metrics.totalRecommendations,
            sortedHistory.map((h) => h.metrics.successfulRecs / h.metrics.totalRecommendations);
            );

            // Calculate performance trend
            const auto performanceTrend = calculateTrend(;
            metrics.avgTokenPerformance,
            sortedHistory.map((h) => h.metrics.avgTokenPerformance);
            );

            return `;
        Username: ${entity.metadata.username}
    Platform: ${entity.metadata.platform}
    ID: ${entity.id}

    == CURRENT METRICS ==;
    Trust Score: ${formatScore(metrics.trustScore)}
    Success Rate: ${formatPercentMetric(metrics.successfulRecs / metrics.totalRecommendations)} (${formatTrendArrow(successTrend)})
    Total Recommendations: ${metrics.totalRecommendations}
    Average Token Performance: ${formatPercent(metrics.avgTokenPerformance)} (${formatTrendArrow(performanceTrend)})

    Risk Analysis:
    - Consistency: ${formatScore(metrics.consistencyScore)}

    Activity Status:
    - Last Active: ${formatDate(metrics.lastUpdated)}

    == PERFORMANCE TRENDS ==;
    ${formatTrends(dailyTrends)}

    ${formatTrends(weeklyTrends)}

    Monthly Average Performance:
    ${formatTrends(monthlyTrends)}`.trim();

}

std::string formatTopRecommendersOverview(const std::vector<Entity>& recommenders, Map<string metrics, auto RecommenderMetrics>, Map<string history, auto RecommenderMetricsHistory[]>) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto sortedRecommenders = [...recommenders].sort((a, b) => {;
        const auto metricsA = metrics.get(a.id);
        const auto metricsB = metrics.get(b.id);
        if (!metricsA || !metricsB) return 0;
        return metricsB.trustScore - metricsA.trustScore;
        });

        return `;
        <top_recommenders>;
        ${sortedRecommenders;
        .map((entity) => {
            const auto metric = metrics.get(entity.id);
            if (!metric) return null;
            const auto historicalData = history.get(entity.id) || [];
            const auto trustTrend = calculateMetricTrend(metric, 'trustScore', historicalData);

            const auto performanceTrend = calculateMetricTrend(metric, 'avgTokenPerformance', historicalData);

            return `;
            ${entity.metadata.username} (${entity.metadata.platform});
            Trust Score: ${formatScore(metric.trustScore)} (${formatPercent(trustTrend.trend)} ${trustTrend.description})
            Performance Score: ${formatScore(metric.avgTokenPerformance)} (${formatPercent(performanceTrend.trend)} ${performanceTrend.description})
        Success Rate: ${formatPercentMetric(metric.successfulRecs / metric.totalRecommendations)}
    Last Active: ${formatDate(metric.lastUpdated)}
    `.trim();
    });
    .filter((report) => report != nullptr);
    .join('\n\n')}
    </top_recommenders>`.trim();

}

} // namespace elizaos
