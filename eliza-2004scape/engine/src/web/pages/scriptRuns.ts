import fs from 'fs';
import path from 'path';
import { escapeHtml, timeAgo, formatDuration, getMimeType } from '../utils.js';

interface RunEvent {
    timestamp: number;
    type: string;
    content: string;
    state?: object;
}

export function handleScriptRunsListPage(url: URL): Response | null {
    if (url.pathname !== '/script_runs' && url.pathname !== '/script_runs/') {
        return null;
    }

    const scriptsDir = '../scripts';
    if (!fs.existsSync(scriptsDir)) {
        return new Response('Scripts directory not found', { status: 404 });
    }

    const scripts = fs.readdirSync(scriptsDir)
        .filter(f => {
            const runsDir = path.join(scriptsDir, f, 'runs');
            return fs.existsSync(runsDir) && fs.statSync(runsDir).isDirectory();
        });

    const allRuns: Array<{
        scriptName: string;
        runName: string;
        mtime: number;
        metadata: any;
        duration: string;
        lastScreenshot: string | null;
        actionCount: number;
    }> = [];

    for (const scriptName of scripts) {
        const runsDir = path.join(scriptsDir, scriptName, 'runs');
        const runs = fs.readdirSync(runsDir)
            .filter(f => fs.statSync(path.join(runsDir, f)).isDirectory());

        for (const runName of runs) {
            const runDir = path.join(runsDir, runName);
            const stat = fs.statSync(runDir);
            const metadataPath = path.join(runDir, 'metadata.json');
            const eventsPath = path.join(runDir, 'events.jsonl');
            const screenshotsDir = path.join(runDir, 'screenshots');

            let metadata = null;
            let duration = '';
            let lastScreenshot: string | null = null;
            let actionCount = 0;

            if (fs.existsSync(metadataPath)) {
                try {
                    metadata = JSON.parse(fs.readFileSync(metadataPath, 'utf-8'));
                    if (metadata.startTime && metadata.endTime) {
                        duration = formatDuration(metadata.endTime - metadata.startTime);
                    }
                } catch {}
            }

            if (fs.existsSync(eventsPath)) {
                try {
                    const eventsRaw = fs.readFileSync(eventsPath, 'utf-8').split('\n').filter(Boolean);
                    actionCount = eventsRaw.filter(line => {
                        try {
                            const event = JSON.parse(line);
                            return event.type === 'action';
                        } catch { return false; }
                    }).length;
                } catch {}
            }

            if (fs.existsSync(screenshotsDir)) {
                try {
                    const screenshots = fs.readdirSync(screenshotsDir)
                        .filter(s => s.endsWith('.png'))
                        .sort();
                    if (screenshots.length > 0) {
                        lastScreenshot = screenshots[screenshots.length - 1];
                    }
                } catch {}
            }

            allRuns.push({
                scriptName,
                runName,
                mtime: stat.mtimeMs,
                metadata,
                duration,
                lastScreenshot,
                actionCount
            });
        }
    }

    allRuns.sort((a, b) => b.mtime - a.mtime);

    const scriptCounts = new Map<string, number>();
    for (const run of allRuns) {
        scriptCounts.set(run.scriptName, (scriptCounts.get(run.scriptName) || 0) + 1);
    }

    const html = `<!DOCTYPE html>
<html><head><title>Script Runs</title>
<style>
* { box-sizing: border-box; }
body { background: #0a0a0a; color: #888; font-family: -apple-system, BlinkMacSystemFont, 'Segoe UI', Roboto, sans-serif; padding: 16px; margin: 0; }
.container { max-width: 1400px; margin: 0 auto; }
h1 { color: #5bf; font-weight: 500; font-size: 18px; margin: 0 0 4px 0; display: inline; }
.subtitle { color: #555; font-size: 12px; display: inline; margin-left: 12px; }
.header { margin-bottom: 12px; }
.script-filter { display: flex; gap: 6px; flex-wrap: wrap; margin-bottom: 12px; }
.script-tag { padding: 2px 8px; background: rgba(255,255,255,0.05); border: 1px solid #333; border-radius: 3px; color: #888; font-size: 11px; text-decoration: none; transition: all 0.15s; }
.script-tag:hover { border-color: #5bf; color: #5bf; }
.script-tag.active { background: rgba(85,187,255,0.1); border-color: #5bf; color: #5bf; }
.runs { display: flex; flex-direction: column; gap: 2px; }
.run { border: 1px solid #222; border-radius: 4px; overflow: hidden; transition: border-color 0.15s; background: rgba(255,255,255,0.02); }
.run:hover { border-color: #444; }
.run a { color: inherit; text-decoration: none; display: flex; gap: 10px; align-items: stretch; }
.run-thumb-wrap { width: 240px; height: 160px; overflow: hidden; background: #111; flex-shrink: 0; }
.run-thumb { width: 360px; height: 240px; object-fit: cover; object-position: 0 0; }
.run-thumb-placeholder { width: 240px; height: 160px; background: #111; flex-shrink: 0; display: flex; align-items: center; justify-content: center; color: #333; font-size: 11px; }
.run-content { flex: 1; min-width: 0; padding: 8px 10px 8px 0; display: flex; flex-direction: column; justify-content: center; }
.run-header { display: flex; justify-content: space-between; align-items: center; margin-bottom: 2px; }
.run-name { font-weight: 500; font-size: 12px; color: #5bf; overflow: hidden; text-overflow: ellipsis; white-space: nowrap; }
.run-time { color: #555; font-size: 11px; flex-shrink: 0; margin-left: 8px; }
.run-goal { font-size: 11px; color: #666; margin-bottom: 4px; overflow: hidden; text-overflow: ellipsis; white-space: nowrap; }
.run-meta { display: flex; gap: 10px; font-size: 11px; color: #555; align-items: center; }
.script-badge { padding: 1px 5px; background: rgba(255,255,255,0.05); border-radius: 2px; font-size: 10px; color: #888; }
.outcome { padding: 1px 5px; border-radius: 2px; font-size: 10px; font-weight: 500; }
.outcome.success { background: rgba(30,126,52,0.2); color: #4a4; }
.outcome.timeout { background: rgba(230,81,0,0.2); color: #e65100; }
.outcome.stall { background: rgba(230,150,0,0.2); color: #ca0; }
.outcome.error { background: rgba(198,40,40,0.2); color: #c44; }
.ss-btn { margin-left: auto; padding: 1px 6px; font-size: 10px; color: #555; text-decoration: none; border: 1px solid #333; border-radius: 2px; }
.ss-btn:hover { border-color: #5bf; color: #5bf; }
.empty { color: #555; padding: 40px; text-align: center; }
</style></head>
<body>
<div class="container">
<div class="header"><h1>Script Runs</h1><span class="subtitle">${allRuns.length} runs across ${scripts.length} scripts</span></div>
<div class="script-filter">
<a href="/script_runs" class="script-tag active">All</a>
${scripts.map(s => `<a href="/script_runs/${s}/" class="script-tag">${s} (${scriptCounts.get(s)})</a>`).join('')}
</div>
<div class="runs">
${allRuns.map(r => `<div class="run">
<a href="/script_runs/${r.scriptName}/${r.runName}/">
${r.lastScreenshot
    ? `<div class="run-thumb-wrap"><img class="run-thumb" src="/script_runs/${r.scriptName}/${r.runName}/screenshots/${r.lastScreenshot}" alt=""></div>`
    : `<div class="run-thumb-placeholder">No screenshot</div>`}
<div class="run-content">
<div class="run-header">
<span class="run-name">${r.runName}</span>
<span class="run-time">${timeAgo(r.mtime)}</span>
</div>
<div class="run-goal">${r.metadata?.goal || 'No goal'}</div>
<div class="run-meta">
<span class="script-badge">${r.scriptName}</span>
${r.metadata?.outcome ? `<span class="outcome ${r.metadata.outcome}">${r.metadata.outcome}</span>` : ''}
<span>${r.actionCount} actions</span>
<span>${r.duration || 'N/A'}</span>
<a href="/script_runs/${r.scriptName}/${r.runName}/?mode=screenshots" class="ss-btn" onclick="event.stopPropagation()">screenshots</a>
</div>
</div>
</a>
</div>`).join('')}
${allRuns.length === 0 ? '<div class="empty">No script runs yet</div>' : ''}
</div>
</div>
</body></html>`;

    return new Response(html, { headers: { 'Content-Type': 'text/html' } });
}

