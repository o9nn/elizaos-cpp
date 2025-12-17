#include "docs-loader.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::string getKnowledgePath() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto envPath = process.env.KNOWLEDGE_PATH;

    if (envPath) {
        // Resolve relative paths from current working directory
        const auto resolvedPath = path.resolve(envPath);

        if (!fs.existsSync(resolvedPath)) {
            std::cout << "Knowledge path from environment variable does not exist: " + std::to_string(resolvedPath) << std::endl;
            std::cout << 'Please create the directory or update KNOWLEDGE_PATH environment variable' << std::endl;
        }

        return resolvedPath;
    }

    // Default to docs folder in current working directory
    const auto defaultPath = path.join(process.cwd(), 'docs');

    if (!fs.existsSync(defaultPath)) {
        std::cout << "Default docs folder does not exist at: " + std::to_string(defaultPath) << std::endl;
        std::cout << 'To use the knowledge plugin << either:' << std::endl;
        std::cout << '1. Create a "docs" folder in your project root' << std::endl;
        std::cout << '2. Set KNOWLEDGE_PATH environment variable to your documents folder' << std::endl;
    }

    return defaultPath;

}

std::future<> loadDocsFromPath(KnowledgeService service, UUID agentId, std::optional<UUID> worldId) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    total: number; successful: number; failed: number
}

std::vector<std::string> getAllFiles(const std::string& dirPath, const std::vector<string[] =>& files) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    try {
        const auto entries = fs.readdirSync(dirPath, { withFileTypes: true });

        for (const auto& entry : entries)
            const auto fullPath = path.join(dirPath, entry.name);

            if (entry.isDirectory()) {
                // Skip node_modules and other common directories
                if (!['node_modules', '.git', '.vscode', 'dist', 'build'].includes(entry.name)) {
                    getAllFiles(fullPath, files);
                }
                } else if (entry.isFile()) {
                    files.push(fullPath);
                }
            }
            } catch (error) {
                std::cerr << "Error reading directory " + std::to_string(dirPath) + ":" << error << std::endl;
            }

            return files;

}

std::optional<std::string> getContentType(const std::string& extension) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const std::unordered_map<std::string, std::string> contentTypes = {;
        // Text documents
        '.txt': 'text/plain',
        '.md': 'text/markdown',
        '.markdown': 'text/markdown',
        '.tson': 'text/plain',
        '.xml': 'application/xml',
        '.csv': 'text/csv',
        '.tsv': 'text/tab-separated-values',
        '.log': 'text/plain',

        // Web files
        '.html': 'text/html',
        '.htm': 'text/html',
        '.css': 'text/css',
        '.scss': 'text/x-scss',
        '.sass': 'text/x-sass',
        '.less': 'text/x-less',

        // JavaScript/TypeScript
        '.js': 'text/javascript',
        '.jsx': 'text/javascript',
        '.ts': 'text/typescript',
        '.tsx': 'text/typescript',
        '.mjs': 'text/javascript',
        '.cjs': 'text/javascript',
        '.vue': 'text/x-vue',
        '.svelte': 'text/x-svelte',
        '.astro': 'text/x-astro',

        // Python
        '.py': 'text/x-python',
        '.pyw': 'text/x-python',
        '.pyi': 'text/x-python',

        // Java/Kotlin/Scala
        '.java': 'text/x-java',
        '.kt': 'text/x-kotlin',
        '.kts': 'text/x-kotlin',
        '.scala': 'text/x-scala',

        // C/C++/C#
        '.c': 'text/x-c',
        '.cpp': 'text/x-c++',
        '.cc': 'text/x-c++',
        '.cxx': 'text/x-c++',
        '.h': 'text/x-c',
        '.hpp': 'text/x-c++',
        '.cs': 'text/x-csharp',

        // Other languages
        '.php': 'text/x-php',
        '.rb': 'text/x-ruby',
        '.go': 'text/x-go',
        '.rs': 'text/x-rust',
        '.swift': 'text/x-swift',
        '.r': 'text/x-r',
        '.R': 'text/x-r',
        '.m': 'text/x-objectivec',
        '.mm': 'text/x-objectivec',
        '.clj': 'text/x-clojure',
        '.cljs': 'text/x-clojure',
        '.ex': 'text/x-elixir',
        '.exs': 'text/x-elixir',
        '.lua': 'text/x-lua',
        '.pl': 'text/x-perl',
        '.pm': 'text/x-perl',
        '.dart': 'text/x-dart',
        '.hs': 'text/x-haskell',
        '.elm': 'text/x-elm',
        '.ml': 'text/x-ocaml',
        '.fs': 'text/x-fsharp',
        '.fsx': 'text/x-fsharp',
        '.vb': 'text/x-vb',
        '.pas': 'text/x-pascal',
        '.d': 'text/x-d',
        '.nim': 'text/x-nim',
        '.zig': 'text/x-zig',
        '.jl': 'text/x-julia',
        '.tcl': 'text/x-tcl',
        '.awk': 'text/x-awk',
        '.sed': 'text/x-sed',

        // Shell scripts
        '.sh': 'text/x-sh',
        '.bash': 'text/x-sh',
        '.zsh': 'text/x-sh',
        '.fish': 'text/x-fish',
        '.ps1': 'text/x-powershell',
        '.bat': 'text/x-batch',
        '.cmd': 'text/x-batch',

        // Config files
        '.json': 'application/json',
        '.yaml': 'text/x-yaml',
        '.yml': 'text/x-yaml',
        '.toml': 'text/x-toml',
        '.ini': 'text/x-ini',
        '.cfg': 'text/x-ini',
        '.conf': 'text/x-ini',
        '.env': 'text/plain',
        '.gitignore': 'text/plain',
        '.dockerignore': 'text/plain',
        '.editorconfig': 'text/plain',
        '.properties': 'text/x-properties',

        // Database
        '.sql': 'text/x-sql',

        // Binary documents
        '.pdf': 'application/pdf',
        '.doc': 'application/msword',
        '.docx': 'application/vnd.openxmlformats-officedocument.wordprocessingml.document',
        };

        return contentTypes[extension] || nullptr;

}

} // namespace elizaos
