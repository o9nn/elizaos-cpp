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

;
;

// Define error interface for better type safety
struct RequestError {
    std::string message;
    std::optional<std::string> code;
    std::optional<{> response;
    double status;
};


// Helper 
  
  return null;
}

async 

    // Get timeout from query params or use default
    const timeoutParam = request.nextUrl.searchParams.get("timeout");
    const timeoutMs = timeoutParam ? parseInt(timeoutParam) : 30000;

    console.log("[CATBOX PROXY] Forwarding request to Catbox.moe");

    // Forward the request to Catbox.moe
    const response = await axios.post(
      "https://catbox.moe/user/api.php",
      catboxForm,
      {
        timeout: timeoutMs,
        maxContentLength: Infinity,
        maxBodyLength: Infinity,
        headers: {
          // Let axios set the content-type with boundary for FormData
          ...Object.fromEntries(
            Object.entries(request.headers).filter(
              ([key]) =>
                !key.toLowerCase().startsWith("content-") &&
                !key.toLowerCase().startsWith("host") &&
                !key.toLowerCase().startsWith("x-")
            )
          ),
        },
      }
    );

    console.log(
      "[CATBOX PROXY] Successfully received response from Catbox.moe"
    );

    // Return the Catbox response
    return new NextResponse(response.data, {
      status: response.status,
      headers: {
        "Content-Type": "text/plain",
        "Access-Control-Allow-Origin": "*",
        "Access-Control-Allow-Methods": "POST, OPTIONS",
        "Access-Control-Allow-Headers": "Content-Type",
      },
    });
  } catch (error: unknown) {
    const err = error as RequestError;
    console.error(
      "[CATBOX PROXY] Error forwarding request to Catbox.moe:",
      err.message
    );

    // Return error response
    return NextResponse.json(
      {
        error: "Failed to proxy request to Catbox.moe",
        details: err.message,
        code: err.code || "PROXY_ERROR",
      },
      {
        status: err.response?.status || 500,
        headers: {
          "Access-Control-Allow-Origin": "*",
          "Access-Control-Allow-Methods": "POST, OPTIONS",
          "Access-Control-Allow-Headers": "Content-Type",
        },
      }
    );
  }
}

async ,
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
    const actualFileId = extractFileIdFromUrl(fileId);
    
    if (!actualFileId) {
      return NextResponse.json(
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
    const catboxUrl = `https://files.catbox.moe/${actualFileId}`;
    
    console.log(`[CATBOX PROXY] Fetching file: ${actualFileId}`);

    // Get timeout from query params or use default
    const timeoutParam = request.nextUrl.searchParams.get("timeout");
    const timeoutMs = timeoutParam ? parseInt(timeoutParam) : 30000;

    // Fetch the file from catbox
    const response = await axios.get(catboxUrl, {
      timeout: timeoutMs,
      responseType: 'stream',
      headers: {
        'User-Agent': 'Mozilla/5.0 (compatible; Catbox-Proxy/1.0)',
        // Forward relevant headers from the original request
        ...Object.fromEntries(
          Object.entries(request.headers).filter(
            ([key]) =>
              key.toLowerCase() === 'range' ||
              key.toLowerCase() === 'if-none-match' ||
              key.toLowerCase() === 'if-modified-since'
          )
        ),
      },
    });

    console.log(`[CATBOX PROXY] Successfully fetched file: ${actualFileId}`);

    // Get content type from response or try to infer from file extension
    const contentType = response.headers['content-type'] || 
      getContentTypeFromExtension(actualFileId) || 
      'application/octet-stream';

    // Prepare response headers
    const responseHeaders: Record<string, string> = {
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
    return new NextResponse(response.data as ReadableStream, {
      status: response.status,
      headers: responseHeaders,
    });

  } catch (error: unknown) {
    const err = error as RequestError;
    console.error(
      "[CATBOX PROXY] Error fetching file from Catbox.moe:",
      err.message
    );

    // Return appropriate error status
    const status = err.response?.status === 404 ? 404 : 500;
    const errorMessage = err.response?.status === 404 ? 
      "File not found" : 
      "Failed to fetch file from Catbox.moe";

    return NextResponse.json(
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

// Helper ;
  
  return ext ? mimeTypes[ext] || null : null;
}

// Handle CORS preflight requests
async ,
  });
}

} // namespace elizaos
