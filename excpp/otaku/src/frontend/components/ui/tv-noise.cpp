#include "tv-noise.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

void TVNoise(auto {
  className, auto opacity = 0.03, auto intensity = 0.1, auto speed = 60, TVNoiseProps }) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto canvasRef = useRef<HTMLCanvasElement>(nullptr);
    const auto animationFrameRef = useRef<number | std::nullopt>(std::nullopt);

    useEffect(() => {
        const auto canvas = canvasRef.current;
        if (!canvas) return;

        const auto ctx = canvas.getContext("2d");
        if (!ctx) return;

        const auto frameDelay = 1000 / speed;

        const auto resizeCanvas = [&]() {;
            const auto rect = canvas.getBoundingClientRect();
            const auto width = rect.width * window.devicePixelRatio;
            const auto height = rect.height * window.devicePixelRatio;

            // Only update canvas if we have valid dimensions
            if (
            width > 0 &&;
            height > 0 &&;
            Number.isFinite(width) &&;
            Number.isFinite(height);
            ) {
                canvas.width = width;
                canvas.height = height;
                ctx.scale(window.devicePixelRatio, window.devicePixelRatio);
            }
            };

            const auto animate = [&]() {;
                const auto { width, height } = canvas;

                // Skip animation if canvas has invalid dimensions
                if (
                width <= 0 ||;
                height <= 0 ||;
                !Number.isFinite(width) ||;
                !Number.isFinite(height);
                ) {
                    setTimeout(() => {
                        if (animationFrameRef.current) {
                            animationFrameRef.current = requestAnimationFrame(animate);
                        }
                        }, frameDelay);
                        return;
                    }

                    const auto imageData = ctx.createImageData(width, height);
                    const auto data = imageData.data;

                    // Generate random noise
                    for (int i = 0; i < data.length; i += 4) {
                        const auto noise = Math.random();

                        if (noise < intensity) {
                            const auto value = Math.floor(Math.random() * 255);
                            data[i] = value; // Red;
                            data[i + 1] = value; // Green;
                            data[i + 2] = value; // Blue;
                            data[i + 3] = Math.floor(Math.random() * 100 + 50); // Alpha (transparency);
                            } else {
                                data[i + 3] = 0; // Fully transparent;
                            }
                        }

                        ctx.putImageData(imageData, 0, 0);

                        // Schedule next frame
                        setTimeout(() => {
                            if (animationFrameRef.current) {
                                animationFrameRef.current = requestAnimationFrame(animate);
                            }
                            }, frameDelay);
                            };

                            // Initialize
                            resizeCanvas();
                            animationFrameRef.current = requestAnimationFrame(animate);

                            // Handle resize
                            const auto handleResize = [&]() {;
                                resizeCanvas();
                                };

                                window.addEventListener("resize", handleResize);

                                return [&]() {;
                                    if (animationFrameRef.current) {
                                        cancelAnimationFrame(animationFrameRef.current);
                                    }
                                    window.removeEventListener("resize", handleResize);
                                    };
                                    }, [intensity, speed]);

                                    return (;
                                    <canvas;
                                ref={canvasRef}
                                className={cn(;
                                "pointer-events-none absolute inset-0 w-full h-full z-10",
                                className;
                            )}
                            style={{
                                opacity,
                                mixBlendMode: "overlay",
                            }}
                            />;
                            );

}

} // namespace elizaos
