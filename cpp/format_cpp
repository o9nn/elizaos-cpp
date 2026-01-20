#include "/home/runner/work/elizaos-cpp/elizaos-cpp/otaku/src/plugins/plugin-clanker/src/utils/format.h"

string formatUsd(double amount)
{
    return ((std::make_shared<Intl::NumberFormat>(std::string("en-US"), object{
        object::pair{std::string("style"), std::string("currency")}, 
        object::pair{std::string("currency"), std::string("USD")}, 
        object::pair{std::string("minimumFractionDigits"), 2}, 
        object::pair{std::string("maximumFractionDigits"), 6}
    })))->format(amount);
};


string formatPercentage(double value)
{
    return string_empty + (value * 100)->toFixed(2) + std::string("%");
};


string shortenAddress(string address)
{
    if (OR((!address), (address->get_length() < 10))) return address;
    return string_empty + address->slice(0, 6) + std::string("...") + address->slice(-4) + string_empty;
};


string formatTransactionHash(string hash)
{
    return shortenAddress(hash);
};


double calculatePriceImpact(any inputAmount, any outputAmount, double inputPrice, double outputPrice)
{
    auto inputValue = Number(inputAmount) * inputPrice;
    auto outputValue = Number(outputAmount) * outputPrice;
    if (inputValue == 0) return 0;
    auto impact = (inputValue - outputValue) / inputValue;
    return Math->abs(impact);
};


string formatTokenInfo(any info)
{
    auto lines = array<string>{ std::string("Token: ") + info["name"] + std::string(" (") + info["symbol"] + std::string(")") };
    auto isNativeEth = info["address"] == NATIVE_TOKEN_ADDRESSES;
    if (!isNativeEth) {
        lines->push(std::string("Address: ") + info["address"] + string_empty);
    } else {
        lines->push(std::string("Type: Native ETH on Base"));
    }
    if (info["price"] != undefined) {
        lines->push(std::string("Price: ") + formatUsd(info["price"]) + string_empty);
    }
    if (info["marketCap"] != undefined) {
        lines->push(std::string("Market Cap: ") + formatCompactUsd(Number(info["marketCap"])) + string_empty);
    }
    if (info["liquidity"] != undefined) {
        lines->push(std::string("Liquidity: ") + formatCompactUsd(Number(info["liquidity"])) + string_empty);
    }
    if (info["holders"] != undefined) {
        lines->push(std::string("Holders: ") + info["holders"]["toLocaleString"]() + string_empty);
    }
    if (info["volume24h"] != undefined) {
        lines->push(std::string("24h Volume: ") + formatCompactUsd(Number(info["volume24h"])) + string_empty);
    }
    return lines->join(std::string("\
"));
};


string formatGasPrice(any gasPrice)
{
    auto gwei = Number(gasPrice) / 1000000000;
    return string_empty + gwei->toFixed(2) + std::string(" gwei");
};


string formatCompactUsd(double amount)
{
    return ((std::make_shared<Intl::NumberFormat>(std::string("en-US"), object{
        object::pair{std::string("style"), std::string("currency")}, 
        object::pair{std::string("currency"), std::string("USD")}, 
        object::pair{std::string("notation"), std::string("compact")}, 
        object::pair{std::string("compactDisplay"), std::string("short")}, 
        object::pair{std::string("maximumFractionDigits"), 1}
    })))->format(amount);
};


