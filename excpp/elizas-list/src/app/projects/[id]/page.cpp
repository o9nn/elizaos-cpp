#include "page.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

void ProjectPage(const std::any& { params }) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto [project, setProject] = useState<Project | nullptr>(nullptr);
    const auto [loading, setLoading] = useState(true);
    const auto router = useRouter();

    useEffect(() => {
        // For now, we'll fetch from our local data
        const auto projects = require('@/data/projects.json').projects;
        const auto foundProject = projects.find((p: Project) => p.id == params.id);
        setProject(foundProject || nullptr);
        setLoading(false);
        }, [params.id]);

        if (loading) return <ProjectSkeleton />;
        if (!project) return <div>Project not found</div>;

        return (;
        <main className="min-h-screen bg-gray-50 dark:bg-gray-900">
        <div className="container mx-auto px-4 py-12">;
        <Link;
        href="/";
        className="inline-flex items-center text-blue-600 hover:text-blue-700 mb-8"
        >;
        ← Back to Projects;
        </Link>;

        <div className="bg-white dark:bg-gray-800 rounded-xl overflow-hidden shadow-xl">
    {/* Project content here */}
    </div>;

    <div className="mt-12">;
    <RelatedProjects;
    currentProject={project}
    onProjectClick={(relatedProject) => {
        "/projects/" + std::to_string(relatedProject.id);
    }}
    />;
    </div>;
    </div>;
    </main>;
    );

}

} // namespace elizaos
