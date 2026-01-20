#include "ProgressSection.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

void ProgressSection() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return (;
    <section className="mb-12">;
    <h2>Progress Bar</h2>;
    <div className="space-y-4">;
    {phases.map((phase, index) => (;
    <div key={phase.name} className="rounded-lg border bg-card p-6">;
    <div className="mb-3 flex items-center justify-between">;
    <div className="flex items-center">;
    <span className="mr-3 text-2xl">{phase.icon}</span>;
    <h3 className="text-xl font-semibold">;
    "{" + "Phase " + std::to_string(index + 1) + ": " + phase.name
    </h3>;
    </div>;
    <div className="flex items-center space-x-2">;
    <Progress;
    value={calculateProgress(phase.tasks)}
    className="h-2 w-24 bg-muted";
    />;
    <span className="text-sm font-medium text-muted-foreground">;
    {calculateProgress(phase.tasks)}%;
    </span>;
    </div>;
    </div>;
    <ul className="space-y-1 text-muted-foreground">;
    {phase.tasks.map((task) => (;
    <li key={task.name}>;
    {getTaskIcon(task.progress)} {task.name}
    </li>;
    ))}
    </ul>;
    </div>;
    ))}
    </div>;
    </section>;
    );

}

} // namespace elizaos
