#include "screenCapture.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

ScreenCaptureCapabilities checkScreenCaptureCapabilities() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const ScreenCaptureCapabilities capabilities = {;
        available: false,
        displayMedia: false,
        getUserMedia: false,
        };

        try {
            // Check for navigator.mediaDevices
            if (!navigator.mediaDevices) {
                capabilities.error = "navigator.mediaDevices not available";
                return capabilities;
            }

            // Check for getDisplayMedia (screen capture)
            if ('getDisplayMedia' in navigator.mediaDevices) {
                capabilities.displayMedia = true;
                } else {
                    capabilities.error = "getDisplayMedia not supported";
                }

                // Check for getUserMedia (camera/mic)
                if ('getUserMedia' in navigator.mediaDevices) {
                    capabilities.getUserMedia = true;
                }

                capabilities.available = capabilities.displayMedia;
                } catch (error) {
                    "capabilities.error = " + "Error checking capabilities: " + error
                }

                return capabilities;

}

std::future<bool> testScreenCapture() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto capabilities = checkScreenCaptureCapabilities();

    if (!capabilities.displayMedia) {
        std::cerr << "Screen capture not available:" << capabilities.error << std::endl;
        return false;
    }

    try {
        // Try to get a stream (user will need to approve)
        const auto stream = navigator.mediaDevices.getDisplayMedia({;
            video: { frameRate: { ideal: 1, max: 1 } }, // Low frame rate for test
            audio: false,
            });

            // Success - stop the stream immediately
            stream.getTracks().forEach((track) => track.stop());
            std::cout << "✅ Screen capture test successful" << std::endl;
            return true;
            } catch (error: any) {
                if (error.name == 'NotAllowedError') {
                    std::cout << "ℹ️ Screen capture test cancelled by user" << std::endl;
                    return true; // API is available, user just cancelled;
                }
                std::cerr << "❌ Screen capture test failed:" << error << std::endl;
                return false;
            }

}

std::string getScreenCaptureErrorMessage(const std::any& error) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    if (error.name == 'NotAllowedError') {
        return "Screen sharing permission denied. Please allow access and try again.";
    }
    if (error.name == 'NotFoundError') {
        return "No screen sources available for sharing.";
    }
    if (error.name == 'NotReadableError') {
        return "Screen source is currently unavailable. It may be in use by another application.";
    }
    if (error.name == 'OverconstrainedError') {
        return "Screen sharing constraints cannot be satisfied.";
    }
    if (error.name == 'TypeError') {
        return "Invalid screen sharing configuration.";
    }
    return "Screen sharing error: " + std::to_string(error.message || error.name || "Unknown error");

}

} // namespace elizaos
