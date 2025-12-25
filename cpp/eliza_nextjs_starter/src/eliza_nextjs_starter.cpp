// eliza_nextjs_starter.cpp - Next.js starter template generator for ElizaOS
// Provides utilities to generate Next.js project templates with ElizaOS integration

#include "elizaos/core.hpp"
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <sys/stat.h>
#include <sys/types.h>

namespace elizaos {
namespace nextjs_starter {

// ==============================================================================
// TEMPLATE GENERATOR
// ==============================================================================

class NextJSTemplateGenerator {
public:
    static bool generateProject(const std::string& projectPath, const std::string& projectName) {
        // Create project directory structure
        if (!createDirectory(projectPath)) {
            return false;
        }
        
        // Generate package.json
        if (!generatePackageJson(projectPath, projectName)) {
            return false;
        }
        
        // Generate tsconfig.json
        if (!generateTsConfig(projectPath)) {
            return false;
        }
        
        // Generate next.config.js
        if (!generateNextConfig(projectPath)) {
            return false;
        }
        
        // Generate pages
        if (!generatePages(projectPath)) {
            return false;
        }
        
        // Generate components
        if (!generateComponents(projectPath)) {
            return false;
        }
        
        // Generate styles
        if (!generateStyles(projectPath)) {
            return false;
        }
        
        // Generate README
        if (!generateReadme(projectPath, projectName)) {
            return false;
        }
        
        return true;
    }
    
private:
    static bool createDirectory(const std::string& path) {
        return mkdir(path.c_str(), 0755) == 0 || errno == EEXIST;
    }
    
    static bool writeFile(const std::string& path, const std::string& content) {
        std::ofstream file(path);
        if (!file.is_open()) {
            return false;
        }
        file << content;
        file.close();
        return true;
    }
    
    static bool generatePackageJson(const std::string& projectPath, const std::string& projectName) {
        std::ostringstream content;
        content << "{\n"
                << "  \"name\": \"" << projectName << "\",\n"
                << "  \"version\": \"0.1.0\",\n"
                << "  \"private\": true,\n"
                << "  \"scripts\": {\n"
                << "    \"dev\": \"next dev\",\n"
                << "    \"build\": \"next build\",\n"
                << "    \"start\": \"next start\",\n"
                << "    \"lint\": \"next lint\"\n"
                << "  },\n"
                << "  \"dependencies\": {\n"
                << "    \"next\": \"14.0.0\",\n"
                << "    \"react\": \"^18.2.0\",\n"
                << "    \"react-dom\": \"^18.2.0\",\n"
                << "    \"@elizaos/client\": \"latest\"\n"
                << "  },\n"
                << "  \"devDependencies\": {\n"
                << "    \"@types/node\": \"^20.0.0\",\n"
                << "    \"@types/react\": \"^18.2.0\",\n"
                << "    \"@types/react-dom\": \"^18.2.0\",\n"
                << "    \"typescript\": \"^5.0.0\",\n"
                << "    \"eslint\": \"^8.0.0\",\n"
                << "    \"eslint-config-next\": \"14.0.0\"\n"
                << "  }\n"
                << "}\n";
        
        return writeFile(projectPath + "/package.json", content.str());
    }
    
    static bool generateTsConfig(const std::string& projectPath) {
        std::string content = R"({
  "compilerOptions": {
    "target": "es5",
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
    "plugins": [
      {
        "name": "next"
      }
    ],
    "paths": {
      "@/*": ["./*"]
    }
  },
  "include": ["next-env.d.ts", "**/*.ts", "**/*.tsx", ".next/types/**/*.ts"],
  "exclude": ["node_modules"]
}
)";
        
        return writeFile(projectPath + "/tsconfig.json", content);
    }
    
    static bool generateNextConfig(const std::string& projectPath) {
        std::string content = R"(/** @type {import('next').NextConfig} */
const nextConfig = {
  reactStrictMode: true,
  swcMinify: true,
}

module.exports = nextConfig
)";
        
