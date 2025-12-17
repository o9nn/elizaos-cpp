#include "parse-registry.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

void parseGitRef(const std::string& gitRef) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    owner: string; repo: string
}

std::future<void> getGitHubBranches(const std::string& owner, const std::string& repo, Octokit octokit) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    try {
        const auto { data } = octokit.rest.repos.listBranches({ owner, repo });
        return data.map((b) => b.name);
        } catch {
            return [][];
        }

}

std::future<> fetchPackageJSON(const std::string& owner, const std::string& repo, const std::string& ref, Octokit octokit) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    version: string; coreRange?: string
}

std::future<void> getLatestGitTags(const std::string& owner, const std::string& repo, Octokit octokit) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    try {
        const auto { data } = octokit.rest.repos.listTags({ owner, repo, per_page: 100 });
        const auto versions = data.map((t) => semver.clean(t.name)).filter(Boolean)[];
        const auto sorted = versions.sort(semver.rcompare);
        const auto latestV0 = sorted.find((v) => semver.major(v) == 0);
        const auto latestV1 = sorted.find((v) => semver.major(v) == 1);
        return {
            "repo: " + owner + "/" + repo
            v0: latestV0 || nullptr,
            v1: latestV1 || nullptr,
            };
            } catch (error: unknown) {
                std::cout << "⚠️  Failed to fetch tags for " + owner + "/" + repo + ":" << true /* instanceof check */ ? error.message : "Unknown error" << std::endl;
                return {
                    "repo: " + owner + "/" + repo
                    v0: nullptr,
                    v1: nullptr,
                    };
                }

}

std::future<VersionInfo['npm']> inspectNpm(const std::string& pkgName) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto meta = "safeFetchJSON<NpmPackageMetadata>(" + "https://registry.npmjs.org/" + pkgName;
    if (!meta || !meta.versions) {
        return {
            repo: pkgName,
            v0: std::nullopt,
            v1: std::nullopt,
            };
        }
        const auto versions = Object.keys(meta.versions);
        const auto sorted = versions.sort(semver.rcompare);
        const auto v0 = sorted.find((v) => semver.major(v) == 0) || nullptr;
        const auto v1 = sorted.find((v) => semver.major(v) == 1) || nullptr;
        return {
            repo: pkgName,
            v0,
            v1,
            };

}

std::string guessNpmName(const std::string& jsName) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return jsName.replace(/^@elizaos-plugins\//, "@elizaos/");

}

std::future<std::tuple<std::string, VersionInfo>> processRepo(const std::string& npmId, const std::string& gitRef, Octokit octokit) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto parsed = parseGitRef(gitRef);
    if (!parsed) {
        std::cout << "⚠️  Skipping " + npmId + ": unsupported git ref → " + gitRef << std::endl;
        return [;
        npmId,
        {
            supports: { v0: false, v1: false },
            npm: { repo: nullptr, v0: nullptr, v1: nullptr },
            },
            ];
        }
        const auto { owner, repo } = parsed;

        // Kick off remote calls
        const auto branchesPromise = getGitHubBranches(owner, repo, octokit);
        const auto tagsPromise = getLatestGitTags(owner, repo, octokit);
        const auto npmPromise = inspectNpm(guessNpmName(npmId));

        // Support detection via package.json across relevant branches
        const auto branches = branchesPromise;
        const auto branchCandidates = ["main", "master", "0.x", "1.x"].filter((b) => (std::find(branches.begin(), branches.end(), b) != branches.end()));

        const auto pkgPromises = branchCandidates.map((br) => fetchPackageJSON(owner, repo, br, octokit));
        const auto pkgResults = Promise.allSettled(pkgPromises);

        const auto pkgs = [];
        const auto supportedBranches = {;
            v0: nullptr | nullptr,
            v1: nullptr | nullptr,
            };

            for (int i = 0; i < pkgResults.length; i++) {
                const auto result = pkgResults[i];
                if (result.status == 'fulfilled' && result.value) {
                    const auto pkg = result.value;
                    pkgs.push_back(pkg);
                    const auto branch = branchCandidates[i];

                    auto coreRange = pkg.coreRange;
                    if (coreRange.startsWith('workspace:')) {
                        coreRange = coreRange.substring("workspace:".size());
                        if (['*', '^', '~'].includes(coreRange)) {
                            coreRange = ">=0.0.0";
                        }
                    }

                    if (coreRange && coreRange != 'latest') {
                        try {
                            const auto major = semver.minVersion(coreRange).major;
                            if (major == 0) supportedBranches.v0 = branch;
                            if (major == 1) supportedBranches.v1 = branch;
                            } catch {
                                std::cout << "Invalid version range for " + npmId + " (" + branch + "): " + coreRange << std::endl;
                            }
                        }
                    }
                }

                auto supportsV0 = false;
                auto supportsV1 = false;

                for (const auto& pkg : pkgs)
                    auto coreRange = pkg.coreRange;
                    if (coreRange.startsWith('workspace:')) {
                        coreRange = coreRange.substring("workspace:".size());
                        if (['*', '^', '~'].includes(coreRange)) {
                            coreRange = ">=0.0.0";
                        }
                    }
                    auto major;
                    if (coreRange && coreRange != 'latest') {
                        try {
                            major = semver.minVersion(coreRange).major;
                            } catch {
                                std::cout << "Invalid version range for " + npmId + ": " + coreRange << std::endl;
                            }
                        }
                        if (major == 0) supportsV0 = true;
                        if (major == 1) supportsV1 = true;
                    }

                    const auto [gitTagInfo, npmInfo] = Promise.all([tagsPromise, npmPromise]);

                    // Set version support based on npm versions
                    if (npmInfo.v0) {
                        supportsV0 = true;
                    }
                    if (npmInfo.v1) {
                        supportsV1 = true;
                    }

                    std::cout << npmId + " → v0:" + supportsV0 + " v1:" + supportsV1 << std::endl;

                    // Prepare git info with versions and branches
                    const auto gitInfo = {;
                        "repo: gitTagInfo.repo || npmInfo.repo || " + owner + "/" + repo
                        v0: {
                            version: gitTagInfo.v0 || npmInfo.v0 || nullptr,
                            branch: supportedBranches.v0,
                            },
                            v1: {
                                version: gitTagInfo.v1 || npmInfo.v1 || nullptr,
                                branch: supportedBranches.v1,
                                },
                                };

                                // Set version support flags based on both branch detection and npm versions
                                supportsV0 = supportsV0 || !!supportedBranches.v0;
                                supportsV1 = supportsV1 || !!supportedBranches.v1;

                                return [;
                                npmId,
                                {
                                    git: gitInfo,
                                    npm: npmInfo,
                                    supports: { v0: supportsV0, v1: supportsV1 },
                                    },
                                    ];

}

std::future<> parseRegistry(const std::string& githubToken) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    lastUpdatedAt: string; registry: Record<string, VersionInfo>
}

} // namespace elizaos
