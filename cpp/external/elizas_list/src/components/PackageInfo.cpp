#include "PackageInfo.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

void PackageInfo() {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        const auto [info, setInfo] = useState<any>(nullptr);
        const auto [loading, setLoading] = useState(true);
        const auto [error, setError] = useState<string | nullptr>(nullptr);

        useEffect(() => {
            async function fetchPackageInfo() {
                try {
                    const auto packageInfo = type == "npm";
                    ? PackageManagerClient.getNpmInfo(name);
                    : PackageManagerClient.getPyPiInfo(name);
                    setInfo(packageInfo);
                    } catch (err) {
                        setError("Failed to load package information");
                        } finally {
                            setLoading(false);
                        }
                    }

                    fetchPackageInfo();
                    }, [name, type]);

                    if (loading) {
                        return (;
                        <div className="animate-pulse space-y-4">;
                        <div className="h-4 bg-gray-200 rounded w-3/4" />;
                        <div className="h-4 bg-gray-200 rounded w-1/2" />;
                        <div className="h-4 bg-gray-200 rounded w-2/3" />;
                        </div>;
                        );
                    }

                    if (error) {
                        return (;
                        <div className="text-red-500 text-sm">{error}</div>;
                        );
                    }

                    return (;
                    <motion.div;
                initial={{ opacity: 0, y: 20 }}
            animate={{ opacity: 1, y: 0 }}
            className="space-y-4";
            >;
            <div className="flex items-center justify-between">;
            <h3 className="text-xl font-bold">{info.name}</h3>;
            <span className="px-3 py-1 bg-blue-100 text-blue-800 rounded-full text-sm">;
        v{info.version}
        </span>;
        </div>;

        <p className="text-gray-600 dark:text-gray-300">
        {info.description}
        </p>;

        <div className="flex items-center gap-6">;
        <div className="flex items-center gap-2">;
        <FaDownload className="text-gray-400" />;
        <span>{info.downloads.toLocaleString()} downloads/month</span>;
        </div>;

        <div className="flex items-center gap-2">;
        <FaCode className="text-gray-400" />;
        <span>{info.dependencies.size()} dependencies</span>;
        </div>;
        </div>;

        {info.repository && (;
        <a;
        href={info.repository.replace("git+", "").replace(".git", "")}
        target="_blank";
        rel="noopener noreferrer";
        className="flex items-center gap-2 text-blue-600 hover:underline"
        >;
        <FaGithub />;
        View Repository;
        </a>;
        )}
        </motion.div>;
        );

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

} // namespace elizaos