        return writeFile(projectPath + "/next.config.js", content);
    }
    
    static bool generatePages(const std::string& projectPath) {
        createDirectory(projectPath + "/pages");
        createDirectory(projectPath + "/pages/api");

        // Generate index page
        std::string indexContent = R"(import Head from 'next/head'
import { ElizaChat } from '@/components/ElizaChat'

export default function Home() {
  return (
    <>
      <Head>
        <title>ElizaOS Next.js App</title>
        <meta name="description" content="ElizaOS Next.js starter application" />
        <meta name="viewport" content="width=device-width, initial-scale=1" />
        <link rel="icon" href="/favicon.ico" />
      </Head>
      <main>
        <h1>Welcome to ElizaOS</h1>
        <ElizaChat />
      </main>
    </>
  )
}
)";

        if (!writeFile(projectPath + "/pages/index.tsx", indexContent)) {
            return false;
        }

        // Generate API route for chat
        if (!generateApiRoutes(projectPath)) {
            return false;
        }

        return true;
    }

    static bool generateApiRoutes(const std::string& projectPath) {
        // Generate chat API route with ElizaOS backend integration
        std::string chatApiContent = R"(import type { NextApiRequest, NextApiResponse } from 'next'
import { ElizaClient } from '@/lib/eliza-client'

type ChatResponse = {
  response: string
  timestamp: number
  agentId?: string
}

type ErrorResponse = {
  error: string
}

// Initialize ElizaOS client with environment configuration
const elizaClient = new ElizaClient({
  endpoint: process.env.ELIZA_API_ENDPOINT || 'http://localhost:3001',
  apiKey: process.env.ELIZA_API_KEY,
  agentId: process.env.ELIZA_AGENT_ID || 'default'
})

export default async function handler(
  req: NextApiRequest,
  res: NextApiResponse<ChatResponse | ErrorResponse>
) {
  if (req.method !== 'POST') {
    return res.status(405).json({ error: 'Method not allowed' })
  }

  const { message, sessionId, userId } = req.body

  if (!message || typeof message !== 'string') {
    return res.status(400).json({ error: 'Message is required' })
  }

  try {
    // Send message to ElizaOS backend
    const response = await elizaClient.sendMessage({
      content: message,
      sessionId: sessionId || generateSessionId(),
      userId: userId || 'anonymous'
    })

    return res.status(200).json({
      response: response.content,
      timestamp: Date.now(),
      agentId: response.agentId
    })
  } catch (error) {
    console.error('ElizaOS API error:', error)
    return res.status(500).json({
      error: 'Failed to process message'
    })
  }
}

function generateSessionId(): string {
  return `session_${Date.now()}_${Math.random().toString(36).substr(2, 9)}`
}
)";

        if (!writeFile(projectPath + "/pages/api/chat.ts", chatApiContent)) {
            return false;
        }

        // Generate agents API route
        std::string agentsApiContent = R"(import type { NextApiRequest, NextApiResponse } from 'next'
import { ElizaClient } from '@/lib/eliza-client'

type Agent = {
  id: string
  name: string
  status: 'active' | 'inactive' | 'busy'
  capabilities: string[]
}

type AgentsResponse = {
  agents: Agent[]
}

type ErrorResponse = {
  error: string
}

const elizaClient = new ElizaClient({
  endpoint: process.env.ELIZA_API_ENDPOINT || 'http://localhost:3001',
  apiKey: process.env.ELIZA_API_KEY
})

