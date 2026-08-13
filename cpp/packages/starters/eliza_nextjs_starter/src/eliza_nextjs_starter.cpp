// ElizaOS Next.js starter generator.

#include "elizaos/eliza_nextjs_starter.hpp"

#include <filesystem>
#include <fstream>
#include <sstream>
#include <system_error>

namespace elizaos {
namespace nextjs_starter {
namespace {

namespace fs = std::filesystem;

std::string escapeJsonString(const std::string& value) {
    std::string escaped;
    escaped.reserve(value.size());
    for (const char c : value) {
        switch (c) {
            case '\\': escaped += "\\\\"; break;
            case '"': escaped += "\\\""; break;
            case '\n': escaped += "\\n"; break;
            case '\r': escaped += "\\r"; break;
            case '\t': escaped += "\\t"; break;
            default: escaped += c; break;
        }
    }
    return escaped;
}

} // namespace

bool NextJSTemplateGenerator::generateProject(const std::string& projectPath,
                                               const std::string& projectName) {
    if (projectPath.empty() || projectName.empty()) return false;
    return createDirectory(projectPath) &&
           generatePackageJson(projectPath, projectName) &&
           generateTsConfig(projectPath) &&
           generateNextConfig(projectPath) &&
           generatePages(projectPath) &&
           generateApiRoutes(projectPath) &&
           generateComponents(projectPath) &&
           generateLibrary(projectPath) &&
           generateStyles(projectPath) &&
           generateReadme(projectPath, projectName);
}

bool NextJSTemplateGenerator::writeFileStatic(const std::string& path,
                                              const std::string& content) {
    return writeFile(path, content);
}

bool NextJSTemplateGenerator::createDirectory(const std::string& path) {
    if (path.empty()) return false;
    std::error_code ec;
    fs::create_directories(path, ec);
    return !ec;
}

bool NextJSTemplateGenerator::writeFile(const std::string& path,
                                        const std::string& content) {
    if (path.empty()) return false;
    const fs::path destination(path);
    std::error_code ec;
    if (!destination.parent_path().empty()) {
        fs::create_directories(destination.parent_path(), ec);
        if (ec) return false;
    }
    fs::path temporary = destination;
    temporary += ".tmp";
    {
        std::ofstream output(temporary, std::ios::binary | std::ios::trunc);
        if (!output) return false;
        output.write(content.data(), static_cast<std::streamsize>(content.size()));
        output.flush();
        if (!output) {
            fs::remove(temporary, ec);
            return false;
        }
    }
    fs::rename(temporary, destination, ec);
    if (ec) {
        fs::remove(destination, ec);
        ec.clear();
        fs::rename(temporary, destination, ec);
    }
    if (ec) fs::remove(temporary, ec);
    return !ec;
}

bool NextJSTemplateGenerator::generatePackageJson(const std::string& projectPath,
                                                   const std::string& projectName) {
    std::ostringstream content;
    content << "{\n"
            << "  \"name\": \"" << escapeJsonString(projectName) << "\",\n"
            << "  \"version\": \"0.1.0\",\n"
            << "  \"private\": true,\n"
            << "  \"scripts\": {\n"
            << "    \"dev\": \"next dev\",\n"
            << "    \"build\": \"next build\",\n"
            << "    \"start\": \"next start\",\n"
            << "    \"lint\": \"next lint\",\n"
            << "    \"typecheck\": \"tsc --noEmit\"\n"
            << "  },\n"
            << "  \"dependencies\": {\n"
            << "    \"next\": \"^14.2.0\",\n"
            << "    \"react\": \"^18.2.0\",\n"
            << "    \"react-dom\": \"^18.2.0\"\n"
            << "  },\n"
            << "  \"devDependencies\": {\n"
            << "    \"@types/node\": \"^20.0.0\",\n"
            << "    \"@types/react\": \"^18.2.0\",\n"
            << "    \"@types/react-dom\": \"^18.2.0\",\n"
            << "    \"eslint\": \"^8.57.0\",\n"
            << "    \"eslint-config-next\": \"^14.2.0\",\n"
            << "    \"typescript\": \"^5.4.0\"\n"
            << "  }\n"
            << "}\n";
    return writeFile(projectPath + "/package.json", content.str());
}

bool NextJSTemplateGenerator::generateTsConfig(const std::string& projectPath) {
    const std::string content = R"JSON({
  "compilerOptions": {
    "target": "es2022",
    "lib": ["dom", "dom.iterable", "esnext"],
    "allowJs": true,
    "skipLibCheck": true,
    "strict": true,
    "forceConsistentCasingInFileNames": true,
    "noEmit": true,
    "esModuleInterop": true,
    "module": "esnext",
    "moduleResolution": "bundler",
    "resolveJsonModule": true,
    "isolatedModules": true,
    "jsx": "preserve",
    "incremental": true,
    "plugins": [{ "name": "next" }],
    "paths": { "@/*": ["./*"] }
  },
  "include": ["next-env.d.ts", "**/*.ts", "**/*.tsx", ".next/types/**/*.ts"],
  "exclude": ["node_modules"]
}
)JSON";
    return writeFile(projectPath + "/tsconfig.json", content);
}