export function handleScriptRunsForScriptPage(url: URL): Response | null {
    const match = url.pathname.match(/^\/script_runs\/([^/]+)\/?$/);
    if (!match) {
        return null;
    }

    const scriptName = match[1];
    const runsDir = path.join('../scripts', scriptName, 'runs');

    if (!fs.existsSync(runsDir) || !fs.statSync(runsDir).isDirectory()) {
        return new Response('Script not found', { status: 404 });
    }

    const runs = fs.readdirSync(runsDir)
        .filter(f => fs.statSync(path.join(runsDir, f)).isDirectory())
        .map(runName => {
            const runDir = path.join(runsDir, runName);
            const stat = fs.statSync(runDir);
            const metadataPath = path.join(runDir, 'metadata.json');
            const eventsPath = path.join(runDir, 'events.jsonl');
            const screenshotsDir = path.join(runDir, 'screenshots');

            let metadata = null;
            let duration = '';
            let lastScreenshot: string | null = null;
            let actionCount = 0;

            if (fs.existsSync(metadataPath)) {
                try {
                    metadata = JSON.parse(fs.readFileSync(metadataPath, 'utf-8'));
                    if (metadata.startTime && metadata.endTime) {
                        duration = formatDuration(metadata.endTime - metadata.startTime);
                    }
                } catch {}
            }

            if (fs.existsSync(eventsPath)) {
                try {
                    const eventsRaw = fs.readFileSync(eventsPath, 'utf-8').split('\n').filter(Boolean);
                    actionCount = eventsRaw.filter(line => {
                        try {
                            const event = JSON.parse(line);
                            return event.type === 'action';
                        } catch { return false; }
                    }).length;
                } catch {}
            }

            if (fs.existsSync(screenshotsDir)) {
                try {
                    const screenshots = fs.readdirSync(screenshotsDir)
                        .filter(s => s.endsWith('.png'))
                        .sort();
                    if (screenshots.length > 0) {
                        lastScreenshot = screenshots[screenshots.length - 1];
                    }
                } catch {}
            }

            return { runName, mtime: stat.mtimeMs, metadata, duration, lastScreenshot, actionCount };
        })
        .sort((a, b) => b.mtime - a.mtime);

    const html = `<!DOCTYPE html>
<html><head><title>${scriptName} - Script Runs</title>
<style>
* { box-sizing: border-box; }
body { background: #0a0a0a; color: #888; font-family: -apple-system, BlinkMacSystemFont, 'Segoe UI', Roboto, sans-serif; padding: 16px; margin: 0; }
.container { max-width: 1400px; margin: 0 auto; }
.back { color: #5bf; text-decoration: none; font-size: 12px; }
.header { margin: 8px 0 12px 0; }
h1 { color: #5bf; font-weight: 500; font-size: 18px; margin: 0; display: inline; }
.subtitle { color: #555; font-size: 12px; display: inline; margin-left: 12px; }
.runs { display: flex; flex-direction: column; gap: 2px; }
.run { border: 1px solid #222; border-radius: 4px; overflow: hidden; transition: border-color 0.15s; background: rgba(255,255,255,0.02); }
.run:hover { border-color: #444; }
.run a { color: inherit; text-decoration: none; display: flex; gap: 10px; align-items: stretch; }
.run-thumb-wrap { width: 240px; height: 160px; overflow: hidden; background: #111; flex-shrink: 0; }
.run-thumb { width: 360px; height: 240px; object-fit: cover; object-position: 0 0; }
.run-thumb-placeholder { width: 240px; height: 160px; background: #111; flex-shrink: 0; display: flex; align-items: center; justify-content: center; color: #333; font-size: 11px; }
.run-content { flex: 1; min-width: 0; padding: 8px 10px 8px 0; display: flex; flex-direction: column; justify-content: center; }
.run-header { display: flex; justify-content: space-between; align-items: center; margin-bottom: 2px; }
.run-name { font-weight: 500; font-size: 12px; color: #5bf; overflow: hidden; text-overflow: ellipsis; white-space: nowrap; }
.run-time { color: #555; font-size: 11px; flex-shrink: 0; margin-left: 8px; }
.run-goal { font-size: 11px; color: #666; margin-bottom: 4px; overflow: hidden; text-overflow: ellipsis; white-space: nowrap; }
.run-meta { display: flex; gap: 10px; font-size: 11px; color: #555; align-items: center; }
.outcome { padding: 1px 5px; border-radius: 2px; font-size: 10px; font-weight: 500; }
.outcome.success { background: rgba(30,126,52,0.2); color: #4a4; }
.outcome.timeout { background: rgba(230,81,0,0.2); color: #e65100; }
.outcome.stall { background: rgba(230,150,0,0.2); color: #ca0; }
.outcome.error { background: rgba(198,40,40,0.2); color: #c44; }
.ss-btn { margin-left: auto; padding: 1px 6px; font-size: 10px; color: #555; text-decoration: none; border: 1px solid #333; border-radius: 2px; }
.ss-btn:hover { border-color: #5bf; color: #5bf; }
.empty { color: #555; padding: 40px; text-align: center; }
</style></head>
<body>
<div class="container">
<a href="/script_runs" class="back">&larr; All Scripts</a>
<div class="header"><h1>${scriptName}</h1><span class="subtitle">${runs.length} runs</span></div>
<div class="runs">
${runs.map(r => `<div class="run">
<a href="/script_runs/${scriptName}/${r.runName}/">
${r.lastScreenshot
    ? `<div class="run-thumb-wrap"><img class="run-thumb" src="/script_runs/${scriptName}/${r.runName}/screenshots/${r.lastScreenshot}" alt=""></div>`
    : `<div class="run-thumb-placeholder">No screenshot</div>`}
<div class="run-content">
<div class="run-header">
<span class="run-name">${r.runName}</span>
<span class="run-time">${timeAgo(r.mtime)}</span>
</div>
<div class="run-goal">${r.metadata?.goal || 'No goal'}</div>
<div class="run-meta">
${r.metadata?.outcome ? `<span class="outcome ${r.metadata.outcome}">${r.metadata.outcome}</span>` : ''}
<span>${r.actionCount} actions</span>
<span>${r.duration || 'N/A'}</span>
<a href="/script_runs/${scriptName}/${r.runName}/?mode=screenshots" class="ss-btn" onclick="event.stopPropagation()">screenshots</a>
</div>
</div>
</a>
</div>`).join('')}
${runs.length === 0 ? '<div class="empty">No runs yet for this script</div>' : ''}
</div>
</div>
</body></html>`;

    return new Response(html, { headers: { 'Content-Type': 'text/html' } });
}

