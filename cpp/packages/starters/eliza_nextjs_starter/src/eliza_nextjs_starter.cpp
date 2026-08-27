// eliza_nextjs_starter.cpp - Next.js starter template generator for ElizaOS.

#include "elizaos/eliza_nextjs_starter.hpp"

#include <filesystem>
#include <fstream>
#include <sstream>
#include <string>
#include <system_error>

namespace elizaos {
namespace nextjs_starter {
namespace {

namespace fs = std::filesystem;

std::string jsonEscape(const std::string& value) {
    std::string escaped;
    escaped.reserve(value.size());
    for (char ch : value) {
        switch (ch) {
            case '\\': escaped += "\\\\"; break;
            case '"': escaped += "\\\""; break;
            case '\n': escaped += "\\n"; break;
            case '\r': escaped += "\\r"; break;
            case '\t': escaped += "\\t"; break;
            default: escaped += ch; break;
        }
    }
    return escaped;
}

bool validProjectArguments(const std::string& projectPath, const std::string& projectName) {
    return !projectPath.empty() && !projectName.empty() && projectName != "." && projectName != "..";
}

} // namespace

bool NextJSTemplateGenerator::generateProject(const std::string& projectPath,
                                               const std::string& projectName) {
    if (!validProjectArguments(projectPath, projectName)) return false;
    if (!createDirectory(projectPath)) return false;

    return generatePackageJson(projectPath, projectName) &&
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
    if (destination.has_parent_path() && !createDirectory(destination.parent_path().string())) {
        return false;
    }

    std::ofstream file(destination, std::ios::binary | std::ios::trunc);
    if (!file) return false;
    file << content;
    return file.good();
}

bool NextJSTemplateGenerator::generatePackageJson(const std::string& projectPath,
                                                   const std::string& projectName) {
    std::ostringstream content;
    content << "{\n"
            << "  \"name\": \"" << jsonEscape(projectName) << "\",\n"
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
            << "    \"next\": \"14.2.31\",\n"
            << "    \"react\": \"^18.3.1\",\n"
            << "    \"react-dom\": \"^18.3.1\"\n"
            << "  },\n"
            << "  \"devDependencies\": {\n"
            << "    \"@types/node\": \"^20.17.0\",\n"
            << "    \"@types/react\": \"^18.3.0\",\n"
            << "    \"@types/react-dom\": \"^18.3.0\",\n"
            << "    \"eslint\": \"^8.57.0\",\n"
            << "    \"eslint-config-next\": \"14.2.31\",\n"
            << "    \"typescript\": \"^5.7.0\"\n"
            << "  }\n"
            << "}\n";
    return writeFile(projectPath + "/package.json", content.str());
}

bool NextJSTemplateGenerator::generateTsConfig(const std::string& projectPath) {
    return writeFile(projectPath + "/tsconfig.json", R"TS({
  "compilerOptions": {
    "target": "ES2020",
    "lib": ["dom", "dom.iterable", "esnext"],
    "allowJs": false,
    "skipLibCheck": true,
    "strict": true,
    "noEmit": true,
    "esModuleInterop": true,
    "module": "esnext",
    "moduleResolution": "bundler",
    "resolveJsonModule": true,
    "isolatedModules": true,
    "jsx": "preserve",
    "incremental": true,
    "plugins": [{ "name": "next" }],
    "baseUrl": ".",
    "paths": { "@/*": ["./*"] }
  },
  "include": ["next-env.d.ts", "**/*.ts", "**/*.tsx", ".next/types/**/*.ts"],
  "exclude": ["node_modules"]
}
)TS");
}

bool NextJSTemplateGenerator::generateNextConfig(const std::string& projectPath) {
    return writeFile(projectPath + "/next.config.js", R"JS(/** @type {import('next').NextConfig} */
const nextConfig = {
  reactStrictMode: true,
}

module.exports = nextConfig
)JS");
}

bool NextJSTemplateGenerator::generatePages(const std::string& projectPath) {
    if (!createDirectory(projectPath + "/pages")) return false;

    const bool app = writeFile(projectPath + "/pages/_app.tsx", R"TS(import type { AppProps } from 'next/app'
import '@/styles/globals.css'

export default function App({ Component, pageProps }: AppProps) {
  return <Component {...pageProps} />
}
)TS");

    const bool index = writeFile(projectPath + "/pages/index.tsx", R"TS(import Head from 'next/head'
import { ElizaChat } from '@/components/ElizaChat'

export default function Home() {
  return (
    <>
      <Head>
        <title>ElizaOS Next.js App</title>
        <meta name="description" content="A typed ElizaOS chat starter" />
        <meta name="viewport" content="width=device-width, initial-scale=1" />
      </Head>
      <main>
        <h1>ElizaOS</h1>
        <ElizaChat />
      </main>
    </>
  )
}
)TS");
    return app && index;
}

bool NextJSTemplateGenerator::generateApiRoutes(const std::string& projectPath) {
    if (!createDirectory(projectPath + "/pages/api")) return false;
    return writeFile(projectPath + "/pages/api/chat.ts", R"TS(import type { NextApiRequest, NextApiResponse } from 'next'

type ChatResponse = { response: string } | { error: string }

export default async function handler(
  request: NextApiRequest,
  response: NextApiResponse<ChatResponse>,
) {
  if (request.method !== 'POST') {
    response.setHeader('Allow', 'POST')
    return response.status(405).json({ error: 'Method not allowed' })
  }

  const message = typeof request.body?.message === 'string' ? request.body.message.trim() : ''
  if (!message) return response.status(400).json({ error: 'message is required' })

  const baseUrl = process.env.ELIZAOS_API_URL ?? 'http://127.0.0.1:3001'
  try {
    const upstream = await fetch(`${baseUrl.replace(/\/$/, '')}/api/chat`, {
      method: 'POST',
      headers: { 'Content-Type': 'application/json' },
      body: JSON.stringify({ message }),
    })
    const payload = (await upstream.json()) as { response?: unknown; error?: unknown }
    if (!upstream.ok) {
      const detail = typeof payload.error === 'string' ? payload.error : `ElizaOS returned ${upstream.status}`
      return response.status(502).json({ error: detail })
    }
    if (typeof payload.response !== 'string') {
      return response.status(502).json({ error: 'ElizaOS returned an invalid response' })
    }
    return response.status(200).json({ response: payload.response })
  } catch (error) {
    const detail = error instanceof Error ? error.message : 'Unknown ElizaOS connection error'
    return response.status(502).json({ error: detail })
  }
}
)TS");
}

bool NextJSTemplateGenerator::generateComponents(const std::string& projectPath) {
    if (!createDirectory(projectPath + "/components")) return false;
    return writeFile(projectPath + "/components/ElizaChat.tsx", R"TS(import { FormEvent, useState } from 'react'
import { sendElizaMessage, type ChatMessage } from '@/lib/eliza'

export function ElizaChat() {
  const [messages, setMessages] = useState<ChatMessage[]>([])
  const [input, setInput] = useState('')
  const [error, setError] = useState('')
  const [sending, setSending] = useState(false)

  async function submit(event: FormEvent<HTMLFormElement>) {
    event.preventDefault()
    const message = input.trim()
    if (!message || sending) return

    setMessages((previous) => [...previous, { role: 'user', content: message }])
    setInput('')
    setError('')
    setSending(true)
    try {
      const reply = await sendElizaMessage(message)
      setMessages((previous) => [...previous, { role: 'assistant', content: reply }])
    } catch (caught) {
      setError(caught instanceof Error ? caught.message : 'Unable to reach ElizaOS')
    } finally {
      setSending(false)
    }
  }

  return (
    <section className="chat-container" aria-label="ElizaOS chat">
      <div className="messages" aria-live="polite">
        {messages.map((message, index) => (
          <p key={`${message.role}-${index}`} className={`message ${message.role}`}>
            <strong>{message.role === 'user' ? 'You' : 'Eliza'}:</strong> {message.content}
          </p>
        ))}
      </div>
      {error ? <p className="error" role="alert">{error}</p> : null}
      <form className="input-area" onSubmit={submit}>
        <input
          type="text"
          value={input}
          onChange={(event) => setInput(event.target.value)}
          placeholder="Type a message..."
          aria-label="Message"
        />
        <button type="submit" disabled={sending || !input.trim()}>
          {sending ? 'Sending...' : 'Send'}
        </button>
      </form>
    </section>
  )
}
)TS");
}

bool NextJSTemplateGenerator::generateLibrary(const std::string& projectPath) {
    if (!createDirectory(projectPath + "/lib")) return false;
    return writeFile(projectPath + "/lib/eliza.ts", R"TS(export type ChatMessage = {
  role: 'user' | 'assistant'
  content: string
}

export async function sendElizaMessage(message: string): Promise<string> {
  const response = await fetch('/api/chat', {
    method: 'POST',
    headers: { 'Content-Type': 'application/json' },
    body: JSON.stringify({ message }),
  })
  const payload = (await response.json()) as { response?: unknown; error?: unknown }
  if (!response.ok) {
    throw new Error(typeof payload.error === 'string' ? payload.error : `Request failed (${response.status})`)
  }
  if (typeof payload.response !== 'string') throw new Error('Invalid ElizaOS response')
  return payload.response
}
)TS");
}

