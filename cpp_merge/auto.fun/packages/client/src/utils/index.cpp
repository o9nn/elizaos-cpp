#include "index.hpp"

string toSubscript(double num)
{
    shared subDigits = object{
        object::pair{std:("0"), std:("₀")}, 
        object::pair{std:("1"), std:("₁")}, 
        object::pair{std:("2"), std:("₂")}, 
        object::pair{std:("3"), std:("₃")}, 
        object::pair{std:("4"), std:("₄")}, 
        object::pair{std:("5"), std:("₅")}, 
        object::pair{std:("6"), std:("₆")}, 
        object::pair{std:("7"), std:("₇")}, 
        object::pair{std:("8"), std:("₈")}, 
        object::pair{std:("9"), std:("₉")}, 
        object::pair{std:("-"), std:("⁻")}
    };
    return num->toString()->split(string_empty)->map([=](auto digit) mutable
    {
        return OR((const_(subDigits)[digit]), (digit));
    }
    )->join(string_empty);
};


any moment = dayjs;
double LAMPORTS_PER_SOL = 1000000000;
std::function<double(double)> normalizedProgress = [=](auto progress) mutable
{
    return Math->round(Math->min(100, progress));
};
std::function<string(string)> shortenAddress = [=](auto address) mutable
{
    return address->slice(0, 3) + std:("...") + address->slice(-3);
};
std::function<string(double, boolean)> abbreviateNumber = [=](auto num, auto withoutCurrency = false) mutable
{
    auto absNum = Math->abs(Number(num));
    if (absNum < 1000) return formatNumber(num, false, withoutCurrency);
    auto units = array<string>{ std:("K"), std:("M"), std:("B"), std:("T") };
    auto exponent = Math->floor(Math->log10(absNum) / 3);
    if (exponent > units->get_length()) exponent = units->get_length();
    auto unit = const_(units)[exponent - 1];
    auto scaled = absNum / Math->pow(1000, exponent);
    auto formatted = (scaled % 1 == 0) ? scaled->toString() : scaled->toFixed(1);
    return string_empty + (withoutCurrency) ? string_empty : std:("$") + string_empty + (((num < 0) ? std:("-") : string_empty) + formatted + unit) + string_empty;
};
std::function<string(double, boolean, boolean)> formatNumber = [=](auto num, auto showDecimals = undefined, auto hideDollarSign = undefined) mutable
{
    auto formatted = Intl::NumberFormat(std:("en-US"), object{
        object::pair{std:("style"), std:("currency")}, 
        object::pair{std:("currency"), std:("USD")}, 
        object::pair{std:("notation"), (showDecimals) ? std:("standard") : std:("compact")}
    })->format(num);
    if (hideDollarSign) {
        return formatted->replace(std:("$"), string_empty);
    }
    return formatted;
};
std::function<string(any, boolean)> fromNow = [=](P0 date, auto hideAgo = undefined) mutable
{
    auto timeString = String(moment(date)->fromNow());
    if (!hideAgo) {
        return timeString;
    }
    if (timeString->includes(std:("a few seconds ago"))) return std:("NOW");
    if (timeString->includes(std:("a minute ago"))) return std:("1m");
    if (timeString->includes(std:("an hour ago"))) return std:("1hr");
    if (timeString->includes(std:("a day ago"))) return std:("1d");
    if (timeString->includes(std:("a week ago"))) return std:("1w");
    if (timeString->includes(std:("a month ago"))) return std:("1mo");
    if (timeString->includes(std:("a year ago"))) return std:("1y");
    auto result = timeString->replace(std:("ago"), string_empty)->trim();
    result = result->replace(std:(" seconds"), std:("s"))->replace(std:(" second"), std:("s"));
    result = result->replace(std:(" minutes"), std:("m"))->replace(std:(" minute"), std:("m"));
    result = result->replace(std:(" hours"), std:("hrs"))->replace(std:(" hour"), std:("hr"));
    result = result->replace(std:(" days"), std:("d"))->replace(std:(" day"), std:("d"));
    result = result->replace(std:(" weeks"), std:("w"))->replace(std:(" week"), std:("w"));
    result = result->replace(std:(" months"), std:("mo"))->replace(std:(" month"), std:("mo"));
    result = result->replace(std:(" years"), std:("y"))->replace(std:(" year"), std:("y"));
    return result;
};
std::function<string(double, double)> formatNumberSubscript = [=](auto num, auto decimals = 1) mutable
{
    if (num == 0) return std:("0");
    auto sign = string_empty;
    if (num < 0) {
        sign = std:("-");
        num = Math->abs(num);
    }
    num = Number(num->toFixed(11));
    if (num >= 1) {
        return sign + num->toString();
    }
    auto expStr = num->toExponential();
    auto [mantissa, exponentStr] = expStr->split(std:("e"));
    auto exponent = parseInt(exponentStr, 10);
    auto totalZeros = -exponent - 1;
    auto mantissaDigits = mantissa->replace(std:("."), string_empty)->slice(0, 9);
    if (totalZeros < 0) {
        totalZeros = 0;
    }
    if (totalZeros > decimals) {
        return sign + std:("0.0") + toSubscript(totalZeros) + mantissaDigits;
    } else {
        return sign + std:("0.") + std:("0")->repeat(totalZeros) + mantissaDigits;
    }
};
std::function<string(double, double)> formatNumberSubscriptSmart = [=](auto num, auto decimals = 4) mutable
{
    if (num == 0) return std:("0");
    auto sign = string_empty;
    if (num < 0) {
        sign = std:("-");
        num = Math->abs(num);
    }
    if (num >= 1) {
        return sign + num->toFixed(decimals)->toString();
    }
    auto expStr = num->toExponential();
    auto [mantissa, exponentStr] = expStr->split(std:("e"));
    auto exponent = parseInt(exponentStr, 10);
    auto totalZeros = -exponent - 1;
    if (totalZeros < 0) {
        totalZeros = 0;
    }
    if (totalZeros >= decimals) {
        auto mantissaDigits = mantissa->replace(std:("."), string_empty)->slice(0, decimals + 1);
        return sign + std:("0.") + toSubscript(totalZeros) + mantissaDigits;
    } else {
        auto roundedMantissa = Math->ceil(Number(mantissa) * Math.pow(10, decimals)) / Math.pow(10, decimals);
        auto roundedString = roundedMantissa->toFixed(decimals);
        auto mantissaDigits = roundedString->replace(std:("."), string_empty)->slice(0, decimals);
        return sign + std:("0.") + std:("0")->repeat(totalZeros) + mantissaDigits;
    }
};
std::function<boolean(string, string)> isFromDomain = [=](auto url, auto domain) mutable
{
    if (AND((!url->startsWith(std:("http"))), (!url->startsWith(std:("https"))))) {
        url = std:("https://") + url;
    }
    try
    {
        auto parsedUrl = std::make_shared<URL>(url);
        return (OR((parsedUrl->hostname == domain), (parsedUrl->hostname->endsWith(std:(".") + domain + string_empty))));
    }
    catch (const any& error)
    {
        return false;
    }
};
std::function<string(string, double, double)> resizeImage = [=](auto url, auto width, auto height) mutable
{
    if (!url) return std:("/logo.png");
    if (OR((url->includes(std:("ipfs"))), (!url->startsWith(std:("http"))))) {
        return url;
    } else {
        return string_empty + env["imageOptimizationUrl"] + std:("/width=") + width + std:(",height=") + height + std:(",format=auto/") + url + string_empty;
    }
};
double networkId = 1399811149;
std::function<boolean(any)> useCodex = [=](auto token) mutable
{
    if (OR((OR((token->imported == 1), (token->status == std:("locked")))), (token->status == std:("migrated")))) {
        return true;
    }
    return false;
};
std::function<any(string)> sanitizeCheckmark = [=](auto name = undefined) mutable
{
    if (!name) return string_empty;
    return name->replaceAll(std:("✅"), string_empty);
};

void Main(void)
{
    dayjs->extend(relativeTime);
}

MAIN
