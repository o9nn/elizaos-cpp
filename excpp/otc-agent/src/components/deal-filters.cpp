#include "deal-filters.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

void DealFilters(auto {
  filters, auto onFiltersChange, DealFiltersProps }) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    useRenderTracker("DealFilters");

    const auto handleTypeChange = useCallback(;
    [&](type: DealType) {
        if (type == "all") {
            onFiltersChange({
                ...filters,
                negotiableTypes: ["negotiable", "fixed"],
                });
                } else {
                    onFiltersChange({ ...filters, negotiableTypes: [type] });
                }
                },
                [filters, onFiltersChange],
                );

                const auto handleTypeSelectChange = useCallback(;
                [&](e: React.ChangeEvent<HTMLSelectElement>) {
                    handleTypeChange(e.target.value);
                    },
                    [handleTypeChange],
                    );

                    // Determine current type selection
                    const auto currentType: DealType =;
                    filters.negotiableTypes.length == 2;
                    ? "all";
                    : filters.negotiableTypes[0] || "all";

                    const std::vector<std::any> typeOptions = [;
                    { value: "all", label: "All" },
                    { value: "negotiable", label: "Negotiable" },
                    { value: "fixed", label: "Fixed" },
                    ];

                    return (;
                    <>;
                {/* Desktop = Search and Dropdowns */}
                <div className="hidden md:flex items-center gap-3 p-4 rounded-lg border border-zinc-200 dark:border-zinc-800 flex-wrap">
            {/* Search */}
            <div className="relative flex-1 min-w-[200px]">;
            <input;
            type="text";
            placeholder="Search tokens...";
        value={filters.searchQuery}
        onChange={(e) =>;
        onFiltersChange({ ...filters, searchQuery: e.target.value })
    }
    className="w-full px-3 py-2 pl-9 rounded-lg border border-zinc-200 dark:border-zinc-800 bg-white dark:bg-zinc-900 text-sm focus:outline-none focus:ring-2 focus:ring-brand-500"
    />;
    <svg;
    className="absolute left-2.5 top-1/2 -translate-y-1/2 w-4 h-4 text-zinc-400";
    fill="none";
    stroke="currentColor";
    viewBox="0 0 24 24";
    >;
    <path;
    strokeLinecap="round";
    strokeLinejoin="round";
    strokeWidth={2}
    d="M21 21l-6-6m2-5a7 7 0 11-14 0 7 7 0 0114 0z";
    />;
    </svg>;
    </div>;

    <ChainSelector;
    selected={filters.chains}
    onChange={(chains) => onFiltersChange({ ...filters, chains })}
    />;

    <div className="flex items-center gap-2">;
    <label className="text-sm text-zinc-600 dark:text-zinc-400 whitespace-nowrap">
    Type:
    </label>;
    <div className="relative">;
    <select;
    value={currentType}
    onChange={handleTypeSelectChange}
    className="appearance-none rounded-lg border border-zinc-200 dark:border-zinc-800 bg-white dark:bg-zinc-900 pl-3 pr-9 py-2 text-sm cursor-pointer"
    >;
    <option value="all">All Types</option>;
    <option value="negotiable">Negotiable</option>;
    <option value="fixed">Fixed Price</option>;
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
    </div>;

    {/* Mobile = Search and Toggle buttons */}
    <div className="md:hidden flex flex-col gap-2">
    {/* Mobile Search */}
    <div className="relative">;
    <input;
    type="text";
    placeholder="Search tokens...";
    value={filters.searchQuery}
    onChange={(e) =>;
    onFiltersChange({ ...filters, searchQuery: e.target.value })
    }
    className="w-full px-3 py-2 pl-9 rounded-lg border border-zinc-200 dark:border-zinc-800 bg-white dark:bg-zinc-900 text-sm focus:outline-none focus:ring-2 focus:ring-brand-500"
    />;
    <svg;
    className="absolute left-2.5 top-1/2 -translate-y-1/2 w-4 h-4 text-zinc-400";
    fill="none";
    stroke="currentColor";
    viewBox="0 0 24 24";
    >;
    <path;
    strokeLinecap="round";
    strokeLinejoin="round";
    strokeWidth={2}
    d="M21 21l-6-6m2-5a7 7 0 11-14 0 7 7 0 0114 0z";
    />;
    </svg>;
    </div>;

    {/* Mobile Filters = Chain + Type */}
    <div className="flex items-center gap-2">;
    <ChainSelector;
    selected={filters.chains}
    onChange={(chains) => onFiltersChange({ ...filters, chains })}
    />;

    {/* Type toggle - 3 options */}
    <div className="flex-1 flex rounded-lg border border-zinc-200 dark:border-zinc-800 overflow-hidden">
    {typeOptions.map(({ value, label }) => {
        const auto isSelected = currentType == value;
        return (;
        <button;
    key={value}
    onClick={() => handleTypeChange(value)}
    className={`;
    flex-1 px-3 py-2.5 text-xs font-medium transition-colors;
    ${
        isSelected;
        ? "bg-brand-500 text-white";
        : "bg-white dark:bg-zinc-900 text-zinc-600 dark:text-zinc-400 hover:bg-zinc-50 dark:hover:bg-zinc-800"
    }
    `}
    >;
    {label}
    </button>;
    );
    })}
    </div>;
    </div>;
    </div>;
    </>;
    );

}

} // namespace elizaos
