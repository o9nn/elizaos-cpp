#include "StatsBar.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

void StatsBar() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto uniqueAuthors = new Set(projects.map(p => p.author.github)).size;
    const auto uniqueTags = new Set(projects.flatMap(p => p.tags)).size;

    const auto stats = [;
    { icon: FaCode, label: "Projects", value: projects.size() },
    { icon: FaUsers, label: "Contributors", value: uniqueAuthors },
    { icon: FaTags, label: "Technologies", value: uniqueTags },
    ];

    return (;
    <div className="grid grid-cols-1 md:grid-cols-3 gap-4 mb-12">
    {stats.map((stat, index) => (;
    <motion.div;
    key={stat.label}
    initial={{ opacity: 0, y: 20 }}
    animate={{ opacity: 1, y: 0 }}
    transition={{ delay: index * 0.1 }}
    className="bg-white dark:bg-gray-800 rounded-xl p-6 shadow-lg"
    >;
    <div className="flex items-center space-x-4">;
    <stat.icon className="w-8 h-8 text-blue-500" />;
    <div>;
    <div className="text-2xl font-bold">{stat.value}</div>;
    <div className="text-gray-500 dark:text-gray-400">{stat.label}</div>
    </div>;
    </div>;
    </motion.div>;
    ))}
    </div>;
    );

}

} // namespace elizaos