bool NextJSTemplateGenerator::generateStyles(const std::string& projectPath) {
    if (!createDirectory(projectPath + "/styles")) return false;
    return writeFile(projectPath + "/styles/globals.css", R"CSS(:root {
  color-scheme: light dark;
  font-family: Inter, ui-sans-serif, system-ui, sans-serif;
}

* { box-sizing: border-box; }
body { margin: 0; min-height: 100vh; background: #0b1020; color: #edf2ff; }
main { width: min(760px, calc(100% - 2rem)); margin: 0 auto; padding: 3rem 0; }
.chat-container { display: grid; gap: 1rem; padding: 1rem; border: 1px solid #33406b; border-radius: 1rem; background: #141b33; }
.messages { min-height: 16rem; max-height: 55vh; overflow-y: auto; }
.message { padding: .75rem; border-radius: .75rem; background: #202a4d; }
.message.user { background: #243d68; }
.input-area { display: grid; grid-template-columns: 1fr auto; gap: .75rem; }
input, button { border: 1px solid #4b5f94; border-radius: .65rem; padding: .8rem; font: inherit; }
button { cursor: pointer; background: #6757ff; color: white; }
button:disabled { cursor: not-allowed; opacity: .55; }
.error { color: #ff9da7; }
)CSS");
}

bool NextJSTemplateGenerator::generateReadme(const std::string& projectPath,
                                              const std::string& projectName) {
    std::ostringstream content;
    content << "# " << projectName << "\n\n"
            << "Typed Next.js starter for an ElizaOS backend.\n\n"
            << "## Configure\n\n"
            << "Set `ELIZAOS_API_URL` to the HTTP backend URL. The generated API route "
            << "proxies `/api/chat` without exposing backend configuration to the browser.\n\n"
            << "For the WebSocket variant, set `NEXT_PUBLIC_ELIZAOS_WS_URL` "
            << "(default `ws://127.0.0.1:9002`).\n\n"
            << "## Run\n\n```bash\nnpm install\nnpm run typecheck\nnpm run dev\n```\n";
    return writeFile(projectPath + "/README.md", content.str());
}

bool WebSocketIntegration::createDirectory(const std::string& path) {
    if (path.empty()) return false;
    std::error_code ec;
    fs::create_directories(path, ec);
    return !ec;
}

bool WebSocketIntegration::generateWebSocketSupport(const std::string& projectPath) {
    if (projectPath.empty() || !createDirectory(projectPath + "/hooks")) return false;
    return NextJSTemplateGenerator::writeFileStatic(
        projectPath + "/hooks/useElizaSocket.ts", R"TS(import { useCallback, useEffect, useRef, useState } from 'react'

export type ElizaSocketMessage = Record<string, unknown>

export function useElizaSocket() {
  const socket = useRef<WebSocket | null>(null)
  const [connected, setConnected] = useState(false)
  const [lastMessage, setLastMessage] = useState<ElizaSocketMessage | null>(null)

  useEffect(() => {
    const url = process.env.NEXT_PUBLIC_ELIZAOS_WS_URL ?? 'ws://127.0.0.1:9002'
    const connection = new WebSocket(url)
    socket.current = connection
    connection.onopen = () => setConnected(true)
    connection.onclose = () => setConnected(false)
    connection.onerror = () => setConnected(false)
    connection.onmessage = (event) => {
      try {
        const parsed: unknown = JSON.parse(String(event.data))
        if (parsed && typeof parsed === 'object' && !Array.isArray(parsed)) {
          setLastMessage(parsed as ElizaSocketMessage)
        }
      } catch {
        setLastMessage({ type: 'text', content: String(event.data) })
      }
    }
    return () => {
      socket.current = null
      connection.close()
    }
  }, [])

  const send = useCallback((message: ElizaSocketMessage) => {
    if (socket.current?.readyState !== WebSocket.OPEN) return false
    socket.current.send(JSON.stringify(message))
    return true
  }, [])

  return { connected, lastMessage, send }
}
)TS");
}

bool generateNextJSProject(const std::string& projectPath,
                           const std::string& projectName) {
    return NextJSTemplateGenerator::generateProject(projectPath, projectName);
}

bool generateNextJSProjectWithWebSocket(const std::string& projectPath,
                                        const std::string& projectName) {
    return NextJSTemplateGenerator::generateProject(projectPath, projectName) &&
           WebSocketIntegration::generateWebSocketSupport(projectPath);
}

} // namespace nextjs_starter
} // namespace elizaos
