#pragma once
#include <algorithm>
#include <chrono>
#include <cstdint>
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <variant>
#include <vector>

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use


/**
 * Formats a price into a currency format.
 *
 * @param {number} price - The price to be formatted.
 * @returns {string} The price formatted as a currency.
 */
std::string formatPrice(double price);

/**
 * Formats a number as a percentage string with two decimal places.
 * @param {number} value - The number to be formatted as a percentage.
 * @returns {string} The formatted percentage string.
 */
std::string formatPercent(double value);

/**
 * Formats a given number into a string representation using the "en-US" number format.
 *
 * @param {number} value - The number to be formatted.
 * @returns {string} The formatted number as a string.
 */
std::string formatNumber(double value);

/**
 * Formats a given date string or Date object into a locale-specific string representation.
 *
 * @param {string | Date} dateString - The date string to be formatted or a Date object.
 * @returns {string} The formatted date string.
 */
std::string formatDate(const std::variant<std::string, std::chrono::system_clock::time_point>& dateString);

/**
 * Function to normalize the balance based on the decimals provided.
 * @param {string | bigint} balanceStr - The balance to normalize, can be a string or bigint.
 * @param {number} decimals - The number of decimal places to normalize to.
 * @returns {number} The normalized balance as a number.
 */
double normalizeBalance(const std::variant<std::string, bigint>& balanceStr, double decimals);

/**
 * Calculate various trade metrics based on transactions and token performance.
 *
 * @param {Transaction[]} transactions - Array of transactions to calculate metrics for.
 * @param {TokenPerformance} token - Token performance object.
 * @returns {TradeMetrics} Object containing calculated trade metrics.
 */
TradeMetrics calculateTradeMetrics(const std::vector<Transaction>& transactions, TokenPerformance token);

/**
 * Calculate the performance metrics of a given position.
 *
 * @param {PositionWithBalance} position The position with balance information.
 * @param {TokenPerformance} token The performance metrics of the token.
 * @param {Transaction[]} transactions The list of transactions related to the position.
 * @returns {PositionPerformance} The performance metrics of the position including current value, initial value,
 * profit/loss, profit/loss percentage, price change, price change percentage, normalized balance, trade metrics,
 * unrealized profit/loss, unrealized profit/loss percentage, total profit/loss, and total profit/loss percentage.
 */

PositionPerformance calculatePositionPerformance(PositionWithBalance position, TokenPerformance token, const std::vector<Transaction>& transactions);

/**
 * Formats the token performance information into a readable string.
 * @param {TokenPerformance} token - The token performance object to format.
 * @returns {string} The formatted token performance information.
 */
std::string formatTokenPerformance(TokenPerformance token);

/**
 * Formats transaction history data into an array of strings for display.
 * @param {Transaction[]} transactions - The list of transactions to format.
 * @param {TokenPerformance} token - The token performance data used for formatting.
 * @returns {string[]} - An array of formatted strings representing each transaction.
 */
std::vector<std::string> formatTransactionHistory(const std::vector<Transaction>& transactions, TokenPerformance token);

/**
 * Format the performance metrics and details of a position.
 *
 * @param {PositionWithBalance} position The position object containing balance information.
 * @param {TokenPerformance} token The token performance object.
 * @param {Transaction[]} transactions The list of transactions associated with the position.
 * @returns {string} The formatted performance details of the position.
 */
std::string formatPositionPerformance(PositionWithBalance position, TokenPerformance token, const std::vector<Transaction>& transactions);

/**
 * Formats a full report based on the provided data.
 *
 * @param {TokenPerformance[]} tokens - Array of token performance data.
 * @param {PositionWithBalance[]} positions - Array of positions with balance data.
 * @param {Transaction[]} transactions - Array of transactions data.
 * @returns {{
 *   tokenReports: Object[],
 *   positionReports: Object[],
 *   totalCurrentValue: string,
 *   totalRealizedPnL: string,
 *   totalUnrealizedPnL: string,
 *   totalPnL: string,
 *   positionsWithBalance: Object[],
 * }} Formatted full report containing token reports, position reports, total values, and positions with balance.
 */
void formatFullReport(const std::vector<TokenPerformance>& tokens, const std::vector<PositionWithBalance>& positions, const std::vector<Transaction>& transactions);

/**
 * Formats a numerical score to have exactly two decimal places.
 *
 * @param {number} score - The numerical score to be formatted.
 * @returns {string} The formatted score with two decimal places.
 */

std::string formatScore(double score);