export default async function handler(
  req: NextApiRequest,
  res: NextApiResponse<AgentsResponse | ErrorResponse>
) {
  if (req.method !== 'GET') {
    return res.status(405).json({ error: 'Method not allowed' })
  }

  try {
    const agents = await elizaClient.listAgents()
    return res.status(200).json({ agents })
  } catch (error) {
    console.error('Failed to fetch agents:', error)
    return res.status(500).json({ error: 'Failed to fetch agents' })
  }
}
)";

        return writeFile(projectPath + "/pages/api/agents.ts", agentsApiContent);
    }
    
    static bool generateComponents(const std::string& projectPath) {
        createDirectory(projectPath + "/components");

        std::string chatComponent = R"(import { useState, useEffect, useRef } from 'react'

interface Message {
  id: string
  content: string
  sender: 'user' | 'agent'
  timestamp: number
}

interface ElizaChatProps {
  agentId?: string
  userId?: string
}

export function ElizaChat({ agentId, userId }: ElizaChatProps = {}) {
  const [messages, setMessages] = useState<Message[]>([])
  const [input, setInput] = useState('')
  const [isLoading, setIsLoading] = useState(false)
  const [sessionId] = useState(() => `session_${Date.now()}_${Math.random().toString(36).substr(2, 9)}`)
  const messagesEndRef = useRef<HTMLDivElement>(null)

  const scrollToBottom = () => {
    messagesEndRef.current?.scrollIntoView({ behavior: 'smooth' })
  }

  useEffect(() => {
    scrollToBottom()
  }, [messages])

  const sendMessage = async () => {
    if (!input.trim() || isLoading) return

    const userMessage: Message = {
      id: `msg_${Date.now()}`,
      content: input,
      sender: 'user',
      timestamp: Date.now()
    }

    setMessages(prev => [...prev, userMessage])
    setInput('')
    setIsLoading(true)

    try {
      const response = await fetch('/api/chat', {
        method: 'POST',
        headers: { 'Content-Type': 'application/json' },
        body: JSON.stringify({
          message: input,
          sessionId,
          userId: userId || 'anonymous',
          agentId
        })
      })

      if (!response.ok) {
        throw new Error('Failed to get response')
      }

      const data = await response.json()

      const agentMessage: Message = {
        id: `msg_${Date.now()}_agent`,
        content: data.response,
        sender: 'agent',
        timestamp: data.timestamp
      }

      setMessages(prev => [...prev, agentMessage])
    } catch (error) {
      console.error('Error sending message:', error)
      const errorMessage: Message = {
        id: `msg_${Date.now()}_error`,
        content: 'Sorry, I encountered an error. Please try again.',
        sender: 'agent',
        timestamp: Date.now()
      }
      setMessages(prev => [...prev, errorMessage])
    } finally {
      setIsLoading(false)
    }
  }

  return (
    <div className="eliza-chat">
      <div className="chat-header">
        <h2>ElizaOS Chat</h2>
        <span className="session-id">Session: {sessionId.slice(-8)}</span>
      </div>
      <div className="messages-container">
        {messages.length === 0 && (
          <div className="welcome-message">
            <p>Hello! I'm your ElizaOS assistant. How can I help you today?</p>
          </div>
        )}
        {messages.map((msg) => (
          <div key={msg.id} className={`message ${msg.sender}`}>
            <div className="message-content">{msg.content}</div>
            <div className="message-time">
              {new Date(msg.timestamp).toLocaleTimeString()}
            </div>
          </div>
        ))}
        {isLoading && (
          <div className="message agent loading">
            <div className="typing-indicator">
              <span></span>
              <span></span>
              <span></span>
            </div>
          </div>
        )}
        <div ref={messagesEndRef} />
      </div>
      <div className="input-area">
        <input
          type="text"
          value={input}
          onChange={(e) => setInput(e.target.value)}
          onKeyPress={(e) => e.key === 'Enter' && sendMessage()}
          placeholder="Type a message..."
          disabled={isLoading}
        />
        <button onClick={sendMessage} disabled={isLoading || !input.trim()}>
          {isLoading ? 'Sending...' : 'Send'}
        </button>
      </div>
    </div>
  )
}
)";

        if (!writeFile(projectPath + "/components/ElizaChat.tsx", chatComponent)) {
            return false;
        }

        // Generate ElizaClient library
        return generateLibrary(projectPath);
    }

    static bool generateLibrary(const std::string& projectPath) {
        createDirectory(projectPath + "/lib");

        std::string elizaClientContent = R"(// ElizaOS Client Library for Next.js
// Provides a TypeScript interface to communicate with ElizaOS backend

export interface ElizaClientConfig {
  endpoint: string
  apiKey?: string
  agentId?: string
  timeout?: number
}

export interface SendMessageParams {
  content: string
  sessionId: string
  userId?: string
  metadata?: Record<string, unknown>
}

export interface MessageResponse {
  content: string
  agentId: string
  timestamp: number
  metadata?: Record<string, unknown>
}

export interface Agent {
  id: string
  name: string
  status: 'active' | 'inactive' | 'busy'
  capabilities: string[]
}

export class ElizaClient {
  private endpoint: string
  private apiKey?: string
  private agentId?: string
  private timeout: number

  constructor(config: ElizaClientConfig) {
    this.endpoint = config.endpoint.replace(/\/$/, '')
    this.apiKey = config.apiKey
    this.agentId = config.agentId
    this.timeout = config.timeout || 30000
  }

  private async request<T>(
    path: string,
    options: RequestInit = {}
  ): Promise<T> {
    const url = `${this.endpoint}${path}`
    const headers: Record<string, string> = {
      'Content-Type': 'application/json',
      ...(options.headers as Record<string, string>)
    }

    if (this.apiKey) {
      headers['Authorization'] = `Bearer ${this.apiKey}`
    }

    const controller = new AbortController()
    const timeoutId = setTimeout(() => controller.abort(), this.timeout)

    try {
      const response = await fetch(url, {
        ...options,
        headers,
        signal: controller.signal
      })

      if (!response.ok) {
        const error = await response.json().catch(() => ({}))
        throw new Error(error.message || `HTTP ${response.status}`)
      }

      return response.json()
    } finally {
      clearTimeout(timeoutId)
    }
  }

  async sendMessage(params: SendMessageParams): Promise<MessageResponse> {
    const body = {
      ...params,
      agentId: this.agentId
    }

    return this.request<MessageResponse>('/api/message', {
      method: 'POST',
      body: JSON.stringify(body)
    })
  }

  async listAgents(): Promise<Agent[]> {
    const response = await this.request<{ agents: Agent[] }>('/api/agents', {
      method: 'GET'
    })
    return response.agents
  }

  async getAgent(agentId: string): Promise<Agent> {
    return this.request<Agent>(`/api/agents/${agentId}`, {
      method: 'GET'
    })
  }

  async createSession(agentId?: string): Promise<{ sessionId: string }> {
    return this.request<{ sessionId: string }>('/api/sessions', {
      method: 'POST',
      body: JSON.stringify({ agentId: agentId || this.agentId })
    })
  }

  async endSession(sessionId: string): Promise<void> {
    await this.request(`/api/sessions/${sessionId}`, {
      method: 'DELETE'
    })
  }

  async getSessionHistory(sessionId: string): Promise<MessageResponse[]> {
    const response = await this.request<{ messages: MessageResponse[] }>(
      `/api/sessions/${sessionId}/messages`,
      { method: 'GET' }
    )
    return response.messages
  }
}

// React hook for ElizaOS client
export function useEliza(config?: Partial<ElizaClientConfig>) {
  const client = new ElizaClient({
    endpoint: config?.endpoint || process.env.NEXT_PUBLIC_ELIZA_ENDPOINT || 'http://localhost:3001',
    apiKey: config?.apiKey || process.env.NEXT_PUBLIC_ELIZA_API_KEY,
    agentId: config?.agentId,
    timeout: config?.timeout
  })

  return client
}
)";

        if (!writeFile(projectPath + "/lib/eliza-client.ts", elizaClientContent)) {
            return false;
        }

        // Generate environment example file
        std::string envExampleContent = R"(# ElizaOS Configuration
