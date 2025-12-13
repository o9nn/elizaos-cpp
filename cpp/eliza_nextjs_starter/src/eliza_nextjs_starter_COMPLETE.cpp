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
        
        return writeFile(projectPath + "/pages/index.tsx", indexContent);
    }
    
    static bool generateComponents(const std::string& projectPath) {
        createDirectory(projectPath + "/components");
        
        std::string chatComponent = R"(import { useState } from 'react'

export function ElizaChat() {
  const [messages, setMessages] = useState<string[]>([])
  const [input, setInput] = useState('')

  const sendMessage = async () => {
    if (!input.trim()) return

    setMessages([...messages, input])
    setInput('')

    // TODO: Integrate with ElizaOS backend
    const response = await fetch('/api/chat', {
      method: 'POST',
      headers: { 'Content-Type': 'application/json' },
      body: JSON.stringify({ message: input })
    })

    const data = await response.json()
    setMessages(prev => [...prev, data.response])
  }

  return (
    <div className="chat-container">
      <div className="messages">
        {messages.map((msg, i) => (
          <div key={i} className="message">{msg}</div>
        ))}
      </div>
      <div className="input-area">
        <input
          type="text"
          value={input}
          onChange={(e) => setInput(e.target.value)}
          onKeyPress={(e) => e.key === 'Enter' && sendMessage()}
          placeholder="Type a message..."
        />
        <button onClick={sendMessage}>Send</button>
      </div>
    </div>
  )
}
)";
        
        return writeFile(projectPath + "/components/ElizaChat.tsx", chatComponent);
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
// EXPORTED API
// ==============================================================================

bool generateNextJSProject(const std::string& projectPath, const std::string& projectName) {
    return NextJSTemplateGenerator::generateProject(projectPath, projectName);
}

} // namespace nextjs_starter
} // namespace elizaos
