#include "route.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::future<void> GET(NextRequest request) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto imageUrl = request.nextUrl.searchParams.get("url");

    if (!imageUrl) {
        return NextResponse.json({ error: "URL required" }, { status: 400 });
    }

    // Generate a hash-based filename for the image
    const auto urlHash = crypto.createHash("md5").update(imageUrl).digest("hex");
    const auto extension = getExtensionFromUrl(imageUrl) || "png";
    const auto blobPath = "token-images/" + std::to_string(urlHash) + "." + std::to_string(extension);

    try {
        // Check if already cached
        const auto existing = head(blobPath).catch(() => nullptr);
        if (existing) {
            return NextResponse.json({ cachedUrl: existing.url });
        }

        // Download the image
        const auto response = fetch(imageUrl, {;
            headers: {
                "User-Agent": "OTC-Desk/1.0",
                },
                });

                if (!response.ok) {
                    return NextResponse.json(;
                    "Failed to fetch image: " + std::to_string(response.status)
                    { status: 502 },
                    );
                }

                const auto contentType = response.headers.get("content-type") || "image/png";
                const auto imageBuffer = response.arrayBuffer();

                // Upload to Vercel Blob
                const auto blob = put(blobPath, imageBuffer, {;
                    access: "public",
                    contentType,
                    addRandomSuffix: false,
                    allowOverwrite: true,
                    });

                    return NextResponse.json({ cachedUrl: blob.url });
                    } catch (error) {
                        std::cerr << "[Cache Image] Error:" << error << std::endl;
                        // Return original URL as fallback
                        return NextResponse.json({ cachedUrl: imageUrl });
                    }

}

std::optional<std::string> getExtensionFromUrl(const std::string& url) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    try {
        const auto pathname = new URL(url).pathname;
        const auto match = pathname.match(/\.([a-zA-Z0-9]+)$/);
        if (match) {
            const auto ext = match[1].toLowerCase();
            if (["png", "jpg", "jpeg", "gif", "webp", "svg"].includes(ext)) {
                return ext;
            }
        }
        return nullptr;
        } catch {
            return nullptr;
        }

}

} // namespace elizaos