bool NextJSTemplateGenerator::generateNextConfig(const std::string& projectPath) {
    const std::string content = R"JS(/** @type {import('next').NextConfig} */
const nextConfig = {
  reactStrictMode: true
}

module.exports = nextConfig
)JS";
    return writeFile(projectPath + "/next.config.js", content);
}

bool NextJSTemplateGenerator::generatePages(const std::string& projectPath) {
    if (!createDirectory(projectPath + "/pages")) return false;
    const std::string indexContent = R"TSX(import Head from 'next/head'
import { ElizaChat } from '@/components/ElizaChat'

export default function Home() {
  return (
    <>
      <Head>
        <title>ElizaOS Next.js App</title>
        <meta name="description" content="ElizaOS Next.js starter application" />
        <meta name="viewport" content="width=device-width, initial-scale=1" />
      </Head>
      <main>
        <h1>Welcome to ElizaOS</h1>
        <ElizaChat />
      </main>
    </>
  )
}
)TSX";
    if (!writeFile(projectPath + "/pages/index.tsx", indexContent)) return false;

    const std::string appContent = R"TSX(import type { AppProps } from 'next/app'
import '@/styles/globals.css'

export default function App({ Component, pageProps }: AppProps) {
  return <Component {...pageProps} />
}
)TSX";
    return writeFile(projectPath + "/pages/_app.tsx", appContent);
}

bool NextJSTemplateGenerator::generateApiRoutes(const std::string& projectPath) {
    if (!createDirectory(projectPath + "/pages/api")) return false;
    const std::string chatApi = R"TS(import type { NextApiRequest, NextApiResponse } from 'next'

type ChatResult = { response?: string; error?: string }

export default async function handler(
  req: NextApiRequest,
  res: NextApiResponse<ChatResult>
) {
  if (req.method !== 'POST') {
    res.setHeader('Allow', 'POST')
    return res.status(405).json({ error: 'Method not allowed' })
  }

  const message = typeof req.body?.message === 'string' ? req.body.message.trim() : ''
  if (!message) return res.status(400).json({ error: 'A non-empty message is required' })

  const baseUrl = process.env.ELIZAOS_API_URL
  if (!baseUrl) return res.status(503).json({ error: 'ELIZAOS_API_URL is not configured' })

  try {
    const endpoint = new URL(process.env.ELIZAOS_CHAT_PATH || '/api/chat', baseUrl)
    const headers: Record<string, string> = { 'Content-Type': 'application/json' }
    if (process.env.ELIZAOS_API_KEY) headers.Authorization = `Bearer ${process.env.ELIZAOS_API_KEY}`

    const upstream = await fetch(endpoint, {
      method: 'POST',
      headers,
      body: JSON.stringify({
        message,
        agentId: process.env.ELIZAOS_AGENT_ID || undefined
      }),
      signal: AbortSignal.timeout(30_000)
    })
    const payload: unknown = await upstream.json().catch(() => ({}))
    if (!upstream.ok) {
      const detail = typeof payload === 'object' && payload && 'error' in payload
        ? String((payload as { error: unknown }).error)
        : `ElizaOS upstream returned HTTP ${upstream.status}`
      return res.status(502).json({ error: detail })
    }

    const value = payload as {
      response?: unknown
      text?: unknown
      message?: { content?: unknown }
    }
    const response = value.response ?? value.text ?? value.message?.content
    if (typeof response !== 'string' || !response.trim()) {
      return res.status(502).json({ error: 'ElizaOS returned no textual response' })
    }
    return res.status(200).json({ response })
  } catch (error) {
    const detail = error instanceof Error ? error.message : 'Unknown upstream failure'
    return res.status(502).json({ error: detail })
  }
}
)TS";
    if (!writeFile(projectPath + "/pages/api/chat.ts", chatApi)) return false;

    const std::string environment =
        "ELIZAOS_API_URL=http://localhost:3001\n"
        "ELIZAOS_CHAT_PATH=/api/chat\n"
        "ELIZAOS_AGENT_ID=\n"
        "ELIZAOS_API_KEY=\n"
        "NEXT_PUBLIC_ELIZAOS_WS_URL=ws://localhost:3001/ws\n";
    return writeFile(projectPath + "/.env.example", environment);
}

