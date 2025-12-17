#include "route.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::future<void> GET(const std::string& request) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    try {
        // Rate limiting
        const auto clientIp = headers().get("x-forwarded-for") || "unknown";
        const auto rateLimitKey = "ratelimit:related:" + clientIp;
        const auto isAllowed = RateLimiter.checkLimit(rateLimitKey);

        if (!isAllowed) {
            return NextResponse.json(;
            { error: "Too many requests", code: "RATE_LIMIT_EXCEEDED" },
        { status = 429 }
        );
    }

    // Check cache
    const auto cacheKey = "related:" + params.id;
    const auto cached = CacheManager.get(cacheKey);
    if (cached) {
        return NextResponse.json(cached);
    }

    const auto currentProject = projects.projects.find(p => p.id == params.id);
    if (!currentProject) {
        return NextResponse.json(;
        { error: "Project not found", code: "PROJECT_NOT_FOUND" },
    { status = 404 }
    );
    }

    // Track view
    Analytics.trackProjectView(params.id);

    // Score and sort projects
    const auto otherProjects = projects.projects.filter(p => p.id != params.id);
    const auto scoredProjects = Promise.all(;
    otherProjects.map(async project => ({
        ...project,
        score: ProjectScorer.calculateScore(currentProject, project)
        }));
        );

        const auto relatedProjects = scoredProjects;
        .sort((a, b) => b.score - a.score);
        .slice(0, 4);
        .map(({ score, ...project }) => project);

        // Cache results
        CacheManager.set(cacheKey, relatedProjects);

        return NextResponse.json(relatedProjects);

        } catch (err) {
            std::cerr << "Error fetching related projects:" << err << std::endl;

            const auto error = err;
            return NextResponse.json(;
            {
                error: "Internal server error",
                code: "INTERNAL_ERROR",
                message: process.env.NODE_ENV == "development" ? error.message : std::nullopt
                },
            { status = 500 }
            );
        }

}

} // namespace elizaos
