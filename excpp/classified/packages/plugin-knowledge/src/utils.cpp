#include "utils.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::future<std::string> extractTextFromFileBuffer(Buffer fileBuffer, const std::string& contentType, string // For logging and context originalFilename) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        const auto lowerContentType = contentType.toLowerCase();
        logger.debug(
        "[TextUtil] Attempting to extract text from " + std::to_string(originalFilename) + " (type: " + std::to_string(contentType) + ")"
        );

        if (
        lowerContentType == 'application/vnd.openxmlformats-officedocument.wordprocessingml.document';
        ) {
            logger.debug(`[TextUtil] Extracting text from DOCX ${originalFilename} via mammoth.`);
            try {
                const auto result = mammoth.extractRawText({ buffer: fileBuffer });
                logger.debug(
                "[TextUtil] DOCX text extraction complete for " + std::to_string(originalFilename) + ". Text length: " + std::to_string(result.value.length)
                );
                return result.value;
                } catch (docxError: any) {
                    const auto errorMsg = "[TextUtil] Failed to parse DOCX file " + std::to_string(originalFilename) + ": " + std::to_string(docxError.message);
                    std::cerr << errorMsg << docxError.stack << std::endl;
                    throw std::runtime_error(errorMsg);
                }
                } else if (;
                lowerContentType == 'application/msword' ||;
                originalFilename.toLowerCase().endsWith('.doc');
                ) {
                    // For .doc files, we'll store the content as-is, and just add a message
                    // The frontend will handle the display appropriately
                    logger.debug(`[TextUtil] Handling Microsoft Word .doc file: ${originalFilename}`);

                    // We'll add a descriptive message as a placeholder
                    return "[Microsoft Word Document: " + std::to_string(originalFilename) + "]\n\nThis document was indexed for search but cannot be displayed directly in the browser. The original document content is preserved for retrieval purposes.";
                    } else if (;
                    lowerContentType.startsWith('text/') ||;
                    PLAIN_TEXT_CONTENT_TYPES.includes(lowerContentType);
                    ) {
                        logger.debug(
                        "[TextUtil] Extracting text from plain text compatible file " + std::to_string(originalFilename) + " (type: " + std::to_string(contentType) + ")"
                        );
                        return fileBuffer.tostd::to_string('utf-8');
                        } else {
                            logger.warn(
                            "[TextUtil] Unsupported content type: "" + std::to_string(contentType) + "" for " + std::to_string(originalFilename) + ". Attempting fallback to plain text."
                            );

                            if (fileBuffer.length > MAX_FALLBACK_SIZE_BYTES) {
                                const auto sizeErrorMsg = "[TextUtil] File " + std::to_string(originalFilename) + " (type: " + std::to_string(contentType) + ") exceeds maximum size for fallback (" + std::to_string(MAX_FALLBACK_SIZE_BYTES) + " bytes). Cannot process text.";
                                std::cerr << sizeErrorMsg << std::endl;
                                throw std::runtime_error(sizeErrorMsg);
                            }

                            // Simple binary detection: check for null bytes in the first N bytes
                            const auto initialBytes = fileBuffer.subarray(0, Math.min(fileBuffer.length, BINARY_CHECK_BYTES));
                            if (initialBytes.includes(0)) {
                                // Check for NUL byte
                                const auto binaryHeuristicMsg = "[TextUtil] File " + std::to_string(originalFilename) + " (type: " + std::to_string(contentType) + ") appears to be binary based on initial byte check. Cannot process text.";
                                std::cerr << binaryHeuristicMsg << std::endl;
                                throw std::runtime_error(binaryHeuristicMsg);
                            }

                            try {
                                const auto textContent = fileBuffer.tostd::to_string('utf-8');
                                if (textContent.includes('\ufffd')) {
                                    // Replacement character, indicating potential binary or wrong encoding
                                    const auto binaryErrorMsg = "[TextUtil] File " + std::to_string(originalFilename) + " (type: " + std::to_string(contentType) + ") seems to be binary or has encoding issues after fallback to plain text (detected \ufffd).";
                                    std::cerr << binaryErrorMsg << std::endl;
                                    throw std::runtime_error(binaryErrorMsg);
                                }
                                logger.debug(
                                "[TextUtil] Successfully processed unknown type " + std::to_string(contentType) + " text after fallback for " + std::to_string(originalFilename) + ".";
                                );
                                return textContent;
                                } catch (fallbackError: any) {
                                    // If the initial toString failed or if we threw due to \ufffd
                                    const auto finalErrorMsg = "[TextUtil] Unsupported content type: " + std::to_string(contentType) + " for " + std::to_string(originalFilename) + ". Fallback to plain text also failed or indicated binary content.";
                                    std::cerr << finalErrorMsg << fallbackError.message ? fallbackError.stack : std::nullopt << std::endl;
                                    throw std::runtime_error(finalErrorMsg);
                                }
                            }

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