bool NextJSTemplateGenerator::generateComponents(const std::string& projectPath) {
    if (!createDirectory(projectPath + "/components")) return false;
    const std::string chatComponent = R"TSX(import { useState } from 'react'
import { sendElizaMessage } from '@/lib/elizaApi'

type ChatMessage = { role: 'user' | 'assistant'; content: string }

export function ElizaChat() {
  const [messages, setMessages] = useState<ChatMessage[]>([])
  const [input, setInput] = useState('')
  const [sending, setSending] = useState(false)

  const sendMessage = async () => {
    const message = input.trim()
    if (!message || sending) return
    setMessages(previous => [...previous, { role: 'user', content: message }])
    setInput('')
    setSending(true)
    try {
      const response = await sendElizaMessage(message)
      setMessages(previous => [...previous, { role: 'assistant', content: response }])
    } catch (error) {
      const detail = error instanceof Error ? error.message : 'Unknown chat failure'
      setMessages(previous => [...previous, { role: 'assistant', content: `Error: ${detail}` }])
    } finally {
      setSending(false)
    }
  }

  return (
    <div className="chat-container">
      <div className="messages" aria-live="polite">
        {messages.map((message, index) => (
          <div key={`${message.role}-${index}`} className={`message ${message.role}`}>
            {message.content}
          </div>
        ))}
      </div>
      <div className="input-area">
        <input
          type="text"
          value={input}
          onChange={event => setInput(event.target.value)}
          onKeyDown={event => { if (event.key === 'Enter') void sendMessage() }}
          placeholder="Type a message..."
          disabled={sending}
        />
        <button onClick={() => void sendMessage()} disabled={sending || !input.trim()}>
          {sending ? 'Sending…' : 'Send'}
        </button>
      </div>
    </div>
  )
}
)TSX";
    return writeFile(projectPath + "/components/ElizaChat.tsx", chatComponent);
}

bool NextJSTemplateGenerator::generateLibrary(const std::string& projectPath) {
    if (!createDirectory(projectPath + "/lib")) return false;
    const std::string apiClient = R"TS(export async function sendElizaMessage(message: string): Promise<string> {
  const response = await fetch('/api/chat', {
    method: 'POST',
    headers: { 'Content-Type': 'application/json' },
    body: JSON.stringify({ message })
  })
  const payload: { response?: string; error?: string } = await response.json()
  if (!response.ok || !payload.response) {
    throw new Error(payload.error || `Chat request failed with HTTP ${response.status}`)
  }
  return payload.response
}
)TS";
    return writeFile(projectPath + "/lib/elizaApi.ts", apiClient);
}

