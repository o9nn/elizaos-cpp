import fs from 'fs';
import path from 'path';
import { timeAgo } from '../utils.js';

export function handleScreenshotsListPage(url: URL): Response | null {
    if (url.pathname !== '/screenshots' && url.pathname !== '/screenshots/') {
        return null;
    }

    const screenshotDir = 'screenshots';
    if (!fs.existsSync(screenshotDir)) {
        fs.mkdirSync(screenshotDir, { recursive: true });
    }

    const files = fs.readdirSync(screenshotDir)
        .filter(f => f.endsWith('.png') || f.endsWith('.jpg'))
        .map(f => {
            const stat = fs.statSync(path.join(screenshotDir, f));
            return { name: f, mtime: stat.mtimeMs };
        })
        .sort((a, b) => b.mtime - a.mtime);

    const html = `<!DOCTYPE html>
<html><head><title>Screenshots</title>
<style>
body{background:#000;color:#04A800;font-family:monospace;padding:20px}
a{color:#04A800;text-decoration:none}
.grid{display:flex;flex-wrap:wrap;gap:15px}
.item{display:flex;flex-direction:column;align-items:center}
.item img{max-width:300px;max-height:200px;border:1px solid #04A800}
.item img:hover{border-color:#fff}
.time{font-size:11px;color:#888;margin-top:4px}
.name{font-size:10px;color:#666;max-width:300px;overflow:hidden;text-overflow:ellipsis}
</style></head>
<body><h1>Screenshots (${files.length})</h1>
<div class="grid">${files.map(f => `<a href="/screenshots/${f.name}" target="_blank" class="item">
<img src="/screenshots/${f.name}">
<span class="time">${timeAgo(f.mtime)}</span>
<span class="name">${f.name}</span>
</a>`).join('')}</div>
${files.length === 0 ? '<p>No screenshots yet</p>' : ''}
</body></html>`;

    return new Response(html, { headers: { 'Content-Type': 'text/html' } });
}

export function handleScreenshotFilePage(url: URL): Response | null {
    if (!url.pathname.startsWith('/screenshots/')) {
        return null;
    }

    const filePath = url.pathname.substring(1); // Remove leading /
    if (fs.existsSync(filePath) && fs.statSync(filePath).isFile()) {
        return new Response(Bun.file(filePath), {
            headers: { 'Content-Type': 'image/png' }
        });
    }

    return new Response(null, { status: 404 });
}
