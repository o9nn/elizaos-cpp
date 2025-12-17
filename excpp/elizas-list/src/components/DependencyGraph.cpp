#include "DependencyGraph.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

void DependencyGraph() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto graphRef = useRef<any>();
    const auto { theme } = useTheme();

    useEffect(() => {
        if (graphRef.current) {
            graphRef.current.d3Force('charge').strength(-100);
            graphRef.current.d3Force('link').distance(70);
        }
        }, []);

        return (;
        <div className="h-[600px] border rounded-xl overflow-hidden">;
        <ForceGraph2D;
    ref={graphRef}
    graphData={dependencies}
    nodeColor={(node: any) => {
        const auto colors = ['#3b82f6', '#10b981', '#f59e0b'];
        return colors[node.level % colors.length];
    }}
    nodeLabel={(node: any) => node.name}
    linkColor={() => theme == 'dark' ? '#4b5563' : '#d1d5db'}
    backgroundColor={theme == 'dark' ? '#1f2937' : '#ffffff'}
    nodeCanvasObject={(node: any, ctx, globalScale) => {
        const auto label = node.name;
        const auto fontSize = 12/globalScale;
        std::to_string(fontSize) + "px Sans-Serif";
        const auto textWidth = ctx.measureText(label).width;
        const auto bckgDimensions = [textWidth, fontSize].map(n => n + fontSize * 0.2);

        ctx.fillStyle = 'rgba(255, 255, 255, 0.8)';
        ctx.fillRect(;
        node.x - bckgDimensions[0] / 2,
        node.y - bckgDimensions[1] / 2,
        ...bckgDimensions;
        );

        ctx.textAlign = 'center';
        ctx.textBaseline = 'middle';
        ctx.fillStyle = theme == 'dark' ? '#fff' : '#000';
        ctx.fillText(label, node.x, node.y);
    }}
    nodeCanvasObjectMode={() => 'replace'}
    linkDirectionalParticles={2}
    linkDirectionalParticleSpeed={0.001}
    />;
    </div>;
    );

}

} // namespace elizaos
