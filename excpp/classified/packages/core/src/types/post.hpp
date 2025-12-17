#pragma once
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#include "service.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



struct PostMedia {
    UUID id;
    std::string url;
    std::string type;
    std::string mimeType;
    double size;
    std::optional<double> width;
    std::optional<double> height;
    std::optional<double> duration;
    std::optional<std::string> thumbnail;
    std::optional<std::string> description;
    std::optional<std::string> altText;
};

struct PostLocation {
    std::string name;
    std::optional<std::string> address;
    double latitude;
    double longitude;
    std::optional<std::string> placeId;
};

struct PostAuthor {
    UUID id;
    std::string username;
    std::string displayName;
    std::optional<std::string> avatar;
    std::optional<bool> verified;
    std::optional<double> followerCount;
    std::optional<double> followingCount;
    std::optional<std::string> bio;
    std::optional<std::string> website;
};

struct PostEngagement {
    double likes;
    double shares;
    double comments;
    std::optional<double> views;
    bool hasLiked;
    bool hasShared;
    bool hasCommented;
    bool hasSaved;
};

struct PostContent {
    std::optional<std::string> text;
    std::optional<std::string> html;
    std::optional<std::vector<PostMedia>> media;
    std::optional<PostLocation> location;
    std::optional<std::vector<std::string>> tags;
    std::optional<std::vector<UUID>> mentions;
    std::string url;
    std::optional<std::string> title;
    std::optional<std::string> description;
    std::optional<std::string> image;
    std::string question;
    std::string text;
    double votes;
    std::optional<Date> expiresAt;
    std::optional<bool> multipleChoice;
};

struct PostInfo {
    UUID id;
    PostAuthor author;
    PostContent content;
    std::string platform;
    std::string platformId;
    std::string url;
    Date createdAt;
    std::optional<Date> editedAt;
    std::optional<Date> scheduledAt;
    PostEngagement engagement;
    std::string visibility;
    std::optional<UUID> replyTo;
    UUID id;
    double position;
    double total;
    std::string platform;
    std::string platformId;
    std::string url;
};

struct PostCreateOptions {
    std::optional<std::vector<std::string>> platforms;
    std::optional<Date> scheduledAt;
    std::optional<PostInfo['visibility']> visibility;
    std::optional<UUID> replyTo;
    std::optional<bool> thread;
    std::optional<PostLocation> location;
    std::optional<std::vector<std::string>> tags;
    std::optional<std::vector<UUID>> mentions;
    std::optional<bool> enableComments;
    std::optional<bool> enableSharing;
    std::optional<std::string> contentWarning;
    std::optional<bool> sensitive;
};

struct PostSearchOptions {
    std::optional<std::string> query;
    std::optional<UUID> author;
    std::optional<std::string> platform;
    std::optional<std::vector<std::string>> tags;
    std::optional<std::vector<UUID>> mentions;
    std::optional<Date> since;
    std::optional<Date> before;
    std::optional<double> limit;
    std::optional<double> offset;
    std::optional<bool> hasMedia;
    std::optional<bool> hasLocation;
    std::optional<PostInfo['visibility']> visibility;
    std::optional<std::string> sortBy;
};

struct PostAnalytics {
    UUID postId;
    std::string platform;
    double impressions;
    double reach;
    PostEngagement engagement;
    double clicks;
    double shares;
    double saves;
    double hour;
    double engagement;
};

/**
 * Interface for social media posting services
 */


} // namespace elizaos
