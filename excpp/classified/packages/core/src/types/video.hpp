#pragma once
#include <algorithm>
#include <chrono>
#include <cstdint>
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include "service.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



struct VideoInfo {
    std::optional<std::string> title;
    std::optional<double> duration;
    std::string url;
    std::optional<std::string> thumbnail;
    std::optional<std::string> description;
    std::optional<std::string> uploader;
    std::optional<double> viewCount;
    std::optional<std::chrono::system_clock::time_point> uploadDate;
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
    std::optional<std::string> quality;
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


} // namespace elizaos
