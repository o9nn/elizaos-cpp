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

const stripZeros = (s: string) =>
  s.replace(/(\.\d*?[1-9])0+$/, "$1").replace(/\.0+$/, "");
const toBN = (x: BigNumber.Value) => new BigNumber(x ?? 0);

B`;
  if (n.abs().gte(1_000_000)) return `$${n.div(1_000_000).toFormat(1)}M`;
  if (n.abs().gte(1_000)) return `$${n.div(1_000).toFormat(1)}K`;
  return `$${n.toFormat(dp)}`;
}

%`;
}

 ${sym.toLowerCase()}`;
}



$/.test(id)) return id;
  return `${id.slice(0, 2 + prefix)}…${id.slice(-suffix)}`;
}

 else if (abs.gte(1e9)) {
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
    const val = n.div(div);
    const dp = val.abs().gte(100) ? 0 : digits;
    return `${stripZeros(val.toFormat(dp))}${suffix}`;
  }
  return stripZeros(n.toFormat(2));
}

 ${sym}`;
}

/**
 * Format data as a clean list for Discord display (no tables or emojis)
 */
>,
): string {
  const lines = [`**${title}**`, ""];

  items.forEach((item, index) => {
    if (index > 0) lines.push(""); // Add spacing between items

    lines.push(`**${item.name}**`);
    Object.entries(item.data).forEach(([key, value]) => {
      lines.push(`  ${key}: ${value}`);
    });
  });

  return lines.join("\n");
}

/**
 * Format a single item with key-value pairs in a clean format
 */
**`, ""];

  Object.entries(data).forEach(([key, value]) => {
    lines.push(`**${key}**: ${value}`);
  });

  if (link) {
    lines.push("", `Open in Morpho: ${link}`);
  }

  return lines.join("\n");
}

} // namespace elizaos
