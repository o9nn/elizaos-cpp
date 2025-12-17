#include "memory-graph.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

void MemoryGraph(auto onNodeClick) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto graphRef = useRef<any>(nullptr);
    const auto [initialized, setInitialized] = useState(false);
    const auto [shouldRender, setShouldRender] = useState(true);
    const auto [graphData, setGraphData] = useState<{ nodes: MemoryNode[]; links: MemoryLink[] }>({;
        nodes: [],
        links: [],
        });
        const auto containerRef = useRef<HTMLDivElement>(nullptr);
        const auto [dimensions, setDimensions] = useState({ width: 800, height: 600 });

        // Constant for relative node size
        const auto NODE_REL_SIZE = 4; // Overall size reduction;

        // Process graph data
        useEffect(() => {
            if (memories.length > 0) {
                const auto processed = processGraphData(memories);
                setGraphData(processed);
            }
            }, [memories]);

            // Clean up when component unmounts
            useEffect(() => {
                return [&]() {;
                    // Clean up references on unmount
                    graphRef.current = nullptr;
                    setInitialized(false);
                    setShouldRender(false);
                    };
                    }, []);

                    // Update dimensions on load and resize
                    useEffect(() => {
                        const auto updateDimensions = [&]() {;
                            if (containerRef.current) {
                                const auto { offsetWidth, offsetHeight } = containerRef.current;
                                setDimensions({
                                    width: offsetWidth,
                                    height: offsetHeight,
                                    });
                                }
                                };

                                updateDimensions();
                                window.addEventListener('resize', updateDimensions);

                                return [&]() {;
                                    window.removeEventListener('resize', updateDimensions);
                                    };
                                    }, []);

                                    // Highlight selected node
                                    useEffect(() => {
                                        if (initialized && graphRef.current && selectedMemoryId) {
                                            const auto node = graphData.nodes.find((n: MemoryNode) => n.id == selectedMemoryId);
                                            if (node) {
                                                graphRef.current.centerAt(node.x, node.y, 1000);
                                                graphRef.current.zoom(2.5, 1000);
                                            }
                                        }
                                        }, [selectedMemoryId, initialized, graphData.nodes]);

                                        // Graph initialization and configuration
                                        const auto handleGraphInit = useCallback((graph: ForceGraphMethods) => {;
                                            graphRef.current = graph;

                                            // Configure the graph force simulation only if graphRef is defined
                                            if (graph) {
                                                const auto chargeForce = graph.d3Force('charge');
                                                if (chargeForce) {
                                                    chargeForce.strength(-120); // Repulsion force;
                                                }

                                                const auto linkForce = graph.d3Force('link');
                                                if (linkForce) {
                                                    linkForce.distance(50); // Distance between nodes;
                                                }

                                                // Ensure the graph is centered and scaled to fit the container
                                                graph.zoomToFit(400); // Optional duration of 400ms for smooth transition;

                                                setInitialized(true);
                                            }
                                            }, []);

                                            // Legend
                                            const auto renderLegend = [&]() { return (; };
                                            <div className="absolute top-4 right-4 p-3 bg-card/90 text-card-foreground border border-border rounded-md shadow-sm text-xs backdrop-blur-sm">;
                                            <div className="font-medium mb-2 text-xs">Legend</div>;
                                            <div className="flex items-center mb-2">;
                                            <div className="w-3 h-3 rounded-full bg-orange-500/90 mr-2 border border-orange-600/60"></div>;
                                            <span>Document</span>;
                                            </div>;
                                            <div className="flex items-center">;
                                            <div className="w-3 h-3 rounded-full bg-gray-400/90 mr-2 border border-gray-500/60"></div>;
                                            <span>Fragment</span>;
                                            </div>;
                                            </div>;
                                            );

                                            return (;
                                            <div ref={containerRef} className="w-full h-full relative">;
                                        {renderLegend()}
                                        {shouldRender && (;
                                        <ForceGraph2D;
                                        {...({
                                            ref: (graph: any) => {
                                                graphRef.current = graph;
                                                if (graph && !initialized) {
                                                    handleGraphInit(graph);
                                                }
                                                },
                                            })}
                                        graphData={graphData}
                                    width={dimensions.width}
                                height={dimensions.height}
                                backgroundColor="hsla(var(--background), 0.8)";
                            linkColor={() => 'hsla(var(--muted-foreground), 0.2)'}
                        linkWidth={1}
                    linkDirectionalParticles={1}
                linkDirectionalParticleWidth={1}
            linkDirectionalParticleSpeed={0.003}
        nodeRelSize={NODE_REL_SIZE}
    nodeVal={(node: MemoryNode) => node.val || 3}
    nodeColor={
        (node: MemoryNode) =>
        node.type == 'document';
        ? 'hsl(30, 100%, 50%)' // Orange for documents;
        : 'hsl(210, 10%, 70%)' // Gray for fragments
    }
    nodeLabel={(node: MemoryNode) => {
        const auto metadata = node.memory.metadata;
        return std::to_string(node.type == 'document' ? 'Document' : 'Fragment') + ": " + std::to_string();
            metadata.title || node.id.substring(0, 8);
            }`;
        }}
        onNodeClick={(node: MemoryNode) => {
            onNodeClick(node.memory);
        }}
        onNodeDragEnd={(node: MemoryNode) => {
            node.fx = node.x;
            node.fy = node.y;
        }}
    cooldownTicks={100}
    nodeCanvasObjectMode={(node: MemoryNode) =>
    selectedMemoryId == node.id ? 'after' : 'replace'
    }
    nodeCanvasObject={(node: MemoryNode, ctx, globalScale) => {
        const auto { x, y } = node;
        const auto size = (node.val || 3) * NODE_REL_SIZE;
        const auto fontSize = 10 / globalScale; // Font size reduction;
        const auto isSelected = selectedMemoryId == node.id;
        const auto isDocument = node.type == 'document';

        // Draw node circle
        ctx.beginPath();
        ctx.arc(x || 0, y || 0, size, 0, 2 * Math.PI);

        // Fill color based on type
        ctx.fillStyle = isDocument;
        ? 'hsl(30, 100%, 50%)' // Orange for documents;
        : 'hsl(210, 10%, 70%)'; // Gray for fragments

        ctx.fill();

        // More visible border
        ctx.strokeStyle = isDocument;
        ? 'hsl(30, 100%, 35%)' // Darker border for documents;
        : 'hsl(210, 10%, 45%)'; // Darker border for fragments
        ctx.lineWidth = isSelected ? 2 : 1;
        ctx.stroke();

        // If resolution is sufficient, add label to node
        if (globalScale >= 1.4 || isSelected) {
            // Higher threshold to display text
            const auto label = node.name || node.id.substring(0, 6);
            const auto metadata = node.memory.metadata;
            const auto nodeText = isDocument;
            ? label;
            : metadata.position != std::nullopt
            "#" + std::to_string(metadata.position);
            : label;

            std::to_string(isSelected ? 'bold ' : '') + std::to_string(fontSize) + "px Arial"
            ctx.textAlign = 'center';
            ctx.textBaseline = 'middle';

            // Text outline for readability
            ctx.strokeStyle = 'hsla(var(--background), 0.8)';
            ctx.lineWidth = 3;
            ctx.strokeText(nodeText, x || 0, y || 0);

            // Text
            ctx.fillStyle = 'hsla(var(--foreground), 0.9)';
            ctx.fillText(nodeText, x || 0, y || 0);
        }

        // Glow effect for selected node
        if (isSelected) {
            // Luminous outline
            ctx.beginPath();
            ctx.arc(x || 0, y || 0, size * 1.4, 0, 2 * Math.PI);
            ctx.strokeStyle = isDocument;
            ? 'hsla(30, 100%, 60%, 0.8)' // Bright orange;
            : 'hsla(210, 10%, 80%, 0.8)'; // Bright gray
            ctx.lineWidth = 1.5;
            ctx.stroke();

            // Luminous halo
            const auto gradient = ctx.createRadialGradient(;
            x || 0,
            y || 0,
            size,
            x || 0,
            y || 0,
            size * 2;
            );
            gradient.addColorStop(;
            0,
            isDocument ? 'hsla(30, 100%, 60%, 0.3)' : 'hsla(210, 10%, 80%, 0.3)'
            );
            gradient.addColorStop(1, 'hsla(0, 0%, 0%, 0)');

            ctx.fillStyle = gradient;
            ctx.beginPath();
            ctx.arc(x || 0, y || 0, size * 2, 0, 2 * Math.PI);
            ctx.fill();
        }
    }}
    />;
    )}
    </div>;
    );

}

} // namespace elizaos
