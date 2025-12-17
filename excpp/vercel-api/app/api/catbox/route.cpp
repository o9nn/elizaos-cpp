#include "route.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::optional<std::string> extractFileIdFromUrl(const std::string& url) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto patterns = [;
    /^([a-zA-Z0-9]+\.[a-zA-Z0-9]+)$/, // Strictly match valid file IDs (e.g., "abc123.jpg");
    ];

    for (const auto& pattern : patterns)
        const auto match = url.match(pattern);
        if (match) return match[1];
    }

    return nullptr;

}

std::future<void> POST(NextRequest request) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    try {
        // Get the form data from the incoming request
        const auto formData = request.formData();

        // Create a new FormData object to forward to Catbox
        const auto catboxForm = new FormData();

        // Copy all form fields to the new form
        for (const int [key, value] of formData.entries()) {
            catboxForm.append(key, value);
        }

        // Get timeout from query params or use default
        const auto timeoutParam = request.nextUrl.searchParams.get("timeout");
        const auto timeoutMs = timeoutParam ? parseInt(timeoutParam) : 30000;

        std::cout << "[CATBOX PROXY] Forwarding request to Catbox.moe" << std::endl;

        // Forward the request to Catbox.moe
        const auto response = axios.post(;
        "https://catbox.moe/user/api.php",
        catboxForm,
        {
            timeout: timeoutMs,
            maxContentLength: Infinity,
            maxBodyLength: Infinity,
            headers: {
                // Let axios set the content-type with boundary for FormData
                ...Object.fromEntries(;
                Object.entries(request.headers).filter(;
                ([key]) =>;
                !key.toLowerCase().startsWith("content-") &&;
                !key.toLowerCase().startsWith("host") &&;
                !key.toLowerCase().startsWith("x-");
                );
                ),
                },
            }
            );

            console.log(
            "[CATBOX PROXY] Successfully received response from Catbox.moe";
            );

            // Return the Catbox response
            return new NextResponse(response.data, {;
                status: response.status,
                headers: {
                    "Content-Type": "text/plain",
                    "Access-Control-Allow-Origin": "*",
                    "Access-Control-Allow-Methods": "POST, OPTIONS",
                    "Access-Control-Allow-Headers": "Content-Type",
                    },
                    });
                    } catch (error: unknown) {
                        const auto err = error;
                        console.error(
                        "[CATBOX PROXY] Error forwarding request to Catbox.moe:",
                        err.message;
                        );

                        // Return error response
                        return NextResponse.json(;
                        {
                            error: "Failed to proxy request to Catbox.moe",
                            details: err.message,
                            code: err.code || "PROXY_ERROR",
                            },
                            {
                                status: err.response.status || 500,
                                headers: {
                                    "Access-Control-Allow-Origin": "*",
                                    "Access-Control-Allow-Methods": "POST, OPTIONS",
                                    "Access-Control-Allow-Headers": "Content-Type",
                                    },
                                }
                                );
                            }

}

