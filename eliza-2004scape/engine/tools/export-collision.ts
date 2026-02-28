#!/usr/bin/env bun
// Exports collision data from a running server
// Usage: bun engine/tools/export-collision.ts [server-url]
// Default: http://localhost:8888

import fs from 'fs';

const serverUrl = process.argv[2] || 'http://localhost:8888';
const outputPath = 'sdk/collision-data.json';

console.log(`Fetching collision data from ${serverUrl}/api/exportCollision...`);

const response = await fetch(`${serverUrl}/api/exportCollision`);
if (!response.ok) {
    console.error(`Failed to fetch: ${response.status} ${response.statusText}`);
    process.exit(1);
}

const data = await response.json();
console.log(`Received ${data.tiles.length} tiles with collision`);

// Write JSON
fs.writeFileSync(outputPath, JSON.stringify(data));
const stats = fs.statSync(outputPath);
console.log(`Written to ${outputPath} (${(stats.size / 1024 / 1024).toFixed(2)} MB)`);

// Write compressed version
const compressed = Bun.gzipSync(Buffer.from(JSON.stringify(data)));
fs.writeFileSync(`${outputPath}.gz`, compressed);
console.log(`Compressed: ${(compressed.length / 1024 / 1024).toFixed(2)} MB`);
