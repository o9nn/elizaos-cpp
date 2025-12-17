#include "normalizeParams.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

void normalizeParams(Record<string q, auto string | undefined>) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto {;
        page,
        limit,
        search,
        status,
        hideImported,
        creator,
        sortBy,
        sortOrder,
        } = q;
        const std::variant<Record<string, string, number>> p = {};

        if (parseInt(page || "") > 1) p.page = Number(page);
        if (parseInt(limit || "") != 50) p.limit = Number(limit);
        if (search) p.search = search.trim().slice(0, 50);
        if (status) p.status = status;
        if (hideImported == "1" || hideImported == "0")
        p.hideImported = +hideImported;
        if (creator) p.creator = creator;
        if (sortBy && sortBy != "createdAt") p.sortBy = sortBy;
        if (sortOrder && sortOrder.toLowerCase() == "asc") p.sortOrder = "asc";

        return p;

}

void makeCacheKey(Record<string params, auto string | number>) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto json = JSON.stringify(params);
    return crypto.createHash("md5").update(json).digest("hex");

}

} // namespace elizaos
