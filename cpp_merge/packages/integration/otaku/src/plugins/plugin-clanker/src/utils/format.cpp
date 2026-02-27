#include "format.hpp"

string formatUsd(double amount)
{
    return ((std::make_shared<Intl::NumberFormat>(std:("en-US"), object{
        object::pair{std:("style"), std:("currency")}, 
        object::pair{std:("currency"), std:("USD")}, 
        object::pair{std:("minimumFractionDigits"), 2}, 
        object::pair{std:("maximumFractionDigits"), 6}
    })))->format(amount);
};


string formatPercentage(double value)
{
    return string_empty + (value * 100)->toFixed(2) + std:("%");
};


string shortenAddress(string address)
{
    if (OR((!address), (address->get_length() < 10))) return address;
    return string_empty + address->slice(0, 6) + std:("...") + address->slice(-4) + string_empty;
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
    auto lines = array<string>{ std:("Token: ") + info["name"] + std:(" (") + info["symbol"] + std:(")") };
    auto isNativeEth = info["address"] == NATIVE_TOKEN_ADDRESSES;
    if (!isNativeEth) {
        lines->push(std:("Address: ") + info["address"] + string_empty);
    } else {
        lines->push(std:("Type: Native ETH on Base"));
    }
    if (info["price"] != undefined) {
        lines->push(std:("Price: ") + formatUsd(info["price"]) + string_empty);
    }
    if (info["marketCap"] != undefined) {
        lines->push(std:("Market Cap: ") + formatCompactUsd(Number(info["marketCap"])) + string_empty);
    }
    if (info["liquidity"] != undefined) {
        lines->push(std:("Liquidity: ") + formatCompactUsd(Number(info["liquidity"])) + string_empty);
    }
    if (info["holders"] != undefined) {
        lines->push(std:("Holders: ") + info["holders"]["toLocaleString"]() + string_empty);
    }
    if (info["volume24h"] != undefined) {
        lines->push(std:("24h Volume: ") + formatCompactUsd(Number(info["volume24h"])) + string_empty);
    }
    return lines->join(std:("\
"));
};


string formatGasPrice(any gasPrice)
{
    auto gwei = Number(gasPrice) / 1000000000;
    return string_empty + gwei->toFixed(2) + std:(" gwei");
};


string formatCompactUsd(double amount)
{
    return ((std::make_shared<Intl::NumberFormat>(std:("en-US"), object{
        object::pair{std:("style"), std:("currency")}, 
        object::pair{std:("currency"), std:("USD")}, 
        object::pair{std:("notation"), std:("compact")}, 
        object::pair{std:("compactDisplay"), std:("short")}, 
        object::pair{std:("maximumFractionDigits"), 1}
    })))->format(amount);
};


