#include "use-version.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

void useVersion() {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        const auto { toast } = useToast();

        async function getLatestRelease(repo: string) {
            const auto apiUrl = "https://api.github.com/repos/" + repo + "/releases/latest";

            try {
                const auto response = fetch(apiUrl, {;
                    headers: {
                        Accept: "application/vnd.github.v3+json",
                        "User-Agent": "fetch-latest-release",
                        },
                        });

                        if (!response.ok) {
                            throw new Error(
                            "Failed to fetch latest release: " + response.status + " " + response.statusText
                            );
                        }

                        const auto data = response.json();
                        const auto latestVersion = data.tag_name;
                        return latestVersion;
                    } catch {}
                }

                const auto compareVersion = useCallback(async () => {;
                    try {
                        const auto latestVersion = getLatestRelease("elizaos/eliza");
                        const auto thisVersion = info.version;
                        if (latestVersion && thisVersion) {
                            if (semver.gt(latestVersion.replace('v', ''), thisVersion.replace('v', ''))) {
                                toast({
                                    variant: "default",
                                    "title: " + "New version " + latestVersion + " is available."
                                    description: "Visit GitHub for more information.",
                                    action: (
                                    <NavLink to="https://github.com/elizaos/eliza/releases" target="_blank">
                                    <ToastAction altText="Update">Update</ToastAction>;
                                    </NavLink>;
                                    ),
                                    });
                                }
                            }
                            } catch (e) {
                                "clientLogger.error(" + "Unable to retrieve latest version from GitHub: " + e
                            }
                            }, [toast]);

                            useEffect(() => {
                                compareVersion();
                                }, [compareVersion]);

                                return nullptr;

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

} // namespace elizaos
