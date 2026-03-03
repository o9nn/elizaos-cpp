import fs from 'fs';
import path from 'path';

const baseDefine = {
    'process.env.SECURE_ORIGIN': JSON.stringify(process.env.SECURE_ORIGIN ?? 'false'),
    // original key, used 2003-2010
    'process.env.LOGIN_RSAE': JSON.stringify(process.env.LOGIN_RSAE ?? '58778699976184461502525193738213253649000149147835990136706041084440742975821'),
    'process.env.LOGIN_RSAN': JSON.stringify(process.env.LOGIN_RSAN ?? '7162900525229798032761816791230527296329313291232324290237849263501208207972894053929065636522363163621000728841182238772712427862772219676577293600221789')
};

// Build mode: 'standard', 'bot', or 'both'
const buildMode = process.env.BUILD_MODE ?? 'both';

// ----

type BunOutput = {
    source: string;
    sourcemap: string;
}

async function bunBuild(entry: string, external: string[] = [], minify = true, drop: string[] = [], customDefine: Record<string, string> = {}): Promise<BunOutput> {
    const build = await Bun.build({
        entrypoints: [entry],
        sourcemap: 'external',
        define: { ...baseDefine, ...customDefine },
        external,
        minify,
        drop,
    });

    if (!build.success) {
        build.logs.forEach(x => console.log(x));
        process.exit(1);
    }

    return {
        source: await build.outputs[0].text(),
        sourcemap: build.outputs[0].sourcemap ? await build.outputs[0].sourcemap.text() : ''
    };
}

// todo: workaround due to a bun bug https://github.com/oven-sh/bun/issues/16509: not remapping external
function replaceDepsUrl(source: string) {
    return source.replaceAll('#3rdparty', '.');
}

// ----

// Create output directories
const outDirs = ['out', 'out/standard', 'out/bot'];
for (const dir of outDirs) {
    if (!fs.existsSync(dir)) {
        fs.mkdirSync(dir, { recursive: true });
    }
}

// Copy shared assets to both directories
for (const outDir of ['out/standard', 'out/bot']) {
    fs.copyFileSync('src/3rdparty/bzip2-wasm/bzip2.wasm', `${outDir}/bzip2.wasm`);
    fs.copyFileSync('src/3rdparty/tinymidipcm/tinymidipcm.wasm', `${outDir}/tinymidipcm.wasm`);
}

// Also copy to root out for backwards compatibility
fs.copyFileSync('src/3rdparty/bzip2-wasm/bzip2.wasm', 'out/bzip2.wasm');
fs.copyFileSync('src/3rdparty/tinymidipcm/tinymidipcm.wasm', 'out/tinymidipcm.wasm');

const args = process.argv.slice(2);
const prod = args[0] !== 'dev';

// Build shared deps
const deps = await bunBuild('./src/3rdparty/deps.js', [], true, ['console']);
fs.writeFileSync('out/deps.js', deps.source);
fs.writeFileSync('out/standard/deps.js', deps.source);
fs.writeFileSync('out/bot/deps.js', deps.source);

// Build configurations
const builds = [
    { name: 'standard', outDir: 'out/standard', enableBotSDK: 'false' },
    { name: 'bot', outDir: 'out/bot', enableBotSDK: 'true' }
];

// Filter builds based on BUILD_MODE
const buildsToRun = builds.filter(b => buildMode === 'both' || buildMode === b.name);

for (const buildConfig of buildsToRun) {
    console.log(`Building ${buildConfig.name} client...`);

    const customDefine = {
        'process.env.ENABLE_BOT_SDK': JSON.stringify(buildConfig.enableBotSDK)
    };

    // Build client
    const clientScript = await bunBuild(
        'src/client/Client.ts',
        ['#3rdparty/*'],
        prod,
        prod ? ['console'] : [],
        customDefine
    );
    fs.writeFileSync(`${buildConfig.outDir}/client.js`, replaceDepsUrl(clientScript.source));

    // Build mapview
    const mapviewScript = await bunBuild(
        'src/mapview/MapView.ts',
        ['#3rdparty/*'],
        prod,
        prod ? ['console'] : [],
        customDefine
    );
    fs.writeFileSync(`${buildConfig.outDir}/mapview.js`, replaceDepsUrl(mapviewScript.source));
}

// Copy bot client to root out for backwards compatibility
if (buildMode === 'both' || buildMode === 'bot') {
    fs.copyFileSync('out/bot/client.js', 'out/client.js');
    fs.copyFileSync('out/bot/mapview.js', 'out/mapview.js');
}

console.log('Build complete!');
