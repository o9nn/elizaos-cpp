#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#pragma once

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use

"use client";

import React, { useEffect, useRef } from "react";
;

struct TVNoiseProps {
    std::optional<std::string> className;
    std::optional<double> opacity;
    std::optional<double> intensity;
    std::optional<double> speed;
};


default : TVNoiseProps) {
  const canvasRef = useRef<HTMLCanvasElement>(null);
  const animationFrameRef = useRef<number | undefined>(undefined);

  useEffect(() => {
    const canvas = canvasRef.current;
    if (!canvas) return;

    const ctx = canvas.getContext("2d");
    if (!ctx) return;

    const frameDelay = 1000 / speed;

    const resizeCanvas = () => {
      const rect = canvas.getBoundingClientRect();
      const width = rect.width * window.devicePixelRatio;
      const height = rect.height * window.devicePixelRatio;

      // Only update canvas if we have valid dimensions
      if (
        width > 0 &&
        height > 0 &&
        Number.isFinite(width) &&
        Number.isFinite(height)
      ) {
        canvas.width = width;
        canvas.height = height;
        ctx.scale(window.devicePixelRatio, window.devicePixelRatio);
      }
    };

    const animate = () => {
      const { width, height } = canvas;

      // Skip animation if canvas has invalid dimensions
      if (
        width <= 0 ||
        height <= 0 ||
        !Number.isFinite(width) ||
        !Number.isFinite(height)
      ) {
        setTimeout(() => {
          if (animationFrameRef.current) {
            animationFrameRef.current = requestAnimationFrame(animate);
          }
        }, frameDelay);
        return;
      }

      const imageData = ctx.createImageData(width, height);
      const data = imageData.data;

      // Generate random noise
      for (let i = 0; i < data.length; i += 4) {
        const noise = Math.random();

        if (noise < intensity) {
          const value = Math.floor(Math.random() * 255);
          data[i] = value; // Red
          data[i + 1] = value; // Green
          data[i + 2] = value; // Blue
          data[i + 3] = Math.floor(Math.random() * 100 + 50); // Alpha (transparency)
        } else {
          data[i + 3] = 0; // Fully transparent
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
    const handleResize = () => {
      resizeCanvas();
    };

    window.addEventListener("resize", handleResize);

    return () => {
      if (animationFrameRef.current) {
        cancelAnimationFrame(animationFrameRef.current);
      }
      window.removeEventListener("resize", handleResize);
    };
  }, [intensity, speed]);

  return (
    <canvas
      ref={canvasRef}
      className={cn(
        "pointer-events-none absolute inset-0 w-full h-full z-10",
        className
      )}
      style={{
        opacity,
        mixBlendMode: "overlay",
      }}
    />
  );
}

} // namespace elizaos