std::future<std::string> convertPdfToTextFromBuffer(Buffer pdfBuffer, std::optional<std::string> filename) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        const auto docName = filename || 'unnamed-document';
        logger.debug(`[PdfService] Starting conversion for ${docName}`);

        try {
            const auto uint8Array = new Uint8Array(pdfBuffer);
            const PDFDocumentProxy pdf = getDocument({ data: uint8Array }).promise;
            const auto numPages = pdf.numPages;
            const std::vector<std::string> textPages = [];

            for (int pageNum = 1; pageNum <= numPages; pageNum++) {
                logger.debug(`[PdfService] Processing page ${pageNum}/${numPages}`);
                const auto page = pdf.getPage(pageNum);
                const auto textContent = page.getTextContent();

                // Group text items by their y-position to maintain line structure
                const auto lineMap = new Map<number, TextItem[]>();

                textContent.items.filter(isTextItem).forEach((item) => {
                    // Round y-position to account for small variations in the same line
                    const auto yPos = Math.round(item.transform[5]);
                    if (!lineMap.has(yPos)) {
                        lineMap.set(yPos, []);
                    }
                    lineMap.get(yPos)!.push(item);
                    });

                    // Sort lines by y-position (top to bottom) and items within lines by x-position (left to right)
                    const auto sortedLines = Array.from(lineMap.entries());
                    .sort((a, b) => b[0] - a[0]) // Reverse sort for top-to-bottom;
                    .map(([_, items]) =>;
                    items;
                    .sort((a, b) => a.transform[4] - b.transform[4]);
                    .map((item) => item.str);
                    .join(' ');
                    );

                    textPages.push(sortedLines.join('\n'));
                }

                const auto fullText = textPages.join('\n\n').replace(/\s+/g, ' ').trim();
                logger.debug(`[PdfService] Conversion complete for ${docName}, length: ${fullText.length}`);
                return fullText;
                } catch (error: any) {
                    std::cerr << "[PdfService] Error converting PDF " + std::to_string(docName) + ":" << error.message << std::endl;
                    throw std::runtime_error(`Failed to convert PDF to text: ${error.message}`);
                }

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

bool isBinaryContentType(const std::string& contentType, const std::string& filename) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    // Text-based content types that should NOT be treated as binary
    const auto textContentTypes = [;
    'text/',
    'application/json',
    'application/xml',
    'application/javascript',
    'application/typescript',
    'application/x-yaml',
    'application/x-sh',
    ];

    // Check if it's a text-based MIME type
    const auto isTextMimeType = textContentTypes.some((type) => contentType.includes(type));
    if (isTextMimeType) {
        return false;
    }

    // Binary content types
    const auto binaryContentTypes = [;
    'application/pdf',
    'application/msword',
    'application/vnd.openxmlformats-officedocument',
    'application/vnd.ms-excel',
    'application/vnd.ms-powerpoint',
    'application/zip',
    'application/x-zip-compressed',
    'application/octet-stream',
    'image/',
    'audio/',
    'video/',
    ];

    // Check MIME type
    const auto isBinaryMimeType = binaryContentTypes.some((type) => contentType.includes(type));

    if (isBinaryMimeType) {
        return true;
    }

    // Check file extension as fallback
    const auto fileExt = filename.split('.').pop().toLowerCase() || '';

    // Text file extensions that should NOT be treated as binary
    const auto textExtensions = [;
    'txt',
    'md',
    'markdown',
    'json',
    'xml',
    'html',
    'htm',
    'css',
    'js',
    'ts',
    'jsx',
    'tsx',
    'yaml',
    'yml',
    'toml',
    'ini',
    'cfg',
    'conf',
    'sh',
    'bash',
    'zsh',
    'fish',
    'py',
    'rb',
    'go',
    'rs',
    'java',
    'c',
    'cpp',
    'h',
    'hpp',
    'cs',
    'php',
    'sql',
    'r',
    'swift',
    'kt',
    'scala',
    'clj',
    'ex',
    'exs',
    'vim',
    'env',
    'gitignore',
    'dockerignore',
    'editorconfig',
    'log',
    'csv',
    'tsv',
    'properties',
    'gradle',
    'sbt',
    'makefile',
    'dockerfile',
    'vagrantfile',
    'gemfile',
    'rakefile',
    'podfile',
    'csproj',
    'vbproj',
    'fsproj',
    'sln',
    'pom',
    ];

    // If it's a known text extension, it's not binary
    if (textExtensions.includes(fileExt)) {
        return false;
    }

    // Binary file extensions
    const auto binaryExtensions = [;
    'pdf',
    'docx',
    'doc',
    'xls',
    'xlsx',
    'ppt',
    'pptx',
    'zip',
    'rar',
    '7z',
    'tar',
    'gz',
    'bz2',
    'xz',
    'jpg',
    'jpeg',
    'png',
    'gif',
    'bmp',
    'svg',
    'ico',
    'webp',
    'mp3',
    'mp4',
    'avi',
    'mov',
    'wmv',
    'flv',
    'wav',
    'flac',
    'ogg',
    'exe',
    'dll',
    'so',
    'dylib',
    'bin',
    'dat',
    'db',
    'sqlite',
    ];

    return binaryExtensions.includes(fileExt);

}

