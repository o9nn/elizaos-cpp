"use client";

import { useState } from "react";
import { useMultiWallet } from "../multiwallet";
import { Button } from "../button";
import { Copy, Check, ArrowLeft, AlertCircle } from "lucide-react";

interface ReviewStepProps {
  formData: {
    tokenId: string;
    amount: string;
    isNegotiable: boolean;
    fixedDiscountBps: number;
    fixedLockupDays: number;
    minDiscountBps: number;
    maxDiscountBps: number;
    minLockupDays: number;
    maxLockupDays: number;
    minDealAmount: string;
    maxDealAmount: string;
    isFractionalized: boolean;
    isPrivate: boolean;
    maxPriceVolatilityBps: number;
    maxTimeToExecuteSeconds: number;
  };
  onBack: () => void;
  onNext: () => void;
  requiredChain?: "evm" | "solana" | null;
  isConnectedToRequiredChain?: boolean;
  onConnect?: () => void;
  privyReady?: boolean;
  selectedTokenSymbol?: string;
  selectedTokenDecimals?: number;
}

export function ReviewStep({
  formData,
  onBack,
  onNext,
  requiredChain,
  isConnectedToRequiredChain,
  onConnect,
  privyReady = true,
  selectedTokenSymbol = "TOKEN",
}: ReviewStepProps) {
  const { activeFamily, evmAddress, solanaPublicKey } = useMultiWallet();
  const [copied, setCopied] = useState(false);
  const [error, setError] = useState<string | null>(null);

  // Extract chain and address from tokenId (format: token-{chain}-{address})
  const getTokenInfo = (tokenId: string) => {
    const parts = tokenId?.split("-") || [];
    const chain = parts[1] || "";
    const address = parts.slice(2).join("-") || "";
    return { chain, address };
  };

  const { chain: tokenChain, address: rawTokenAddress } = getTokenInfo(
    formData.tokenId,
  );

  const getDisplayAddress = (addr: string) => {
    if (!addr || addr.length <= 12) return addr;
    return `${addr.slice(0, 6)}...${addr.slice(-4)}`;
  };

  const handleCopyToken = async () => {
    await navigator.clipboard.writeText(rawTokenAddress);
    setCopied(true);
    setTimeout(() => setCopied(false), 2000);
  };

  const handleProceed = () => {
    setError(null);
    const consignerAddress =
      activeFamily === "solana" ? solanaPublicKey : evmAddress;

    if (!consignerAddress) {
      setError("Please connect your wallet before creating a consignment");
      return;
    }

    if (!formData.tokenId) {
      setError("Please select a token first");
      return;
    }

    if (!formData.amount) {
      setError("Please enter an amount");
      return;
    }

    // Proceed to submission step
    onNext();
  };

  const formatAmount = (amount: string) => {
    const num = parseFloat(amount) || 0;
    return num.toLocaleString();
  };

  return (
    <div className="flex flex-col h-full min-h-0 overflow-y-auto space-y-6">
      {/* Header */}
      <div className="text-center pb-4 border-b border-zinc-200 dark:border-zinc-700">
        <h3 className="text-lg font-semibold text-zinc-900 dark:text-zinc-100">
          Review Your Listing
        </h3>
        <p className="text-sm text-zinc-500">
          Confirm the details before creating
        </p>
      </div>

      {/* Token Info */}
      <div className="flex items-center gap-3 p-4 rounded-xl bg-gradient-to-br from-brand-500/10 to-brand-400/10 border border-brand-500/20">
        <div className="w-12 h-12 rounded-full bg-gradient-to-br from-brand-400 to-brand-500 flex items-center justify-center">
          <span className="text-white font-bold text-lg">
            {selectedTokenSymbol.charAt(0)}
          </span>
        </div>
        <div className="flex-1">
          <p className="font-semibold text-lg text-zinc-900 dark:text-zinc-100">
            {formatAmount(formData.amount)} {selectedTokenSymbol}
          </p>
          <div className="flex items-center gap-2">
            <span className="text-xs text-zinc-500 font-mono">
              {getDisplayAddress(rawTokenAddress)}
            </span>
            <button
              onClick={handleCopyToken}
              className="p-1 hover:bg-zinc-100 dark:hover:bg-zinc-800 rounded transition-colors"
              title="Copy token address"
            >
              {copied ? (
                <Check className="w-3 h-3 text-green-600" />
              ) : (
                <Copy className="w-3 h-3 text-zinc-400" />
              )}
            </button>
          </div>
        </div>
        <div className="px-2 py-1 rounded-full bg-zinc-200 dark:bg-zinc-700 text-xs font-medium text-zinc-600 dark:text-zinc-400 uppercase">
          {tokenChain}
        </div>
      </div>

      {/* Details Grid */}
      <div className="grid gap-3">
        <div className="flex justify-between items-center p-3 rounded-lg bg-zinc-50 dark:bg-zinc-800/50">
          <span className="text-zinc-600 dark:text-zinc-400">Pricing Type</span>
          <span className="font-medium text-zinc-900 dark:text-zinc-100">
            {formData.isNegotiable ? "Negotiable" : "Fixed Price"}
          </span>
        </div>

        {formData.isNegotiable ? (
          <>
            <div className="flex justify-between items-center p-3 rounded-lg bg-zinc-50 dark:bg-zinc-800/50">
              <span className="text-zinc-600 dark:text-zinc-400">
                Discount Range
              </span>
              <span className="font-medium text-zinc-900 dark:text-zinc-100">
                {formData.minDiscountBps / 100}% –{" "}
                {formData.maxDiscountBps / 100}%
              </span>
            </div>
            <div className="flex justify-between items-center p-3 rounded-lg bg-zinc-50 dark:bg-zinc-800/50">
              <span className="text-zinc-600 dark:text-zinc-400">
                Lockup Range
              </span>
              <span className="font-medium text-zinc-900 dark:text-zinc-100">
                {formData.minLockupDays} – {formData.maxLockupDays} days
              </span>
            </div>
          </>
        ) : (
          <>
            <div className="flex justify-between items-center p-3 rounded-lg bg-zinc-50 dark:bg-zinc-800/50">
              <span className="text-zinc-600 dark:text-zinc-400">Discount</span>
              <span className="font-medium text-zinc-900 dark:text-zinc-100">
                {formData.fixedDiscountBps / 100}%
              </span>
            </div>
            <div className="flex justify-between items-center p-3 rounded-lg bg-zinc-50 dark:bg-zinc-800/50">
              <span className="text-zinc-600 dark:text-zinc-400">Lockup</span>
              <span className="font-medium text-zinc-900 dark:text-zinc-100">
                {formData.fixedLockupDays} days
              </span>
            </div>
          </>
        )}

        <div className="flex justify-between items-center p-3 rounded-lg bg-zinc-50 dark:bg-zinc-800/50">
          <span className="text-zinc-600 dark:text-zinc-400">
            Deal Size Range
          </span>
          <span className="font-medium text-zinc-900 dark:text-zinc-100">
            {formatAmount(formData.minDealAmount)} –{" "}
            {formatAmount(formData.maxDealAmount)} {selectedTokenSymbol}
          </span>
        </div>

        <div className="flex justify-between items-center p-3 rounded-lg bg-zinc-50 dark:bg-zinc-800/50">
          <span className="text-zinc-600 dark:text-zinc-400">Visibility</span>
          <span className="font-medium text-zinc-900 dark:text-zinc-100">
            {formData.isPrivate ? "Private" : "Public"}
          </span>
        </div>
      </div>

      {/* Error Display */}
      {error && (
        <div className="p-3 rounded-lg bg-red-500/10 border border-red-500/20 flex items-start gap-2">
          <AlertCircle className="w-4 h-4 text-red-500 mt-0.5 flex-shrink-0" />
          <p className="text-sm text-red-600 dark:text-red-400">{error}</p>
        </div>
      )}

      {/* Actions */}
      <div className="flex gap-3 pt-4">
        <Button
          onClick={onBack}
          color="dark"
          className="flex items-center gap-2 px-6 py-3"
        >
          <ArrowLeft className="w-4 h-4" />
          Back
        </Button>
        {formData.tokenId && requiredChain && !isConnectedToRequiredChain ? (
          <Button
            onClick={onConnect}
            disabled={!privyReady}
            color={
              requiredChain === "solana"
                ? ("purple" as const)
                : ("blue" as const)
            }
            className="flex-1 py-3"
          >
            {privyReady
              ? `Connect ${requiredChain === "solana" ? "Solana" : "EVM"} Wallet`
              : "Loading..."}
          </Button>
        ) : (
          <Button onClick={handleProceed} color="brand" className="flex-1 py-3">
            Create Listing
          </Button>
        )}
      </div>
    </div>
  );
}
