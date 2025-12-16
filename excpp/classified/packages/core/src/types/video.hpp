#include "service.hpp"
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

struct VideoInfo {
    std::optional<std::string> title;
    std::optional<double> duration;
    std::string url;
    std::optional<std::string> thumbnail;
    std::optional<std::string> description;
    std::optional<std::string> uploader;
    std::optional<double> viewCount;
    std::optional<Date> uploadDate;
    std::optional<std::vector<VideoFormat>> formats;
};


struct VideoFormat {
    std::string formatId;
    std::string url;
    std::string extension;
    std::string quality;
    std::optional<double> fileSize;
    std::optional<std::string> videoCodec;
    std::optional<std::string> audioCodec;
    std::optional<std::string> resolution;
    std::optional<double> fps;
    std::optional<double> bitrate;
};


struct VideoDownloadOptions {
    std::optional<std::string> format;
    std::optional<'best' | 'worst' | 'bestvideo' | 'bestaudio' | string> quality;
    std::optional<std::string> outputPath;
    std::optional<bool> audioOnly;
    std::optional<bool> videoOnly;
    std::optional<bool> subtitles;
    std::optional<bool> embedSubs;
    std::optional<bool> writeInfoJson;
};


struct VideoProcessingOptions {
    std::optional<double> startTime;
    std::optional<double> endTime;
    std::optional<std::string> outputFormat;
    std::optional<std::string> resolution;
    std::optional<std::string> bitrate;
    std::optional<double> framerate;
    std::optional<std::string> audioCodec;
    std::optional<std::string> videoCodec;
};


/**
 * Interface for video processing and download services
 */
abstract class IVideoService extends Service {
  static override readonly serviceType = ServiceType.VIDEO;

  public readonly capabilityDescription = 'Video download, processing, and conversion capabilities';

  /**
   * Get video information without downloading
   * @param url - Video URL
   * @returns Promise resolving to video information
   */
  abstract getVideoInfo(url: string): Promise<VideoInfo>;

  /**
   * Download a video from URL
   * @param url - Video URL
   * @param options - Download options
   * @returns Promise resolving to downloaded file path
   */
  abstract downloadVideo(url: string, options?: VideoDownloadOptions): Promise<string>;

  /**
   * Extract audio from video
   * @param videoPath - Path to video file or video URL
   * @param outputPath - Optional output path for audio file
   * @returns Promise resolving to audio file path
   */
  abstract extractAudio(videoPath: string, outputPath?: string): Promise<string>;

  /**
   * Generate thumbnail from video
   * @param videoPath - Path to video file or video URL
   * @param timestamp - Timestamp in seconds to capture thumbnail
   * @returns Promise resolving to thumbnail image path
   */
  abstract getThumbnail(videoPath: string, timestamp?: number): Promise<string>;

  /**
   * Convert video to different format
   * @param videoPath - Path to input video file
   * @param outputPath - Path for output video file
   * @param options - Processing options
   * @returns Promise resolving to converted video path
   */
  abstract convertVideo(
    videoPath: string,
    outputPath: string,
    options?: VideoProcessingOptions
  ): Promise<string>;

  /**
   * Get available formats for a video URL
   * @param url - Video URL
   * @returns Promise resolving to available formats
   */
  abstract getAvailableFormats(url: string): Promise<VideoFormat[]>;
}

} // namespace elizaos
