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

"use client";

import React, { useState, useCallback } from "react";
;
;
;

struct WalletAddressBadgeProps {
    std::string address;
    React.ReactNode icon;
    string; // e.g., "ETH" or "SOL" label;
    std::optional<std::string> className;
};


const truncateAddress = (address: string, firstChars = 4, lastChars = 4) => {
  if (!address) return "";
  if (address.length <= firstChars + lastChars) return address;
  const extraCharsStart = address.startsWith("0x") ? 2 : 0;
  return `${address.substring(0, firstChars + extraCharsStart)}...${address.substring(address.length - lastChars)}`;
};

const WalletAddressBadge: React.FC<WalletAddressBadgeProps> = ({
  address,
  icon,
  label,
  className,
}) => {
  const [copied, setCopied] = useState(false);

  const handleCopy = useCallback(async () => {
    if (!address) return;
    try {
      await navigator.clipboard.writeText(address);
      setCopied(true);
      setTimeout(() => setCopied(false), 1500);
    } catch (err) {
      console.error("Failed to copy address: ", err);
      // You could add a user-facing error message here if desired
    }
  }, [address]);

  if (!address) {
    return null; // Don't render anything if address is not provided
  }

  return (
    <Badge
      variant="secondary"
      className={cn(
        "group inline-flex cursor-pointer items-center gap-1.5 px-2 py-1 text-xs",
        className,
      )}
      onClick={handleCopy}
      title={`Copy ${label} address: ${address}`}
    >
      {icon}
      <span>{truncateAddress(address)}</span>
      {copied ? (
        <Check className="h-4 w-4 text-green-500" />
      ) : (
        <Copy className="mr-1 h-3 w-3 opacity-50 transition-opacity group-hover:opacity-100" />
      )}
    </Badge>
  );
};

} // namespace elizaos
