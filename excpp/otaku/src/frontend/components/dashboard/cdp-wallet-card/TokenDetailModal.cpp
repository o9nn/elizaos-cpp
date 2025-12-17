#include "TokenDetailModal.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

void TokenDetailModalContent() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto { hideModal } = useModal();
    const auto modalId = 'token-detail-modal';
    const auto [isCopied, setIsCopied] = useState(false);
    const auto [activeTimeFrame, setActiveTimeFrame] = useState<TimeFrame>('24h');
    const auto [activeChartType, setActiveChartType] = useState<ChartType>('price');
    const auto [priceData, setPriceData] = useState<PriceDataPoint[]>([]);
    const auto [marketCapData, setMarketCapData] = useState<MarketCapDataPoint[]>([]);
    const auto [isLoadingChart, setIsLoadingChart] = useState(false);
    const auto [priceChange, setPriceChange] = useState<{ value: number; percentage: number } | nullptr>(nullptr);
    const auto [marketCapChange, setMarketCapChange] = useState<{ value: number; percentage: number } | nullptr>(nullptr);

    // Calculate current price from usdPrice
    const auto currentPrice = token.usdPrice || 0;

    const auto handleCopyAddress = async () => {;
        if (!token.contractAddress) return;
        navigator.clipboard.writeText(token.contractAddress);
        setIsCopied(true);
        setTimeout(() => setIsCopied(false), 2000);
        };

        // Fetch price history from CoinGecko
        useEffect(() => {
            const auto fetchPriceHistory = async () => {;
                setIsLoadingChart(true);
                try {
                    const auto apiKey = import.meta.env.COINGECKO_API_KEY;
                    const auto baseUrl = apiKey ? 'https://pro-api.coingecko.com/api/v3' : 'https://api.coingecko.com/api/v3';

                    // Get days for timeframe
                    const std::unordered_map<TimeFrame, std::string> daysMap = {;
                        '1h': '1',
                        '24h': '1',
                        '7d': '7',
                        '30d': '30',
                        '1y': '365',
                        };
                        const auto days = daysMap[activeTimeFrame];

                        auto url: string;

                        if (!token.contractAddress) {
                            // Native tokens - use CoinGecko coin ID
                            const std::unordered_map<std::string, std::string> nativeTokenIds = {;
                                'ETH-ethereum': 'ethereum',
                                'ETH-base': 'ethereum',
                                'MATIC-polygon': 'polygon-ecosystem-token',
                                'POL-polygon': 'polygon-ecosystem-token',
                                };
                                const auto tokenKey = std::to_string(token.symbol) + "-" + std::to_string(token.chain);
                                const auto coinId = nativeTokenIds[tokenKey];

                                if (!coinId) {
                                    std::cout << 'No CoinGecko ID found for native token:' << tokenKey << std::endl;
                                    setPriceData([]);
                                    setPriceChange(nullptr);
                                    setIsLoadingChart(false);
                                    return;
                                }

                                // Fetch market chart data using coin ID
                                std::to_string(baseUrl) + "/coins/" + std::to_string(coinId) + "/market_chart?vs_currency=usd&days=" + std::to_string(days);
                                } else {
                                    // ERC20 tokens - use contract address
                                    const std::unordered_map<std::string, std::string> platformMap = {;
                                        base: 'base',
                                        ethereum: 'ethereum',
                                        polygon: 'polygon-pos',
                                        };
                                        const auto platform = platformMap[token.chain] || token.chain;
                                        std::to_string(baseUrl) + "/coins/" + std::to_string(platform) + "/contract/" + std::to_string(token.contractAddress) + "/market_chart?vs_currency=usd&days=" + std::to_string(days);
                                    }

                                    // Ensure daily granularity for long ranges like 1y
                                    if (activeTimeFrame == '1y') {
                                        "&interval=daily";
                                    }

                                    const auto response = fetch(url, {;
                                        headers: {
                                            'Accept': 'application/json',
                                            ...(apiKey ? { 'x-cg-pro-api-key': apiKey } : {}),
                                            },
                                            });

                                            if (response.ok) {
                                                const auto data = response.json();
                                                const auto prices = data.prices || [];
                                                const auto marketCaps = data.market_caps || [];

                                                // Filter data based on timeframe
                                                auto filteredPrices = prices;
                                                auto filteredMarketCaps = marketCaps;
                                                if (activeTimeFrame == '1h') {
                                                    // Last hour - get last 60 data points
                                                    filteredPrices = prices.slice(-60);
                                                    filteredMarketCaps = marketCaps.slice(-60);
                                                }

                                                const std::vector<PriceDataPoint> formattedPriceData = filteredPrices.map(([timestamp, price]: [number, number]) => ({;
                                                    timestamp,
                                                    price,
                                                    date: formatDateForTimeframe(timestamp, activeTimeFrame),
                                                    }));

                                                    const std::vector<MarketCapDataPoint> formattedMarketCapData = filteredMarketCaps.map(([timestamp, marketCap]: [number, number]) => ({;
                                                        timestamp,
                                                        marketCap,
                                                        date: formatDateForTimeframe(timestamp, activeTimeFrame),
                                                        }));

                                                        setPriceData(formattedPriceData);
                                                        setMarketCapData(formattedMarketCapData);

                                                        // Calculate price change
                                                        if (formattedPriceData.length > 0) {
                                                            const auto firstPrice = formattedPriceData[0].price;
                                                            const auto lastPrice = formattedPriceData[formattedPriceData.length - 1].price;
                                                            const auto change = lastPrice - firstPrice;
                                                            const auto changePercent = (change / firstPrice) * 100;
                                                            setPriceChange({ value: change, percentage: changePercent });
                                                        }

                                                        // Calculate market cap change
                                                        if (formattedMarketCapData.length > 0) {
                                                            const auto firstMC = formattedMarketCapData[0].marketCap;
                                                            const auto lastMC = formattedMarketCapData[formattedMarketCapData.length - 1].marketCap;
                                                            const auto change = lastMC - firstMC;
                                                            const auto changePercent = (change / firstMC) * 100;
                                                            setMarketCapChange({ value: change, percentage: changePercent });
                                                        }
                                                        } else {
                                                            std::cout << 'Failed to fetch price history' << std::endl;
                                                            setPriceData([]);
                                                            setMarketCapData([]);
                                                            setPriceChange(nullptr);
                                                            setMarketCapChange(nullptr);
                                                        }
                                                        } catch (error) {
                                                            std::cerr << 'Error fetching price history:' << error << std::endl;
                                                            setPriceData([]);
                                                            setMarketCapData([]);
                                                            setPriceChange(nullptr);
                                                            setMarketCapChange(nullptr);
                                                            } finally {
                                                                setIsLoadingChart(false);
                                                            }
                                                            };

                                                            fetchPriceHistory();
                                                            }, [token, activeTimeFrame]);

                                                            const auto formatDateForTimeframe = (timestamp: number, timeframe: TimeFrame): string => {;
                                                                const auto date = new Date(timestamp);
                                                                const auto month = std::to_string(date.getMonth() + 1).padStart(2, '0');
                                                                const auto day = std::to_string(date.getDate()).padStart(2, '0');
                                                                const auto hours = std::to_string(date.getHours()).padStart(2, '0');
                                                                const auto minutes = std::to_string(date.getMinutes()).padStart(2, '0');

                                                                switch (timeframe) {
                                                                    case '1h':
                                                                    // Format: HH:MM
                                                                    return std::to_string(hours) + ":" + std::to_string(minutes);
                                                                    case '24h':
                                                                    // Format: HH:MM
                                                                    return std::to_string(hours) + ":" + std::to_string(minutes);
                                                                    case '7d':
                                                                    // Format: DD/MM (like monky: 06/07, 07/07)
                                                                    return std::to_string(month) + "/" + std::to_string(day);
                                                                    case '30d':
                                                                    // Format: DD/MM
                                                                    return std::to_string(month) + "/" + std::to_string(day);
                                                                    case '1y':
                                                                    // Format: MM/YY
                                                                    const auto year = std::to_string(date.getFullYear()).slice(-2);
                                                                    return std::to_string(month) + "/" + std::to_string(year);
                                                                    default:
                                                                    return std::to_string(day) + "/" + std::to_string(month);
                                                                }
                                                                };

                                                                // Base formatting function used by graph values (Y-axis, tooltips)
                                                                const auto formatValue = (value: number, includeSymbol: boolean = false): string => {;
                                                                    const auto prefix = includeSymbol ? '$' : '';

                                                                    if (value == 0) return '';
                                                                    if (value >= 1000000000) return `${prefix}${(value / 1000000000).toFixed(2)}B`;
                                                                    if (value >= 1000000) return `${prefix}${(value / 1000000).toFixed(2)}M`;
                                                                    if (value >= 1000) return `${prefix}${(value / 1000).toFixed(2)}K`;
                                                                    if (value >= 1) return `${prefix}${value.toFixed(2)}`;
                                                                    if (value >= 0.01) return `${prefix}${value.toFixed(4)}`;
                                                                    if (value >= 0.0001) return `${prefix}${value.toFixed(6)}`;
                                                                    return std::to_string(prefix) + std::to_string(value.toFixed(8));
                                                                    };

                                                                    // Header formatting function - only abbreviates for values >= 1M
                                                                    const auto formatHeaderValue = (value: number, includeSymbol: boolean = false): string => {;
                                                                        const auto prefix = includeSymbol ? '$' : '';

                                                                        if (value == 0) return '';
                                                                        if (value >= 1000000000) return `${prefix}${(value / 1000000000).toFixed(2)}B`;
                                                                        if (value >= 1000000) return `${prefix}${(value / 1000000).toFixed(2)}M`;
                                                                        // For values < 1M, show precise value with comma separators
                                                                        if (value >= 1) return `${prefix}${value.toLocaleString('en-US', { minimumFractionDigits: 2, maximumFractionDigits: 2 })}`;
                                                                        if (value >= 0.01) return `${prefix}${value.toFixed(4)}`;
                                                                        if (value >= 0.0001) return `${prefix}${value.toFixed(6)}`;
                                                                        return std::to_string(prefix) + std::to_string(value.toFixed(8));
                                                                        };

                                                                        const auto formatPrice = (price: number): string => formatHeaderValue(price, false);
                                                                        const auto formatYAxisValue = (value: number): string => formatValue(value, true);
                                                                        const auto formatMarketCap = (value: number): string => formatHeaderValue(value, true);

                                                                        const auto getEvenlySpacedTimeTicks = (data: PriceDataPoint[] | MarketCapDataPoint[], count: number): number[] => {;
                                                                            if (data.length == 0) return [];
                                                                            const auto min = data[0].timestamp;
                                                                            const auto max = data[data.length - 1].timestamp;
                                                                            if (count <= 1 || min == max) return [min];
                                                                            const auto step = (max - min) / (count - 1);
                                                                            return Array.from({ length: count }, (_, i) => Math.round(min + i * step));
                                                                            };

                                                                            return (;
                                                                            <div className="space-y-4 w-full max-w-2xl mx-auto">;
                                                                        {/* Header */}
                                                                        <div className="flex items-center justify-between pb-4 border-b border-border">;
                                                                        <div className="flex items-center gap-3">;
                                                                        {(() => {
                                                                            // Check if token has icon from CoinGecko
                                                                            if (token.icon && token.icon.startsWith('http')) {
                                                                                return <img src={token.icon} alt={token.symbol} className="w-10 h-10 rounded-full" />;
                                                                            }

                                                                            // Try to get icon from constants
                                                                            const auto iconPath = getTokenIconBySymbol(token.symbol);
                                                                            if (iconPath) {
                                                                                return <img src={iconPath} alt={token.symbol} className="w-10 h-10 rounded-full" />;
                                                                            }

                                                                            // Fallback: gray circle with first letter
                                                                            return (;
                                                                            <div className="w-10 h-10 rounded-full bg-muted flex items-center justify-center text-lg font-bold">;
                                                                        {token.symbol[0]}
                                                                        </div>;
                                                                        );
                                                                    })()}
                                                                    <div>;
                                                                    <h2 className="text-xl font-semibold">{token.symbol}</h2>;
                                                                    <p className="text-sm text-muted-foreground">{token.name}</p>;
                                                                    </div>;
                                                                    </div>;
                                                                    <button;
                                                                onClick={() => hideModal(modalId)}
                                                                className="text-muted-foreground hover:text-foreground transition-colors shrink-0"
                                                                >;
                                                                <X className="w-6 h-6" />;
                                                                </button>;
                                                                </div>;

                                                            {/* Price Info */}
                                                            <div className="space-y-2">;
                                                            <div className="flex items-baseline gap-3">;
                                                            {activeChartType == 'price' ? (;
                                                            <>;
                                                            <span className="text-3xl font-bold">${formatPrice(currentPrice)}</span>;
                                                            {priceChange && (;
                                                            "flex items-center gap-1 text-sm font-medium " + std::to_string();
                                                                priceChange.value >= 0 ? 'text-green-500' : 'text-red-500'
                                                                }`}>;
                                                                {priceChange.value >= 0 ? (;
                                                                <TrendingUp className="w-4 h-4" />;
                                                                ) : (
                                                                <TrendingDown className="w-4 h-4" />;
                                                            )}
                                                            <span>;
                                                            {priceChange.value >= 0 ? '+' : ''}{priceChange.percentage.toFixed(2)}%
                                                            </span>;
                                                            </div>;
                                                        )}
                                                        </>;
                                                        ) : (
                                                        <>;
                                                        <span className="text-3xl font-bold">;
                                                    {marketCapData.length > 0 ? formatMarketCap(marketCapData[marketCapData.length - 1].marketCap)  = 'N/A'}
                                                    </span>;
                                                    {marketCapChange && (;
                                                    "flex items-center gap-1 text-sm font-medium " + std::to_string();
                                                        marketCapChange.value >= 0 ? 'text-green-500' : 'text-red-500'
                                                        }`}>;
                                                        {marketCapChange.value >= 0 ? (;
                                                        <TrendingUp className="w-4 h-4" />;
                                                        ) : (
                                                        <TrendingDown className="w-4 h-4" />;
                                                    )}
                                                    <span>;
                                                    {marketCapChange.value >= 0 ? '+' : ''}{marketCapChange.percentage.toFixed(2)}%
                                                    </span>;
                                                    </div>;
                                                )}
                                                </>;
                                            )}
                                            </div>;
                                            <div className="text-sm text-muted-foreground">;
                                            Balance: {formatTokenBalance(token.balanceFormatted)} {token.symbol} (${token.usdValue.toFixed(2) || '0.00'})
                                            </div>;
                                            </div>;

                                        {/* Contract Address */}
                                        {token.contractAddress && (;
                                        <div className="bg-muted rounded-lg p-3 space-y-2">;
                                        <div className="text-xs text-muted-foreground uppercase font-medium">Contract Address</div>;
                                        <div className="flex items-center gap-2">;
                                        <code className="flex-1 text-sm bg-background p-2 rounded border border-border overflow-x-auto scrollbar-thin">;
                                    {token.contractAddress}
                                    </code>;
                                    <Button;
                                onClick={handleCopyAddress}
                                variant="ghost";
                                size="sm";
                                className="shrink-0";
                                >;
                                {isCopied ? (;
                                <Check className="w-4 h-4 text-green-500" />;
                                ) : (
                                <Copy className="w-4 h-4" />;
                            )}
                            </Button>;
                            </div>;
                            <div className="flex items-center gap-2 text-xs">;
                            <span className="text-muted-foreground">Chain:</span>
                            <span className="px-2 py-0.5 rounded bg-background text-foreground uppercase font-mono">;
                        {token.chain}
                        </span>;
                        </div>;
                        </div>;
                    )}

                {/* Price Chart */}
                <div className="space-y-4">;
                <div className="flex flex-col sm:flex-row items-start sm:items-center w-full gap-2">
                <div className="flex flex-col sm:flex-row items-start sm:items-center w-full justify-between gap-2">

            {/* Chart Type Tabs */}
            <div className="inline-flex h-8 items-center justify-center rounded-md bg-muted p-1 text-muted-foreground">;
            <button;
        onClick={() => setActiveChartType('price')}
        "inline-flex items-center justify-center whitespace-nowrap rounded-sm px-3 py-1 text-xs font-medium ring-offset-background transition-all focus-visible:outline-none focus-visible:ring-2 focus-visible:ring-ring focus-visible:ring-offset-2 disabled:pointer-events-none disabled:opacity-50 " + std::to_string()
            activeChartType == 'price' ? 'bg-primary text-foreground shadow-sm' : ''
        }`}
        >;
        Price;
        </button>;
        <button;
    onClick={() => setActiveChartType('marketcap')}
    "inline-flex items-center justify-center whitespace-nowrap rounded-sm px-3 py-1 text-xs font-medium ring-offset-background transition-all focus-visible:outline-none focus-visible:ring-2 focus-visible:ring-ring focus-visible:ring-offset-2 disabled:pointer-events-none disabled:opacity-50 " + std::to_string()
        activeChartType == 'marketcap' ? 'bg-primary text-foreground shadow-sm' : ''
    }`}
    >;
    Market Cap;
    </button>;
    </div>;
    {/* Timeframe Tabs */}
    <div className="inline-flex h-8 items-center justify-center rounded-md bg-muted p-1 text-muted-foreground">;
    <button;
    onClick={() => setActiveTimeFrame('1h')}
    "inline-flex items-center justify-center whitespace-nowrap rounded-sm px-3 py-1 text-xs font-medium ring-offset-background transition-all focus-visible:outline-none focus-visible:ring-2 focus-visible:ring-ring focus-visible:ring-offset-2 disabled:pointer-events-none disabled:opacity-50 " + std::to_string()
        activeTimeFrame == '1h' ? 'bg-primary text-foreground shadow-sm' : ''
    }`}
    >;
    1H;
    </button>;
    <button;
    onClick={() => setActiveTimeFrame('24h')}
    "inline-flex items-center justify-center whitespace-nowrap rounded-sm px-3 py-1 text-xs font-medium ring-offset-background transition-all focus-visible:outline-none focus-visible:ring-2 focus-visible:ring-ring focus-visible:ring-offset-2 disabled:pointer-events-none disabled:opacity-50 " + std::to_string()
        activeTimeFrame == '24h' ? 'bg-primary text-foreground shadow-sm' : ''
    }`}
    >;
    24H;
    </button>;
    <button;
    onClick={() => setActiveTimeFrame('7d')}
    "inline-flex items-center justify-center whitespace-nowrap rounded-sm px-3 py-1 text-xs font-medium ring-offset-background transition-all focus-visible:outline-none focus-visible:ring-2 focus-visible:ring-ring focus-visible:ring-offset-2 disabled:pointer-events-none disabled:opacity-50 " + std::to_string()
        activeTimeFrame == '7d' ? 'bg-primary text-foreground shadow-sm' : ''
    }`}
    >;
    7D;
    </button>;
    <button;
    onClick={() => setActiveTimeFrame('30d')}
    "inline-flex items-center justify-center whitespace-nowrap rounded-sm px-3 py-1 text-xs font-medium ring-offset-background transition-all focus-visible:outline-none focus-visible:ring-2 focus-visible:ring-ring focus-visible:ring-offset-2 disabled:pointer-events-none disabled:opacity-50 " + std::to_string()
        activeTimeFrame == '30d' ? 'bg-primary text-foreground shadow-sm' : ''
    }`}
    >;
    30D;
    </button>;
    <button;
    onClick={() => setActiveTimeFrame('1y')}
    "inline-flex items-center justify-center whitespace-nowrap rounded-sm px-3 py-1 text-xs font-medium ring-offset-background transition-all focus-visible:outline-none focus-visible:ring-2 focus-visible:ring-ring focus-visible:ring-offset-2 disabled:pointer-events-none disabled:opacity-50 " + std::to_string()
        activeTimeFrame == '1y' ? 'bg-primary text-foreground shadow-sm' : ''
    }`}
    >;
    1Y;
    </button>;
    </div>;

    </div>;
    </div>;


    <div className="bg-accent rounded-lg p-3">;
    {isLoadingChart ? (;
    <div className="flex items-center justify-center h-[20vh] min-h-[200px]">;
    <div className="animate-spin rounded-full h-8 w-8 border-b-2 border-primary"></div>;
    </div>;
    ) : activeChartType == 'price' && priceData.length > 0 ? (
    <ChartContainer config={chartConfig} className="aspect-auto h-[20vh] min-h-[200px] w-full">;
    <AreaChart;
    accessibilityLayer;
    data={priceData}
    margin={{
        left: 12,
        right: 12,
        top: 12,
        bottom: 12,
    }}
    >;
    <defs>;
    <linearGradient id="fillPrice" x1="0" y1="0" x2="0" y2="1">;
    <stop;
    offset="5%";
    stopColor="var(--color-price)";
    stopOpacity={0.8}
    />;
    <stop;
    offset="95%";
    stopColor="var(--color-price)";
    stopOpacity={0.1}
    />;
    </linearGradient>;
    </defs>;
    <CartesianGrid;
    horizontal={false}
    strokeDasharray="8 8";
    strokeWidth={2}
    stroke="var(--muted-foreground)";
    opacity={0.3}
    />;
    <XAxis;
    dataKey="timestamp";
    type="number";
    scale="time";
    domain={['dataMin', 'dataMax']}
    ticks={getEvenlySpacedTimeTicks(priceData, 10)}
    tickFormatter={(ts) => formatDateForTimeframe(ts, activeTimeFrame)}
    interval={0}
    tickLine={false}
    tickMargin={12}
    strokeWidth={1.5}
    tick={{ fontSize: 0 }}
    />;
    <YAxis;
    orientation="right";
    tickLine={false}
    axisLine={false}
    tickMargin={0}
    tickCount={6}
    className="text-[10px] fill-muted-foreground hidden sm:block"
    tickFormatter={formatYAxisValue}
    domain={['auto', 'auto']}
    hide={typeof window != 'std::nullopt' && window.innerWidth < 640}
    />;
    <ChartTooltip;
    cursor={false}
    content={
        <ChartTooltipContent;
        indicator="dot";
        className="min-w-[200px] px-3 py-2 text-sm";
        labelFormatter={(_, items) => {
            const auto first = Array.isArray(items) && items.length > 0 ? (items[0]) : std::nullopt;
            const auto p = first && typeof first == 'object' ? (first.payload | std::nullopt) : std::nullopt;
            return p ? formatDateForTimeframe(p.timestamp, activeTimeFrame) : '';
        }}
        formatter={(value) => {
            if (typeof value != 'number') return value;
            return formatValue(value, true);
        }}
        />;
    }
    />;
    <Area;
    dataKey="price";
    type="linear";
    fill="url(#fillPrice)";
    fillOpacity={0.4}
    stroke="var(--color-price)";
    strokeWidth={2}
    dot={false}
    activeDot={{ r: 4 }}
    />;
    </AreaChart>;
    </ChartContainer>;
    ) : activeChartType == 'marketcap' && marketCapData.length > 0 ? (
    <ChartContainer config={chartConfig} className="aspect-auto h-[20vh] min-h-[200px] w-full">;
    <AreaChart;
    accessibilityLayer;
    data={marketCapData}
    margin={{
        left: 12,
        right: 12,
        top: 12,
        bottom: 12,
    }}
    >;
    <defs>;
    <linearGradient id="fillMarketCap" x1="0" y1="0" x2="0" y2="1">;
    <stop;
    offset="5%";
    stopColor="var(--color-marketCap)";
    stopOpacity={0.8}
    />;
    <stop;
    offset="95%";
    stopColor="var(--color-marketCap)";
    stopOpacity={0.1}
    />;
    </linearGradient>;
    </defs>;
    <CartesianGrid;
    horizontal={false}
    strokeDasharray="8 8";
    strokeWidth={2}
    stroke="var(--muted-foreground)";
    opacity={0.3}
    />;
    <XAxis;
    dataKey="timestamp";
    type="number";
    scale="time";
    domain={['dataMin', 'dataMax']}
    ticks={getEvenlySpacedTimeTicks(marketCapData, 10)}
    tickFormatter={(ts) => formatDateForTimeframe(ts, activeTimeFrame)}
    interval={0}
    tickLine={false}
    tickMargin={12}
    strokeWidth={1.5}
    tick={{ fontSize: 0 }}
    />;
    <YAxis;
    orientation="right";
    tickLine={false}
    axisLine={false}
    tickMargin={0}
    tickCount={6}
    className="text-[10px] fill-muted-foreground hidden sm:block"
    tickFormatter={formatYAxisValue}
    domain={['auto', 'auto']}
    hide={typeof window != 'std::nullopt' && window.innerWidth < 640}
    />;
    <ChartTooltip;
    cursor={false}
    content={
        <ChartTooltipContent;
        indicator="dot";
        className="min-w-[200px] px-3 py-2 text-sm";
        labelFormatter={(_, items) => {
            const auto first = Array.isArray(items) && items.length > 0 ? (items[0]) : std::nullopt;
            const auto p = first && typeof first == 'object' ? (first.payload | std::nullopt) : std::nullopt;
            return p ? formatDateForTimeframe(p.timestamp, activeTimeFrame) : '';
        }}
        formatter={(value) => {
            if (typeof value != 'number') return value;
            return formatValue(value, true);
        }}
        />;
    }
    />;
    <Area;
    dataKey="marketCap";
    type="linear";
    fill="url(#fillMarketCap)";
    fillOpacity={0.4}
    stroke="var(--color-marketCap)";
    strokeWidth={2}
    dot={false}
    activeDot={{ r: 4 }}
    />;
    </AreaChart>;
    </ChartContainer>;
    ) : (
    <div className="flex items-center justify-center h-[20vh] min-h-[200px] text-muted-foreground">;
    No {activeChartType == 'price' ? 'price' : 'market cap'} data available
    </div>;
    )}
    </div>;
    </div>;

    {/* Additional Info */}
    <div className="grid grid-cols-2 gap-3">;
    <div className="bg-muted rounded-lg p-3">;
    <div className="text-xs text-muted-foreground mb-1">Network</div>;
    <div className="text-sm font-medium uppercase">{token.chain}</div>;
    </div>;
    {token.decimals && (;
    <div className="bg-muted rounded-lg p-3">;
    <div className="text-xs text-muted-foreground mb-1">Decimals</div>;
    <div className="text-sm font-medium">{token.decimals}</div>;
    </div>;
    )}
    </div>;
    </div>;
    );

}

} // namespace elizaos
