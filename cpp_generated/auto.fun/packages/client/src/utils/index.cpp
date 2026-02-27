#include "/home/runner/work/elizaos-cpp/elizaos-cpp/auto.fun/packages/client/src/utils/index.h"

string toSubscript(double num)
{
    shared subDigits = object{
        object::pair{std::string("0"), std::string("₀")}, 
        object::pair{std::string("1"), std::string("₁")}, 
        object::pair{std::string("2"), std::string("₂")}, 
        object::pair{std::string("3"), std::string("₃")}, 
        object::pair{std::string("4"), std::string("₄")}, 
        object::pair{std::string("5"), std::string("₅")}, 
        object::pair{std::string("6"), std::string("₆")}, 
        object::pair{std::string("7"), std::string("₇")}, 
        object::pair{std::string("8"), std::string("₈")}, 
        object::pair{std::string("9"), std::string("₉")}, 
        object::pair{std::string("-"), std::string("⁻")}
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
    return address->slice(0, 3) + std::string("...") + address->slice(-3);
};
std::function<string(double, boolean)> abbreviateNumber = [=](auto num, auto withoutCurrency = false) mutable
{
    auto absNum = Math->abs(Number(num));
    if (absNum < 1000) return formatNumber(num, false, withoutCurrency);
    auto units = array<string>{ std::string("K"), std::string("M"), std::string("B"), std::string("T") };
    auto exponent = Math->floor(Math->log10(absNum) / 3);
    if (exponent > units->get_length()) exponent = units->get_length();
    auto unit = const_(units)[exponent - 1];
    auto scaled = absNum / Math->pow(1000, exponent);
    auto formatted = (scaled % 1 == 0) ? scaled->toString() : scaled->toFixed(1);
    return string_empty + (withoutCurrency) ? string_empty : std::string("$") + string_empty + (((num < 0) ? std::string("-") : string_empty) + formatted + unit) + string_empty;
};
std::function<string(double, boolean, boolean)> formatNumber = [=](auto num, auto showDecimals = undefined, auto hideDollarSign = undefined) mutable
{
    auto formatted = Intl::NumberFormat(std::string("en-US"), object{
        object::pair{std::string("style"), std::string("currency")}, 
        object::pair{std::string("currency"), std::string("USD")}, 
        object::pair{std::string("notation"), (showDecimals) ? std::string("standard") : std::string("compact")}
    })->format(num);
    if (hideDollarSign) {
        return formatted->replace(std::string("$"), string_empty);
    }
    return formatted;
};
std::function<string(any, boolean)> fromNow = [=](P0 date, auto hideAgo = undefined) mutable
{
    auto timeString = String(moment(date)->fromNow());
    if (!hideAgo) {
        return timeString;
    }
    if (timeString->includes(std::string("a few seconds ago"))) return std::string("NOW");
    if (timeString->includes(std::string("a minute ago"))) return std::string("1m");
    if (timeString->includes(std::string("an hour ago"))) return std::string("1hr");
    if (timeString->includes(std::string("a day ago"))) return std::string("1d");
    if (timeString->includes(std::string("a week ago"))) return std::string("1w");
    if (timeString->includes(std::string("a month ago"))) return std::string("1mo");
    if (timeString->includes(std::string("a year ago"))) return std::string("1y");
    auto result = timeString->replace(std::string("ago"), string_empty)->trim();
    result = result->replace(std::string(" seconds"), std::string("s"))->replace(std::string(" second"), std::string("s"));
    result = result->replace(std::string(" minutes"), std::string("m"))->replace(std::string(" minute"), std::string("m"));
    result = result->replace(std::string(" hours"), std::string("hrs"))->replace(std::string(" hour"), std::string("hr"));
    result = result->replace(std::string(" days"), std::string("d"))->replace(std::string(" day"), std::string("d"));
    result = result->replace(std::string(" weeks"), std::string("w"))->replace(std::string(" week"), std::string("w"));
    result = result->replace(std::string(" months"), std::string("mo"))->replace(std::string(" month"), std::string("mo"));
    result = result->replace(std::string(" years"), std::string("y"))->replace(std::string(" year"), std::string("y"));
    return result;
};
std::function<string(double, double)> formatNumberSubscript = [=](auto num, auto decimals = 1) mutable
{
    if (num == 0) return std::string("0");
    auto sign = string_empty;
    if (num < 0) {
        sign = std::string("-");
        num = Math->abs(num);
    }
    num = Number(num->toFixed(11));
    if (num >= 1) {
        return sign + num->toString();
    }
    auto expStr = num->toExponential();
    auto [mantissa, exponentStr] = expStr->split(std::string("e"));
    auto exponent = parseInt(exponentStr, 10);
    auto totalZeros = -exponent - 1;
    auto mantissaDigits = mantissa->replace(std::string("."), string_empty)->slice(0, 9);
    if (totalZeros < 0) {
        totalZeros = 0;
    }
    if (totalZeros > decimals) {
        return sign + std::string("0.0") + toSubscript(totalZeros) + mantissaDigits;
    } else {
        return sign + std::string("0.") + std::string("0")->repeat(totalZeros) + mantissaDigits;
    }
};
std::function<string(double, double)> formatNumberSubscriptSmart = [=](auto num, auto decimals = 4) mutable
{
    if (num == 0) return std::string("0");
    auto sign = string_empty;
    if (num < 0) {
        sign = std::string("-");
        num = Math->abs(num);
    }
    if (num >= 1) {
        return sign + num->toFixed(decimals)->toString();
    }
    auto expStr = num->toExponential();
    auto [mantissa, exponentStr] = expStr->split(std::string("e"));
    auto exponent = parseInt(exponentStr, 10);
    auto totalZeros = -exponent - 1;
    if (totalZeros < 0) {
        totalZeros = 0;
    }
    if (totalZeros >= decimals) {
        auto mantissaDigits = mantissa->replace(std::string("."), string_empty)->slice(0, decimals + 1);
        return sign + std::string("0.") + toSubscript(totalZeros) + mantissaDigits;
    } else {
        auto roundedMantissa = Math->ceil(Number(mantissa) * Math.pow(10, decimals)) / Math.pow(10, decimals);
        auto roundedString = roundedMantissa->toFixed(decimals);
        auto mantissaDigits = roundedString->replace(std::string("."), string_empty)->slice(0, decimals);
        return sign + std::string("0.") + std::string("0")->repeat(totalZeros) + mantissaDigits;
    }
};
std::function<boolean(string, string)> isFromDomain = [=](auto url, auto domain) mutable
{
    if (AND((!url->startsWith(std::string("http"))), (!url->startsWith(std::string("https"))))) {
        url = std::string("https://") + url;
    }
    try
    {
        auto parsedUrl = std::make_shared<URL>(url);
        return (OR((parsedUrl->hostname == domain), (parsedUrl->hostname->endsWith(std::string(".") + domain + string_empty))));
    }
    catch (const any& error)
    {
        return false;
    }
};
std::function<string(string, double, double)> resizeImage = [=](auto url, auto width, auto height) mutable
{
    if (!url) return std::string("/logo.png");
    if (OR((url->includes(std::string("ipfs"))), (!url->startsWith(std::string("http"))))) {
        return url;
    } else {
        return string_empty + env["imageOptimizationUrl"] + std::string("/width=") + width + std::string(",height=") + height + std::string(",format=auto/") + url + string_empty;
    }
};
double networkId = 1399811149;
std::function<boolean(any)> useCodex = [=](auto token) mutable
{
    if (OR((OR((token->imported == 1), (token->status == std::string("locked")))), (token->status == std::string("migrated")))) {
        return true;
    }
    return false;
};
std::function<any(string)> sanitizeCheckmark = [=](auto name = undefined) mutable
{
    if (!name) return string_empty;
    return name->replaceAll(std::string("✅"), string_empty);
};

void Main(void)
{
    dayjs->extend(relativeTime);
}

MAIN
