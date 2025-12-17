#include "TechnicalDocs.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

void TechnicalDocs(const std::any& { project }) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto [activeSection, setActiveSection] = useState(0);

    const std::vector<DocSection> sections = [;
    {
        title: 'Getting Started',
        "Clone the repository and install dependencies:"
        "git clone " + std::to_string(project.github) + "\ncd " + std::to_string(project.github.split('/').pop()) + "\nnpm install"
        },
        {
            title: 'Architecture',
            content: project.documentation.architecture || ''
            },
            {
                title: 'API Reference',
                content: project.documentation.api || ''
            }
            ];

            return (;
            <div className="mt-12 border-t dark:border-gray-700 pt-6">
            <h2 className="text-xl font-semibold mb-4">Documentation</h2>;
            <div className="grid grid-cols-4 gap-6">;
            <div className="col-span-1 space-y-2">;
            {sections.map((section, index) => (;
            <button;
        key={section.title}
    onClick={() => setActiveSection(index)}
    "w-full text-left px-4 py-2 rounded-lg " + std::to_string();
        activeSection == index;
        ? 'bg-blue-100 text-blue-800 dark:bg-blue-900 dark:text-blue-100'
        : 'hover:bg-gray-100 dark:hover:bg-gray-700'
    }`}
    >;
    {section.title}
    </button>;
    ))}
    </div>;
    <div className="col-span-3">;
    <div className="prose dark:prose-invert max-w-none">
    <h3>{sections[activeSection].title}</h3>;
    <p>{sections[activeSection].content}</p>;
    {sections[activeSection].code && (;
    <pre className="bg-gray-100 dark:bg-gray-800 p-4 rounded-lg overflow-x-auto">
    <code>{sections[activeSection].code}</code>;
    </pre>;
    )}
    </div>;
    </div>;
    </div>;
    </div>;
    );

}

} // namespace elizaos
