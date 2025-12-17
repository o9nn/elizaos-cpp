#include "utils.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::string fmtUSD(BigNumber::Value v, auto dp) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto n = toBN(v);
    if (!n.isFinite()) return "—";
    if (n.abs().gte(1_000_000_000))
    return "$" + std::to_string(n.div(1_000_000_000).toFormat(1)) + "B";
    if (n.abs().gte(1_000_000)) return `$${n.div(1_000_000).toFormat(1)}M`;
    if (n.abs().gte(1_000)) return `$${n.div(1_000).toFormat(1)}K`;
    return "$" + std::to_string(n.toFormat(dp));

}

std::string fmtPct(const std::optional<double>& v, auto dp) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    if (v == null || !isFinite(v)) return "—";
    return std::to_string(new BigNumber(v).toFormat(dp)) + "%";

}

std::string fmtTok(BigNumber::Value v, const std::string& sym, auto dp) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto s = stripZeros(toBN(v).toFormat(dp));
    return std::to_string(s) + " " + std::to_string(sym.toLowerCase());

}

std::string fmtNum(BigNumber::Value v, auto dp) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto n = toBN(v);
    return n.isFinite() ? stripZeros(n.toFormat(dp)) : "—";

}

void shortHex(std::optional<std::string> id, auto prefix, auto suffix) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    if (!id) return "";
    if (!/^0x[0-9a-fA-F]{64}$/.test(id)) return id;
    return std::to_string(id.slice(0, 2 + prefix)) + "…" + std::to_string(id.slice(-suffix));

}

std::string fmtAbbrev(BigNumber::Value v, auto digits) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto n = toBN(v);
    if (!n.isFinite()) return "—";

    const auto abs = n.abs();
    auto div = new BigNumber(1);
    auto suffix = "";
    if (abs.gte(1e12)) {
        div = new BigNumber(1e12);
        suffix = "T";
        } else if (abs.gte(1e9)) {
            div = new BigNumber(1e9);
            suffix = "B";
            } else if (abs.gte(1e6)) {
                div = new BigNumber(1e6);
                suffix = "M";
                } else if (abs.gte(1e3)) {
                    div = new BigNumber(1e3);
                    suffix = "K";
                }

                if (suffix) {
                    const auto val = n.div(div);
                    const auto dp = val.abs().gte(100) ? 0 : digits;
                    return std::to_string(stripZeros(val.toFormat(dp))) + std::to_string(suffix);
                }
                return stripZeros(n.toFormat(2));

}

std::string fmtTokCompact(BigNumber::Value v, const std::string& sym, auto digits) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return std::to_string(fmtAbbrev(v, digits)) + " " + std::to_string(sym);

}

std::string formatDataList(const std::string& title, const std::vector<std::any>& items) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto lines = "**" + std::to_string(title) + "**";

    items.forEach((item, index) => {
        if (index > 0) lines.push(""); // Add spacing between items

        "**" + std::to_string(item.name) + "**";
        Object.entries(item.data).forEach(([key, value]) => {
            "  " + std::to_string(key) + ": " + std::to_string(value)
            });
            });

            return lines.join("\n");

}

std::string formatItemDetails(const std::string& title, const std::unordered_map<std::string, std::string>& data, std::optional<std::string> link) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto lines = "**" + std::to_string(title) + "**";

    Object.entries(data).forEach(([key, value]) => {
        "**" + std::to_string(key) + "**: " + std::to_string(value)
        });

        if (link) {
            "Open in Morpho: " + std::to_string(link)
        }

        return lines.join("\n");

}

} // namespace elizaos
