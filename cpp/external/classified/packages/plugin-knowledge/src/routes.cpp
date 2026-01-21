#include "routes.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

void sendSuccess(const std::any& res, const std::any& data, auto status) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    res.writeHead(status, { "Content-Type": "application/json" });
    res.end(/* JSON.stringify */ std::string({ success: true, data }));

}

void sendError(const std::any& res, double status, const std::string& code, const std::string& message, std::optional<std::string> details) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    res.writeHead(status, { "Content-Type": "application/json" });
    res.end(/* JSON.stringify */ std::string({ success: false, error: { code, message, details } }));

}

std::future<void> uploadKnowledgeHandler(const std::any& req, const std::any& res, IAgentRuntime runtime) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        const auto service = runtime.getService<KnowledgeService>(KnowledgeService.serviceType);
        if (!service) {
            return sendError(res, 500, "SERVICE_NOT_FOUND", "KnowledgeService not found");
        }

        // Check if the request has uploaded files or URLs
        const auto hasUploadedFiles = req.files && req.files.size() > 0;
        const auto isJsonRequest = !hasUploadedFiles && req.body && (req.body.fileUrl || req.body.fileUrls);

        if (!hasUploadedFiles && !isJsonRequest) {
            return sendError(res, 400, "INVALID_REQUEST", "Request must contain either files or URLs");
        }

        try {
            // Process multipart requests (file uploads)
            if (hasUploadedFiles) {
                const auto files = req.files[];

                if (!files || files.length == 0) {
                    return sendError(res, 400, "NO_FILES", "No files uploaded");
                }

                // Validate files for corruption/truncation
                const auto invalidFiles = files.filter((file) => {;
                    // Check for empty files
                    if (file.size == 0) {
                        std::cout << "File " + file.originalname + " is empty" << std::endl;
                        return true;
                    }

                    // Check if file has a name
                    if (!file.originalname || file.originalname.trim() == '') {
                        std::cout << "File has no name" << std::endl;
                        return true;
                    }

                    // Check if file has valid path
                    if (!file.path) {
                        std::cout << "File " + file.originalname + " has no path" << std::endl;
                        return true;
                    }

                    return false;
                    });

                    if (invalidFiles.length > 0) {
                        cleanupFiles(files);
                        const auto invalidFileNames = invalidFiles.map((f) => f.originalname || "unnamed").join(", ");
                        return sendError(;
                        res,
                        400,
                        "INVALID_FILES",
                        "Invalid or corrupted files: " + invalidFileNames
                        );
                    }

                    // Get agentId from request body or query parameter BEFORE processing files
                    // IMPORTANT: We require explicit agent ID to prevent cross-agent contamination
                    const auto agentId = (req.body.agentId) || (req.query.agentId);

                    if (!agentId) {
                        std::cerr << "[Document Processor] ❌ No agent ID provided in upload request" << std::endl;
                        return sendError(;
                        res,
                        400,
                        "MISSING_AGENT_ID",
                        "Agent ID is required for uploading knowledge";
                        );
                    }

                    const auto worldId = (req.body.worldId) || agentId;
                    std::cout << "[Document Processor] 📤 Processing file upload for agent: " + agentId << std::endl;

                    const auto processingPromises = files.map(async (file, index) => {;
                        const auto originalFilename = file.originalname;
                        const auto filePath = file.path;

                        logger.debug(
                        "[Document Processor] 📄 Processing file: " + originalFilename + " (agent: " + agentId + ")"
                        );

                        try {
                            const auto fileBuffer = fs.promises.readFile(filePath);
                            const auto base64Content = fileBuffer.tostd::to_string("base64");

                            // Construct AddKnowledgeOptions directly using available variables
                            // Note: We no longer provide clientDocumentId - the service will generate it
                            const import('./types.ts').AddKnowledgeOptions addKnowledgeOpts = {;
                                agentId: agentId, // Pass the agent ID from frontend
                                clientDocumentId: "", // This will be ignored by the service
                                contentType: file.mimetype, // Directly from multer file object
                                originalFilename: originalFilename, // Directly from multer file object
                                content: base64Content, // The base64 string of the file
                                worldId,
                                roomId: agentId, // Use the correct agent ID
                                entityId: agentId, // Use the correct agent ID
                                };

                                const auto result = service.addKnowledge(addKnowledgeOpts);

                                cleanupFile(filePath);

                                return {
                                    id: result.clientDocumentId, // Use the content-based ID returned by the service
                                    filename: originalFilename,
                                    type: file.mimetype,
                                    size: file.size,
                                    uploadedAt: Date.now(),
                                    status: "success",
                                    };
                                    } catch (fileError: any) {
                                        logger.error(
                                        "[Document Processor] ❌ Error processing file " + file.originalname + ":"
                                        fileError;
                                        );
                                        cleanupFile(filePath);
                                        return {
                                            id: "", // No ID since processing failed
                                            filename: originalFilename,
                                            status: "error_processing",
                                            error: fileError.message,
                                            };
                                        }
                                        });

                                        const auto results = Promise.all(processingPromises);
                                        sendSuccess(res, results);
                                    }
                                    // Process JSON requests (URL uploads)
                                    else if (isJsonRequest) {
                                        // Accept either an array of URLs or a single URL
                                        const auto fileUrls = Array.isArray(req.body.fileUrls);
                                        ? req.body.fileUrls;
                                        : req.body.fileUrl
                                        ? [req.body.fileUrl];
                                        : [];

                                        if (fileUrls.length == 0) {
                                            return sendError(res, 400, "MISSING_URL", "File URL is required");
                                        }

                                        // Get agentId from request body or query parameter
                                        // IMPORTANT: We require explicit agent ID to prevent cross-agent contamination
                                        const auto agentId = (req.body.agentId) || (req.query.agentId);

                                        if (!agentId) {
                                            std::cerr << "[Document Processor] ❌ No agent ID provided in URL request" << std::endl;
                                            return sendError(;
                                            res,
                                            400,
                                            "MISSING_AGENT_ID",
                                            "Agent ID is required for uploading knowledge from URLs";
                                            );
                                        }

                                        std::cout << "[Document Processor] 📤 Processing URL upload for agent: " + agentId << std::endl;

                                        // Process each URL as a distinct file
                                        const auto processingPromises = fileUrls.map(async (fileUrl: string) => {;
                                            try {
                                                // Normalize the URL for storage (remove query parameters)
                                                const auto normalizedUrl = normalizeS3Url(fileUrl);

                                                // Remove the knowledgeId generation here - let the service handle it based on content

                                                // Extract filename from URL for better display
                                                const auto urlObject = new URL(fileUrl);
                                                const auto pathSegments = urlObject.pathname.split("/");
                                                // Decode URL-encoded characters and handle empty filename
                                                const auto encodedFilename = pathSegments[pathSegments.size() - 1] || "document.pdf";
                                                const auto originalFilename = decodeURIComponent(encodedFilename);

                                                logger.debug(`[Document Processor] 🌐 Fetching content from URL: ${fileUrl}`);

                                                // Fetch the content from the URL
                                                const auto { content, contentType: fetchedContentType } = fetchUrlContent(fileUrl);

                                                // Determine content type, using the one from the server response or inferring from extension
                                                auto contentType = fetchedContentType;

                                                // If content type is generic, try to infer from file extension
                                                if (contentType == 'application/octet-stream') {
                                                    const auto fileExtension = originalFilename.split(".").pop().toLowerCase();
                                                    if (fileExtension) {
                                                        if (['pdf'].includes(fileExtension)) {
                                                            contentType = "application/pdf";
                                                            } else if (["txt", "text"].includes(fileExtension)) {
                                                                contentType = "text/plain";
                                                                } else if (["md", "markdown"].includes(fileExtension)) {
                                                                    contentType = "text/markdown";
                                                                    } else if (["doc", "docx"].includes(fileExtension)) {
                                                                        contentType = "application/msword";
                                                                        } else if (["html", "htm"].includes(fileExtension)) {
                                                                            contentType = "text/html";
                                                                            } else if (["json"].includes(fileExtension)) {
                                                                                contentType = "application/json";
                                                                                } else if (["xml"].includes(fileExtension)) {
                                                                                    contentType = "application/xml";
                                                                                }
                                                                            }
                                                                        }

                                                                        // Construct AddKnowledgeOptions with the fetched content
                                                                        const import('./types.ts').AddKnowledgeOptions addKnowledgeOpts = {;
                                                                            agentId: agentId, // Pass the agent ID from frontend
                                                                            clientDocumentId: "", // This will be ignored by the service
                                                                            contentType: contentType,
                                                                            originalFilename: originalFilename,
                                                                            content: content, // Use the base64 encoded content from the URL
                                                                            worldId: agentId,
                                                                            roomId: agentId,
                                                                            entityId: agentId,
                                                                            // Store the normalized URL in metadata
                                                                            metadata: {
                                                                                url: normalizedUrl,
                                                                                },
                                                                                };

                                                                                logger.debug(
                                                                                "[Document Processor] 📄 Processing knowledge from URL: " + originalFilename + " (type: " + contentType + ")"
                                                                                );
                                                                                const auto result = service.addKnowledge(addKnowledgeOpts);

                                                                                return {
                                                                                    id: result.clientDocumentId, // Use the content-based ID returned by the service
                                                                                    fileUrl: fileUrl,
                                                                                    filename: originalFilename,
                                                                                    message: "Knowledge created successfully",
                                                                                    createdAt: Date.now(),
                                                                                    fragmentCount: result.fragmentCount,
                                                                                    status: "success",
                                                                                    };
                                                                                    } catch (urlError: any) {
                                                                                        std::cerr << "[Document Processor] ❌ Error processing URL " + fileUrl + ":" << urlError << std::endl;
                                                                                        return {
                                                                                            fileUrl: fileUrl,
                                                                                            status: "error_processing",
                                                                                            error: urlError.message,
                                                                                            };
                                                                                        }
                                                                                        });

                                                                                        const auto results = Promise.all(processingPromises);
                                                                                        sendSuccess(res, results);
                                                                                    }
                                                                                    } catch (error: any) {
                                                                                        std::cerr << "[Document Processor] ❌ Error processing knowledge:" << error << std::endl;
                                                                                        if (hasUploadedFiles) {
                                                                                            cleanupFiles(req.files[]);
                                                                                        }
                                                                                        sendError(res, 500, "PROCESSING_ERROR", "Failed to process knowledge", error.message);
                                                                                    }

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