ELIZA_API_ENDPOINT=http://localhost:3001
ELIZA_API_KEY=your-api-key-here
ELIZA_AGENT_ID=default

# Next.js Public Environment Variables (accessible in browser)
NEXT_PUBLIC_ELIZA_ENDPOINT=http://localhost:3001
)";

        return writeFile(projectPath + "/.env.example", envExampleContent);
    }
    
    static bool generateStyles(const std::string& projectPath) {
        createDirectory(projectPath + "/styles");
        
        std::string globalStyles = R"(:root {
  --max-width: 1100px;
  --border-radius: 12px;
  --font-mono: ui-monospace, Menlo, Monaco, 'Cascadia Mono', 'Segoe UI Mono',
    'Roboto Mono', 'Oxygen Mono', 'Ubuntu Monospace', 'Source Code Pro',
    'Fira Mono', 'Droid Sans Mono', 'Courier New', monospace;

  --foreground-rgb: 0, 0, 0;
  --background-start-rgb: 214, 219, 220;
  --background-end-rgb: 255, 255, 255;
}

* {
  box-sizing: border-box;
  padding: 0;
  margin: 0;
}

html,
body {
  max-width: 100vw;
  overflow-x: hidden;
}

body {
  color: rgb(var(--foreground-rgb));
  background: linear-gradient(
      to bottom,
      transparent,
      rgb(var(--background-end-rgb))
    )
    rgb(var(--background-start-rgb));
}

a {
  color: inherit;
  text-decoration: none;
}
)";
        
        return writeFile(projectPath + "/styles/globals.css", globalStyles);
    }
    
public:
    // Static writeFile for use by other classes
    static bool writeFileStatic(const std::string& path, const std::string& content) {
        std::ofstream file(path);
        if (!file.is_open()) {
            return false;
        }
        file << content;
        file.close();
        return true;
    }

