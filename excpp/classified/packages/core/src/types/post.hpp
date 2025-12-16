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
import type { UUID } from './primitives';

struct PostMedia {
    UUID id;
    std::string url;
    'image' | 'video' | 'audio' | 'document' type;
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
    std::optional<{> coordinates;
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
    std::optional<Array<{> links;
    std::string url;
    std::optional<std::string> title;
    std::optional<std::string> description;
    std::optional<std::string> image;
    std::optional<{> poll;
    std::string question;
    Array<{ options;
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
    'public' | 'private' | 'followers' | 'friends' | 'unlisted' visibility;
    std::optional<UUID> replyTo;
    std::optional<{> thread;
    UUID id;
    double position;
    double total;
    std::optional<Array<{> crossPosted;
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
    std::optional<'date' | 'engagement' | 'relevance'> sortBy;
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
    std::optional<{> demographics;
    std::optional<std::unordered_map<std::string, double>> age;
    std::optional<std::unordered_map<std::string, double>> gender;
    std::optional<std::unordered_map<std::string, double>> location;
    std::optional<Array<{> topPerformingHours;
    double hour;
    double engagement;
};


/**
 * Interface for social media posting services
 */
abstract class IPostService extends Service {
  static override readonly serviceType = ServiceType.POST;

  public readonly capabilityDescription =
    'Social media posting and content management capabilities';

  /**
   * Create and publish a new post
   * @param content - Post content
   * @param options - Publishing options
   * @returns Promise resolving to post ID
   */
  abstract createPost(content: PostContent, options?: PostCreateOptions): Promise<UUID>;

  /**
   * Get posts from timeline or specific user
   * @param options - Search options
   * @returns Promise resolving to array of posts
   */
  abstract getPosts(options?: PostSearchOptions): Promise<PostInfo[]>;

  /**
   * Get a specific post by ID
   * @param postId - Post ID
   * @returns Promise resolving to post info
   */
  abstract getPost(postId: UUID): Promise<PostInfo>;

  /**
   * Edit an existing post
   * @param postId - Post ID
   * @param content - New post content
   * @returns Promise resolving when edit completes
   */
  abstract editPost(postId: UUID, content: PostContent): Promise<void>;

  /**
   * Delete a post
   * @param postId - Post ID
   * @returns Promise resolving when deletion completes
   */
  abstract deletePost(postId: UUID): Promise<void>;

  /**
   * Like/unlike a post
   * @param postId - Post ID
   * @param like - True to like, false to unlike
   * @returns Promise resolving when operation completes
   */
  abstract likePost(postId: UUID, like: boolean): Promise<void>;

  /**
   * Share/repost a post
   * @param postId - Post ID
   * @param comment - Optional comment when sharing
   * @returns Promise resolving to share ID
   */
  abstract sharePost(postId: UUID, comment?: string): Promise<UUID>;

  /**
   * Save/unsave a post
   * @param postId - Post ID
   * @param save - True to save, false to unsave
   * @returns Promise resolving when operation completes
   */
  abstract savePost(postId: UUID, save: boolean): Promise<void>;

  /**
   * Comment on a post
   * @param postId - Post ID
   * @param content - Comment content
   * @returns Promise resolving to comment ID
   */
  abstract commentOnPost(postId: UUID, content: PostContent): Promise<UUID>;

  /**
   * Get comments for a post
   * @param postId - Post ID
   * @param options - Search options
   * @returns Promise resolving to array of comments
   */
  abstract getComments(postId: UUID, options?: PostSearchOptions): Promise<PostInfo[]>;

  /**
   * Schedule a post for later publishing
   * @param content - Post content
   * @param scheduledAt - When to publish
   * @param options - Publishing options
   * @returns Promise resolving to scheduled post ID
   */
  abstract schedulePost(
    content: PostContent,
    scheduledAt: Date,
    options?: PostCreateOptions
  ): Promise<UUID>;

  /**
   * Get analytics for a post
   * @param postId - Post ID
   * @returns Promise resolving to post analytics
   */
  abstract getPostAnalytics(postId: UUID): Promise<PostAnalytics>;

  /**
   * Get trending posts
   * @param options - Search options
   * @returns Promise resolving to trending posts
   */
  abstract getTrendingPosts(options?: PostSearchOptions): Promise<PostInfo[]>;

  /**
   * Search posts across platforms
   * @param query - Search query
   * @param options - Search options
   * @returns Promise resolving to search results
   */
  abstract searchPosts(query: string, options?: PostSearchOptions): Promise<PostInfo[]>;
}

} // namespace elizaos