bool NextJSTemplateGenerator::generateStyles(const std::string& projectPath) {
    if (!createDirectory(projectPath + "/styles")) return false;
    const std::string globalStyles = R"CSS(:root {
  color-scheme: light dark;
  font-family: Inter, system-ui, sans-serif;
  background: #10131a;
  color: #f2f5fa;
}
* { box-sizing: border-box; }
body { margin: 0; min-height: 100vh; background: radial-gradient(circle at top, #24324d, #10131a 55%); }
main { width: min(760px, calc(100% - 2rem)); margin: 0 auto; padding: 3rem 0; }
.chat-container { display: grid; gap: 1rem; padding: 1rem; border: 1px solid #3b4760; border-radius: 14px; background: #151a24; }
.messages { min-height: 18rem; max-height: 55vh; overflow-y: auto; display: grid; align-content: start; gap: .75rem; }
.message { max-width: 85%; padding: .75rem 1rem; border-radius: 12px; white-space: pre-wrap; }
.message.user { justify-self: end; background: #3156a5; }
.message.assistant { justify-self: start; background: #252d3d; }
.input-area { display: grid; grid-template-columns: 1fr auto; gap: .75rem; }
input, button { font: inherit; border: 1px solid #52617e; border-radius: 10px; padding: .8rem 1rem; }
input { background: #0f131c; color: inherit; }
button { cursor: pointer; background: #4f73c9; color: white; }
button:disabled { cursor: not-allowed; opacity: .55; }
)CSS";
    return writeFile(projectPath + "/styles/globals.css", globalStyles);
}

bool NextJSTemplateGenerator::generateReadme(const std::string& projectPath,
                                              const std::string& projectName) {
    std::ostringstream content;
    content << "# " << projectName << "\n\n"
            << "A typed Next.js ElizaOS chat starter with server-side credential isolation.\n\n"
            << "## Setup\n\n"
            << "```bash\n"
            << "cp .env.example .env.local\n"
            << "npm install\n"
            << "npm run typecheck\n"
            << "npm run dev\n"
            << "```\n\n"
            << "Set `ELIZAOS_API_URL` to a running ElizaOS HTTP service. The `/api/chat` route "
            << "validates input, optionally supplies `ELIZAOS_API_KEY` and `ELIZAOS_AGENT_ID`, "
            << "enforces a 30-second timeout, and propagates truthful upstream failures.\n\n"
            << "`generateNextJSProjectWithWebSocket` also adds a typed reconnecting browser client "
            << "configured by `NEXT_PUBLIC_ELIZAOS_WS_URL`.\n";
    return writeFile(projectPath + "/README.md", content.str());
}

bool WebSocketIntegration::createDirectory(const std::string& path) {
    if (path.empty()) return false;
    std::error_code ec;
    fs::create_directories(path, ec);
    return !ec;
}

bool WebSocketIntegration::generateWebSocketSupport(const std::string& projectPath) {
    if (projectPath.empty() || !createDirectory(projectPath + "/lib")) return false;
    const std::string socketClient = R"TS(export type ElizaSocketEvent = {
  type: string
  data?: unknown
  message?: string
}

type Listener = (event: ElizaSocketEvent) => void

export class ElizaSocket {
  private socket: WebSocket | null = null
  private listeners = new Set<Listener>()
  private reconnectTimer: ReturnType<typeof setTimeout> | null = null
  private reconnectAttempt = 0
  private closedByClient = false

  constructor(private readonly url: string) {}

  connect() {
    if (!this.url || this.socket?.readyState === WebSocket.OPEN ||
        this.socket?.readyState === WebSocket.CONNECTING) return
    this.closedByClient = false
    this.socket = new WebSocket(this.url)
    this.socket.onopen = () => { this.reconnectAttempt = 0 }
    this.socket.onmessage = message => {
      try {
        const event = JSON.parse(String(message.data)) as ElizaSocketEvent
        this.listeners.forEach(listener => listener(event))
      } catch {
        this.listeners.forEach(listener => listener({ type: 'protocol_error', message: 'Invalid JSON frame' }))
      }
    }
    this.socket.onclose = () => {
      this.socket = null
      if (!this.closedByClient) this.scheduleReconnect()
    }
  }

  disconnect() {
    this.closedByClient = true
    if (this.reconnectTimer) clearTimeout(this.reconnectTimer)
    this.reconnectTimer = null
    this.socket?.close()
    this.socket = null
  }

  subscribe(listener: Listener) {
    this.listeners.add(listener)
    return () => this.listeners.delete(listener)
  }

  send(event: ElizaSocketEvent) {
    if (this.socket?.readyState !== WebSocket.OPEN) return false
    this.socket.send(JSON.stringify(event))
    return true
  }

  private scheduleReconnect() {
    const delay = Math.min(30_000, 500 * 2 ** this.reconnectAttempt++)
    this.reconnectTimer = setTimeout(() => this.connect(), delay)
  }
}
)TS";
    return NextJSTemplateGenerator::writeFileStatic(projectPath + "/lib/elizaSocket.ts",
                                                     socketClient);
}

bool generateNextJSProject(const std::string& projectPath, const std::string& projectName) {
    return NextJSTemplateGenerator::generateProject(projectPath, projectName);
}

bool generateNextJSProjectWithWebSocket(const std::string& projectPath,
                                        const std::string& projectName) {
    return NextJSTemplateGenerator::generateProject(projectPath, projectName) &&
           WebSocketIntegration::generateWebSocketSupport(projectPath);
}

} // namespace nextjs_starter
} // namespace elizaos