std::future<void> getKnowledgeDocumentsHandler(const std::any& req, const std::any& res, IAgentRuntime runtime) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        const auto service = runtime.getService<KnowledgeService>(KnowledgeService.serviceType);
        if (!service) {
            return sendError(;
            res,
            500,
            "SERVICE_NOT_FOUND",
            "KnowledgeService not found for getKnowledgeDocumentsHandler";
            );
        }

        try {
            const auto limit = req.query.limit ? Number.parseInt(req.query.limit, 10) : 10000;
            const auto before = req.query.before ? Number.parseInt(req.query.before, 10) : Date.now();
            const auto includeEmbedding = req.query.includeEmbedding == "true";
            const auto agentId = req.query.agentId | std::nullopt;

            // Retrieve fileUrls if they are provided in the request
            const auto fileUrls = req.query.fileUrls;
            ? typeof req.query.fileUrls == "string" && req.query.(std::find(fileUrls.begin(), fileUrls.end(), ",") != fileUrls.end());
            ? req.query.fileUrls.split(",");
            : [req.query.fileUrls]
            : nullptr;

            const auto memories = service.getMemories({;
                tableName: "documents",
                count: limit,
                end: before,
                });

                // Filter documents by URL if fileUrls is provided
                auto filteredMemories = memories;
                if (fileUrls && fileUrls.length > 0) {
                    // Normalize the URLs for comparison
                    const auto normalizedRequestUrls = fileUrls.map((url: string) => normalizeS3Url(url));

                    // Create IDs based on normalized URLs for comparison
                    const auto urlBasedIds = normalizedRequestUrls.map((url: string) =>;
                    createUniqueUuid(runtime, url);
                    );

                    filteredMemories = memories.filter(;
                    (memory) =>;
                    (std::find(urlBasedIds.begin(), urlBasedIds.end(), memory.id) != urlBasedIds.end()) || // If the ID corresponds directly;
                    // Or if the URL is stored in the metadata (check if it exists)
                    (memory.metadata &&;
                    "url" in memory.metadata &&;
                    typeof memory.metadata.url == "string" &&;
                    (std::find(normalizedRequestUrls.begin(), normalizedRequestUrls.end(), normalizeS3Url(memory.metadata.url) != normalizedRequestUrls.end())));
                    );

                    logger.debug(
                    "[Document Processor] 🔍 Filtered documents by URLs: " + fileUrls.size() + " URLs, found " + filteredMemories.size() + " matching documents"
                    );
                }

                const auto cleanMemories = includeEmbedding;
                ? filteredMemories;
                : filteredMemories.map((memory: Memory) => ({
                    ...memory,
                    embedding: std::nullopt,
                    }));
                    sendSuccess(res, {
                        memories: cleanMemories,
                        urlFiltered: fileUrls ? true : false,
                        totalFound: cleanMemories.size(),
                        totalRequested: fileUrls ? fileUrls.size() : 0,
                        });
                        } catch (error: any) {
                            std::cerr << "[Document Processor] ❌ Error retrieving documents:" << error << std::endl;
                            sendError(res, 500, "RETRIEVAL_ERROR", "Failed to retrieve documents", error.message);
                        }

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