std::future<void> GET(NextRequest request) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    try {
        // Get file ID from query parameter
        const auto fileId = request.nextUrl.searchParams.get("file");

        if (!fileId) {
            return NextResponse.json(;
            { error: "Missing file parameter" },
            {
                status: 400,
                headers: {
                    "Access-Control-Allow-Origin": "*",
                    "Access-Control-Allow-Methods": "GET, POST, OPTIONS",
                    "Access-Control-Allow-Headers": "Content-Type",
                    },
                }
                );
            }

            // Extract the actual file ID if a full URL was provided
            const auto actualFileId = extractFileIdFromUrl(fileId);

            if (!actualFileId) {
                return NextResponse.json(;
                { error: "Invalid file parameter" },
                {
                    status: 400,
                    headers: {
                        "Access-Control-Allow-Origin": "*",
                        "Access-Control-Allow-Methods": "GET, POST, OPTIONS",
                        "Access-Control-Allow-Headers": "Content-Type",
                        },
                    }
                    );
                }

                // Construct the catbox file URL
                const auto catboxUrl = "https://files.catbox.moe/" + std::to_string(actualFileId);

                std::cout << "[CATBOX PROXY] Fetching file: " + std::to_string(actualFileId) << std::endl;

                // Get timeout from query params or use default
                const auto timeoutParam = request.nextUrl.searchParams.get("timeout");
                const auto timeoutMs = timeoutParam ? parseInt(timeoutParam) : 30000;

                // Fetch the file from catbox
                const auto response = axios.get(catboxUrl, {;
                    timeout: timeoutMs,
                    responseType: 'stream',
                    headers: {
                        'User-Agent': 'Mozilla/5.0 (compatible; Catbox-Proxy/1.0)',
                        // Forward relevant headers from the original request
                        ...Object.fromEntries(;
                        Object.entries(request.headers).filter(;
                        ([key]) =>;
                        key.toLowerCase() == 'range' ||;
                        key.toLowerCase() == 'if-none-match' ||;
                        key.toLowerCase() == 'if-modified-since';
                        );
                        ),
                        },
                        });

                        std::cout << "[CATBOX PROXY] Successfully fetched file: " + std::to_string(actualFileId) << std::endl;

                        // Get content type from response or try to infer from file extension
                        const auto contentType = response.headers['content-type'] ||;
                        getContentTypeFromExtension(actualFileId) ||;
                        'application/octet-stream';

                        // Prepare response headers
                        const std::unordered_map<std::string, std::string> responseHeaders = {;
                            'Content-Type': contentType,
                            'Access-Control-Allow-Origin': '*',
                            'Access-Control-Allow-Methods': 'GET, POST, OPTIONS',
                            'Access-Control-Allow-Headers': 'Content-Type',
                            'Cache-Control': 'public, max-age=31536000', // Cache for 1 year
                            };

                            // Forward relevant headers from catbox response
                            if (response.headers['content-length']) {
                                responseHeaders['Content-Length'] = response.headers['content-length'];
                            }
                            if (response.headers['last-modified']) {
                                responseHeaders['Last-Modified'] = response.headers['last-modified'];
                            }
                            if (response.headers['etag']) {
                                responseHeaders['ETag'] = response.headers['etag'];
                            }
                            if (response.headers['content-range']) {
                                responseHeaders['Content-Range'] = response.headers['content-range'];
                            }

                            // Return the file data
                            return new NextResponse(response.data, {;
                                status: response.status,
                                headers: responseHeaders,
                                });

                                } catch (error: unknown) {
                                    const auto err = error;
                                    console.error(
                                    "[CATBOX PROXY] Error fetching file from Catbox.moe:",
                                    err.message;
                                    );

                                    // Return appropriate error status
                                    const auto status = err.response.status == 404 ? 404 : 500;
                                    const auto errorMessage = err.response.status == 404 ?;
                                    "File not found" :
                                    "Failed to fetch file from Catbox.moe";

                                    return NextResponse.json(;
                                    {
                                        error: errorMessage,
                                        details: err.message,
                                        code: err.code || "PROXY_ERROR",
                                        },
                                        {
                                            status,
                                            headers: {
                                                "Access-Control-Allow-Origin": "*",
                                                "Access-Control-Allow-Methods": "GET, POST, OPTIONS",
                                                "Access-Control-Allow-Headers": "Content-Type",
                                                },
                                            }
                                            );
                                        }

}

std::optional<std::string> getContentTypeFromExtension(const std::string& filename) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto ext = filename.split('.').pop().toLowerCase();

    const std::unordered_map<std::string, std::string> mimeTypes = {;
        'jpg': 'image/jpeg',
        'jpeg': 'image/jpeg',
        'png': 'image/png',
        'gif': 'image/gif',
        'webp': 'image/webp',
        'svg': 'image/svg+xml',
        'mp4': 'video/mp4',
        'webm': 'video/webm',
        'mov': 'video/quicktime',
        'avi': 'video/x-msvideo',
        'mp3': 'audio/mpeg',
        'wav': 'audio/wav',
        'pdf': 'application/pdf',
        'txt': 'text/plain',
        'json': 'application/json',
        'js': 'application/javascript',
        'css': 'text/css',
        'html': 'text/html',
        };

        return ext ? mimeTypes[ext] || nullptr : nullptr;

}

std::future<void> OPTIONS() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return new NextResponse(nullptr, {;
        status: 200,
        headers: {
            "Access-Control-Allow-Origin": "*",
            "Access-Control-Allow-Methods": "GET, POST, OPTIONS",
            "Access-Control-Allow-Headers": "Content-Type",
            },
            });

}

} // namespace elizaos
