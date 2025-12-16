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

/**
 * Compare multiple run results
 */

;
;

/**
 * Get resolved instance IDs from results file
 */


  return new Set(data.resolved_ids || []);
}

/**
 * Get submitted instance IDs from results file
 */


/**
 * Show statistics for a single run
 */
`);
  console.log(`Total resolved: ${resolvedIds.length}`);
}

/**
 * Compare many runs
 */


  // Build comparison table
  const header = ['ID', ...paths.map((_, i) => String(i)), 'Success rate'];
  const table: Array<Array<string | number>> = [];

  
    if (resolved.includes(id)) {
      return '✅';
    }
    return '❌';
  }

  const idsToCompare = new Set(evaluatedIds.get(paths[0]) || []);

  for (const id of Array.from(idsToCompare).sort()) {
    const row: Array<string | number> = [id];

    for (const filePath of paths) {
      row.push(getEmoji(id, filePath));
    }

    const nSuccess = paths.filter((p) => (resolvedIds.get(p) || []).includes(id)).length;
    const nEvaluated = paths.filter((p) => (evaluatedIds.get(p) || []).includes(id)).length;

    row.push(nEvaluated > 0 ? (nSuccess / nEvaluated).toFixed(2) : '0.00');
    table.push(row);
  }

  // Add summary rows
  const successes: Array<string | number> = ['Successes'];
  const successRates: Array<string | number> = ['Success rates'];

  for (const filePath of paths) {
    const nSuccess = Array.from(idsToCompare).filter((id) => (resolvedIds.get(filePath) || []).includes(id)).length;
    const nEvaluated = Array.from(idsToCompare).filter((id) => (evaluatedIds.get(filePath) || []).includes(id)).length;

    successes.push(nSuccess);
    successRates.push(nEvaluated > 0 ? (nSuccess / nEvaluated).toFixed(2) : '0.00');
  }

  table.push(successes);
  table.push(successRates);

  // Print table
  console.table(
    table.map((row) => {
      const obj: Record<string, any> = {};
      header.forEach((h, i) => {
        obj[h] = row[i];
      });
      return obj;
    }),
  );

  // Print summary
  console.log('\nSummary:');
  const summaryTable = paths.map((p, i) => ({
    '#': i,
    ID: path.basename(path.dirname(p)),
    Successes: successes[i + 1],
    'Success rate': successRates[i + 1],
  }));
  console.table(summaryTable);
}

/**
 * Compare a pair of runs
 */
, old ${oldEvaluatedIds.length}`);
  console.log(`Total resolved: new ${resolvedIds.length}, old ${oldResolvedIds.length}`);
  console.log('-'.repeat(80));
  console.log('Emoji legend:');
  console.log("❓: Not evaluated in old version, so guessing it's either 😀 or 👾");
  console.log('😀: Newly resolved in new version');
  console.log('✅: Resolved in both');
  console.log('❌: Resolved in old, not in new');
  console.log('👾: Unresolved in both');
  console.log('-'.repeat(80));

  for (const id of evaluatedIds) {
    const resolvedNow = resolvedIds.includes(id);
    const resolvedBefore = oldResolvedIds.includes(id);
    const inOldEvaluated = oldEvaluatedIds.includes(id);

    let emoji: string;

    if (!inOldEvaluated && resolvedNow) {
      emoji = '😀❓';
    } else if (!inOldEvaluated && !resolvedNow) {
      emoji = '👾❓';
    } else if (resolvedNow && !resolvedBefore) {
      emoji = '😀';
    } else if (resolvedNow && resolvedBefore) {
      emoji = '✅';
      if (!showSame) {
        continue;
      }
    } else if (!resolvedNow && resolvedBefore) {
      emoji = '❌';
    } else {
      emoji = '👾';
      if (!showSame) {
        continue;
      }
    }

    console.log(`${emoji} ${id}`);
  }
}

/**
 * Main `);
    }
  }

  if (resultPaths.length === 1) {
    statsSingle(resultPaths[0]);
  } else if (resultPaths.length === 2) {
    comparePair(resultPaths[0], resultPaths[1], showSame);
  } else {
    compareMany(resultPaths);
  }
}

} // namespace elizaos