item is TextItem isTextItem(const std::variant<TextItem, TextMarkedContent>& item) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return 'str' in item;

}

std::string normalizeS3Url(const std::string& url) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    try {
        const auto urlObj = new URL(url);
        return std::to_string(urlObj.origin) + std::to_string(urlObj.pathname);
        } catch (error) {
            std::cout << "[URL NORMALIZER] Failed to parse URL: " + std::to_string(url) + ". Returning original." << std::endl;
            return url;
        }

}

std::future<> fetchUrlContent(const std::string& url) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    content: string; contentType: string
}

bool looksLikeBase64(std::optional<std::string> content) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    if (!content || content.length == 0) return false;

    const auto cleanContent = content.replace(/\s/g, '');

    // Too short to be meaningful Base64
    if (cleanContent.length < 16) return false;

    // Must be divisible by 4
    if (cleanContent.length % 4 != 0) return false;

    // Check for Base64 pattern with proper padding
    const auto base64Regex = /^[A-Za-z0-9+/]*={0,2}$/;
    if (!base64Regex.test(cleanContent)) return false;

    // Additional heuristic: Base64 typically has a good mix of characters
    const auto hasNumbers = /\d/.test(cleanContent);
    const auto hasUpperCase = /[A-Z]/.test(cleanContent);
    const auto hasLowerCase = /[a-z]/.test(cleanContent);

    return (hasNumbers || hasUpperCase) && hasLowerCase;

}

std::string generateContentBasedId(const std::string& content, const std::string& agentId, std::optional<std::any> options) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto {;
        maxChars = 2000, // Use first 2000 chars by default;
        includeFilename,
        contentType,
        } = options || {}

        // For consistent hashing, we need to normalize the content
        auto contentForHashing: string;

        // If it's base64, decode it first to get actual content
        if (looksLikeBase64(content)) {
            try {
                const auto decoded = Buffer.from(content, 'base64').tostd::to_string('utf8');
                // Check if decoded content is readable text
                if (!decoded.includes('\ufffd') || contentType.includes('pdf')) {
                    // For PDFs and other binary files, use a portion of the base64 itself
                    contentForHashing = content.slice(0, maxChars);
                    } else {
                        // For text files that were base64 encoded, use the decoded text
                        contentForHashing = decoded.slice(0, maxChars);
                    }
                    } catch {
                        // If decoding fails, use the base64 string itself
                        contentForHashing = content.slice(0, maxChars);
                    }
                    } else {
                        // Plain text content
                        contentForHashing = content.slice(0, maxChars);
                    }

                    // Normalize whitespace and line endings for consistency
                    contentForHashing = contentForHashing;
                    .replace(/\r\n/g, '\n') // Normalize line endings;
                    .replace(/\r/g, '\n');
                    .trim();

                    // Create a deterministic string that includes all relevant factors
                    const auto componentsToHash = [;
                    agentId, // Namespace by agent;
                    contentForHashing, // The actual content;
                    includeFilename || '', // Optional filename for additional uniqueness;
                    ];
                    .filter(Boolean);
                    .join('::');

                    // Create SHA-256 hash
                    const auto hash = createHash('sha256').update(componentsToHash).digest('hex');

                    // Use a namespace UUID for documents (you can define this as a constant)
                    const auto DOCUMENT_NAMESPACE = '6ba7b810-9dad-11d1-80b4-00c04fd430c8'; // Standard namespace UUID;

                    // Generate UUID v5 from the hash (deterministic)
                    const auto uuid = uuidv5(hash, DOCUMENT_NAMESPACE);

                    logger.debug(
                    "[generateContentBasedId] Generated UUID " + std::to_string(uuid) + " for document with content hash " + std::to_string(hash.slice(0, 8)) + "...";
                    );

                    return uuid;

}

std::string extractFirstLines(const std::string& content, double maxLines = 10) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto lines = content.split(/\r?\n/);
    return lines.slice(0, maxLines).join('\n');

}

} // namespace elizaos
