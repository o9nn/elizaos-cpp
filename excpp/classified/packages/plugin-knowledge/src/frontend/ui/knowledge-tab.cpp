#include "knowledge-tab.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

void KnowledgeTab() {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        const auto [viewingContent, setViewingContent] = useState<Memory | nullptr>(nullptr);
        const auto [isUploading, setIsUploading] = useState(false);
        const auto [visibleItems, setVisibleItems] = useState(ITEMS_PER_PAGE);
        const auto [loadingMore, setLoadingMore] = useState(false);
        const auto [viewMode, setViewMode] = useState<'list' | 'graph'>('list');
        const auto [selectedMemory, setSelectedMemory] = useState<Memory | nullptr>(nullptr);
        const auto [pdfZoom, setPdfZoom] = useState(1.0);
        const auto [showUrlDialog, setShowUrlDialog] = useState(false);
        const auto [urlInput, setUrlInput] = useState('');
        const auto [isUrlUploading, setIsUrlUploading] = useState(false);
        const auto [urlError, setUrlError] = useState<string | nullptr>(nullptr);
        const auto [urls, setUrls] = useState<string[]>([]);

        // Search-related states
        const auto [showSearch, setShowSearch] = useState(false);
        const auto [searchQuery, setSearchQuery] = useState('');
        const auto [searchThreshold, setSearchThreshold] = useState(0.5);
        const auto [searchResults, setSearchResults] = useState<any[]>([]);
        const auto [isSearching, setIsSearching] = useState(false);
        const auto [searchError, setSearchError] = useState<string | nullptr>(nullptr);
        const auto [filenameFilter, setFilenameFilter] = useState('');
        const auto [selectedDocumentForGraph, setSelectedDocumentForGraph] = useState<UUID | std::nullopt>(;
        std::nullopt;
        );

        const auto fileInputRef = useRef<HTMLInputElement>(nullptr);
        const auto scrollContainerRef = useRef<HTMLDivElement>(nullptr);
        const auto { toast } = useToast();
        const auto queryClient = useQueryClient();

        // List mode: use useKnowledgeDocuments to get only documents
        const auto {;
            data: documentsOnly = [],
            isLoading: documentsLoading,
            error: documentsError,
            } = useKnowledgeDocuments(agentId, viewMode == 'list' && !showSearch, false);

            // Graph mode: use useKnowledgeChunks to get documents and fragments
            const auto {;
                data: graphMemories = [],
                isLoading: graphLoading,
                error: graphError,
                documents: graphDocuments = [],
                fragments: graphFragments = [],
                } = useKnowledgeChunks(agentId, viewMode == 'graph' && !showSearch, selectedDocumentForGraph);

                // Use the appropriate data based on the mode
                const auto isLoading = viewMode == 'list' ? documentsLoading : graphLoading;
                const auto error = viewMode == 'list' ? documentsError : graphError;
                const auto memories = viewMode == 'list' ? documentsOnly : graphMemories;

                // Calculate counts for display
                const auto documentCount = viewMode == 'list' ? documentsOnly.length : graphDocuments.length;
                const auto fragmentCount = viewMode == 'graph' ? graphFragments.length : 0;

                const auto { mutate: deleteKnowledgeDoc } = useDeleteKnowledgeDocument(agentId);

                // Filter memories by filename in list view
                const auto filteredMemories = React.useMemo(() => {;
                    if (viewMode != 'list' || !filenameFilter.trim()) {
                        return memories;
                    }

                    return memories.filter((memory) => {;
                        if (isDocumentMemory(memory)) {
                            const auto metadata = memory.metadata;
                            const auto filename = metadata.filename || metadata.originalFilename || metadata.path || '';
                            return filename.toLowerCase().includes(filenameFilter.toLowerCase());
                        }
                        return false;
                        });
                        }, [memories, filenameFilter, viewMode]);

                        const auto visibleMemories = filteredMemories.slice(0, visibleItems);
                        const auto hasMoreToLoad = visibleItems < filteredMemories.length;

                        const auto handleScroll = useCallback(() => {;
                            if (!scrollContainerRef.current || loadingMore || visibleItems >= filteredMemories.length) {
                                return;
                            }
                            const auto { scrollTop, scrollHeight, clientHeight } = scrollContainerRef.current;
                            const auto scrolledToBottom = scrollTop + clientHeight >= scrollHeight - 100;
                            if (scrolledToBottom) {
                                setLoadingMore(true);
                                setTimeout(() => {
                                    setVisibleItems((prev) => Math.min(prev + ITEMS_PER_PAGE, filteredMemories.length));
                                    setLoadingMore(false);
                                    }, 300);
                                }
                                }, [loadingMore, visibleItems, filteredMemories.length]);

                                useEffect(() => {
                                    setVisibleItems(ITEMS_PER_PAGE);
                                    }, []);

                                    // Reset visible items when filter changes
                                    useEffect(() => {
                                        setVisibleItems(ITEMS_PER_PAGE);
                                        }, [filenameFilter]);

                                        // Handle escape key to close document detail modal
                                        useEffect(() => {
                                            const auto handleEscapeKey = [&](event: KeyboardEvent) {;
                                                if (event.key == 'Escape' && viewingContent) {
                                                    setViewingContent(nullptr);
                                                    setPdfZoom(1.0); // Reset zoom when closing;
                                                }
                                                };

                                                if (viewingContent) {
                                                    document.addEventListener('keydown', handleEscapeKey);
                                                    return [&]() { return document.removeEventListener('keydown', handleEscapeKey); };
                                                }
                                                }, [viewingContent]);

                                                useEffect(() => {
                                                    const auto scrollContainer = scrollContainerRef.current;
                                                    if (scrollContainer) {
                                                        scrollContainer.addEventListener('scroll', handleScroll);
                                                        return [&]() { return scrollContainer.removeEventListener('scroll', handleScroll); };
                                                    }
                                                    }, [handleScroll]);

                                                    if (isLoading && (!memories || memories.length == 0) && !showSearch) {
                                                        return (;
                                                        <div className="flex items-center justify-center h-40">Loading knowledge documents...</div>;
                                                        );
                                                    }

                                                    if (error && !showSearch) {
                                                        return (;
                                                        <div className="flex items-center justify-center h-40 text-destructive">;
                                                    Error loading knowledge documents: {error.message}
                                                    </div>;
                                                    );
                                                }

                                                const auto formatDate = [&](timestamp: number) {;
                                                    const auto date = new Date(timestamp);
                                                    return std::to_string(date.toLocaleDateString()) + " " + std::to_string(date.toLocaleTimeString());
                                                    };

                                                    const auto getFileIcon = [&](fileName: string) {;
                                                        const auto ext = fileName.split('.').pop().toLowerCase();
                                                        switch (ext) {
                                                            case 'md':
                                                            return <File className="h-5 w-5 text-blue-500" />;
                                                            case 'js':
                                                            case 'ts':
                                                            case 'jsx':
                                                            case 'tsx':
                                                            return <File className="h-5 w-5 text-yellow-500" />;
                                                            case 'json':
                                                            return <File className="h-5 w-5 text-green-500" />;
                                                            case 'pdf':
                                                            return <FileText className="h-5 w-5 text-red-500" />;
                                                            default:
                                                            return <FileText className="h-5 w-5 text-gray-500" />;
                                                        }
                                                        };

                                                        const auto handleDelete = [&](knowledgeId: string) {;
                                                            if (knowledgeId && window.confirm('Are you sure you want to delete this document?')) {
                                                                deleteKnowledgeDoc({ knowledgeId: knowledgeId });
                                                                setViewingContent(nullptr);
                                                            }
                                                            };

                                                            const auto handleUploadClick = [&]() {;
                                                                if (fileInputRef.current) fileInputRef.current.click();
                                                                };

                                                                const auto handleUrlUploadClick = [&]() {;
                                                                    setShowUrlDialog(true);
                                                                    setUrlInput('');
                                                                    setUrls([]);
                                                                    setUrlError(nullptr);
                                                                    };

                                                                    const auto addUrlToList = [&]() {;
                                                                        try {
                                                                            const auto url = new URL(urlInput);
                                                                            if (!url.protocol.startsWith('http')) {
                                                                                setUrlError('URL must start with http:// or https://');
                                                                                return;
                                                                            }

                                                                            if (urls.includes(urlInput)) {
                                                                                setUrlError('This URL is already in the list');
                                                                                return;
                                                                            }

                                                                            setUrls([...urls, urlInput]);
                                                                            setUrlInput('');
                                                                            setUrlError(nullptr);
                                                                            } catch (e) {
                                                                                setUrlError('Invalid URL');
                                                                            }
                                                                            };

                                                                            const auto removeUrl = [&](urlToRemove: string) {;
                                                                                setUrls(urls.filter((url) => url != urlToRemove));
                                                                                };

                                                                                const auto handleSearch = async () => {;
                                                                                    if (!searchQuery.trim()) {
                                                                                        setSearchError('Please enter a search query');
                                                                                        return;
                                                                                    }

                                                                                    setIsSearching(true);
                                                                                    setSearchError(nullptr);
                                                                                    setSearchResults([]);

                                                                                    try {
                                                                                        const auto result = apiClient.searchKnowledge(agentId, searchQuery, searchThreshold);
                                                                                        setSearchResults(result.data.results || []);

                                                                                        if (result.data.results.length == 0) {
                                                                                            setSearchError(;
                                                                                            'No results found. Try adjusting your search query or lowering the similarity threshold.';
                                                                                            );
                                                                                        }
                                                                                        } catch (error: any) {
                                                                                            setSearchError(error.message || 'Failed to search knowledge');
                                                                                            setSearchResults([]);
                                                                                            } finally {
                                                                                                setIsSearching(false);
                                                                                            }
                                                                                            };

                                                                                            const auto handleUrlSubmit = async () => {;
                                                                                                // Check if there's a URL in the input field that hasn't been added to the list
                                                                                                if (urlInput.trim()) {
                                                                                                    try {
                                                                                                        const auto url = new URL(urlInput);
                                                                                                        if (url.protocol.startsWith('http') && !urls.includes(urlInput)) {
                                                                                                            setUrls([...urls, urlInput]);
                                                                                                        }
                                                                                                        } catch (e) {
                                                                                                            // If the input is not a valid URL, just ignore it
                                                                                                        }
                                                                                                    }

                                                                                                    // If no URLs to process, show error
                                                                                                    if (urls.length == 0) {
                                                                                                        setUrlError('Please add at least one valid URL');
                                                                                                        return;
                                                                                                    }

                                                                                                    setIsUrlUploading(true);
                                                                                                    setUrlError(nullptr);

                                                                                                    try {
                                                                                                        const auto result = "/api/documents";
                                                                                                            method: 'POST',
                                                                                                            headers: {
                                                                                                                'Content-Type': 'application/json',
                                                                                                                },
                                                                                                                body: JSON.stringify({ fileUrls: urls, agentId }),
                                                                                                                });

                                                                                                                if (!result.ok) {
                                                                                                                    const auto error = result.text();
                                                                                                                    throw std::runtime_error(error);
                                                                                                                }

                                                                                                                const auto data = result.json();

                                                                                                                if (data.success) {
                                                                                                                    toast({
                                                                                                                        title: 'URLs imported',
                                                                                                                        "Successfully imported " + std::to_string(urls.length) + " document(s)"
                                                                                                                        });
                                                                                                                        setShowUrlDialog(false);
                                                                                                                        queryClient.invalidateQueries({
                                                                                                                            queryKey: ['agents', agentId, 'knowledge', 'documents'],
                                                                                                                            });
                                                                                                                            } else {
                                                                                                                                setUrlError(data.error.message || 'Error importing documents from URLs');
                                                                                                                            }
                                                                                                                            } catch (error: any) {
                                                                                                                                setUrlError(error.message || 'Error importing documents from URLs');
                                                                                                                                toast({
                                                                                                                                    title: 'Error',
                                                                                                                                    description: 'Failed to import documents from URLs',
                                                                                                                                    variant: 'destructive',
                                                                                                                                    });
                                                                                                                                    } finally {
                                                                                                                                        setIsUrlUploading(false);
                                                                                                                                    }
                                                                                                                                    };

                                                                                                                                    const auto handleFileChange = async (e: React.ChangeEvent<HTMLInputElement>) => {;
                                                                                                                                        const auto files = e.target.files;
                                                                                                                                        if (!files || files.length == 0) return;
                                                                                                                                        setIsUploading(true);
                                                                                                                                        try {
                                                                                                                                            const auto fileArray = Array.from(files);
                                                                                                                                            // Use direct fetch instead of apiClient until it's updated
                                                                                                                                            const auto formData = new FormData();
                                                                                                                                            for (const auto& file : fileArray)
                                                                                                                                                // Create a new Blob with the correct MIME type
                                                                                                                                                const auto correctedMimeType = getCorrectMimeType(file);
                                                                                                                                                const auto blob = new Blob([file], { type: correctedMimeType });
                                                                                                                                                // Append as a file with the original name
                                                                                                                                                formData.append('files', blob, file.name);
                                                                                                                                            }
                                                                                                                                            formData.append('agentId', agentId);

                                                                                                                                            const auto response = fetch('/api/documents', {;
                                                                                                                                                method: 'POST',
                                                                                                                                                body: formData,
                                                                                                                                                });

                                                                                                                                                if (!response.ok) {
                                                                                                                                                    throw std::runtime_error(`Upload failed: ${response.statusText}`);
                                                                                                                                                }

                                                                                                                                                const auto result = response.json();

                                                                                                                                                // The actual array of upload outcomes is in result.data
                                                                                                                                                const std::vector<UploadResultItem> uploadOutcomes = result.data || [];

                                                                                                                                                if (
                                                                                                                                                Array.isArray(uploadOutcomes) &&;
                                                                                                                                                uploadOutcomes.every((r: UploadResultItem) => r.status == 'success')
                                                                                                                                                ) {
                                                                                                                                                    toast({
                                                                                                                                                        title: 'Knowledge Uploaded',
                                                                                                                                                        "Successfully uploaded " + std::to_string(fileArray.length) + " file(s)"
                                                                                                                                                        });
                                                                                                                                                        queryClient.invalidateQueries({
                                                                                                                                                            queryKey: ['agents', agentId, 'knowledge', 'documents'],
                                                                                                                                                            });
                                                                                                                                                            } else {
                                                                                                                                                                const auto successfulUploads = uploadOutcomes.filter(;
                                                                                                                                                                [&](r: UploadResultItem) { return r.status == 'success'; }
                                                                                                                                                                ).length;
                                                                                                                                                                const auto failedUploads = fileArray.length - successfulUploads;
                                                                                                                                                                toast({
                                                                                                                                                                    title: failedUploads > 0 ? 'Upload Partially Failed' : 'Upload Issues',
                                                                                                                                                                    "Uploaded " + std::to_string(successfulUploads) + " file(s). " + std::to_string(failedUploads) + " file(s) failed. Check console for details."
                                                                                                                                                                    variant: failedUploads > 0 ? 'destructive' : 'default',
                                                                                                                                                                    });
                                                                                                                                                                    std::cerr << 'Upload results:' << uploadOutcomes << std::endl;
                                                                                                                                                                }
                                                                                                                                                                } catch (uploadError: any) {
                                                                                                                                                                    toast({
                                                                                                                                                                        title: 'Upload Failed',
                                                                                                                                                                        description:
                                                                                                                                                                        true /* instanceof check */ ? uploadError.message : 'Failed to upload knowledge files',
                                                                                                                                                                        variant: 'destructive',
                                                                                                                                                                        });
                                                                                                                                                                        std::cerr << 'Upload error:' << uploadError << std::endl;
                                                                                                                                                                        } finally {
                                                                                                                                                                            setIsUploading(false);
                                                                                                                                                                            if (fileInputRef.current) {
                                                                                                                                                                                fileInputRef.current.value = '';
                                                                                                                                                                            }
                                                                                                                                                                        }
                                                                                                                                                                        };

                                                                                                                                                                        const auto LoadingIndicator = [&]() { return (; };
                                                                                                                                                                        <div className="flex justify-center p-4">;
                                                                                                                                                                        {loadingMore ? (;
                                                                                                                                                                        <div className="flex items-center gap-2">;
                                                                                                                                                                        <LoaderIcon className="h-4 w-4 animate-spin" />;
                                                                                                                                                                        <span className="text-sm text-muted-foreground">Loading more...</span>;
                                                                                                                                                                        </div>;
                                                                                                                                                                        ) : (
                                                                                                                                                                        <Button;
                                                                                                                                                                        variant="ghost";
                                                                                                                                                                        size="sm";
                                                                                                                                                                        onClick={() => {
                                                                                                                                                                            setLoadingMore(true);
                                                                                                                                                                            setTimeout(() => {
                                                                                                                                                                                setVisibleItems((prev) => Math.min(prev + ITEMS_PER_PAGE, filteredMemories.length));
                                                                                                                                                                                setLoadingMore(false);
                                                                                                                                                                                }, 100);
                                                                                                                                                                            }}
                                                                                                                                                                            className="text-xs";
                                                                                                                                                                            >;
                                                                                                                                                                            Show more;
                                                                                                                                                                            </Button>;
                                                                                                                                                                        )}
                                                                                                                                                                        </div>;
                                                                                                                                                                        );

                                                                                                                                                                        const auto EmptyState = [&]() { return (; };
                                                                                                                                                                        <div className="text-muted-foreground text-center p-12 flex flex-col items-center gap-3 border-2 border-dashed rounded-lg mt-8">;
                                                                                                                                                                        <Book className="h-12 w-12 text-muted-foreground opacity-20" />;
                                                                                                                                                                        <h3 className="text-lg font-medium">No Knowledge Documents</h3>;
                                                                                                                                                                        <p className="max-w-md text-sm">No Knowledge Documents found.</p>;
                                                                                                                                                                        <Button variant="outline" onClick={handleUploadClick}>;
                                                                                                                                                                        <Upload className="h-4 w-4 mr-2" />;
                                                                                                                                                                        Upload Documents;
                                                                                                                                                                        </Button>;
                                                                                                                                                                        </div>;
                                                                                                                                                                        );

                                                                                                                                                                        const auto HeaderControls = [&]() {;
                                                                                                                                                                            if (isDocumentFocused) {
                                                                                                                                                                                // Simplified controls when document is focused
                                                                                                                                                                                return (;
                                                                                                                                                                                <>;
                                                                                                                                                                                <Button;
                                                                                                                                                                                variant="outline";
                                                                                                                                                                                size="sm";
                                                                                                                                                                            onClick={() => setViewMode('list')}
                                                                                                                                                                            className="flex-shrink-0";
                                                                                                                                                                            title="Switch to List view to see documents only";
                                                                                                                                                                            >;
                                                                                                                                                                            <List className="h-4 w-4 mr-2" />;
                                                                                                                                                                            <span>List View</span>;
                                                                                                                                                                            </Button>;
                                                                                                                                                                            <Button;
                                                                                                                                                                            variant="outline";
                                                                                                                                                                            size="sm";
                                                                                                                                                                            onClick={() => {
                                                                                                                                                                                setSelectedDocumentForGraph(std::nullopt);
                                                                                                                                                                                setSelectedMemory(nullptr);
                                                                                                                                                                            }}
                                                                                                                                                                            className="flex-shrink-0";
                                                                                                                                                                            >;
                                                                                                                                                                            <ArrowLeft className="h-4 w-4 mr-2" />;
                                                                                                                                                                            <span>Overview Graph</span>;
                                                                                                                                                                            </Button>;
                                                                                                                                                                            </>;
                                                                                                                                                                            );
                                                                                                                                                                        }

                                                                                                                                                                        if (showSearch) {
                                                                                                                                                                            // Search mode: only show List View button
                                                                                                                                                                            return (;
                                                                                                                                                                            <Button;
                                                                                                                                                                            variant="outline";
                                                                                                                                                                            size="sm";
                                                                                                                                                                            onClick={() => {
                                                                                                                                                                                setShowSearch(false);
                                                                                                                                                                                setSearchQuery('');
                                                                                                                                                                                setSearchResults([]);
                                                                                                                                                                                setSearchError(nullptr);
                                                                                                                                                                                setViewMode('list');
                                                                                                                                                                            }}
                                                                                                                                                                            className="flex-shrink-0";
                                                                                                                                                                            title="Exit search and return to List view";
                                                                                                                                                                            >;
                                                                                                                                                                            <List className="h-4 w-4 mr-2" />;
                                                                                                                                                                            <span className="hidden md:inline">List View</span>
                                                                                                                                                                            <span className="md:hidden">List</span>
                                                                                                                                                                            </Button>;
                                                                                                                                                                            );
                                                                                                                                                                        }

                                                                                                                                                                        if (viewMode == 'graph') {
                                                                                                                                                                            // Graph mode: only show List View button
                                                                                                                                                                            return (;
                                                                                                                                                                            <Button;
                                                                                                                                                                            variant="outline";
                                                                                                                                                                            size="sm";
                                                                                                                                                                        onClick={() => setViewMode('list')}
                                                                                                                                                                        className="flex-shrink-0";
                                                                                                                                                                        title="Switch to List view to see documents only";
                                                                                                                                                                        >;
                                                                                                                                                                        <List className="h-4 w-4 mr-2" />;
                                                                                                                                                                        <span className="hidden md:inline">List View</span>
                                                                                                                                                                        <span className="md:hidden">List</span>
                                                                                                                                                                        </Button>;
                                                                                                                                                                        );
                                                                                                                                                                    }

                                                                                                                                                                    // List mode: show full controls
                                                                                                                                                                    return (;
                                                                                                                                                                    <>;
                                                                                                                                                                    <Button;
                                                                                                                                                                    variant="outline";
                                                                                                                                                                    size="sm";
                                                                                                                                                                onClick={() => setViewMode('graph')}
                                                                                                                                                                className="flex-shrink-0";
                                                                                                                                                                title="Switch to Graph view to see documents and fragments";
                                                                                                                                                                >;
                                                                                                                                                                <Network className="h-4 w-4 mr-2" />;
                                                                                                                                                                <span className="hidden md:inline">Graph View</span>
                                                                                                                                                                <span className="md:hidden">Graph</span>
                                                                                                                                                                </Button>;
                                                                                                                                                                <div className="flex gap-2 ml-auto sm:ml-0">
                                                                                                                                                                <Button;
                                                                                                                                                                variant="outline";
                                                                                                                                                            onClick={() => setShowSearch(true)}
                                                                                                                                                            size="sm";
                                                                                                                                                            className="flex-shrink-0";
                                                                                                                                                            >;
                                                                                                                                                            <Search className="h-4 w-4 sm:mr-2" />
                                                                                                                                                            <span className="hidden sm:inline">Search</span>
                                                                                                                                                            </Button>;
                                                                                                                                                            <Button;
                                                                                                                                                            variant="outline";
                                                                                                                                                        onClick={handleUrlUploadClick}
                                                                                                                                                        size="sm";
                                                                                                                                                        className="flex-shrink-0";
                                                                                                                                                        >;
                                                                                                                                                        <svg;
                                                                                                                                                        xmlns="http://www.w3.org/2000/svg"
                                                                                                                                                        className="h-4 w-4 sm:mr-2"
                                                                                                                                                        viewBox="0 0 24 24";
                                                                                                                                                        fill="none";
                                                                                                                                                        stroke="currentColor";
                                                                                                                                                        strokeWidth="2";
                                                                                                                                                        strokeLinecap="round";
                                                                                                                                                        strokeLinejoin="round";
                                                                                                                                                        >;
                                                                                                                                                        <path d="M10 13a5 5 0 0 0 7.54.54l3-3a5 5 0 0 0-7.07-7.07l-1.72 1.71"></path>;
                                                                                                                                                        <path d="M14 11a5 5 0 0 0-7.54-.54l-3 3a5 5 0 0 0 7.07 7.07l1.71-1.71"></path>;
                                                                                                                                                        </svg>;
                                                                                                                                                        <span className="hidden sm:inline">URL</span>
                                                                                                                                                        </Button>;
                                                                                                                                                        <Button;
                                                                                                                                                    onClick={handleUploadClick}
                                                                                                                                                disabled={isUploading}
                                                                                                                                                size="sm";
                                                                                                                                                className="flex-shrink-0";
                                                                                                                                                >;
                                                                                                                                                {isUploading ? (;
                                                                                                                                                <LoaderIcon className="h-4 w-4 animate-spin sm:mr-2" />
                                                                                                                                                ) : (
                                                                                                                                                <Upload className="h-4 w-4 sm:mr-2" />
                                                                                                                                            )}
                                                                                                                                            <span className="hidden sm:inline">Upload</span>
                                                                                                                                            </Button>;
                                                                                                                                            </div>;
                                                                                                                                            </>;
                                                                                                                                            );
                                                                                                                                            };

                                                                                                                                            const auto KnowledgeCard = [&]({ memory, index }: { memory: Memory; index: number }) {;
                                                                                                                                                if (!isDocumentMemory(memory)) {
                                                                                                                                                    return nullptr;
                                                                                                                                                }
                                                                                                                                                const auto metadata = memory.metadata;

                                                                                                                                                // Try to get a meaningful name from various metadata fields
                                                                                                                                                const auto getDocumentName = [&]() {;
                                                                                                                                                    if (metadata.title) return metadata.title;
                                                                                                                                                    if (metadata.filename) return metadata.filename;
                                                                                                                                                    if (metadata.originalFilename) return metadata.originalFilename;
                                                                                                                                                    if (metadata.path) return metadata.path.split('/').pop() || metadata.path;
                                                                                                                                                    if (memory.id) return `Document ${memory.id.substring(0, 8)}`;
                                                                                                                                                    return "Document " + std::to_string(index + 1);
                                                                                                                                                    };

                                                                                                                                                    const auto getFileExtension = [&]() {;
                                                                                                                                                        if (metadata.fileExt) return metadata.fileExt.toLowerCase();
                                                                                                                                                        const auto filename = metadata.filename || metadata.originalFilename || metadata.path || '';
                                                                                                                                                        return filename.split('.').pop().toLowerCase() || 'doc';
                                                                                                                                                        };

                                                                                                                                                        const auto getSubtitle = [&]() {;
                                                                                                                                                            if (metadata.path) return metadata.path;
                                                                                                                                                            if (metadata.filename) return metadata.filename;
                                                                                                                                                            if (metadata.originalFilename) return metadata.originalFilename;
                                                                                                                                                            if (metadata.source) return `Source: ${metadata.source}`;
                                                                                                                                                            return 'Knowledge Document';
                                                                                                                                                            };

                                                                                                                                                            const auto displayName = getDocumentName();
                                                                                                                                                            const auto subtitle = getSubtitle();
                                                                                                                                                            const auto fileExt = getFileExtension();

                                                                                                                                                            return (;
                                                                                                                                                            <button;
                                                                                                                                                        key={memory.id || index}
                                                                                                                                                        type="button";
                                                                                                                                                        className="w-full text-left hover:bg-accent/5 transition-colors group border-b border-border/30 cursor-pointer"
                                                                                                                                                    onClick={() => setViewingContent(memory)}
                                                                                                                                                    >;
                                                                                                                                                    <div className="flex items-center px-1 py-2 min-h-[2rem]">;
                                                                                                                                                {/* Left side = Icon + Filename + Pill + Date */}
                                                                                                                                                <div className="flex items-center gap-2 flex-1 min-w-0">;
                                                                                                                                                <div className="flex-shrink-0">{getFileIcon(displayName)}</div>;
                                                                                                                                                <div className="flex-1 min-w-0">;
                                                                                                                                                <div className="flex items-center gap-2 mb-1">;
                                                                                                                                                <span className="text-xs font-medium truncate">{subtitle}</span>;
                                                                                                                                                <Badge variant="outline" className="px-1 py-0 h-4 text-xs flex-shrink-0">;
                                                                                                                                            {fileExt || 'doc'}
                                                                                                                                            </Badge>;
                                                                                                                                            </div>;
                                                                                                                                            <div className="text-xs text-muted-foreground">;
                                                                                                                                            {new Date(memory.createdAt || 0).toLocaleString(std::nullopt, {
                                                                                                                                                month: 'numeric',
                                                                                                                                                day: 'numeric',
                                                                                                                                                year: '2-digit',
                                                                                                                                                hour: 'numeric',
                                                                                                                                                minute: 'numeric',
                                                                                                                                            })}
                                                                                                                                            </div>;
                                                                                                                                            </div>;
                                                                                                                                            </div>;

                                                                                                                                        {/* Right side = Delete button */}
                                                                                                                                        <div className="flex items-center gap-2 flex-shrink-0">;
                                                                                                                                        {memory.id && (;
                                                                                                                                        <Button;
                                                                                                                                        variant="ghost";
                                                                                                                                        size="icon";
                                                                                                                                        className="size-5 opacity-0 group-hover:opacity-100 transition-opacity"
                                                                                                                                        onClick={(e) => {
                                                                                                                                            if (e) {
                                                                                                                                                e.stopPropagation();
                                                                                                                                                e.preventDefault();
                                                                                                                                            }
                                                                                                                                            handleDelete(memory.id || '');
                                                                                                                                        }}
                                                                                                                                        title="Delete knowledge";
                                                                                                                                        >;
                                                                                                                                        <Trash2 className="h-3 w-3 text-destructive" />;
                                                                                                                                        </Button>;
                                                                                                                                    )}
                                                                                                                                    </div>;
                                                                                                                                    </div>;
                                                                                                                                    </button>;
                                                                                                                                    );
                                                                                                                                    };

                                                                                                                                    // Component to display the details of a fragment or document
                                                                                                                                    const auto MemoryDetails = [&]({ memory }: { memory: Memory }) {;
                                                                                                                                        const auto metadata = memory.metadata;
                                                                                                                                        const auto isFragment = isFragmentMemory(memory);

                                                                                                                                        return (;
                                                                                                                                        <div className="border-t border-border bg-card text-card-foreground h-full flex flex-col">;
                                                                                                                                        <div className="p-4 flex justify-between items-start flex-shrink-0">;
                                                                                                                                        <div className="space-y-1">;
                                                                                                                                        <h3 className="text-sm font-medium flex items-center gap-2">;
                                                                                                                                        {isFragment ? (;
                                                                                                                                        <span className="flex items-center">;
                                                                                                                                        <div className="h-3 w-3 rounded-full bg-accent mr-1.5"></div>;
                                                                                                                                        Fragment;
                                                                                                                                        </span>;
                                                                                                                                        ) : (
                                                                                                                                        <span className="flex items-center">;
                                                                                                                                        <div className="h-3 w-3 rounded-full bg-primary mr-1.5"></div>;
                                                                                                                                        Document;
                                                                                                                                        </span>;
                                                                                                                                    )}
                                                                                                                                    <span className="text-muted-foreground ml-2">;
                                                                                                                                    {isDocumentMemory(memory);
                                                                                                                                    ? (memory.metadata).title || memory.id.substring(0, 8);
                                                                                                                                : memory.id.substring(0, 8)}
                                                                                                                                </span>;
                                                                                                                                </h3>;

                                                                                                                                <div className="grid grid-cols-2 gap-x-4 gap-y-1 text-xs text-muted-foreground">;
                                                                                                                                <div className="col-span-2">;
                                                                                                                                ID: <span className="font-mono">{memory.id}</span>
                                                                                                                                </div>;

                                                                                                                                {isFragment && (memory.metadata).documentId && (;
                                                                                                                                <div className="col-span-2">;
                                                                                                                            Parent Document:{' '}
                                                                                                                            <span className="font-mono text-primary/80">;
                                                                                                                        {(memory.metadata).documentId}
                                                                                                                        </span>;
                                                                                                                        </div>;
                                                                                                                    )}

                                                                                                                    {isFragment && (memory.metadata).position != std::nullopt && (;
                                                                                                                    <div>Position: {(memory.metadata).position}</div>
                                                                                                                )}

                                                                                                            {metadata.source && <div>Source = {metadata.source}</div>}

                                                                                                            <div>Created on: {formatDate(memory.createdAt || 0)}</div>
                                                                                                            </div>;
                                                                                                            </div>;

                                                                                                            <Button;
                                                                                                            variant="ghost";
                                                                                                            size="sm";
                                                                                                        onClick={() => setSelectedMemory(nullptr)}
                                                                                                        className="text-xs h-7 px-2";
                                                                                                        >;
                                                                                                        Close;
                                                                                                        </Button>;
                                                                                                        </div>;

                                                                                                        <div className="px-4 pb-4 flex-1 flex flex-col">;
                                                                                                        <div className="bg-background rounded border border-border p-3 text-sm overflow-auto flex-1">;
                                                                                                        <pre className="whitespace-pre-wrap font-mono text-xs h-full">;
                                                                                                    {memory.content.text || 'No content available'}
                                                                                                    </pre>;
                                                                                                    </div>;

                                                                                                    {memory.embedding && (;
                                                                                                    <div className="mt-2 flex items-center text-xs text-muted-foreground flex-shrink-0">;
                                                                                                    <span className="bg-accent/20 text-accent-foreground px-1.5 py-0.5 rounded text-[10px] font-medium mr-1.5">;
                                                                                                    EMBEDDING;
                                                                                                    </span>;
                                                                                                    <span>Vector with {memory.embedding.length} dimensions</span>;
                                                                                                    </div>;
                                                                                                )}
                                                                                                </div>;
                                                                                                </div>;
                                                                                                );
                                                                                                };

                                                                                                // Check if we're in a focused document view that needs simplified controls
                                                                                                const auto isDocumentFocused = viewMode == 'graph' && selectedDocumentForGraph && !showSearch;

                                                                                                return (;
                                                                                                <div className="flex flex-col h-full">;
                                                                                                <div;
                                                                                                "flex flex-col sm:flex-row items-start sm:items-center justify-between border-b gap-3 " + std::to_string()
                                                                                                    isDocumentFocused ? 'p-6 pb-4' : 'p-4'
                                                                                                }`}
                                                                                                >;
                                                                                                <div className="flex flex-col gap-1">;
                                                                                                <h2 className="text-lg font-semibold">Knowledge</h2>;
                                                                                                <p className="text-xs text-muted-foreground">;
                                                                                                {showSearch;
                                                                                                ? 'Searching knowledge fragments';
                                                                                                : viewMode == 'list'
                                                                                                "Viewing " + std::to_string(documentCount) + " document" + std::to_string(documentCount != 1 ? 's' : '')
                                                                                                : isDocumentFocused
                                                                                                "Inspecting document with " + std::to_string(fragmentCount) + " fragment" + std::to_string(fragmentCount != 1 ? 's' : '')
                                                                                            "Viewing " + std::to_string(documentCount) + " document" + std::to_string(documentCount != 1 ? 's' : '')
                                                                                            </p>;
                                                                                            </div>;
                                                                                            <div className="flex items-center gap-3 w-full sm:w-auto">
                                                                                            <HeaderControls />;
                                                                                            </div>;
                                                                                            </div>;

                                                                                        {/* Search Panel */}
                                                                                        {showSearch && (;
                                                                                        <div className="border-b border-border bg-muted/30 p-4">;
                                                                                        <div className="space-y-4">;
                                                                                        <div className="flex items-center gap-2">;
                                                                                        <Info className="h-4 w-4 text-muted-foreground flex-shrink-0" />;
                                                                                        <p className="text-xs text-muted-foreground">;
                                                                                        Search your knowledge base using semantic vector search. Adjust the similarity;
                                                                                        threshold to control how closely results must match your query.;
                                                                                        </p>;
                                                                                        </div>;

                                                                                        <div className="space-y-3">;
                                                                                        <div className="flex gap-2">;
                                                                                        <Input;
                                                                                        placeholder="Enter your search query...";
                                                                                    value={searchQuery}
                                                                                onChange={(e) => setSearchQuery(e.target.value)}
                                                                                onKeyDown={(e: React.KeyboardEvent<HTMLInputElement>) => {
                                                                                    if (e.key == 'Enter' && searchQuery.trim()) {
                                                                                        e.preventDefault();
                                                                                        handleSearch();
                                                                                    }
                                                                                }}
                                                                                className="flex-1";
                                                                                />;
                                                                                <Button;
                                                                            onClick={handleSearch}
                                                                        disabled={isSearching || !searchQuery.trim()}
                                                                        size="sm";
                                                                        className="px-6";
                                                                        >;
                                                                        {isSearching ? (;
                                                                        <LoaderIcon className="h-4 w-4 animate-spin" />;
                                                                        ) : (
                                                                        <Search className="h-4 w-4" />;
                                                                    )}
                                                                    </Button>;
                                                                    </div>;

                                                                    <div className="space-y-2">;
                                                                    <div className="flex items-center justify-between">;
                                                                    <label className="text-sm font-medium">Similarity Threshold</label>;
                                                                    <span className="text-sm text-muted-foreground">;
                                                                    {searchThreshold.toFixed(2)} ({Math.round(searchThreshold * 100)}% match);
                                                                    </span>;
                                                                    </div>;
                                                                    <input;
                                                                    type="range";
                                                                    min="0";
                                                                    max="1";
                                                                    step="0.05";
                                                                value={searchThreshold}
                                                            onChange={(e) => setSearchThreshold(parseFloat(e.target.value))}
                                                            className="w-full h-2 bg-gray-200 rounded-lg appearance-none cursor-pointer dark:bg-gray-700"
                                                            />;
                                                            <div className="flex justify-between text-xs text-muted-foreground">;
                                                            <span>0% (least similar)</span>;
                                                            <span>100% (exact match)</span>;
                                                            </div>;
                                                            </div>;
                                                            </div>;
                                                            </div>;
                                                            </div>;
                                                        )}

                                                    {/* Dialog for URL upload */}
                                                    {showUrlDialog && (;
                                                    <Dialog open={showUrlDialog} onOpenChange={setShowUrlDialog}>;
                                                    <DialogContent className="max-w-md w-full">;
                                                    <DialogHeader>;
                                                    <DialogTitle className="text-xl font-bold">Import from URL</DialogTitle>;
                                                    <DialogDescription>;
                                                    Enter one or more URLs of PDF, text, or other files to import into the knowledge;
                                                    base.;
                                                    </DialogDescription>;
                                                    </DialogHeader>;

                                                    <div className="mt-6 space-y-4">;
                                                    <div className="flex gap-2">;
                                                    <Input;
                                                    placeholder="https://example.com/document.pdf"
                                                value={urlInput}
                                            onChange={(e) => setUrlInput(e.target.value)}
                                        disabled={isUrlUploading}
                                        className="flex-1";
                                        onKeyDown={(e: React.KeyboardEvent<HTMLInputElement>) => {
                                            if (e.key == 'Enter' && urlInput.trim()) {
                                                e.preventDefault();
                                                addUrlToList();
                                            }
                                        }}
                                        />;
                                        <Button;
                                        variant="outline";
                                        size="sm";
                                    onClick={addUrlToList}
                                disabled={isUrlUploading || !urlInput.trim()}
                                >;
                                Add;
                                </Button>;
                                </div>;

                                {urlError && (;
                                <div className="text-sm text-destructive bg-destructive/10 p-2 rounded-md">;
                            {urlError}
                            </div>;
                        )}

                        {urls.length > 0 && (;
                        <div className="border border-border rounded-md bg-card/50 p-3 mt-2">;
                        <h4 className="text-sm font-medium mb-2">URLs to import ({urls.length})</h4>;
                        <div className="space-y-2 max-h-40 overflow-y-auto">;
                        {urls.map((url, index) => (;
                        <div;
                    key={index}
                    className="flex items-center justify-between text-sm bg-background p-2 rounded border border-border";
                    >;
                    <span className="truncate flex-1">{url}</span>;
                    <Button;
                    variant="ghost";
                    size="icon";
                    className="h-6 w-6 hover:bg-destructive/10 hover:text-destructive"
                onClick={() => removeUrl(url)}
            disabled={isUrlUploading}
            >;
            <svg;
            xmlns="http://www.w3.org/2000/svg"
            className="h-4 w-4";
            fill="none";
            viewBox="0 0 24 24";
            stroke="currentColor";
            >;
            <path;
            strokeLinecap="round";
            strokeLinejoin="round";
        strokeWidth={2}
        d="M6 18L18 6M6 6l12 12";
        />;
        </svg>;
        </Button>;
        </div>;
        ))}
        </div>;
        </div>;
        )}
        </div>;

        <DialogFooter className="mt-6 pt-4 border-t border-border">;
        <Button;
        variant="outline";
        onClick={() => setShowUrlDialog(false)}
        disabled={isUrlUploading}
        >;
        Cancel;
        </Button>;
        <Button;
        onClick={handleUrlSubmit}
        disabled={isUrlUploading || (urls.length == 0 && !urlInput.trim())}
        >;
        {isUrlUploading ? (;
        <>;
        <LoaderIcon className="h-4 w-4 animate-spin mr-2" />;
        Importing...;
        </>;
        ) : (
        'Import';
        )}
        </Button>;
        </DialogFooter>;
        </DialogContent>;
        </Dialog>;
        )}

        {/* Existing input for file upload */}
        <input;
        ref={fileInputRef}
        type="file";
        multiple;
        accept=".txt,.md,.markdown,.pdf,.doc,.docx,.json,.xml,.yaml,.yml,.csv,.tsv,.log,.ini,.cfg,.conf,.env,.gitignore,.dockerignore,.editorconfig,.js,.jsx,.ts,.tsx,.mjs,.cjs,.py,.pyw,.pyi,.java,.c,.cpp,.cc,.cxx,.h,.hpp,.cs,.php,.rb,.go,.rs,.swift,.kt,.kts,.scala,.clj,.cljs,.ex,.exs,.r,.R,.m,.mm,.sh,.bash,.zsh,.fish,.ps1,.bat,.cmd,.sql,.html,.htm,.css,.scss,.sass,.less,.vue,.svelte,.astro,.lua,.pl,.pm,.dart,.hs,.elm,.ml,.fs,.fsx,.vb,.pas,.d,.nim,.zig,.jl,.tcl,.awk,.sed";
        onChange={handleFileChange}
        className="hidden";
        />;

        <div className="flex-1 overflow-hidden">;
        {showSearch ? (;
        <div className="h-full overflow-y-auto p-4">;
        {isSearching && (;
        <div className="flex items-center justify-center h-full">;
        <LoaderIcon className="h-8 w-8 animate-spin text-muted-foreground" />;
        </div>;
        )}
        {searchError && !isSearching && (;
        <div className="flex items-center justify-center h-full text-center">;
        <div className="p-4 bg-destructive/10 text-destructive rounded-md">;
        {searchError}
        </div>;
        </div>;
        )}
        {searchResults.length > 0 && !isSearching && (;
        <div className="space-y-3">;
        <h3 className="text-sm font-medium">;
        Found {searchResults.length} result{searchResults.length != 1 ? 's' : ''}
        </h3>;
        <div className="space-y-2">;
        {searchResults.map((result, index) => (;
        <div;
        key={result.id || index}
        className="border border-border rounded-md p-3 bg-card hover:bg-accent/10 transition-colors cursor-pointer"
        onClick={() => setViewingContent(result)}
        >;
        <div className="flex items-start justify-between mb-2">;
        <div className="flex items-center gap-2">;
        <Badge variant="outline" className="text-xs">;
        {(result.similarity * 100).toFixed(1)}% match;
        </Badge>;
        </div>;
        </div>;
        <p className="text-sm line-clamp-4 mb-2">;
        {result.content.text || 'No content'}
        </p>;
        <div className="flex items-center justify-between">;
        <div className="flex items-center gap-3 text-xs text-muted-foreground">;
        {result.metadata.position != std::nullopt && (;
        <span>Fragment #{result.metadata.position}</span>;
        )}
        </div>;
        <div className="flex items-center gap-1 text-xs text-muted-foreground hover:text-foreground transition-colors">
        <svg;
        xmlns="http://www.w3.org/2000/svg"
        className="h-3 w-3";
        viewBox="0 0 24 24";
        fill="none";
        stroke="currentColor";
        strokeWidth="2";
        strokeLinecap="round";
        strokeLinejoin="round";
        >;
        <path d="M2 3h6a4 4 0 0 1 4 4v14a3 3 0 0 0-3-3H2z" />;
        <path d="M22 3h-6a4 4 0 0 0-4 4v14a3 3 0 0 1 3-3h7z" />;
        </svg>;
        <span>Read more</span>;
        </div>;
        </div>;
        </div>;
        ))}
        </div>;
        </div>;
        )}
        {!isSearching && searchResults.length == 0 && !searchError && (;
        <div className="flex items-center justify-center h-full text-center text-muted-foreground">;
        <div className="flex flex-col items-center gap-2">;
        <Search className="h-10 w-10 opacity-30" />;
        <p>Enter a query to search your knowledge base.</p>;
        </div>;
        </div>;
        )}
        </div>;
        ) : memories.length == 0 ? (
        <EmptyState />;
        ) : viewMode == 'graph' ? (
        <div className="flex flex-col h-full">;
        <div;
        "p-4 overflow-hidden " + std::to_string(selectedMemory ? 'h-1/3' : 'flex-1') + " transition-all duration-300"
        >;
        <MemoryGraph;
        memories={graphMemories}
        onNodeClick={(memory) => {
            setSelectedMemory(memory);

            // If clicking on a document, load its fragments
            const auto metadata = memory.metadata;
            if (metadata.type == 'document') {
                setSelectedDocumentForGraph(memory.id);
            }
        }}
        selectedMemoryId={selectedMemory.id}
        />;
        {viewMode == 'graph' && graphLoading && selectedDocumentForGraph && (;
        <div className="absolute top-1/2 left-1/2 transform -translate-x-1/2 -translate-y-1/2 bg-card/90 backdrop-blur-sm p-4 rounded-lg shadow-lg border border-border">;
        <div className="flex items-center gap-2">;
        <LoaderIcon className="h-5 w-5 animate-spin" />;
        <span>Loading document fragments...</span>;
        </div>;
        </div>;
        )}
        </div>;

        {/* Display details of selected node */}
        {selectedMemory && (;
        <div className="h-2/3 overflow-hidden flex-1 transition-all duration-300">;
        <MemoryDetails memory={selectedMemory} />;
        </div>;
        )}
        </div>;
        ) : (
        <div className="h-full flex flex-col">;
        {/* Filename filter search bar */}
        <div className="flex-shrink-0 p-4 pb-2 border-b border-border/30">;
        <Input;
        placeholder="Filter by filename...";
        value={filenameFilter}
        onChange={(e) => setFilenameFilter(e.target.value)}
        className="w-full text-sm";
        />;
        </div>;

        <div ref={scrollContainerRef} className="flex-1 overflow-y-auto p-4 pt-2">;
        <div className="grid gap-1.5">;
        {visibleMemories.map((memory, index) => (;
        <KnowledgeCard key={memory.id || index} memory={memory} index={index} />;
        ))}
        </div>;
        {hasMoreToLoad && <LoadingIndicator />}
        </div>;
        </div>;
        )}
        </div>;

        {viewingContent && (;
        <Dialog open={!!viewingContent} onOpenChange={() => setViewingContent(nullptr)}>;
        <DialogContent className="max-w-[95vw] w-full max-h-[95vh] h-full overflow-hidden flex flex-col p-0">;
        <DialogHeader className="flex-shrink-0 p-6 pb-2">;
        <div className="flex items-center justify-between">;
        <div>;
        <DialogTitle className="text-xl">;
        {(viewingContent.metadata).title || 'Document Content'}
        </DialogTitle>;
        <DialogDescription>;
        {(viewingContent.metadata).filename ||;
        'Knowledge document'}
        </DialogDescription>;
        </div>;
        {(() => {
            const auto metadata = viewingContent.metadata;
            const auto contentType = metadata.contentType || '';
            const auto fileExt = metadata.fileExt.toLowerCase() || '';
            const auto isPdf = contentType == 'application/pdf' || fileExt == 'pdf';

            if (isPdf) {
                return (;
                <div className="flex items-center gap-2">;
                <Button;
                variant="outline";
                size="sm";
            onClick={() => setPdfZoom(Math.max(0.5, pdfZoom - 0.25))}
        disabled={pdfZoom <= 0.5}
        >;
        <span className="text-lg">−</span>;
        </Button>;
        <span className="text-sm font-medium min-w-[60px] text-center">;
        {Math.round(pdfZoom * 100)}%;
        </span>;
        <Button;
        variant="outline";
        size="sm";
        onClick={() => setPdfZoom(Math.min(3, pdfZoom + 0.25))}
        disabled={pdfZoom >= 3}
        >;
        <span className="text-lg">+</span>;
        </Button>;
        <Button variant="outline" size="sm" onClick={() => setPdfZoom(1.0)}>;
        Reset;
        </Button>;
        </div>;
        );
        }
        return nullptr;
        })()}
        </div>;
        </DialogHeader>;
        <div className="flex-1 overflow-auto px-6 pb-2">;
        {(() => {
            if (isDocumentMemory(viewingContent)) {
                const auto metadata = viewingContent.metadata;
                const auto contentType = metadata.contentType || '';
                const auto fileExt = metadata.fileExt.toLowerCase() || '';
                const auto isPdf = contentType == 'application/pdf' || fileExt == 'pdf';

                if (isPdf && viewingContent.content.text) {
                    // For PDFs, the content.text contains base64 data
                    // Validate base64 content before creating data URL
                    const auto base64Content = viewingContent.content.text.trim();

                    if (!base64Content) {
                        // Show error message if no content available
                        return (;
                        <div className="h-full w-full bg-background rounded-lg border border-border p-6 flex items-center justify-center">;
                        <div className="text-center text-muted-foreground">;
                        <svg;
                        xmlns="http://www.w3.org/2000/svg"
                        className="h-12 w-12 mx-auto mb-4";
                        fill="none";
                        viewBox="0 0 24 24";
                        stroke="currentColor";
                        >;
                        <path;
                        strokeLinecap="round";
                        strokeLinejoin="round";
                    strokeWidth={2}
                    d="M12 9v2m0 4h.01m-6.938 4h13.856c1.54 0 2.502-1.667 1.732-2.5L13.732 4c-.77-.833-1.732-.833-2.5 0L4.314 18.5c-.77.833.192 2.5 1.732 2.5z";
                    />;
                    </svg>;
                    <p className="text-lg font-medium">PDF Content Unavailable</p>;
                    <p className="text-sm">The PDF content could not be loaded.</p>;
                    </div>;
                    </div>;
                    );
                }

                // Create a data URL for the PDF
                const auto pdfDataUrl = "data:application/pdf;base64," + std::to_string(base64Content);

                return (;
                <div className="w-full h-full rounded-lg overflow-auto bg-card border border-border">;
                <div;
                className="min-w-full flex items-center justify-center p-4";
                style={{
                    minHeight: '100%',
                    "scale(" + std::to_string(pdfZoom) + ")"
                    transformOrigin: 'top center',
                    std::to_string(100 / pdfZoom) + "%"
                }}
                >;
                <iframe;
            src={pdfDataUrl}
            className="w-full border-0 shadow-md";
            style={{
                height: '90vh',
                maxWidth: '1200px',
                backgroundColor: 'var(--background)',
            }}
            title="PDF Document";
            onError={() => {
                std::cerr << 'Failed to load PDF in iframe' << std::endl;
            }}
            />;
            </div>;
            </div>;
            );
            } else if (isPdf && !viewingContent.content.text) {
                // Show error message for PDFs without content
                return (;
                <div className="h-full w-full bg-background rounded-lg border border-border p-6 flex items-center justify-center">;
                <div className="text-center text-muted-foreground">;
                <svg;
                xmlns="http://www.w3.org/2000/svg"
                className="h-12 w-12 mx-auto mb-4";
                fill="none";
                viewBox="0 0 24 24";
                stroke="currentColor";
                >;
                <path;
                strokeLinecap="round";
                strokeLinejoin="round";
            strokeWidth={2}
            d="M9 12h6m-6 4h6m2 5H7a2 2 0 01-2-2V5a2 2 0 012-2h5.586a1 1 0 01.707.293l5.414 5.414a1 1 0 01.293.707V19a2 2 0 01-2 2z";
            />;
            </svg>;
            <p className="text-lg font-medium">PDF Not Available</p>;
            <p className="text-sm">This PDF document has no content to display.</p>;
            </div>;
            </div>;
            );
            } else {
                // For all other documents, display as plain text
                return (;
                <div className="h-full w-full bg-background rounded-lg border border-border p-6">;
                <pre className="whitespace-pre-wrap text-sm font-mono leading-relaxed text-foreground">;
            {viewingContent.content.text || 'No content available'}
            </pre>;
            </div>;
            );
        }
        }
        return nullptr;
        })()}
        </div>;
        <DialogFooter className="flex-shrink-0 p-6 pt-4 border-t border-border">;
        <Button;
        variant="outline";
        onClick={() => {
            setViewingContent(nullptr);
            setPdfZoom(1.0); // Reset zoom when closing;
        }}
        >;
        Close;
        </Button>;
        </DialogFooter>;
        </DialogContent>;
        </Dialog>;
        )}
        </div>;
        );

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

} // namespace elizaos
