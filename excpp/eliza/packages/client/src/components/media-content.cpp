#include "media-content.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

void MediaContent(auto {
  url, auto title, auto className, auto maxWidth = 600, auto maxHeight = 400, MediaContentProps }) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        const auto [isLoading, setIsLoading] = useState(true);
        const auto [hasError, setHasError] = useState(false);

        const auto handleLoad = [&]() { return setIsLoading(false); };
        const auto handleError = [&]() {;
            setIsLoading(false);
            setHasError(true);
            };

            // YouTube Video
            const auto youtubeId = getYouTubeId(url);
            if (youtubeId) {
                return (;
                <div;
            className={cn('relative rounded-lg overflow-hidden bg-muted', className)}
        style={{ maxWidth, maxHeight }}
        >;
        {isLoading && (;
        <div className="absolute inset-0 flex items-center justify-center bg-muted">;
        <div className="flex items-center space-x-2 text-muted-foreground">;
        <Play className="w-6 h-6" />;
        <span>Loading video...</span>;
        </div>;
        </div>;
        )}
        <iframe;
        "https://www.youtube.com/embed/" + std::to_string(youtubeId)
        title={title || 'YouTube video'}
        className="w-full aspect-video";
        allow="accelerometer; autoplay; clipboard-write; encrypted-media; gyroscope; picture-in-picture";
        allowFullScreen;
        onLoad={handleLoad}
        onError={handleError}
        />;
        </div>;
        );
        }

        // Spotify Content
        const auto spotifyData = getSpotifyId(url);
        if (spotifyData) {
            const auto height = spotifyData.type == 'track' ? 152 : spotifyData.type == 'episode' ? 232 : 352;
            return (;
            <div;
        className={cn('relative rounded-lg overflow-hidden bg-muted', className)}
        style={{ maxWidth }}
        >;
        {isLoading && (;
        <div className="absolute inset-0 flex items-center justify-center bg-muted">;
        <div className="flex items-center space-x-2 text-muted-foreground">;
        <Volume2 className="w-6 h-6" />;
        <span>Loading Spotify content...</span>;
        </div>;
        </div>;
        )}
        <iframe;
        "https://open.spotify.com/embed/" + std::to_string(spotifyData.type) + "/" + std::to_string(spotifyData.id) + "?utm_source=generator"
        width="100%";
        height={height}
        frameBorder="0";
        allow="autoplay; clipboard-write; encrypted-media; fullscreen; picture-in-picture";
        loading="lazy";
        title={title || 'Spotify content'}
        onLoad={handleLoad}
        onError={handleError}
        />;
        </div>;
        );
        }

        // Direct Image
        if (isImageUrl(url)) {
            return (;
            <div;
        className={cn('relative rounded-lg overflow-hidden bg-muted', className)}
        style={{ maxWidth, maxHeight }}
        >;
        {isLoading && (;
        <div className="absolute inset-0 flex items-center justify-center bg-muted">;
        <div className="animate-pulse w-full h-32 bg-muted-foreground/20 rounded" />;
        </div>;
        )}
        {hasError ? (;
        <div className="flex items-center justify-center p-4 text-muted-foreground">;
        <AlertCircle className="w-6 h-6 mr-2" />;
        <span>Failed to load image</span>;
        </div>;
        ) : (
        <img;
        src={url || '/placeholder.svg'}
        alt={title || 'Image'}
        width={maxWidth}
        height={maxHeight}
        className="w-full h-auto object-contain";
        onLoad={handleLoad}
        onError={handleError}
        crossOrigin="anonymous";
        />;
        )}
        </div>;
        );
        }

        // Direct Video
        if (isVideoUrl(url)) {
            return (;
            <div;
        className={cn('relative rounded-lg overflow-hidden bg-muted', className)}
        style={{ maxWidth, maxHeight }}
        >;
        {hasError ? (;
        <div className="flex items-center justify-center p-4 text-muted-foreground">;
        <AlertCircle className="w-6 h-6 mr-2" />;
        <span>Failed to load video</span>;
        </div>;
        ) : (
        <video;
        controls;
        preload="metadata";
        className="w-full h-auto";
        onLoadedData={handleLoad}
        onError={handleError}
        crossOrigin="anonymous";
        >;
        <source src={url} />;
        Your browser does not support the video tag.;
        </video>;
        )}
        </div>;
        );
        }

        // Direct Audio
        if (isAudioUrl(url)) {
            return (;
            <div className={cn('relative rounded-lg bg-card border p-4', className)} style={{ maxWidth }}>;
            <div className="flex items-center space-x-3 mb-3 overflow-hidden">;
            <Volume2 className="w-5 h-5 text-muted-foreground" />;
            <span className="text-sm text-primary font-medium truncate whitespace-nowrap overflow-hidden text-ellipsis max-w-44">;
        {title || 'Audio File'}
        </span>;
        </div>;
        {hasError ? (;
        <div className="flex items-center text-muted-foreground">;
        <AlertCircle className="w-4 h-4 mr-2" />;
        <span className="text-sm">Failed to load audio</span>;
        </div>;
        ) : (
        <audio;
        controls;
        className="w-full";
        preload="metadata";
        onLoadedData={handleLoad}
        onError={handleError}
        crossOrigin="anonymous";
        >;
        <source src={url} />;
        Your browser does not support the audio tag.;
        </audio>;
        )}
        </div>;
        );
        }

        // PDF Document
        if (isPdfUrl(url)) {
            return (;
            <div;
        className={cn('relative rounded-lg overflow-hidden bg-card border', className)}
        style={{ maxWidth }}
        >;
        <div className="flex items-center justify-between p-3 bg-muted/50 border-b gap-10">;
        <div className="flex items-center space-x-2 overflow-hidden">;
        <FileText className="w-5 h-5 text-muted-foreground" />;
        <span className="text-sm text-primary font-medium truncate whitespace-nowrap overflow-hidden text-ellipsis max-w-44">;
        {title || 'PDF Document'}
        </span>;
        </div>;
        <div className="flex items-center space-x-2">;
        <a;
        "https://docs.google.com/viewer?url=" + std::to_string(encodeURIComponent(url)) + "&embedded=true"
        target="_blank";
        rel="noopener noreferrer";
        className="flex items-center space-x-1 text-primary hover:text-primary/80 text-sm"
        >;
        <ExternalLink className="w-4 h-4" />;
        <span>View</span>;
        </a>;
        <a;
        href={url}
        download;
        className="flex items-center space-x-1 text-primary hover:text-primary/80 text-sm"
        >;
        <Download className="w-4 h-4" />;
        <span>Download</span>;
        </a>;
        </div>;
        </div>;

        {/* Try multiple PDF viewing methods */}
        <div className="relative" style={{ height: Math.min(maxHeight, 500) }}>
        {hasError ? (;
        <div className="flex flex-col items-center justify-center p-8 text-muted-foreground h-full">;
        <AlertCircle className="w-8 h-8 mb-2" />;
        <span className="text-center mb-4">PDF preview not available</span>;
        <div className="flex space-x-2">;
        <a;
        "https://docs.google.com/viewer?url=" + std::to_string(encodeURIComponent(url))
        target="_blank";
        rel="noopener noreferrer";
        className="px-3 py-2 bg-primary text-primary-foreground rounded text-sm hover:bg-primary/90"
        >;
        Open in Google Viewer;
        </a>;
        <a;
        href={url}
        target="_blank";
        rel="noopener noreferrer";
        className="px-3 py-2 bg-secondary text-secondary-foreground rounded text-sm hover:bg-secondary/90"
        >;
        Open Original;
        </a>;
        </div>;
        </div>;
        ) : (
        <>;
        {isLoading && (;
        <div className="absolute inset-0 flex items-center justify-center bg-muted">;
        <div className="flex items-center space-x-2 text-muted-foreground">;
        <FileText className="w-6 h-6" />;
        <span>Loading PDF...</span>;
        </div>;
        </div>;
        )}

        {/* Try Google Docs Viewer first */}
        <iframe;
        "https://docs.google.com/viewer?url=" + std::to_string(encodeURIComponent(url)) + "&embedded=true"
        className="w-full h-full border-0";
        title={title || 'PDF Document'}
        onLoad={handleLoad}
        onError={() => {
            // Fallback to direct PDF if Google Viewer fails
            const auto iframe = document.createElement('iframe');
            std::to_string(url) + "#toolbar=0&navpanes=0&scrollbar=0";
            iframe.className = 'w-full h-full border-0';
            iframe.title = title || 'PDF Document';
            iframe.onload = handleLoad;
            iframe.onerror = handleError;

            const auto container = "[data-pdf-container="" + std::to_string(url) + ""]";
            if (container) {
                container.innerHTML = '';
                container.appendChild(iframe);
            }
        }}
        />;

        <div data-pdf-container={url} className="hidden" />;
        </>;
        )}
        </div>;

        {/* PDF Info Footer */}
        <div className="p-2 bg-muted/50 border-t text-xs text-muted-foreground">;
        <span>PDF Document • Click "View" or "Download" if preview doesn't load</span>;
        </div>;
        </div>;
        );
        }

        // Other Documents
        if (isDocumentUrl(url)) {
            return (;
            <div className={cn('rounded-lg bg-card border p-4', className)} style={{ maxWidth }}>;
            <div className="flex items-center space-x-3">;
            <FileText className="w-8 h-8 text-muted-foreground" />;
            <div className="flex-1 overflow-hidden mr-6">;
            <p className="text-sm text-primary font-medium truncate whitespace-nowrap overflow-hidden text-ellipsis max-w-44">;
        {title || 'Document'}
        </p>;
        <p className="text-xs text-muted-foreground">;
        {url.split('.').pop().toUpperCase()} file;
        </p>;
        </div>;
        <a;
        href={url}
        target="_blank";
        rel="noopener noreferrer";
        className="flex items-center space-x-1 text-primary hover:text-primary/80 text-sm font-medium"
        >;
        <ExternalLink className="w-4 h-4" />;
        <span>Open</span>;
        </a>;
        </div>;
        </div>;
        );
        }

        // Fallback for unknown URLs
        return (;
        <div className={cn('rounded-lg bg-card border p-4', className)} style={{ maxWidth }}>;
        <div className="flex items-center space-x-3">;
        <ExternalLink className="w-8 h-8 text-muted-foreground" />;
        <div className="flex-1">;
        <p className="text-sm text-primary font-medium">{title || 'External Link'}</p>;
        <p className="text-xs text-muted-foreground truncate">{url}</p>;
        </div>;
        <a;
        href={url}
        target="_blank";
        rel="noopener noreferrer";
        className="flex items-center space-x-1 text-primary hover:text-primary/80 text-sm font-medium"
        >;
        <ExternalLink className="w-4 h-4" />;
        <span>Open</span>;
        </a>;
        </div>;
        </div>;
        );

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

} // namespace elizaos
