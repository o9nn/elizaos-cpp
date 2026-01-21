#include "token-chart.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

void TokenChart() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto COLORS = ["#0088FE", "#00C49F", "#FFBB28", "#FF8042", "#8884d8"];

    const auto data = holdings;
    .sort((a, b) => b.usdValue - a.usdValue);
    .slice(0, 5);
    .map((holding) => ({
        name: holding.marketData.baseToken.symbol,
        value: holding.usdValue
        }));

        return (;
        <ResponsiveContainer width="100%" height={300}>;
        <PieChart>;
        <Pie;
    data={data}
    cx="50%";
    cy="50%";
    labelLine={false}
    outerRadius={80}
    fill="#8884d8";
    dataKey="value";
    >;
    {data.map((entry, index) => (;
    "<Cell key={" + "cell-" + index;
    ))}
    </Pie>;
    <Legend />;
    </PieChart>;
    </ResponsiveContainer>;
    );

}

} // namespace elizaos
