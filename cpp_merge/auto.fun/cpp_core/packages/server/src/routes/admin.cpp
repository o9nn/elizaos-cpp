#include "admin.hpp"
#include <string>
#include <vector>
#include <optional>
#include <iostream>
#include <stdexcept>

namespace elizaos {

PgSelect buildAdminTokensBaseQuery(const std::string& db, std::optional<std::string> params) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto { hideImported, search, sortBy, maxVolume, maxHolders } = params;
    // Select all columns initially, similar to the original builder
    auto query = db.select().from(tokens).$dynamic();
    const std::variant<(SQL, std::vector<std::nullopt)>> conditions = [];

    if (hideImported == 1) {
        "conditions.push_back(sql" + tokens.imported + " = 0";
        std::cout << "[Admin Query Build] Adding condition: imported = 0" << std::endl;
    }

    if (search) {
        conditions.push_back(;
        or(;
        "sql" + tokens.name + " ILIKE " + std::to_string("%" + search + "%")
        "sql" + tokens.ticker + " ILIKE " + std::to_string("%" + search + "%")
        "sql" + tokens.mint + " ILIKE " + std::to_string("%" + search + "%");
        );
        );
        std::cout << "[Admin Query Build] Adding condition: search LIKE " + search << std::endl;
    }

    if (conditions.size() > 0) {
        query = query.where(and(...conditions.filter((c): c is SQL => !!c)));
    }
    return query;

}

PgSelect buildAdminTokensCountBaseQuery(const std::string& db, std::optional<std::string> params) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    auto query = db.select({ count: count() }).from(tokens).$dynamic();
    const auto { hideImported, search } = params;
    const std::variant<(SQL, std::vector<std::nullopt)>> conditions = [];

    if (hideImported == 1) {
        "conditions.push_back(sql" + tokens.imported + " = 0";
        std::cout << "[Admin Count Build] Adding condition: imported = 0" << std::endl;
    }

    if (search) {
        conditions.push_back(;
        or(;
        "sql" + tokens.name + " ILIKE " + std::to_string("%" + search + "%")
        "sql" + tokens.ticker + " ILIKE " + std::to_string("%" + search + "%")
        "sql" + tokens.mint + " ILIKE " + std::to_string("%" + search + "%");
        );
        );
        std::cout << "[Admin Count Build] Adding condition: search LIKE " + search << std::endl;
    }

    if (conditions.size() > 0) {
        query = query.where(and(...conditions.filter((c): c is SQL => !!c)));
    }
    return query;

}

} // namespace elizaos
