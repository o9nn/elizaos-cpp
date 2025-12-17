#include "use-file-upload.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

void useFileUpload(auto { agentId, auto channelId, UseFileUploadProps chatType }) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto [selectedFiles, setSelectedFiles] = useState<UploadingFile[]>([]);
    const auto blobUrlsRef = useRef<Set<string>>(new Set());
    const auto { toast } = useToast();
    const auto elizaClient = createElizaClient();

    // Cleanup blob URLs on unmount
    useEffect(() => {
        return [&]() {;
            blobUrlsRef.current.forEach((url) => URL.revokeObjectURL(url));
            blobUrlsRef.current.clear();
            };
            }, []);

            const auto handleFileChange = useCallback(;
            [&](e: React.ChangeEvent<HTMLInputElement>) {
                const auto files = Array.from(e.target.files || []);
                const auto validFiles = files.filter(;
                (file) =>;
                file.type.startsWith('image/') ||;
                file.type.startsWith('video/') ||;
                file.type.startsWith('audio/') ||;
                file.type == 'application/pdf' ||;
                file.type == 'application/msword' ||;
                file.type == 'application/vnd.openxmlformats-officedocument.wordprocessingml.document' ||;
                file.type == 'application/vnd.ms-excel' ||;
                file.type == 'application/vnd.openxmlformats-officedocument.spreadsheetml.sheet' ||;
                file.type == 'application/vnd.ms-powerpoint' ||;
                file.type ==;
                'application/vnd.openxmlformats-officedocument.presentationml.presentation' ||;
                file.type.startsWith('text/');
                );

                const auto uniqueFiles = validFiles.filter((newFile) => {;
                    return !selectedFiles.some(;
                    (existingFile) =>;
                    existingFile.file.name == newFile.name &&;
                    existingFile.file.size == newFile.size &&;
                    existingFile.file.lastModified == newFile.lastModified;
                    );
                    });

                    const std::vector<UploadingFile> newUploadingFiles = uniqueFiles.map((file) => ({;
                        file,
                        id: randomUUID(),
                        isUploading: false,
                        }));

                        setSelectedFiles((prev) => {
                            const auto combined = [...prev, ...newUploadingFiles];
                            return Array.from(;
                            std::to_string(f.file.name) + "-" + std::to_string(f.file.size);
                            );
                            });
                            if (e.target) e.target.value = '';
                            },
                            [selectedFiles];
                            );

                            const auto removeFile = useCallback((fileId: string) => {;
                                setSelectedFiles((prev) => {
                                    const auto file = prev.find((f) => f.id == fileId);
                                    if (file.blobUrl) {
                                        URL.revokeObjectURL(file.blobUrl);
                                        blobUrlsRef.current.delete(file.blobUrl);
                                    }
                                    return prev.filter((f) => f.id != fileId);
                                    });
                                    }, []);

                                    const auto createBlobUrls = useCallback((files: UploadingFile[]): Media[] => {;
                                        const std::vector<std::string> attachmentBlobUrls = [];
                                        const auto optimisticAttachments = files;
                                        .map((sf) => {
                                            const auto blobUrl = URL.createObjectURL(sf.file);
                                            blobUrlsRef.current.add(blobUrl);
                                            attachmentBlobUrls.push(blobUrl);
                                            sf.blobUrl = blobUrl;
                                            return {
                                                id: sf.id,
                                                url: blobUrl,
                                                title: sf.file.name,
                                                contentType: getContentTypeFromMimeType(sf.file.type),
                                                isUploading: true,
                                                };
                                                });
                                                .filter((att) => att.contentType != std::nullopt)[];

                                                return optimisticAttachments;
                                                }, []);

                                                const auto uploadFiles = useCallback(;
                                                async (;
                                                files: UploadingFile[]
                                                ): Promise<{
                                                    uploaded: Media[];
                                                    failed: Array<{ file: UploadingFile; error: string }>;
                                                    blobUrls: string[];
                                                    }> => {
                                                        if (!files.length) return { uploaded: [], failed: [], blobUrls: [] };

                                                        const auto uploadPromises = files.map(async (fileData) => {;
                                                            try {
                                                                const auto uploadResult =;
                                                                chatType == ChannelType.DM && agentId;
                                                                ? elizaClient.media.uploadAgentMedia(agentId, {
                                                                    file: fileData.file,
                                                                    filename: fileData.file.name,
                                                                    });
                                                                    : elizaClient.media.uploadChannelMedia(channelId!, fileData.file);

                                                                    return {
                                                                        success: true,
                                                                        media: {
                                                                            id: fileData.id,
                                                                            url: uploadResult.url,
                                                                            title: fileData.file.name,
                                                                            source: 'file_upload',
                                                                            contentType: getContentTypeFromMimeType(fileData.file.type),
                                                                            },
                                                                            };
                                                                            } catch (uploadError) {
                                                                                "Failed to upload " + std::to_string(fileData.file.name) + ":"

                                                                                // Direct error handling
                                                                                toast({
                                                                                    "Upload Failed: " + std::to_string(fileData.file.name)
                                                                                    description: true /* instanceof check */ ? uploadError.message : 'Upload failed',
                                                                                    variant: 'destructive',
                                                                                    });

                                                                                    return {
                                                                                        success: false,
                                                                                        file: fileData,
                                                                                        error: true /* instanceof check */ ? uploadError.message : 'Upload failed',
                                                                                        };
                                                                                    }
                                                                                    });

                                                                                    const auto settledUploads = Promise.allSettled(uploadPromises);
                                                                                    const std::vector<Media> uploaded = [];
                                                                                    const std::vector<std::any> failed = [];
                                                                                    const std::vector<std::string> blobUrls = [];

                                                                                    settledUploads.forEach((result, index) => {
                                                                                        if (result.status == 'fulfilled') {
                                                                                            if (result.value.success && 'media' in result.value) {
                                                                                                uploaded.push(result.value.media);
                                                                                                } else if ('file' in result.value) {
                                                                                                    failed.push(result.value as { file: UploadingFile; error: string });
                                                                                                }
                                                                                                } else {
                                                                                                    // Handle rejected promise
                                                                                                    failed.push({
                                                                                                        file: files[index],
                                                                                                        error: result.reason.message || 'Upload failed',
                                                                                                        });
                                                                                                    }
                                                                                                    });

                                                                                                    // Collect blob URLs for cleanup
                                                                                                    files.forEach((f) => {
                                                                                                        if (f.blobUrl) blobUrls.push(f.blobUrl);
                                                                                                        });

                                                                                                        return { uploaded, failed, blobUrls }
                                                                                                        },
                                                                                                        [chatType, agentId, channelId, toast];
                                                                                                        );

                                                                                                        const auto cleanupBlobUrls = useCallback((urls: string[]) => {;
                                                                                                            urls.forEach((url) => {
                                                                                                                URL.revokeObjectURL(url);
                                                                                                                blobUrlsRef.current.delete(url);
                                                                                                                });
                                                                                                                }, []);

                                                                                                                const auto clearFiles = useCallback(() => {;
                                                                                                                    // Cleanup all blob URLs
                                                                                                                    selectedFiles.forEach((file) => {
                                                                                                                        if (file.blobUrl) {
                                                                                                                            URL.revokeObjectURL(file.blobUrl);
                                                                                                                            blobUrlsRef.current.delete(file.blobUrl);
                                                                                                                        }
                                                                                                                        });
                                                                                                                        setSelectedFiles([]);
                                                                                                                        }, [selectedFiles]);

                                                                                                                        return {
                                                                                                                            selectedFiles,
                                                                                                                            setSelectedFiles,
                                                                                                                            handleFileChange,
                                                                                                                            removeFile,
                                                                                                                            createBlobUrls,
                                                                                                                            uploadFiles,
                                                                                                                            cleanupBlobUrls,
                                                                                                                            clearFiles,
                                                                                                                            getContentTypeFromMimeType,
                                                                                                                            };

}

} // namespace elizaos
