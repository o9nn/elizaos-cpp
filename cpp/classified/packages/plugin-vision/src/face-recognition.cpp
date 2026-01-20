#include "face-recognition.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::future<void> initializeCanvas() {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        try {
            const auto canvas = import("canvas");
            Canvas = canvas.Canvas;
            Image = canvas.Image;
            ImageData = canvas.ImageData;

            // Polyfill for face-api.js
            faceapi.env.monkeyPatch({ Canvas, Image, ImageData });
            } catch (error) {
                std::cerr << "[FaceRecognition] Canvas module not available:" << error << std::endl;
                throw new Error(
                "Canvas module is required for face recognition. Install with: npm install canvas"
                );
            }

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

} // namespace elizaos
