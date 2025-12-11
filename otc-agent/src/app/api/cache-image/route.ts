import { NextRequest, NextResponse } from "next/server";
import { put, head } from "@vercel/blob";
import crypto from "crypto";

/**
 * Cache external images to Vercel Blob storage
 * GET /api/cache-image?url=<external_url>
 * Returns { cachedUrl: string } or { error: string }
 */
export async function GET(request: NextRequest) {
  const imageUrl = request.nextUrl.searchParams.get("url");

  if (!imageUrl) {
    return NextResponse.json({ error: "URL required" }, { status: 400 });
  }

  // Generate a hash-based filename for the image
  const urlHash = crypto.createHash("md5").update(imageUrl).digest("hex");
  const extension = getExtensionFromUrl(imageUrl) || "png";
  const blobPath = `token-images/${urlHash}.${extension}`;

  try {
    // Check if already cached
    const existing = await head(blobPath).catch(() => null);
    if (existing) {
      return NextResponse.json({ cachedUrl: existing.url });
    }

    // Download the image
    const response = await fetch(imageUrl, {
      headers: {
        "User-Agent": "OTC-Desk/1.0",
      },
    });

    if (!response.ok) {
      return NextResponse.json(
        { error: `Failed to fetch image: ${response.status}` },
        { status: 502 },
      );
    }

    const contentType = response.headers.get("content-type") || "image/png";
    const imageBuffer = await response.arrayBuffer();

    // Upload to Vercel Blob
    const blob = await put(blobPath, imageBuffer, {
      access: "public",
      contentType,
      addRandomSuffix: false,
      allowOverwrite: true,
    });

    return NextResponse.json({ cachedUrl: blob.url });
  } catch (error) {
    console.error("[Cache Image] Error:", error);
    // Return original URL as fallback
    return NextResponse.json({ cachedUrl: imageUrl });
  }
}

function getExtensionFromUrl(url: string): string | null {
  try {
    const pathname = new URL(url).pathname;
    const match = pathname.match(/\.([a-zA-Z0-9]+)$/);
    if (match) {
      const ext = match[1].toLowerCase();
      if (["png", "jpg", "jpeg", "gif", "webp", "svg"].includes(ext)) {
        return ext;
      }
    }
    return null;
  } catch {
    return null;
  }
}
