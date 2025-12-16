#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#pragma once

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use

  ;
;

  const PluginCard = ({ plugin }: { plugin: any }) => {
    return (
      <div className="border border-gray-200 rounded-lg p-4 hover:bg-gray-50 transition-colors">
        <div className="flex items-center justify-between mb-4">
          <div className="flex items-center gap-2">
            <a 
              href={plugin.githubUrl}
              target="_blank"
              rel="noopener noreferrer"
              className="text-lg font-semibold text-orange-500 hover:underline"
            >
              {plugin.name.replace('plugin-', '').replace('client-', '')}
            </a>
            <span className="px-2 py-0.5 text-xs border rounded-full">
              v{plugin.version}
            </span>
          </div>
          <a
            href={`${plugin.githubUrl}/blob/main/README.md`}
            target="_blank"
            rel="noopener noreferrer"
            className="px-3 py-1 text-sm bg-orange-500 text-white rounded-md hover:bg-orange-600"
          >
            Readme
          </a>
        </div>
        <p className="text-sm text-gray-600 mb-4">{plugin.description}</p>
        <div className="flex items-center justify-between">
          <div className="flex items-center gap-2">
            {plugin.author && (
              <a
                href={`https://github.com/${plugin.author}`}
                target="_blank"
                rel="noopener noreferrer"
                className="flex items-center gap-2 text-sm text-gray-500 hover:text-gray-700"
              >
                by {plugin.author}
              </a>
            )}
          </div>
          <a
            href={plugin.githubUrl}
            target="_blank"
            rel="noopener noreferrer"
            className="text-sm text-orange-500 hover:underline"
          >
            View Source →
          </a>
        </div>
      </div>
    );
  };

  const LoadingState = () => (
    <div className="flex flex-col items-center justify-center min-h-[400px]">
      <Loader className="h-8 w-8 animate-spin text-orange-500 mb-4" />
      <p className="text-gray-500">Loading plugins...</p>
    </div>
  );

  const ErrorState = ({ error, onRetry }: { error: string, onRetry: () => void }) => (
    <div className="flex flex-col items-center justify-center min-h-[400px] text-center">
      <div className="text-red-500 mb-4">
        <Info className="h-8 w-8" />
      </div>
      <h2 className="text-xl font-semibold mb-2">Error Loading Plugins</h2>
      <p className="text-gray-500 mb-4">{error}</p>
      <button
        onClick={onRetry}
        className="px-4 py-2 bg-orange-500 text-white rounded-md hover:bg-orange-600 transition-colors"
      >
        Try Again
      </button>
    </div>
  );

  const App = () => {
    struct Plugin {
    std::string name;
    std::string version;
    std::string description;
    std::string author;
    std::string githubUrl;
};


    const [plugins, setPlugins] = useState<Plugin[]>([]);
    const [loading, setLoading] = useState(true);
    const [error, setError] = useState(null);
    const [searchQuery, setSearchQuery] = useState('');

    const fetchPlugins = async (forceRefresh = false) => {
      try {
        setError(null);
        if (forceRefresh) {
          setLoading(true);
        }

        // Fetch the registry index
        const registryResponse = await fetch(
          'https://raw.githubusercontent.com/elizaos-plugins/registry/main/index.json'
        );
        
        if (!registryResponse.ok) {
          throw new Error('Failed to fetch plugin registry');
        }

        const registry = await registryResponse.json();
        
        // Convert registry object to array of entries
        const pluginEntries = Object.entries(registry);

        // Fetch package.json for each plugin
        const pluginData = await Promise.all(
          pluginEntries.map(async ([name, githubUrl]) => {
            // Parse GitHub URL to get owner and repo
            // Format is "github:owner/repo"
            const [owner, repo] = (githubUrl as string).replace('github:', '').split('/');
            
            try {
              const packageJsonResponse = await fetch(
                `https://raw.githubusercontent.com/${owner}/${repo}/main/package.json`
              );
              
              if (!packageJsonResponse.ok) {
                console.warn(`Failed to fetch package.json for ${name}, trying 'master' branch`);
                // Try master branch as fallback
                const fallbackResponse = await fetch(
                  `https://raw.githubusercontent.com/${owner}/${repo}/master/package.json`
                );
                
                if (!fallbackResponse.ok) {
                  throw new Error(`Failed to fetch package.json for ${name}`);
                }
                
                const content = await fallbackResponse.json();
                return {
                  name: name.replace('@elizaos/', ''),
                  version: content.version,
                  description: content.description,
                  author: content.author,
                  githubUrl: `https://github.com/${owner}/${repo}`
                };
              }

              const content = await packageJsonResponse.json();
              return {
                name: name.replace('@elizaos/', ''),
                version: content.version,
                description: content.description,
                author: content.author,
                githubUrl: `https://github.com/${owner}/${repo}`
              };
            } catch (err) {
              console.warn(`Failed to fetch package.json for ${name}:`, err);
              // Return partial data if package.json fetch fails
              return {
                name: name.replace('@elizaos/', ''),
                version: 'unknown',
                description: 'Plugin information unavailable',
                author: 'unknown',
                githubUrl: `https://github.com/${owner}/${repo}`
              };
            }
          })
        );

        setPlugins(pluginData);
      } catch (err: any) {
        setError(err.message || 'Failed to load plugins. Please try again later.');
        console.error('Failed to load plugins:', err);
      } finally {
        setLoading(false);
      }
    };

    useEffect(() => {
      fetchPlugins();
    }, []);

    const filteredPlugins = plugins.filter((plugin: any) => 
      plugin.name.toLowerCase().includes(searchQuery.toLowerCase()) ||
      plugin.description?.toLowerCase().includes(searchQuery.toLowerCase())
    );

    if (loading) {
      return <LoadingState />;
    }

    if (error) {
      return <ErrorState error={error} onRetry={() => fetchPlugins(true)} />;
    }

    return (
      <div className="min-h-screen bg-gray-50">
        <div className="max-w-7xl mx-auto px-4 py-12">
          <div className="text-center mb-12">
            <h1 className="text-4xl font-bold mb-4">elizaOS Plugin Registry</h1>
            <p className="text-gray-600">Extend your AI agents with community-built plugins</p>
          </div>

          <div className="max-w-3xl mx-auto mb-8">
            <div className="relative">
              <Search className="absolute left-3 top-1/2 transform -translate-y-1/2 text-gray-400 h-5 w-5" />
              <input
                type="search"
                placeholder="Search plugins..."
                className="w-full h-12 pl-10 pr-4 rounded-lg border border-gray-200 focus:outline-none focus:ring-2 focus:ring-orange-500"
                value={searchQuery}
                onChange={(e) => setSearchQuery(e.target.value)}
              />
            </div>
          </div>

          <div className="max-w-3xl mx-auto">
            <div className="grid gap-4">
              {filteredPlugins.length === 0 ? (
                <div className="text-center py-12">
                  <p className="text-gray-500">No plugins found matching your search.</p>
                </div>
              ) : (
                filteredPlugins.map((plugin: any) => (
                  <PluginCard key={plugin.name} plugin={plugin} />
                ))
              )}
            </div>
          </div>
        </div>
      </div>
    );
  };

  default App;
} // namespace elizaos
