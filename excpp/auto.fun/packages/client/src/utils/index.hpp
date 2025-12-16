#include "env.hpp"
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#pragma once

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use

;
;
;
;

dayjs.extend(relativeTime);

const moment = dayjs;

const LAMPORTS_PER_SOL = 1000000000;

const normalizedProgress = (progress: number) =>
  Math.round(Math.min(100, progress));

const shortenAddress = (address: string) => {
  return address.slice(0, 3) + "..." + address.slice(-3);
};

const abbreviateNumber = (
  num: number,
  withoutCurrency: boolean = false,
): string => {
  const absNum = Math.abs(Number(num));
  if (absNum < 1000) return formatNumber(num, false, withoutCurrency);

  const units = ["K", "M", "B", "T"];
  let exponent = Math.floor(Math.log10(absNum) / 3);
  if (exponent > units.length) exponent = units.length;
  const unit = units[exponent - 1];
  const scaled = absNum / Math.pow(1000, exponent);
  const formatted = scaled % 1 === 0 ? scaled.toString() : scaled.toFixed(1);

  return `${withoutCurrency ? "" : "$"}${(num < 0 ? "-" : "") + formatted + unit}`;
};

const formatNumber = (
  num: number,
  showDecimals?: boolean,
  hideDollarSign?: boolean,
) => {
  const formatted = Intl.NumberFormat("en-US", {
    style: "currency",
    currency: "USD",
    notation: showDecimals ? "standard" : "compact",
  }).format(num);

  if (hideDollarSign) {
    return formatted?.replace("$", "");
  }

  return formatted;
};

const fromNow = (
  date: string | Date | number,
  hideAgo?: boolean,
): string => {
  const timeString = String(moment(date).fromNow());

  if (!hideAgo) {
    return timeString;
  }

  // Handle special cases first
  if (timeString.includes("a few seconds ago")) return "NOW";
  if (timeString.includes("a minute ago")) return "1m";
  if (timeString.includes("an hour ago")) return "1hr";
  if (timeString.includes("a day ago")) return "1d";
  if (timeString.includes("a week ago")) return "1w";
  if (timeString.includes("a month ago")) return "1mo";
  if (timeString.includes("a year ago")) return "1y";

  // Handle regular cases with replacements
  let result = timeString.replace("ago", "").trim();
  result = result.replace(" seconds", "s").replace(" second", "s");
  result = result.replace(" minutes", "m").replace(" minute", "m");
  result = result.replace(" hours", "hrs").replace(" hour", "hr");
  result = result.replace(" days", "d").replace(" day", "d");
  result = result.replace(" weeks", "w").replace(" week", "w");
  result = result.replace(" months", "mo").replace(" month", "mo");
  result = result.replace(" years", "y").replace(" year", "y");

  return result;
};

 = {
    "0": "\u2080",
    "1": "\u2081",
    "2": "\u2082",
    "3": "\u2083",
    "4": "\u2084",
    "5": "\u2085",
    "6": "\u2086",
    "7": "\u2087",
    "8": "\u2088",
    "9": "\u2089",
    "-": "\u207B",
  };
  return num
    .toString()
    .split("")
    .map((digit) => subDigits[digit] || digit)
    .join("");
}

const formatNumberSubscript = (
  num: number,
  decimals: number = 1,
): string => {
  if (num === 0) return "0";
  let sign = "";
  if (num < 0) {
    sign = "-";
    num = Math.abs(num);
  }

  num = Number(num.toFixed(11));

  if (num >= 1) {
    return sign + num.toString();
  }

  const expStr = num.toExponential();
  const [mantissa, exponentStr] = expStr.split("e");
  const exponent = parseInt(exponentStr, 10);
  let totalZeros = -exponent - 1;
  const mantissaDigits = mantissa.replace(".", "").slice(0, 9);

  if (totalZeros < 0) {
    totalZeros = 0;
  }

  if (totalZeros > decimals) {
    return sign + "0.0" + toSubscript(totalZeros) + mantissaDigits;
  } else {
    return sign + "0." + "0".repeat(totalZeros) + mantissaDigits;
  }
};

const formatNumberSubscriptSmart = (
  num: number,
  decimals: number = 4,
): string => {
  if (num === 0) return "0";
  let sign = "";
  if (num < 0) {
    sign = "-";
    num = Math.abs(num);
  }

  if (num >= 1) {
    return sign + num.toFixed(decimals).toString();
  }

  const expStr = num.toExponential();
  const [mantissa, exponentStr] = expStr.split("e");
  const exponent = parseInt(exponentStr, 10);
  let totalZeros = -exponent - 1;

  if (totalZeros < 0) {
    totalZeros = 0;
  }

  if (totalZeros >= decimals) {
    const mantissaDigits = mantissa.replace(".", "").slice(0, decimals + 1);
    return sign + "0." + toSubscript(totalZeros) + mantissaDigits;
  } else {
    const roundedMantissa =
      Math.ceil(Number(mantissa) * 10 ** decimals) / 10 ** decimals;
    const roundedString = roundedMantissa.toFixed(decimals);
    const mantissaDigits = roundedString.replace(".", "").slice(0, decimals);

    return sign + "0." + "0".repeat(totalZeros) + mantissaDigits;
  }
};

const isFromDomain = (url: string, domain: string): boolean => {
  // if url does not have http or https, add it
  if (!url.startsWith("http") && !url.startsWith("https")) {
    url = "https://" + url;
  }

  try {
    const parsedUrl = new URL(url);
    return (
      parsedUrl.hostname === domain || parsedUrl.hostname.endsWith(`.${domain}`)
    );
  } catch (error) {
    return false;
  }
};

const resizeImage = (url: string, width: number, height: number) => {
  if (!url) return "/logo.png";
  if (url.includes("ipfs") || !url.startsWith("http")) {
    return url;
  } else {
    return `${env.imageOptimizationUrl}/width=${width},height=${height},format=auto/${url}`;
  }
};

const networkId = 1399811149;

const useCodex = (token: IToken) => {
  if (
    token?.imported === 1 ||
    token?.status === "locked" ||
    token?.status === "migrated"
  ) {
    return true;
  }

  return false;
};

const sanitizeCheckmark = (name?: string | null) => {
  if (!name) return "";
  return name.replaceAll("✅", "");
};

} // namespace elizaos
