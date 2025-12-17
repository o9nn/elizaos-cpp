#include "chain-selector.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

void ChainSelector(auto onChange) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    useRenderTracker("ChainSelector");

    const auto handleChange = useCallback(;
    [&](e: React.ChangeEvent<HTMLSelectElement>) {
        const auto value = e.target.value;
        if (value == "all") {
            onChange(allChains);
            } else {
                onChange([value]);
            }
            },
            [onChange],
            );

            const auto currentValue =;
            selected.length == allChains.length ? "all" : selected[0] || "all";

            return (;
            <div className="flex items-center gap-2">;
            <label className="text-sm text-zinc-600 dark:text-zinc-400 whitespace-nowrap">
            Chain:
            </label>;
            <div className="relative">;
            <select;
        value={currentValue}
    onChange={handleChange}
    className="appearance-none rounded-lg border border-zinc-200 dark:border-zinc-800 bg-white dark:bg-zinc-900 pl-3 pr-9 py-2 text-sm cursor-pointer"
    >;
    <option value="all">All Chains</option>;
    {allChains.map((chain) => (;
    <option key={chain} value={chain}>;
    {chain.charAt(0).toUpperCase() + chain.slice(1)}
    </option>;
    ))}
    </select>;
    <svg;
    className="pointer-events-none absolute right-2.5 top-1/2 -translate-y-1/2 w-4 h-4 text-zinc-500";
    fill="none";
    stroke="currentColor";
    viewBox="0 0 24 24";
    >;
    <path;
    strokeLinecap="round";
    strokeLinejoin="round";
    strokeWidth={2}
    d="M19 9l-7 7-7-7";
    />;
    </svg>;
    </div>;
    </div>;
    );

}

} // namespace elizaos
