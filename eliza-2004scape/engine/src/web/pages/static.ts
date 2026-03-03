import fs from 'fs';
import path from 'path';
import { MIME_TYPES } from '../utils.js';

export function handleDisclaimerPage(url: URL): Response | null {
    if (url.pathname !== '/disclaimer' && url.pathname !== '/disclaimer/') {
        return null;
    }

    const html = `<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Disclaimer</title>
    <style>
        body {
            font-family: -apple-system, BlinkMacSystemFont, 'Segoe UI', Roboto, sans-serif;
            background: #0a0a0a;
            color: #ccc;
            margin: 0;
            padding: 40px 20px;
            line-height: 1.6;
        }
        .container {
            max-width: 700px;
            margin: 0 auto;
            background: rgba(255,255,255,0.03);
            padding: 40px;
            border-radius: 8px;
        }
        h1 {
            color: #5bf;
            margin-bottom: 24px;
        }
        p {
            margin-bottom: 16px;
        }
    </style>
</head>
<body>
    <div class="container">
        <h1>Disclaimer</h1>
        <p>This is a free, open-source, community-run project.</p>
        <p>The goal is strictly education and scientific research.</p>
        <p>LostCity Server was written from scratch after many hours of research and peer review. Everything you see is completely and transparently open source.</p>
        <p>We have not been endorsed by, authorized by, or officially communicated with Jagex Ltd. on our efforts here.</p>
        <p>You cannot play Old School RuneScape here, buy RuneScape gold, or access any of the official game's services!</p>
    </div>
</body>
</html>`;

    return new Response(html, { headers: { 'Content-Type': 'text/html' } });
}

export function handlePublicFiles(url: URL): Response | null {
    const publicPath = `public${url.pathname}`;
    if (fs.existsSync(publicPath) && fs.statSync(publicPath).isFile()) {
        return new Response(Bun.file(publicPath), {
            headers: {
                'Content-Type': MIME_TYPES.get(path.extname(url.pathname ?? '')) ?? 'text/plain'
            }
        });
    }
    return null;
}
