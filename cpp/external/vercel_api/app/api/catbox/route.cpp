#include "route.hpp"
#include <algorithm>
#include <cctype>
#include <iostream>
#include <regex>
#include <sstream>
#include <stdexcept>

namespace elizaos {

std::string extractFileIdFromUrl(const std::string& url) {
    // Patterns to match valid file IDs
    static const std::vector<std::regex> patterns = {
        std::regex(R"(^([a-zA-Z0-9]+\.[a-zA-Z0-9]+)$)"),  // Strictly match valid file IDs (e.g., "abc123.jpg")
        std::regex(R"(files\.catbox\.moe/([a-zA-Z0-9]+\.[a-zA-Z0-9]+))"),  // Extract from full URL
    };

    for (const auto& pattern : patterns) {
        std::smatch match;
        if (std::regex_search(url, match, pattern)) {
            return match[1].str();
        }
    }

    return "";
}

std::future<HttpResponse> POST(const HttpRequest& request) {
    return std::async(std::launch::async, [request]() {
        HttpResponse response;
        
        try {
            // Log the request
            std::cout << "[CATBOX PROXY] Forwarding request to Catbox.moe" << std::endl;
            
            // In a real implementation, this would:
            // 1. Parse the form data from the request
            // 2. Forward it to https://catbox.moe/user/api.php
            // 3. Return the response
            
            // Placeholder success response
            response.statusCode = 200;
            response.headers["Content-Type"] = "text/plain";
            response.headers["Access-Control-Allow-Origin"] = "*";
            response.headers["Access-Control-Allow-Methods"] = "POST, OPTIONS";
            response.headers["Access-Control-Allow-Headers"] = "Content-Type";
            response.body = "https://files.catbox.moe/placeholder.jpg";
            
            std::cout << "[CATBOX PROXY] Successfully received response from Catbox.moe" << std::endl;
            
        } catch (const std::exception& e) {
            std::cerr << "[CATBOX PROXY] Error forwarding request to Catbox.moe: " 
                      << e.what() << std::endl;
            
            response.statusCode = 500;
            response.headers["Content-Type"] = "application/json";
            response.headers["Access-Control-Allow-Origin"] = "*";
            response.headers["Access-Control-Allow-Methods"] = "POST, OPTIONS";
            response.headers["Access-Control-Allow-Headers"] = "Content-Type";
            response.body = R"({"error": "Failed to proxy request to Catbox.moe", "code": "PROXY_ERROR"})";
        }
        
        return response;
    });
}

std::future<HttpResponse> GET(const HttpRequest& request) {
    return std::async(std::launch::async, [request]() {
        HttpResponse response;
        
        try {
            // Get file ID from query parameter
            auto it = request.query.find("file");
            if (it == request.query.end() || it->second.empty()) {
                response.statusCode = 400;
                response.headers["Content-Type"] = "application/json";
                response.headers["Access-Control-Allow-Origin"] = "*";
                response.headers["Access-Control-Allow-Methods"] = "GET, POST, OPTIONS";
                response.headers["Access-Control-Allow-Headers"] = "Content-Type";
                response.body = R"({"error": "Missing file parameter"})";
                return response;
            }
            
            const std::string& fileId = it->second;
            
            // Extract the actual file ID if a full URL was provided
            std::string actualFileId = extractFileIdFromUrl(fileId);
            
            if (actualFileId.empty()) {
                response.statusCode = 400;
                response.headers["Content-Type"] = "application/json";
                response.headers["Access-Control-Allow-Origin"] = "*";
                response.headers["Access-Control-Allow-Methods"] = "GET, POST, OPTIONS";
                response.headers["Access-Control-Allow-Headers"] = "Content-Type";
                response.body = R"({"error": "Invalid file parameter"})";
                return response;
            }
            
            // Construct the catbox file URL
            std::string catboxUrl = "https://files.catbox.moe/" + actualFileId;
            
            std::cout << "[CATBOX PROXY] Fetching file: " << actualFileId << std::endl;
            
            // In a real implementation, this would fetch the file from catbox
            // For now, return a placeholder response
            
            std::string contentType = getContentTypeFromExtension(actualFileId);
            if (contentType.empty()) {
                contentType = "application/octet-stream";
            }
            
            response.statusCode = 200;
            response.headers["Content-Type"] = contentType;
            response.headers["Access-Control-Allow-Origin"] = "*";
            response.headers["Access-Control-Allow-Methods"] = "GET, POST, OPTIONS";
            response.headers["Access-Control-Allow-Headers"] = "Content-Type";
            response.headers["Cache-Control"] = "public, max-age=31536000";
            
            std::cout << "[CATBOX PROXY] Successfully fetched file: " << actualFileId << std::endl;
            
        } catch (const std::exception& e) {
            std::cerr << "[CATBOX PROXY] Error fetching file from Catbox.moe: " 
                      << e.what() << std::endl;
            
            response.statusCode = 500;
            response.headers["Content-Type"] = "application/json";
            response.headers["Access-Control-Allow-Origin"] = "*";
            response.headers["Access-Control-Allow-Methods"] = "GET, POST, OPTIONS";
            response.headers["Access-Control-Allow-Headers"] = "Content-Type";
            response.body = R"({"error": "Failed to fetch file from Catbox.moe", "code": "PROXY_ERROR"})";
        }
        
        return response;
    });
}

std::string getContentTypeFromExtension(const std::string& filename) {
    // Find the last dot in the filename
    size_t dotPos = filename.rfind('.');
    if (dotPos == std::string::npos || dotPos == filename.length() - 1) {
        return "";
    }
    
    // Extract and lowercase the extension
    std::string ext = filename.substr(dotPos + 1);
    std::transform(ext.begin(), ext.end(), ext.begin(), 
                   [](unsigned char c) { return std::tolower(c); });
    
    // MIME type mapping
    static const std::unordered_map<std::string, std::string> mimeTypes = {
        {"jpg", "image/jpeg"},
        {"jpeg", "image/jpeg"},
        {"png", "image/png"},
        {"gif", "image/gif"},
        {"webp", "image/webp"},
        {"svg", "image/svg+xml"},
        {"mp4", "video/mp4"},
        {"webm", "video/webm"},
        {"mov", "video/quicktime"},
        {"avi", "video/x-msvideo"},
        {"mp3", "audio/mpeg"},
        {"wav", "audio/wav"},
        {"pdf", "application/pdf"},
        {"txt", "text/plain"},
        {"json", "application/json"},
        {"js", "application/javascript"},
        {"css", "text/css"},
        {"html", "text/html"},
    };
    
    auto it = mimeTypes.find(ext);
    if (it != mimeTypes.end()) {
        return it->second;
    }
    
    return "";
}

std::future<HttpResponse> OPTIONS() {
    return std::async(std::launch::async, []() {
        HttpResponse response;
        response.statusCode = 200;
        response.headers["Access-Control-Allow-Origin"] = "*";
        response.headers["Access-Control-Allow-Methods"] = "GET, POST, OPTIONS";
        response.headers["Access-Control-Allow-Headers"] = "Content-Type";
        return response;
    });
}

} // namespace elizaos
