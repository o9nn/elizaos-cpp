#include "form-step.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

void FormStep(auto updateFormData, auto onNext, auto onBack, auto selectedTokenBalance, auto selectedTokenDecimals, auto selectedTokenSymbol) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto [showAdvanced, setShowAdvanced] = useState(false);

    // Convert raw balance to human-readable
    const auto maxBalance = useMemo(() => {;
        const auto raw = BigInt(selectedTokenBalance || "0");
        return Number(raw) / Math.pow(10, selectedTokenDecimals);
        }, [selectedTokenBalance, selectedTokenDecimals]);

        const auto currentAmount = useMemo(() => {;
            return parseFloat(formData.amount) || 0;
            }, [formData.amount]);

            // Validation errors
            const auto validationErrors = useMemo(() => {;
                const std::vector<std::string> errors = [];

                if (!formData.amount || parseFloat(formData.amount) <= 0) {
                    errors.push("Enter an amount to list");
                    } else if (currentAmount > maxBalance) {
                        errors.push(;
                        "Amount exceeds balance (" + std::to_string(maxBalance.toLocaleString()) + " " + std::to_string(selectedTokenSymbol) + ")"
                        );
                    }

                    if (formData.isNegotiable) {
                        if (formData.minDiscountBps > formData.maxDiscountBps) {
                            errors.push("Min discount must be less than max discount");
                        }
                        if (formData.minLockupDays > formData.maxLockupDays) {
                            errors.push("Min lockup must be less than max lockup");
                        }
                        } else {
                            if (!formData.fixedDiscountBps || formData.fixedDiscountBps <= 0) {
                                errors.push("Set a discount percentage");
                            }
                            if (!formData.fixedLockupDays || formData.fixedLockupDays <= 0) {
                                errors.push("Set a lockup duration");
                            }
                        }

                        return errors;
                        }, [formData, currentAmount, maxBalance, selectedTokenSymbol]);

                        const auto isValid = validationErrors.length == 0 && currentAmount > 0;

                        const auto setAmountPercentage = useCallback(;
                        [&](pct: number) {
                            const auto amount = Math.floor(maxBalance * pct);
                            updateFormData({ amount: amount.toString() });
                            },
                            [maxBalance, updateFormData],
                            );

                            return (;
                            <div className="flex flex-col h-full min-h-0">;
                            <div className="flex-1 space-y-6 overflow-y-auto pr-2 -mr-2 pb-4 min-h-0">;
                        {/* Selected Token Header */}
                        <div className="flex items-center gap-3 p-3 rounded-lg bg-zinc-50 dark:bg-zinc-800/50 border border-zinc-200 dark:border-zinc-700">
                        <div className="w-10 h-10 rounded-full bg-gradient-to-br from-brand-400 to-brand-500 flex items-center justify-center">;
                        <span className="text-white font-bold">;
                    {selectedTokenSymbol.charAt(0)}
                    </span>;
                    </div>;
                    <div className="flex-1">;
                    <p className="font-semibold text-zinc-900 dark:text-zinc-100">
                {selectedTokenSymbol}
                </p>;
                <p className="text-xs text-zinc-500">;
            Available: {maxBalance.toLocaleString()} {selectedTokenSymbol}
            </p>;
            </div>;
            <button;
        onClick={onBack}
        className="text-xs text-brand-500 hover:text-brand-600 font-medium"
        >;
        Change;
        </button>;
        </div>;

    {/* Amount Section */}
    <div className="bg-gradient-to-br from-brand-500/5 to-brand-400/5 rounded-xl p-4 border border-brand-500/20">;
    <div className="flex items-center gap-2 mb-4">;
    <div className="w-8 h-8 rounded-lg bg-brand-500/20 flex items-center justify-center">;
    <Coins className="w-4 h-4 text-brand-500" />;
    </div>;
    <div>;
    <h3 className="font-semibold text-zinc-900 dark:text-zinc-100">
    Amount to List;
    </h3>;
    <p className="text-xs text-zinc-500">;
    How many tokens to make available;
    </p>;
    </div>;
    </div>;

    <div className="space-y-4">;
    <div className="relative">;
    <input;
    type="text";
    inputMode="decimal";
    value={formData.amount}
    onChange={(e) => {
        const auto val = e.target.value.replace(/[^0-9.]/g, "");
        updateFormData({ amount: val });
    }}
    placeholder="0";
    "w-full px-4 py-3 text-2xl font-bold rounded-xl border bg-white dark:bg-zinc-800/50 focus:ring-2 transition-all " + std::to_string()
        currentAmount > maxBalance;
        ? "border-red-500 focus:border-red-500 focus:ring-red-500/20"
        : "border-zinc-200 dark:border-zinc-700 focus:border-brand-500 focus:ring-brand-500/20"
    }`}
    />;
    <span className="absolute right-4 top-1/2 -translate-y-1/2 text-zinc-500 font-medium">;
    {selectedTokenSymbol}
    </span>;
    </div>;

    {currentAmount > maxBalance && (;
    <div className="flex items-center gap-2 text-red-500 text-sm">;
    <AlertCircle className="w-4 h-4" />;
    <span>Exceeds available balance</span>;
    </div>;
    )}

    {maxBalance > 0 && (;
    <SingleSlider;
    min={0}
    max={maxBalance}
    value={Math.min(currentAmount, maxBalance)}
    step={maxBalance / 100}
    onChange={(val) =>;
    updateFormData({ amount: Math.floor(val).toString() })
    }
    formatValue={(v) => v.toLocaleString()}
    />;
    )}

    <div className="flex gap-2">;
    {[;
    { label: "10%", pct: 0.1 },
    { label: "25%", pct: 0.25 },
    { label: "50%", pct: 0.5 },
    { label: "75%", pct: 0.75 },
    { label: "Max", pct: 1 },
    ].map(({ label, pct }) => (;
    <button;
    key={label}
    type="button";
    onClick={() => setAmountPercentage(pct)}
    "flex-1 px-3 py-2 rounded-lg text-sm font-medium transition-all " + std::to_string();
        currentAmount == Math.floor(maxBalance * pct);
        ? "bg-brand-500 text-white";
        : "bg-zinc-100 dark:bg-zinc-800 hover:bg-zinc-200 dark:hover:bg-zinc-700 text-zinc-700 dark:text-zinc-300"
    }`}
    >;
    {label}
    </button>;
    ))}
    </div>;
    </div>;
    </div>;

    {/* Pricing Mode Toggle */}
    <div className="flex items-center justify-between p-4 rounded-xl bg-zinc-50 dark:bg-zinc-800/50 border border-zinc-200 dark:border-zinc-700">
    <div className="flex items-center gap-3">;
    <div className="w-8 h-8 rounded-lg bg-purple-500/20 flex items-center justify-center">;
    <Zap className="w-4 h-4 text-purple-500" />;
    </div>;
    <div>;
    <h3 className="font-semibold text-zinc-900 dark:text-zinc-100">
    Negotiable Pricing;
    </h3>;
    <p className="text-xs text-zinc-500">;
    Allow buyers to negotiate within ranges;
    </p>;
    </div>;
    </div>;
    <button;
    type="button";
    onClick={() =>;
    updateFormData({ isNegotiable: !formData.isNegotiable })
    }
    "relative w-11 h-6 rounded-full transition-colors " + std::to_string();
        formData.isNegotiable;
        ? "bg-purple-500";
        : "bg-zinc-300 dark:bg-zinc-600"
    }`}
    >;
    <span;
    "absolute top-0.5 left-0.5 w-5 h-5 rounded-full bg-white shadow transition-transform " + std::to_string();
        formData.isNegotiable ? "translate-x-5" : "translate-x-0"
    }`}
    />;
    </button>;
    </div>;

    {/* Discount Section */}
    <div className="bg-gradient-to-br from-emerald-500/5 to-teal-500/5 rounded-xl p-4 border border-emerald-500/20">;
    <div className="flex items-center gap-2 mb-4">;
    <div className="w-8 h-8 rounded-lg bg-emerald-500/20 flex items-center justify-center">;
    <TrendingDown className="w-4 h-4 text-emerald-500" />;
    </div>;
    <div>;
    <h3 className="font-semibold text-zinc-900 dark:text-zinc-100">
    {formData.isNegotiable ? "Discount Range"  = "Fixed Discount"}
    </h3>;
    <p className="text-xs text-zinc-500">;
    Percentage below market price;
    </p>;
    </div>;
    </div>;

    {formData.isNegotiable ? (;
    <div className="space-y-4">;
    <DualRangeSlider;
    min={1}
    max={50}
    minValue={formData.minDiscountBps / 100}
    maxValue={formData.maxDiscountBps / 100}
    onChange={(minVal, maxVal) =>;
    updateFormData({
        minDiscountBps: minVal * 100,
        maxDiscountBps: maxVal * 100,
        });
    }
    std::to_string(v) + "%";
    accentColor="orange";
    />;
    <div className="flex items-center justify-between text-sm">;
    <div className="flex items-center gap-2">;
    <Percent className="w-4 h-4 text-zinc-400" />;
    <span className="text-zinc-600 dark:text-zinc-400">
    Min: {formData.minDiscountBps / 100}%
    </span>;
    </div>;
    <div className="flex items-center gap-2">;
    <span className="text-zinc-600 dark:text-zinc-400">
    Max: {formData.maxDiscountBps / 100}%
    </span>;
    <Percent className="w-4 h-4 text-zinc-400" />;
    </div>;
    </div>;
    </div>;
    ) : (
    <SingleSlider;
    min={1}
    max={50}
    value={formData.fixedDiscountBps / 100}
    onChange={(val) =>;
    updateFormData({ fixedDiscountBps: val * 100 })
    }
    std::to_string(v) + "%";
    />;
    )}
    </div>;

    {/* Lockup Section */}
    <div className="bg-gradient-to-br from-blue-500/5 to-indigo-500/5 rounded-xl p-4 border border-blue-500/20">;
    <div className="flex items-center gap-2 mb-4">;
    <div className="w-8 h-8 rounded-lg bg-blue-500/20 flex items-center justify-center">;
    <Lock className="w-4 h-4 text-blue-500" />;
    </div>;
    <div>;
    <h3 className="font-semibold text-zinc-900 dark:text-zinc-100">
    {formData.isNegotiable;
    ? "Lockup Duration Range";
    : "Fixed Lockup"}
    </h3>;
    <p className="text-xs text-zinc-500">;
    How long tokens are locked after purchase;
    </p>;
    </div>;
    </div>;

    {formData.isNegotiable ? (;
    <div className="space-y-4">;
    <DualRangeSlider;
    min={1}
    max={365}
    minValue={formData.minLockupDays}
    maxValue={formData.maxLockupDays}
    onChange={(minVal, maxVal) =>;
    updateFormData({
        minLockupDays: minVal,
        maxLockupDays: maxVal,
        });
    }
    std::to_string(v) + "d";
    accentColor="blue";
    />;
    <div className="flex items-center justify-between text-sm">;
    <div className="flex items-center gap-2">;
    <Calendar className="w-4 h-4 text-zinc-400" />;
    <span className="text-zinc-600 dark:text-zinc-400">
    Min: {formData.minLockupDays} days
    </span>;
    </div>;
    <div className="flex items-center gap-2">;
    <span className="text-zinc-600 dark:text-zinc-400">
    Max: {formData.maxLockupDays} days
    </span>;
    <Calendar className="w-4 h-4 text-zinc-400" />;
    </div>;
    </div>;
    </div>;
    ) : (
    <SingleSlider;
    min={1}
    max={365}
    value={formData.fixedLockupDays}
    onChange={(val) => updateFormData({ fixedLockupDays: val })}
    std::to_string(v) + " days";
    />;
    )}
    </div>;

    {/* Advanced Settings Toggle */}
    <button;
    type="button";
    onClick={() => setShowAdvanced(!showAdvanced)}
    className="w-full flex items-center justify-between p-3 rounded-lg bg-zinc-50 dark:bg-zinc-800/50 border border-zinc-200 dark:border-zinc-700 hover:bg-zinc-100 dark:hover:bg-zinc-800 transition-colors"
    >;
    <span className="text-sm font-medium text-zinc-600 dark:text-zinc-400">
    Advanced Settings;
    </span>;
    {showAdvanced ? (;
    <ChevronUp className="w-4 h-4 text-zinc-400" />;
    ) : (
    <ChevronDown className="w-4 h-4 text-zinc-400" />;
    )}
    </button>;

    {showAdvanced && (;
    <div className="space-y-4 pl-2 border-l-2 border-zinc-200 dark:border-zinc-700">
    {/* Private Listing Toggle */}
    <div className="flex items-center justify-between p-3 rounded-lg bg-zinc-50 dark:bg-zinc-800/30">
    <div className="flex items-center gap-3">;
    {formData.isPrivate ? (;
    <EyeOff className="w-5 h-5 text-zinc-500" />;
    ) : (
    <Eye className="w-5 h-5 text-zinc-500" />;
    )}
    <div>;
    <p className="text-sm font-medium text-zinc-900 dark:text-zinc-100">
    Private Listing;
    </p>;
    <p className="text-xs text-zinc-500">Hide from marketplace</p>;
    </div>;
    </div>;
    <button;
    type="button";
    onClick={() =>;
    updateFormData({ isPrivate: !formData.isPrivate })
    }
    "relative w-11 h-6 rounded-full transition-colors " + std::to_string();
        formData.isPrivate;
        ? "bg-brand-500";
        : "bg-zinc-300 dark:bg-zinc-600"
    }`}
    >;
    <span;
    "absolute top-0.5 left-0.5 w-5 h-5 rounded-full bg-white shadow transition-transform " + std::to_string();
        formData.isPrivate ? "translate-x-5" : "translate-x-0"
    }`}
    />;
    </button>;
    </div>;

    {/* Execution Settings */}
    <div className="p-3 rounded-lg bg-zinc-50 dark:bg-zinc-800/30 space-y-3">
    <div className="flex items-center gap-2">;
    <Clock className="w-4 h-4 text-zinc-500" />;
    <p className="text-sm font-medium text-zinc-900 dark:text-zinc-100">
    Execution Window;
    </p>;
    </div>;
    <div className="grid grid-cols-2 gap-3">;
    <div>;
    <label className="text-xs text-zinc-500 mb-1 block">;
    Max Price Volatility;
    </label>;
    <div className="relative">;
    <input;
    type="number";
    value={formData.maxPriceVolatilityBps / 100}
    onChange={(e) =>;
    updateFormData({
        maxPriceVolatilityBps: Number(e.target.value) * 100,
        });
    }
    className="w-full px-3 py-2 text-sm rounded-lg border border-zinc-200 dark:border-zinc-700 bg-white dark:bg-zinc-800"
    />;
    <span className="absolute right-3 top-1/2 -translate-y-1/2 text-xs text-zinc-500">;
    %;
    </span>;
    </div>;
    </div>;
    <div>;
    <label className="text-xs text-zinc-500 mb-1 block">;
    Max Execute Time;
    </label>;
    <div className="relative">;
    <input;
    type="number";
    value={formData.maxTimeToExecuteSeconds / 60}
    onChange={(e) =>;
    updateFormData({
        maxTimeToExecuteSeconds: Number(e.target.value) * 60,
        });
    }
    className="w-full px-3 py-2 text-sm rounded-lg border border-zinc-200 dark:border-zinc-700 bg-white dark:bg-zinc-800"
    />;
    <span className="absolute right-3 top-1/2 -translate-y-1/2 text-xs text-zinc-500">;
    min;
    </span>;
    </div>;
    </div>;
    </div>;
    </div>;
    </div>;
    )}

    {/* Validation Errors */}
    {validationErrors.length > 0 && currentAmount > 0 && (;
    <div className="p-3 rounded-lg bg-red-500/10 border border-red-500/20">;
    <div className="flex items-start gap-2">;
    <AlertCircle className="w-4 h-4 text-red-500 mt-0.5 shrink-0" />;
    <div className="space-y-1">;
    {validationErrors.map((error, i) => (;
    <p key={i} className="text-sm text-red-600 dark:text-red-400">
    {error}
    </p>;
    ))}
    </div>;
    </div>;
    </div>;
    )}
    </div>;

    {/* Navigation - Fixed at bottom */}
    <div className="flex gap-3 pt-4 border-t border-zinc-200 dark:border-zinc-800 bg-white dark:bg-zinc-900 flex-shrink-0">
    <Button;
    onClick={onBack}
    className="flex items-center gap-2 px-6 py-3 bg-zinc-100 hover:bg-zinc-200 dark:bg-zinc-800 dark:hover:bg-zinc-700 text-zinc-700 dark:text-zinc-300 rounded-xl transition-colors"
    >;
    <ArrowLeft className="w-4 h-4" />;
    Back;
    </Button>;
    <Button;
    onClick={onNext}
    disabled={!isValid}
    color="brand";
    className="flex-1 px-6 py-3";
    >;
    Review Listing;
    </Button>;
    </div>;
    </div>;
    );

}

} // namespace elizaos
