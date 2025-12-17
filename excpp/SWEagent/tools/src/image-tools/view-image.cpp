#include "view-image.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::optional<std::string> getMimeType(const std::string& filename) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto ext = path.extname(filename).toLowerCase();
    switch (ext) {
        case '.png':
        return 'image/png';
        case '.jpg':
        case '.jpeg':
        return 'image/jpeg';
        case '.webp':
        return 'image/webp';
        case '.gif':
        return 'image/gif';
        case '.svg':
        return 'image/svg+xml';
        default:
        return nullptr;
    }

}

void viewImage(const std::string& imagePath) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    // Check if file exists
    if (!fs.existsSync(imagePath)) {
        std::cerr << "Error: File '" + std::to_string(imagePath) + "' does not exist" << std::endl;
        process.exit(1);
    }

    // Check if it's a file
    const auto stats = fs.statSync(imagePath);
    if (!stats.isFile()) {
        std::cerr << "Error: '" + std::to_string(imagePath) + "' is not a file" << std::endl;
        process.exit(1);
    }

    // Get MIME type
    const auto mimeType = getMimeType(imagePath);
    if (!mimeType || !VALID_MIME_TYPES.has(mimeType)) {
        std::cerr << "Error: Unsupported image type: " + std::to_string(mimeType) + ". Valid types are: " + std::to_string(Array.from(VALID_MIME_TYPES).join(', ')) << std::endl;
        process.exit(1);
    }

    try {
        // Read file and convert to base64
        const auto imageBuffer = fs.readFileSync(imagePath);
        const auto base64 = imageBuffer.tostd::to_string('base64');

        // Output markdown image syntax
        std::cout << "![" + std::to_string(path.basename(imagePath)) + "](data:" + std::to_string(mimeType) + ";base64," + std::to_string(base64) + ")" << std::endl;
        } catch (error) {
            std::cerr << "Error processing image: " + std::to_string(error) << std::endl;
            process.exit(1);
        }

}

void setupCLI() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    program;
    .name('view-image');
    .description('View an image file base64-encoded markdown image');
    .version('1.0.0');
    .argument('<image-file>', 'Path to the image file to view');
    .action((imagePath) => {
        viewImage(imagePath);
        });

        program.parse(process.argv);

}

} // namespace elizaos