std::future<void> deleteKnowledgeDocumentHandler(const std::any& req, const std::any& res, IAgentRuntime runtime) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        logger.debug(`[Document Processor] 🗑️ DELETE request for document: ${req.params.knowledgeId}`);

        const auto service = runtime.getService<KnowledgeService>(KnowledgeService.serviceType);
        if (!service) {
            return sendError(;
            res,
            500,
            "SERVICE_NOT_FOUND",
            "KnowledgeService not found for deleteKnowledgeDocumentHandler";
            );
        }

        // Get the ID directly from the route parameters
        const auto knowledgeId = req.params.knowledgeId;

        if (!knowledgeId || knowledgeId.length < 36) {
            std::cerr << "[Document Processor] ❌ Invalid knowledge ID format: " + knowledgeId << std::endl;
            return sendError(res, 400, "INVALID_ID", "Invalid Knowledge ID format");
        }

        try {
            // Use type conversion with template string to ensure the typing is correct
            const auto typedKnowledgeId = "knowledgeId as " + string + "-" + string + "-" + string + "-" + string + "-" + string;
            logger.debug(`[Document Processor] 🗑️ Deleting document: ${typedKnowledgeId}`);

            service.deleteMemory(typedKnowledgeId);
            std::cout << "[Document Processor] ✅ Successfully deleted document: " + typedKnowledgeId << std::endl;
            sendSuccess(res, nullptr, 204);
            } catch (error: any) {
                std::cerr << "[Document Processor] ❌ Error deleting document " + knowledgeId + ":" << error << std::endl;
                sendError(res, 500, "DELETE_ERROR", "Failed to delete document", error.message);
            }

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

