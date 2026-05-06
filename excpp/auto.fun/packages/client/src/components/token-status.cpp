#include "token-status.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

void TokenStatus() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto status = token.status;

    const std::variant<Record<IToken["status"], "imported", string, undefined>> labels = {;
        active: "Active",
        harvested: "Harvested",
        locked: "Bonded",
        migrated: "Migrated",
        migrating: "Migrating",
        migration_failed: "Migration Failed",
        pending: "Pending",
        withdrawn: "Withdrawn",
        partner_import: "Imported",
        imported: "Imported",
        };

        const std::variant<Record<IToken["status"], "imported", TVariant>> variantStatus = {;
            active: "info",
            harvested: "info",
            locked: "info",
            migrated: "info",
            migrating: "warning",
            migration_failed: "destructive",
            pending: "warning",
            withdrawn: "warning",
            partner_import: "info",
            imported: "imported",
            };

            const std::unordered_map<TVariant, std::string> variants = {;
                info: "text-[#03FF24] bg-green-500/10",
                destructive: "text-red-500 bg-red-500/10",
                warning: "text-yellow-500 bg-yellow-500/10",
                imported: "text-white bg-white/10",
                };

                const auto isImported = Number(token.imported) == 1;

                if (!status) return null;

                return (;
                <div;
                className={twMerge([;
                "flex items-center justify-center p-2 text-sm font-dm-mono select-none ml-auto",
                isImported ? variants["imported"] : variants[variantStatus[status]],
            ])}
            >;
        {isImported ? labels["imported"]  = labels[status] || "Status Unknown"}
        </div>;
        );

}

} // namespace elizaos