export function handleScriptRunViewerPage(url: URL): Response | null {
    const match = url.pathname.match(/^\/script_runs\/([^/]+)\/([^/]+)\/?$/);
    if (!match) {
        return null;
    }

    const scriptName = match[1];
    const runName = match[2];
    const runDir = path.join('../scripts', scriptName, 'runs', runName);

    if (!fs.existsSync(runDir) || !fs.statSync(runDir).isDirectory()) {
        return new Response('Run not found', { status: 404 });
    }

    const metadataPath = path.join(runDir, 'metadata.json');
    const eventsPath = path.join(runDir, 'events.jsonl');

    if (!fs.existsSync(metadataPath)) {
        return new Response('Run metadata not found', { status: 404 });
    }

    const metadata = JSON.parse(fs.readFileSync(metadataPath, 'utf-8'));
    const events: RunEvent[] = fs.existsSync(eventsPath)
        ? fs.readFileSync(eventsPath, 'utf-8').split('\n').filter(Boolean).map(line => { try { return JSON.parse(line); } catch { return null; } }).filter(Boolean)
        : [];

    const duration = metadata.endTime ? ((metadata.endTime - metadata.startTime) / 1000).toFixed(1) : 'ongoing';
    const screenshotsMode = url.searchParams.get('mode') === 'screenshots';

    const screenshotsDir = path.join(runDir, 'screenshots');
    const screenshots = fs.existsSync(screenshotsDir)
        ? fs.readdirSync(screenshotsDir).filter(f => f.endsWith('.png')).sort()
        : [];

    const renderEvent = (event: RunEvent): string => {
        const time = new Date(event.timestamp).toLocaleTimeString('en-US', {
            hour12: false,
            hour: '2-digit',
            minute: '2-digit',
            second: '2-digit'
        });

        let content = escapeHtml(event.content || '');
        let extraHtml = '';

        if (event.type === 'screenshot') {
            extraHtml = `<img class="screenshot-thumb" src="/script_runs/${scriptName}/${runName}/screenshots/${event.content}" alt="Screenshot">`;
            content = '';
        } else if (event.type === 'code' || event.type === 'result') {
            if (event.type === 'result') {
                try {
                    const parsed = JSON.parse(event.content);
                    content = escapeHtml(JSON.stringify(parsed, null, 2));
                } catch {}
            }
            const lang = event.type === 'result' ? 'json' : 'typescript';
            extraHtml = `<pre class="code-content"><code class="language-${lang}">${content}</code></pre>`;
            content = '';
        } else if (event.type === 'state') {
            extraHtml = `<div class="state-delta">${content}</div>`;
            content = '';
        }

        return `<div class="event ${event.type}">
            <div class="event-row">
                <div class="event-main">
                    ${content ? `<span class="event-content">${content}</span>` : ''}
                    ${extraHtml}
                </div>
                <span class="event-time">${time}</span>
            </div>
        </div>`;
    };

    const eventHtml = events.map(e => renderEvent(e)).join('');

    if (screenshotsMode) {
        const html = `<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Screenshots - ${escapeHtml(metadata.goal)}</title>
    <style>
        * { box-sizing: border-box; }
        body { background: #0a0a0a; color: #888; font-family: -apple-system, BlinkMacSystemFont, sans-serif; margin: 0; padding: 12px; }
        .header { display: flex; align-items: center; gap: 16px; margin-bottom: 12px; }
        .back { color: #5bf; text-decoration: none; font-size: 12px; }
        h1 { color: #5bf; font-size: 14px; margin: 0; font-weight: 500; }
        .meta { color: #555; font-size: 11px; }
        .mode-toggle { margin-left: auto; }
        .mode-toggle a { color: #5bf; font-size: 11px; text-decoration: none; padding: 2px 8px; border: 1px solid #333; border-radius: 3px; }
        .mode-toggle a:hover { border-color: #5bf; }
        .gallery { display: flex; flex-wrap: wrap; gap: 4px; }
        .gallery img { height: 280px; width: auto; cursor: pointer; border: 1px solid #222; border-radius: 2px; }
        .gallery img:hover { border-color: #444; }
        .lightbox { display: none; position: fixed; top: 0; left: 0; width: 100%; height: 100%; background: rgba(0,0,0,0.95); z-index: 1000; justify-content: center; align-items: center; }
        .lightbox.active { display: flex; }
        .lightbox img { max-width: 95%; max-height: 95%; }
        .lightbox-close { position: absolute; top: 20px; right: 30px; color: #666; font-size: 30px; cursor: pointer; }
        .empty { color: #555; padding: 40px; text-align: center; }
    </style>
</head>
<body>
    <div class="header">
        <a href="/script_runs/${scriptName}/" class="back">&larr; ${scriptName}</a>
        <h1>${escapeHtml(metadata.goal)}</h1>
        <span class="meta">${screenshots.length} screenshots &middot; ${duration}s</span>
        <div class="mode-toggle"><a href="?">Events</a></div>
    </div>
    <div class="gallery">
        ${screenshots.length > 0 ? screenshots.map(s => `<img src="/script_runs/${scriptName}/${runName}/screenshots/${s}" alt="${s}">`).join('') : '<div class="empty">No screenshots</div>'}
    </div>
    <div class="lightbox" onclick="this.classList.remove('active')">
        <span class="lightbox-close">&times;</span>
        <img src="" alt="Screenshot">
    </div>
    <script>
        document.querySelectorAll('.gallery img').forEach(img => {
            img.addEventListener('click', e => {
                const lb = document.querySelector('.lightbox');
                lb.querySelector('img').src = img.src;
                lb.classList.add('active');
            });
        });
    </script>
</body>
</html>`;
        return new Response(html, { headers: { 'Content-Type': 'text/html' } });
    }

    const html = `<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>${escapeHtml(metadata.goal)} - ${scriptName}</title>
    <script src="https://cdnjs.cloudflare.com/ajax/libs/highlight.js/11.9.0/highlight.min.js"></script>
    <script src="https://cdnjs.cloudflare.com/ajax/libs/highlight.js/11.9.0/languages/typescript.min.js"></script>
    <script src="https://cdnjs.cloudflare.com/ajax/libs/highlight.js/11.9.0/languages/json.min.js"></script>
    <style>
        * { box-sizing: border-box; margin: 0; padding: 0; }
        body { font-family: -apple-system, BlinkMacSystemFont, 'Segoe UI', Roboto, sans-serif; background: #0a0a0a; color: #888; padding: 12px; line-height: 1.4; }
        .container { max-width: 1400px; margin: 0 auto; }
        .header { display: flex; align-items: center; gap: 12px; margin-bottom: 8px; flex-wrap: wrap; }
        .back { color: #5bf; text-decoration: none; font-size: 12px; }
        h1 { color: #5bf; font-size: 14px; font-weight: 500; }
        .meta { display: flex; gap: 12px; font-size: 11px; color: #555; align-items: center; }
        .outcome { padding: 1px 6px; border-radius: 2px; font-size: 10px; font-weight: 500; }
        .outcome.success { background: rgba(30,126,52,0.2); color: #4a4; }
        .outcome.timeout { background: rgba(230,81,0,0.2); color: #e65100; }
        .outcome.stall { background: rgba(230,150,0,0.2); color: #ca0; }
        .outcome.error { background: rgba(198,40,40,0.2); color: #c44; }
        .mode-toggle { margin-left: auto; }
        .mode-toggle a { color: #5bf; font-size: 11px; text-decoration: none; padding: 2px 8px; border: 1px solid #333; border-radius: 3px; }
        .mode-toggle a:hover { border-color: #5bf; }
        .timeline { display: flex; flex-direction: column; }
        .event { border-left: 2px solid #333; padding: 1px 0 1px 8px; }
        .event.system { border-left-color: #444; }
        .event.thinking { border-left-color: #6666aa; }
        .event.action { border-left-color: #aa8844; }
        .event.console { border-left-color: #666; }
        .event.code { border-left-color: #888855; }
        .event.result { border-left-color: #558855; }
        .event.error { border-left-color: #884444; }
        .event.screenshot { border-left-color: #885588; }
        .event.state { border-left-color: #448888; }
        .event-row { display: flex; align-items: flex-start; gap: 8px; }
        .event-main { flex: 1; min-width: 0; }
        .event-time { color: #333; font-size: 9px; font-family: monospace; flex-shrink: 0; min-width: 60px; text-align: right; }
        .event-content { white-space: pre-wrap; word-break: break-word; font-size: 11px; color: #777; }
        .code-content { margin: 0; padding: 2px 0; overflow-x: auto; background: transparent; }
        .code-content code { font-family: monospace; font-size: 10px; line-height: 1.3; white-space: pre-wrap; word-break: break-word; color: #666; }
        .state-delta { font-family: monospace; font-size: 10px; color: #557777; white-space: pre-wrap; }
        .screenshot-thumb { max-width: 300px; max-height: 200px; cursor: pointer; border: 1px solid #222; border-radius: 2px; display: block; margin: 2px 0; }
        .screenshot-thumb:hover { border-color: #444; }
        .lightbox { display: none; position: fixed; top: 0; left: 0; width: 100%; height: 100%; background: rgba(0,0,0,0.95); z-index: 1000; justify-content: center; align-items: center; }
        .lightbox.active { display: flex; }
        .lightbox img { max-width: 95%; max-height: 95%; }
        .lightbox-close { position: absolute; top: 20px; right: 30px; color: #666; font-size: 30px; cursor: pointer; }
        .hljs { background: transparent; color: #666; }
        .hljs-keyword { color: #806080; }
        .hljs-built_in { color: #608060; }
        .hljs-string { color: #806050; }
        .hljs-number { color: #608050; }
        .hljs-literal { color: #506080; }
        .hljs-comment { color: #404040; }
        .empty-events { color: #444; padding: 20px; text-align: center; font-size: 12px; }
        ::-webkit-scrollbar { width: 4px; height: 4px; }
        ::-webkit-scrollbar-track { background: transparent; }
        ::-webkit-scrollbar-thumb { background: #222; border-radius: 2px; }
    </style>
</head>
<body>
    <div class="container">
        <div class="header">
            <a href="/script_runs/${scriptName}/" class="back">&larr; ${scriptName}</a>
            <h1>${escapeHtml(metadata.goal)}</h1>
            <div class="meta">
                ${metadata.outcome ? `<span class="outcome ${metadata.outcome}">${metadata.outcome}</span>` : ''}
                <span>${duration}s</span>
                <span>${events.length} events</span>
            </div>
            <div class="mode-toggle"><a href="?mode=screenshots">Screenshots</a></div>
        </div>
        <div class="timeline">
            ${eventHtml || '<div class="empty-events">No events</div>'}
        </div>
    </div>
    <div class="lightbox" onclick="this.classList.remove('active')">
        <span class="lightbox-close">&times;</span>
        <img src="" alt="Screenshot">
    </div>
    <script>
        document.addEventListener('DOMContentLoaded', function() {
            document.querySelectorAll('.code-content code').forEach(block => {
                hljs.highlightElement(block);
            });
        });
        document.querySelectorAll('.screenshot-thumb').forEach(img => {
            img.addEventListener('click', e => {
                e.stopPropagation();
                const lb = document.querySelector('.lightbox');
                lb.querySelector('img').src = img.src;
                lb.classList.add('active');
            });
        });
    </script>
</body>
</html>`;

    return new Response(html, { headers: { 'Content-Type': 'text/html' } });
}

export function handleScriptRunFilesPage(url: URL): Response | null {
    if (!url.pathname.startsWith('/script_runs/')) {
        return null;
    }

    const parts = url.pathname.replace(/^\/script_runs\//, '').split('/');
    if (parts.length >= 3) {
        const scriptName = parts[0];
        const runName = parts[1];
        const filePath = path.join('../scripts', scriptName, 'runs', runName, ...parts.slice(2));
        if (fs.existsSync(filePath) && fs.statSync(filePath).isFile()) {
            return new Response(Bun.file(filePath), {
                headers: { 'Content-Type': getMimeType(filePath) }
            });
        }
    }

    return new Response('File not found', { status: 404 });
}