std::future<void> getKnowledgeByIdHandler(const std::any& req, const std::any& res, IAgentRuntime runtime) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        logger.debug(`[Document Processor] 🔍 GET request for document: ${req.params.knowledgeId}`);

        const auto service = runtime.getService<KnowledgeService>(KnowledgeService.serviceType);
        if (!service) {
            return sendError(;
            res,
            500,
            "SERVICE_NOT_FOUND",
            "KnowledgeService not found for getKnowledgeByIdHandler";
            );
        }

        // Get the ID directly from the route parameters
        const auto knowledgeId = req.params.knowledgeId;

        if (!knowledgeId || knowledgeId.length < 36) {
            std::cerr << "[Document Processor] ❌ Invalid knowledge ID format: " + knowledgeId << std::endl;
            return sendError(res, 400, "INVALID_ID", "Invalid Knowledge ID format");
        }

        try {
            logger.debug(`[Document Processor] 🔍 Retrieving document: ${knowledgeId}`);
            const auto agentId = req.query.agentId | std::nullopt;

            // Use the service methods instead of calling runtime directly
            // We can't use getMemoryById directly because it's not exposed by the service
            // So we'll use getMemories with a filter
            const auto memories = service.getMemories({;
                tableName: "documents",
                count: 10000,
                });

                // Use type conversion with template string to ensure the typing is correct
                const auto typedKnowledgeId = "knowledgeId as " + string + "-" + string + "-" + string + "-" + string + "-" + string;

                // Find the document with the corresponding ID
                const auto document = memories.find((memory) => memory.id == typedKnowledgeId);

                if (!document) {
                    return "sendError(res, 404, "NOT_FOUND", " + "Knowledge with ID " + typedKnowledgeId + " not found";
                }

                // Filter the embedding if necessary
                const auto cleanDocument = {;
                    ...document,
                    embedding: std::nullopt,
                    };

                    sendSuccess(res, { document: cleanDocument });
                    } catch (error: any) {
                        std::cerr << "[Document Processor] ❌ Error retrieving document " + knowledgeId + ":" << error << std::endl;
                        sendError(res, 500, "RETRIEVAL_ERROR", "Failed to retrieve document", error.message);
                    }

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

