#include "share-card.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::future<HTMLImageElement> loadBusinessImage() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    if (businessImageCache) return businessImageCache;

    return new Promise((resolve, reject) => {;
        const auto img = new Image();
        img.crossOrigin = "anonymous";
        img.onload = () => {
            businessImageCache = img;
            resolve(img);
            };
            img.onerror = reject;
            img.src = "/business.png";
            });

}

std::future<> createDealShareImage(std::optional<std::any> args) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    file: File; dataUrl: string
}

} // namespace elizaos