private:
    static bool generateReadme(const std::string& projectPath, const std::string& projectName) {
        std::ostringstream content;
        content << "# " << projectName << "\n\n"
                << "This is a [Next.js](https://nextjs.org/) project bootstrapped with ElizaOS integration.\n\n"
                << "## Getting Started\n\n"
                << "First, install dependencies:\n\n"
                << "```bash\n"
                << "npm install\n"
                << "# or\n"
                << "yarn install\n"
                << "```\n\n"
                << "Then, run the development server:\n\n"
                << "```bash\n"
                << "npm run dev\n"
                << "# or\n"
                << "yarn dev\n"
                << "```\n\n"
                << "Open [http://localhost:3000](http://localhost:3000) with your browser to see the result.\n\n"
                << "## ElizaOS Integration\n\n"
                << "This project includes ElizaOS client integration for AI agent interactions.\n\n"
                << "## Learn More\n\n"
                << "- [Next.js Documentation](https://nextjs.org/docs)\n"
                << "- [ElizaOS Documentation](https://elizaos.ai/docs)\n";
        
        return writeFile(projectPath + "/README.md", content.str());
    }
};

// ==============================================================================
// WEBSOCKET INTEGRATION
// ==============================================================================

class WebSocketIntegration {
public:
    static bool generateWebSocketSupport(const std::string& projectPath) {
        // Generate WebSocket hook
        std::string useWebSocketContent = R"(import { useState, useEffect, useRef, useCallback } from 'react'

interface WebSocketMessage {
  type: 'message' | 'status' | 'error' | 'typing'
  content?: string
  agentId?: string
  timestamp: number
  metadata?: Record<string, unknown>
}

interface UseWebSocketOptions {
  url: string
  sessionId: string
  onMessage?: (message: WebSocketMessage) => void
  onConnect?: () => void
  onDisconnect?: () => void
  onError?: (error: Event) => void
  reconnectInterval?: number
  maxReconnectAttempts?: number
}

export function useElizaWebSocket({
  url,
  sessionId,
  onMessage,
  onConnect,
  onDisconnect,
  onError,
  reconnectInterval = 3000,
  maxReconnectAttempts = 5
}: UseWebSocketOptions) {
  const [isConnected, setIsConnected] = useState(false)
  const [isReconnecting, setIsReconnecting] = useState(false)
  const wsRef = useRef<WebSocket | null>(null)
  const reconnectAttemptsRef = useRef(0)
  const reconnectTimeoutRef = useRef<NodeJS.Timeout | null>(null)

  const connect = useCallback(() => {
    if (wsRef.current?.readyState === WebSocket.OPEN) return

    const wsUrl = `${url}?sessionId=${sessionId}`
    const ws = new WebSocket(wsUrl)

    ws.onopen = () => {
      setIsConnected(true)
      setIsReconnecting(false)
      reconnectAttemptsRef.current = 0
      onConnect?.()
    }

    ws.onmessage = (event) => {
      try {
        const message: WebSocketMessage = JSON.parse(event.data)
        onMessage?.(message)
      } catch (error) {
        console.error('Failed to parse WebSocket message:', error)
      }
    }

    ws.onclose = () => {
      setIsConnected(false)
      onDisconnect?.()

      // Auto-reconnect logic
      if (reconnectAttemptsRef.current < maxReconnectAttempts) {
        setIsReconnecting(true)
        reconnectTimeoutRef.current = setTimeout(() => {
          reconnectAttemptsRef.current++
          connect()
        }, reconnectInterval)
      }
    }

    ws.onerror = (error) => {
      onError?.(error)
    }

    wsRef.current = ws
  }, [url, sessionId, onMessage, onConnect, onDisconnect, onError, reconnectInterval, maxReconnectAttempts])

  const disconnect = useCallback(() => {
    if (reconnectTimeoutRef.current) {
      clearTimeout(reconnectTimeoutRef.current)
    }
    reconnectAttemptsRef.current = maxReconnectAttempts // Prevent auto-reconnect
    wsRef.current?.close()
    wsRef.current = null
    setIsConnected(false)
    setIsReconnecting(false)
  }, [maxReconnectAttempts])

  const sendMessage = useCallback((content: string, metadata?: Record<string, unknown>) => {
    if (wsRef.current?.readyState === WebSocket.OPEN) {
      wsRef.current.send(JSON.stringify({
        type: 'message',
        content,
        sessionId,
        timestamp: Date.now(),
        metadata
      }))
      return true
    }
    return false
  }, [sessionId])

  useEffect(() => {
    connect()
    return () => disconnect()
  }, [connect, disconnect])

  return {
    isConnected,
    isReconnecting,
    sendMessage,
    connect,
    disconnect
  }
}
)";

        if (!NextJSTemplateGenerator::writeFileStatic(projectPath + "/hooks/useElizaWebSocket.ts", useWebSocketContent)) {
            return false;
        }

