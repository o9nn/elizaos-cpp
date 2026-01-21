#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_CLASSIFIED_PACKAGES_CORE_SRC_TYPES_POST_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_CLASSIFIED_PACKAGES_CORE_SRC_TYPES_POST_H
#include "core.h"
#include "./service.h"
#include "./primitives.h"

class PostMedia;
class PostLocation;
class PostAuthor;
class PostEngagement;
class PostContent;
class PostInfo;
class PostCreateOptions;
class PostSearchOptions;
class PostAnalytics;
class IPostService;

class PostMedia : public object, public std::enable_shared_from_this<PostMedia> {
public:
    using std::enable_shared_from_this<PostMedia>::shared_from_this;
    std::shared_ptr<UUID> id;

    string url;

    any type;

    string mimeType;

    double size;

    double width;

    double height;

    double duration;

    string thumbnail;

    string description;

    string altText;
};

class PostLocation : public object, public std::enable_shared_from_this<PostLocation> {
public:
    using std::enable_shared_from_this<PostLocation>::shared_from_this;
    string name;

    string address;

    object coordinates;

    string placeId;
};

class PostAuthor : public object, public std::enable_shared_from_this<PostAuthor> {
public:
    using std::enable_shared_from_this<PostAuthor>::shared_from_this;
    std::shared_ptr<UUID> id;

    string username;

    string displayName;

    string avatar;

    boolean verified;

    double followerCount;

    double followingCount;

    string bio;

    string website;
};

class PostEngagement : public object, public std::enable_shared_from_this<PostEngagement> {
public:
    using std::enable_shared_from_this<PostEngagement>::shared_from_this;
    double likes;

    double shares;

    double comments;

    double views;

    boolean hasLiked;

    boolean hasShared;

    boolean hasCommented;

    boolean hasSaved;
};

class PostContent : public object, public std::enable_shared_from_this<PostContent> {
public:
    using std::enable_shared_from_this<PostContent>::shared_from_this;
    string text;

    string html;

    array<std::shared_ptr<PostMedia>> media;

    std::shared_ptr<PostLocation> location;

    array<string> tags;

    array<std::shared_ptr<UUID>> mentions;

    array<object> links;

    object poll;
};

class PostInfo : public object, public std::enable_shared_from_this<PostInfo> {
public:
    using std::enable_shared_from_this<PostInfo>::shared_from_this;
    std::shared_ptr<UUID> id;

    std::shared_ptr<PostAuthor> author;

    std::shared_ptr<PostContent> content;

    string platform;

    string platformId;

    string url;

    std::shared_ptr<Date> createdAt;

    std::shared_ptr<Date> editedAt;

    std::shared_ptr<Date> scheduledAt;

    std::shared_ptr<PostEngagement> engagement;

    any visibility;

    std::shared_ptr<UUID> replyTo;

    object thread;

    array<object> crossPosted;
};

class PostCreateOptions : public object, public std::enable_shared_from_this<PostCreateOptions> {
public:
    using std::enable_shared_from_this<PostCreateOptions>::shared_from_this;
    array<string> platforms;

    std::shared_ptr<Date> scheduledAt;

    any visibility;

    std::shared_ptr<UUID> replyTo;

    boolean thread;

    std::shared_ptr<PostLocation> location;

    array<string> tags;

    array<std::shared_ptr<UUID>> mentions;

    boolean enableComments;

    boolean enableSharing;

    string contentWarning;

    boolean sensitive;
};

class PostSearchOptions : public object, public std::enable_shared_from_this<PostSearchOptions> {
public:
    using std::enable_shared_from_this<PostSearchOptions>::shared_from_this;
    string query;

    std::shared_ptr<UUID> author;

    string platform;

    array<string> tags;

    array<std::shared_ptr<UUID>> mentions;

    std::shared_ptr<Date> since;

    std::shared_ptr<Date> before;

    double limit;

    double offset;

    boolean hasMedia;

    boolean hasLocation;

    any visibility;

    any sortBy;
};

class PostAnalytics : public object, public std::enable_shared_from_this<PostAnalytics> {
public:
    using std::enable_shared_from_this<PostAnalytics>::shared_from_this;
    std::shared_ptr<UUID> postId;

    string platform;

    double impressions;

    double reach;

    std::shared_ptr<PostEngagement> engagement;

    double clicks;

    double shares;

    double saves;

    object demographics;

    array<object> topPerformingHours;
};

class IPostService : public Service, public std::enable_shared_from_this<IPostService> {
public:
    using std::enable_shared_from_this<IPostService>::shared_from_this;
    static any override;

    string serviceType = ServiceType["POST"];

    string capabilityDescription = std::string("Social media posting and content management capabilities");

    virtual std::shared_ptr<Promise<std::shared_ptr<UUID>>> createPost(std::shared_ptr<PostContent> content, std::shared_ptr<PostCreateOptions> options = undefined) = 0;
    virtual std::shared_ptr<Promise<array<std::shared_ptr<PostInfo>>>> getPosts(std::shared_ptr<PostSearchOptions> options = undefined) = 0;
    virtual std::shared_ptr<Promise<std::shared_ptr<PostInfo>>> getPost(std::shared_ptr<UUID> postId) = 0;
    virtual std::shared_ptr<Promise<void>> editPost(std::shared_ptr<UUID> postId, std::shared_ptr<PostContent> content) = 0;
    virtual std::shared_ptr<Promise<void>> deletePost(std::shared_ptr<UUID> postId) = 0;
    virtual std::shared_ptr<Promise<void>> likePost(std::shared_ptr<UUID> postId, boolean like) = 0;
    virtual std::shared_ptr<Promise<std::shared_ptr<UUID>>> sharePost(std::shared_ptr<UUID> postId, string comment = undefined) = 0;
    virtual std::shared_ptr<Promise<void>> savePost(std::shared_ptr<UUID> postId, boolean save) = 0;
    virtual std::shared_ptr<Promise<std::shared_ptr<UUID>>> commentOnPost(std::shared_ptr<UUID> postId, std::shared_ptr<PostContent> content) = 0;
    virtual std::shared_ptr<Promise<array<std::shared_ptr<PostInfo>>>> getComments(std::shared_ptr<UUID> postId, std::shared_ptr<PostSearchOptions> options = undefined) = 0;
    virtual std::shared_ptr<Promise<std::shared_ptr<UUID>>> schedulePost(std::shared_ptr<PostContent> content, std::shared_ptr<Date> scheduledAt, std::shared_ptr<PostCreateOptions> options = undefined) = 0;
    virtual std::shared_ptr<Promise<std::shared_ptr<PostAnalytics>>> getPostAnalytics(std::shared_ptr<UUID> postId) = 0;
    virtual std::shared_ptr<Promise<array<std::shared_ptr<PostInfo>>>> getTrendingPosts(std::shared_ptr<PostSearchOptions> options = undefined) = 0;
    virtual std::shared_ptr<Promise<array<std::shared_ptr<PostInfo>>>> searchPosts(string query, std::shared_ptr<PostSearchOptions> options = undefined) = 0;
    IPostService(std::shared_ptr<IAgentRuntime> runtime = undefined);
};

#endif