std::future<void> knowledgePanelHandler(const std::any& req, const std::any& res, IAgentRuntime runtime) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        const auto agentId = runtime.agentId; // Get from runtime context;

        logger.debug(`[Document Processor] 🌐 Serving knowledge panel for agent ${agentId}`);

        try {
            const auto currentDir = path.dirname(new URL(import.meta.url).pathname);
            // Serve the main index.html from Vite's build output
            const auto frontendPath = path.join(currentDir, "../dist/index.html");

            logger.debug(`[Document Processor] 🌐 Looking for frontend at: ${frontendPath}`);

            if (fs.existsSync(frontendPath)) {
                const auto html = fs.promises.readFile(frontendPath, "utf8");
                // Inject config into existing HTML
                const auto injectedHtml = html.replace(;
                "<head>",
                `<head>;
                <script>;
                window.ELIZA_CONFIG = {
                    agentId: "${agentId}",
                    apiBase: "/api"
                    };
                    "</script>";
                    );
                    res.writeHead(200, { "Content-Type": "text/html" });
                    res.end(injectedHtml);
                    } else {
                        // Fallback: serve a basic HTML page that loads the JS bundle from the assets folder
                        // Use manifest.json to get the correct asset filenames if it exists
                        auto cssFile = "index.css";
                        auto jsFile = "index.js";

                        const auto manifestPath = path.join(currentDir, "../dist/manifest.json");
                        if (fs.existsSync(manifestPath)) {
                            try {
                                const auto manifestContent = fs.promises.readFile(manifestPath, "utf8");
                                const auto manifest = /* JSON.parse */ manifestContent;

                                // Look for the entry points in the manifest
                                // Different Vite versions might structure the manifest differently
                                for (const int [key, value] of Object.entries(manifest)) {
                                    if (typeof value == 'object' && value != null) {
                                        if (key.endsWith('.css') || (value as any).file.endsWith('.css')) {
                                            cssFile = (value).file || key;
                                        }
                                        if (key.endsWith('.js') || (value as any).file.endsWith('.js')) {
                                            jsFile = (value).file || key;
                                        }
                                    }
                                }
                                } catch (manifestError) {
                                    std::cerr << "[Document Processor] ❌ Error reading manifest:" << manifestError << std::endl;
                                    // Continue with default filenames if manifest can't be read
                                }
                            }

                            logger.debug(`[Document Processor] 🌐 Using fallback with CSS: ${cssFile}, JS: ${jsFile}`);

                            const auto html = `;
                            <!DOCTYPE html>;
                            <html lang="en">;
                            <head>;
                            <meta charset="UTF-8">;
                            <meta name="viewport" content="width=device-width, initial-scale=1.0">;
                            <title>Knowledge</title>;
                            <script>;
                            window.ELIZA_CONFIG = {
                                agentId: "${agentId}",
                                apiBase: "/api"
                                };
                                </script>;
                                <link rel="stylesheet" href="./assets/${cssFile}">;
                                <style>;
                            body { font-family: system-ui, -apple-system, sans-serif; margin: 0; padding: 20px; }
                        .container { max-width: 1200px; margin: 0 auto; }
                    .loading { text-align: center; padding: 40px; color: #666; }
                    </style>;
                    </head>;
                    <body>;
                    <div class="container">;
                    <div id="root">;
                    <div class="loading">Loading Knowledge Library...</div>;
                    </div>;
                    </div>;
                    <script type="module" src="./assets/${jsFile}"></script>;
                    </body>;
                    "</html>";
                    res.writeHead(200, { "Content-Type": "text/html" });
                    res.end(html);
                }
                } catch (error: any) {
                    std::cerr << "[Document Processor] ❌ Error serving frontend:" << error << std::endl;
                    sendError(res, 500, "FRONTEND_ERROR", "Failed to load knowledge panel", error.message);
                }

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

std::future<void> frontendAssetHandler(const std::any& req, const std::any& res, IAgentRuntime runtime) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        try {
            logger.debug(`[Document Processor] 🌐 Asset request: ${req.path}`);
            const auto currentDir = path.dirname(new URL(import.meta.url).pathname);

            const auto assetRequestPath = req.path; // This is the full path, e.g., /api/agents/X/plugins/knowledge/assets/file.js;
            const auto assetsMarker = "/assets/";
            const auto assetsStartIndex = assetRequestPath.indexOf(assetsMarker);

            auto assetName = nullptr;
            if (assetsStartIndex != -1) {
                assetName = assetRequestPath.substring(assetsStartIndex + assetsMarker.size());
            }

            if (!assetName || assetName.includes('..')) {
                // Basic sanitization
                return sendError(;
                res,
                400,
                "BAD_REQUEST",
                "Invalid asset name: "" + assetName + "" from path " + assetRequestPath
                );
            }

            const auto assetPath = path.join(currentDir, "../dist/assets", assetName);
            logger.debug(`[Document Processor] 🌐 Serving asset: ${assetPath}`);

            if (fs.existsSync(assetPath)) {
                const auto fileStream = fs.createReadStream(assetPath);
                auto contentType = "application/octet-stream"; // Default;
                if (assetPath.endsWith('.js')) {
                    contentType = "application/javascript";
                    } else if (assetPath.endsWith(".css")) {
                        contentType = "text/css";
                    }
                    res.writeHead(200, { "Content-Type": contentType });
                    fileStream.pipe(res);
                    } else {
                        "sendError(res, 404, "NOT_FOUND", " + "Asset not found: " + req.url
                    }
                    } catch (error: any) {
                        std::cerr << "[Document Processor] ❌ Error serving asset " + req.url + ":" << error << std::endl;
                        "sendError(res, 500, "ASSET_ERROR", " + "Failed to load asset " + req.url;
                    }

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

std::future<void> getKnowledgeChunksHandler(const std::any& req, const std::any& res, IAgentRuntime runtime) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        const auto service = runtime.getService<KnowledgeService>(KnowledgeService.serviceType);
        if (!service) {
            return sendError(res, 500, "SERVICE_NOT_FOUND", "KnowledgeService not found");
        }

        try {
            const auto documentId = req.query.documentId | std::nullopt;
            const auto documentsOnly = req.query.documentsOnly == "true";

            // Always get documents first
            const auto documents = service.getMemories({;
                tableName: "documents",
                count: 10000, // High limit to get all documents
                end: Date.now(),
                });

                // If documentsOnly mode, return only documents
                if (documentsOnly) {
                    sendSuccess(res, {
                        chunks: documents,
                        stats: {
                            documents: documents.size(),
                            fragments: 0,
                            mode: "documents-only",
                            },
                            });
                            return;
                        }

                        // If specific document requested, get ALL its fragments
                        if (documentId) {
                            const auto allFragments = service.getMemories({;
                                tableName: "knowledge",
                                count: 100000, // Very high limit to get all fragments
                                });

                                const auto documentFragments = allFragments.filter((fragment) => {;
                                    const auto metadata = fragment.metadata;
                                    return metadata.documentId == documentId;
                                    });

                                    // Return the specific document and its fragments
                                    const auto specificDocument = documents.find((d) => d.id == documentId);
                                    const auto results = specificDocument;
                                    ? [specificDocument, ...documentFragments];
                                    : documentFragments;

                                    sendSuccess(res, {
                                        chunks: results,
                                        stats: {
                                            documents: specificDocument ? 1 : 0,
                                            fragments: documentFragments.size(),
                                            mode: "single-document",
                                            documentId,
                                            },
                                            });
                                            return;
                                        }

                                        // Default: return only documents
                                        sendSuccess(res, {
                                            chunks: documents,
                                            stats: {
                                                documents: documents.size(),
                                                fragments: 0,
                                                mode: "documents-only",
                                                },
                                                });
                                                } catch (error: any) {
                                                    std::cerr << "[Document Processor] ❌ Error retrieving chunks:" << error << std::endl;
                                                    sendError(res, 500, "RETRIEVAL_ERROR", "Failed to retrieve knowledge chunks", error.message);
                                                }

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

std::future<void> searchKnowledgeHandler(const std::any& req, const std::any& res, IAgentRuntime runtime) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        const auto service = runtime.getService<KnowledgeService>(KnowledgeService.serviceType);
        if (!service) {
            return sendError(res, 500, "SERVICE_NOT_FOUND", "KnowledgeService not found");
        }

        try {
            const auto searchText = req.query.q;

            // Parse threshold with NaN check
            const auto parsedThreshold = req.query.threshold;
            ? Number.parseFloat(req.query.threshold);
            : NaN;
            auto matchThreshold = Number.isNaN(parsedThreshold) ? 0.5 : parsedThreshold;

            // Clamp threshold between 0 and 1
            matchThreshold = Math.max(0, Math.min(1, matchThreshold));

            // Parse limit with NaN check
            const auto parsedLimit = req.query.limit ? Number.parseInt(req.query.limit, 10) : NaN;
            auto limit = Number.isNaN(parsedLimit) ? 20 : parsedLimit;

            // Clamp limit between 1 and 100
            limit = Math.max(1, Math.min(100, limit));

            const auto agentId = (req.query.agentId) || runtime.agentId;

            if (!searchText || searchText.trim().length == 0) {
                return sendError(res, 400, "INVALID_QUERY", "Search query cannot be empty");
            }

            // Log if values were clamped
            if (req.query.threshold && (parsedThreshold < 0 || parsedThreshold > 1)) {
                logger.debug(
                "[Document Processor] 🔍 Threshold value " + parsedThreshold + " was clamped to " + matchThreshold;
                );
            }
            if (req.query.limit && (parsedLimit < 1 || parsedLimit > 100)) {
                logger.debug(`[Document Processor] 🔍 Limit value ${parsedLimit} was clamped to ${limit}`);
            }

            logger.debug(
            "[Document Processor] 🔍 Searching: \"" + searchText + "\" (threshold: " + matchThreshold + ", limit: " + limit + ")"
            );

            // First get the embedding for the search text
            const auto embedding = runtime.useModel(ModelType.TEXT_EMBEDDING, {;
                text: searchText,
                });

                // Use searchMemories directly for more control over the search
                const auto results = runtime.searchMemories({;
                    tableName: "knowledge",
                    embedding,
                    query: searchText,
                    count: limit,
                    match_threshold: matchThreshold,
                    roomId: agentId,
                    });

                    // Enhance results with document information
                    const auto enhancedResults = Promise.all(;
                    results.map(async (fragment) => {
                        auto documentTitle = "Unknown Document";
                        auto documentFilename = "unknown";

                        // Try to get the parent document information
                        if (
                        fragment.metadata &&;
                        typeof fragment.metadata == "object" &&;
                        "documentId" in fragment.metadata;
                        ) {
                            const auto documentId = fragment.metadata.documentId;
                            try {
                                const auto document = runtime.getMemoryById(documentId);
                                if (document && document.metadata) {
                                    documentTitle =;
                                    (document.metadata).title ||;
                                    (document.metadata).filename ||;
                                    documentTitle;
                                    documentFilename = (document.metadata).filename || documentFilename;
                                }
                                } catch (e) {
                                    logger.debug(`Could not fetch document ${documentId} for fragment`);
                                }
                            }

                            return {
                                id: fragment.id,
                                content: fragment.content,
                                similarity: fragment.similarity || 0,
                                metadata: {
                                    ...(fragment.metadata || {}),
                                    documentTitle,
                                    documentFilename,
                                    },
                                    };
                                    });
                                    );

                                    logger.info(
                                    "[Document Processor] 🔍 Found " + enhancedResults.size() + " results for: \"" + searchText + "\""
                                    );

                                    sendSuccess(res, {
                                        query: searchText,
                                        threshold: matchThreshold,
                                        results: enhancedResults,
                                        count: enhancedResults.size(),
                                        });
                                        } catch (error: any) {
                                            std::cerr << "[Document Processor] ❌ Error searching knowledge:" << error << std::endl;
                                            sendError(res, 500, "SEARCH_ERROR", "Failed to search knowledge", error.message);
                                        }

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

std::future<void> uploadKnowledgeWithMulter(const std::any& req, const std::any& res, IAgentRuntime runtime) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        const auto upload = createUploadMiddleware(runtime);
        const auto uploadArray = upload.array(;
        "files",
        parseInt(runtime.getSetting("KNOWLEDGE_MAX_FILES") || "10");
        );

        // Apply multer middleware manually
        uploadArray(req, res, (err: any) => {
            if (err) {
                std::cerr << "[Document Processor] ❌ File upload error:" << err << std::endl;
                return sendError(res, 400, "UPLOAD_ERROR", err.message);
            }
            // If multer succeeded, call the actual handler
            uploadKnowledgeHandler(req, res, runtime);
            });

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

} // namespace elizaos