        // Generate WebSocket chat component
        std::string wsChatContent = R"(import { useState, useEffect, useRef } from 'react'
import { useElizaWebSocket } from '@/hooks/useElizaWebSocket'

interface Message {
  id: string
  content: string
  sender: 'user' | 'agent'
  timestamp: number
}

interface ElizaWebSocketChatProps {
  wsUrl?: string
  agentId?: string
}

export function ElizaWebSocketChat({
  wsUrl = process.env.NEXT_PUBLIC_ELIZA_WS_URL || 'ws://localhost:3001/ws',
  agentId
}: ElizaWebSocketChatProps) {
  const [messages, setMessages] = useState<Message[]>([])
  const [input, setInput] = useState('')
  const [isTyping, setIsTyping] = useState(false)
  const [sessionId] = useState(() => `ws_session_${Date.now()}_${Math.random().toString(36).substr(2, 9)}`)
  const messagesEndRef = useRef<HTMLDivElement>(null)

  const { isConnected, isReconnecting, sendMessage } = useElizaWebSocket({
    url: wsUrl,
    sessionId,
    onMessage: (message) => {
      if (message.type === 'typing') {
        setIsTyping(true)
      } else if (message.type === 'message' && message.content) {
        setIsTyping(false)
        const agentMessage: Message = {
          id: `msg_${Date.now()}_agent`,
          content: message.content,
          sender: 'agent',
          timestamp: message.timestamp
        }
        setMessages(prev => [...prev, agentMessage])
      }
    },
    onConnect: () => console.log('WebSocket connected'),
    onDisconnect: () => console.log('WebSocket disconnected'),
    onError: (error) => console.error('WebSocket error:', error)
  })

  const scrollToBottom = () => {
    messagesEndRef.current?.scrollIntoView({ behavior: 'smooth' })
  }

  useEffect(() => {
    scrollToBottom()
  }, [messages])

  const handleSend = () => {
    if (!input.trim() || !isConnected) return

    const userMessage: Message = {
      id: `msg_${Date.now()}`,
      content: input,
      sender: 'user',
      timestamp: Date.now()
    }

    setMessages(prev => [...prev, userMessage])
    sendMessage(input, { agentId })
    setInput('')
  }

  return (
    <div className="eliza-ws-chat">
      <div className="chat-header">
        <h2>ElizaOS Live Chat</h2>
        <div className={`connection-status ${isConnected ? 'connected' : isReconnecting ? 'reconnecting' : 'disconnected'}`}>
          {isConnected ? 'Connected' : isReconnecting ? 'Reconnecting...' : 'Disconnected'}
        </div>
      </div>
      <div className="messages-container">
        {messages.map((msg) => (
          <div key={msg.id} className={`message ${msg.sender}`}>
            <div className="message-content">{msg.content}</div>
            <div className="message-time">
              {new Date(msg.timestamp).toLocaleTimeString()}
            </div>
          </div>
        ))}
        {isTyping && (
          <div className="message agent typing">
            <div className="typing-indicator">
              <span></span><span></span><span></span>
            </div>
          </div>
        )}
        <div ref={messagesEndRef} />
      </div>
      <div className="input-area">
        <input
          type="text"
          value={input}
          onChange={(e) => setInput(e.target.value)}
          onKeyPress={(e) => e.key === 'Enter' && handleSend()}
          placeholder={isConnected ? "Type a message..." : "Connecting..."}
          disabled={!isConnected}
        />
        <button onClick={handleSend} disabled={!isConnected || !input.trim()}>
          Send
        </button>
      </div>
    </div>
  )
}
)";

        return NextJSTemplateGenerator::writeFileStatic(projectPath + "/components/ElizaWebSocketChat.tsx", wsChatContent);
    }

private:
    static bool createDirectory(const std::string& path) {
        return mkdir(path.c_str(), 0755) == 0 || errno == EEXIST;
    }
};

// ==============================================================================
// EXPORTED API
// ==============================================================================

bool generateNextJSProject(const std::string& projectPath, const std::string& projectName) {
    if (!NextJSTemplateGenerator::generateProject(projectPath, projectName)) {
        return false;
    }

    // Create hooks directory and add WebSocket support
    mkdir((projectPath + "/hooks").c_str(), 0755);
    return WebSocketIntegration::generateWebSocketSupport(projectPath);
}

bool generateNextJSProjectWithWebSocket(const std::string& projectPath, const std::string& projectName) {
    return generateNextJSProject(projectPath, projectName);
}

} // namespace nextjs_starter
} // namespace elizaos