/**
 * Formats a numeric value into a percentage string with one decimal place.
 *
 * @param {number} value - The numeric value to be formatted as a percentage.
 * @returns {string} The formatted percentage string.
 */
std::string formatPercentMetric(double value);

/**
 * TypeScript type to retrieve the keys of a given object `T` that have numeric values.
 */

/**
 * Represents the numeric keys from the `RecommenderMetrics` type.
 */
using RecommenderNumericMetrics = NumericKeys<RecommenderMetrics>;

/**
 * Calculate the trend of a specific metric based on historical data.
 * @template Metric - The type of metric to calculate trend for.
 * @param {RecommenderMetrics} current - The current metrics values.
 * @param {Metric} metric - The specific metric to calculate trend for.
 * @param {RecommenderMetricsHistory[]} history - Array of historical metrics data.
 * @returns {{ trend: number; description: string }} - Object containing trend value and description.
 */

/**
 * Calculate the percentage trend between the current value and the first value in the historical values array.
 *
 * @param {number} current - The current value.
 * @param {number[]} historicalValues - An array of historical values.
 * @returns {number} The calculated trend percentage.
 */
double calculateTrend(double current, const std::vector<double>& historicalValues);

/**
 * Formats the trend arrow based on the trend value.
 * An arrow pointing upwards ("↑") is returned if the trend is greater than 5.
 * An arrow pointing downwards ("↓") is returned if the trend is less than -5.
 * A horizontal arrow ("→") is returned if the trend is between -5 and 5 (inclusive).
 *
 * @param trend The value representing the trend
 * @returns The formatted arrow representing the trend direction
 */
std::string formatTrendArrow(double trend);

/**
 * Represents a time period with a label and number of days.
 * @typedef {Object} TimePeriod
 * @property {string} label - The label for the time period.
 * @property {number} days - The number of days in the time period.
 */
using TimePeriod = {

/**
 * Calculates and returns trends for a given history of recommended metrics.
 * If a specific period is provided, trends are calculated for that period.
 * If no period is provided, monthly trends are calculated.
 *
 * @param {RecommenderMetricsHistory[]} history - The history of recommended metrics.
 * @param {TimePeriod} period - The time period for which trends should be calculated. If not provided, monthly trends are calculated.
 * @returns {Array<{
 * 	period: string;
 * 	avgPerformance: number;
 * 	successRate: number;
 * 	recommendations: number;
 * }>} An array of objects containing period, average performance, success rate, and total recommendations.
 */
  // For monthly grouping

  // For daily and weekly periods

/**
 * Formats an array of trends into a string representation.
 *
 * @param {Array<{ period: string; avgPerformance: number; successRate: number; recommendations: number; }>} trends The array of trends to format.
 * @returns {string} The formatted trends as a string with each trend separated by two new lines.
 */

/**
 * Formats the recommender profile for a given entity based on the provided metrics and history.
 * @param {Entity} entity - The entity for which the profile is being formatted.
 * @param {RecommenderMetrics} metrics - The metrics related to the recommendations for the entity.
 * @param {RecommenderMetricsHistory[]} history - The history of metrics for the entity.
 * @returns {string} The formatted recommender profile string.
 */
std::string formatRecommenderProfile(Entity entity, RecommenderMetrics metrics, const std::vector<RecommenderMetricsHistory>& history);

/**
 * Formats a recommender report for an entity with provided metrics and history.
 * @param {Entity} entity - The entity for which the report is being generated.
 * @param {RecommenderMetrics} metrics - The metrics for the entity's recommendations.
 * @param {RecommenderMetricsHistory[]} history - The historical metrics for the entity's recommendations.
 * @returns {string} The formatted recommender report.
 */
std::string formatRecommenderReport(Entity entity, RecommenderMetrics metrics, const std::vector<RecommenderMetricsHistory>& history);

/**
 * Formats the top recommenders overview based on the provided data.
 *
 * @param {Entity[]} recommenders - The list of recommenders to be formatted
 * @param {Map<string, RecommenderMetrics>} metrics - The map of recommender metrics
 * @param {Map<string, RecommenderMetricsHistory[]>} history - The map of historical metrics data
 * @returns {string} The formatted top recommenders overview in XML format
 */
std::string formatTopRecommendersOverview(const std::vector<Entity>& recommenders, const std::unordered_map<std::string, RecommenderMetrics>& metrics, const std::unordered_map<std::string, std::vector<RecommenderMetricsHistory>>& history);

} // namespace elizaos
