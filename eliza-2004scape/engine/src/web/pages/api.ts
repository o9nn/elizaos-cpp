import fs from 'fs';
import * as rsmod from '@2004scape/rsmod-pathfinder';
import { CollisionFlag } from '@2004scape/rsmod-pathfinder';

export async function handleScreenshotUpload(req: Request, url: URL): Promise<Response | null> {
    if (url.pathname !== '/api/screenshot' || req.method !== 'POST') {
        return null;
    }

    try {
        const data = await req.text();
        const base64Data = data.replace(/^data:image\/png;base64,/, '');
        const filename = `screenshot-${Date.now()}.png`;
        const filepath = `screenshots/${filename}`;
        fs.writeFileSync(filepath, Buffer.from(base64Data, 'base64'));
        return new Response(JSON.stringify({ success: true, filename }), {
            headers: { 'Content-Type': 'application/json' }
        });
    } catch (e: any) {
        return new Response(JSON.stringify({ success: false, error: e.message }), {
            status: 500,
            headers: { 'Content-Type': 'application/json' }
        });
    }
}

// Export collision data for SDK bundling
export function handleExportCollisionApi(url: URL): Response | null {
    if (url.pathname !== '/api/exportCollision') {
        return null;
    }

    try {
        console.log('Exporting collision data...');

        // Flag bits to export (excluding WALL_* flags)
        // Wall flags are excluded so the pathfinder routes through doorways.
        // The server enforces actual collision - doors just need to be opened at runtime.
        const FLAG_BITS = [
            CollisionFlag.LOC,
            CollisionFlag.FLOOR,
            CollisionFlag.ROOF,
        ];

        // World bounds (mapsquare coordinates)
        const MIN_MX = 29, MAX_MX = 54;
        const MIN_MZ = 44, MAX_MZ = 64;
        const LEVELS = 4;

        const tiles: Array<[number, number, number, number]> = [];
        // Track all allocated zones so SDK can allocate them even if they have no collision tiles
        const zones: Array<[number, number, number]> = [];

        for (let level = 0; level < LEVELS; level++) {
            for (let mx = MIN_MX; mx <= MAX_MX; mx++) {
                for (let mz = MIN_MZ; mz <= MAX_MZ; mz++) {
                    const baseX = mx << 6;
                    const baseZ = mz << 6;

                    for (let zx = 0; zx < 8; zx++) {
                        for (let zz = 0; zz < 8; zz++) {
                            const zoneBaseX = baseX + (zx << 3);
                            const zoneBaseZ = baseZ + (zz << 3);

                            if (!rsmod.isZoneAllocated(zoneBaseX, zoneBaseZ, level)) {
                                continue;
                            }

                            // Record this zone as allocated
                            zones.push([level, zoneBaseX, zoneBaseZ]);

                            for (let dx = 0; dx < 8; dx++) {
                                for (let dz = 0; dz < 8; dz++) {
                                    const x = zoneBaseX + dx;
                                    const z = zoneBaseZ + dz;

                                    let flags = 0;
                                    for (const bit of FLAG_BITS) {
                                        if (rsmod.isFlagged(x, z, level, bit)) {
                                            flags |= bit;
                                        }
                                    }

                                    if (flags !== 0) {
                                        tiles.push([level, x, z, flags]);
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }

        console.log(`Exported ${tiles.length} tiles with collision, ${zones.length} zones allocated`);

        return new Response(JSON.stringify({ tiles, zones }), {
            headers: { 'Content-Type': 'application/json' }
        });
    } catch (e: any) {
        return new Response(JSON.stringify({
            success: false,
            error: e.message
        }), {
            status: 500,
            headers: { 'Content-Type': 'application/json' }
        });
    }
}
