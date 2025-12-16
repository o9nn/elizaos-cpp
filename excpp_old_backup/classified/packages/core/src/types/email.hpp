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



struct EmailAddress {
    std::string email;
    std::optional<std::string> name;
};

struct EmailAttachment {
    std::string filename;
    Buffer | string content;
    std::optional<std::string> contentType;
    std::optional<'attachment' | 'inline'> contentDisposition;
    std::optional<std::string> cid;
};

struct EmailMessage {
    EmailAddress from;
    std::vector<EmailAddress> to;
    std::optional<std::vector<EmailAddress>> cc;
    std::optional<std::vector<EmailAddress>> bcc;
    std::string subject;
    std::optional<std::string> text;
    std::optional<std::string> html;
    std::optional<std::vector<EmailAttachment>> attachments;
    std::optional<EmailAddress> replyTo;
    std::optional<Date> date;
    std::optional<std::string> messageId;
    std::optional<std::vector<std::string>> references;
    std::optional<std::string> inReplyTo;
    std::optional<'high' | 'normal' | 'low'> priority;
};

struct EmailSendOptions {
    std::optional<double> retry;
    std::optional<double> timeout;
    std::optional<bool> trackOpens;
    std::optional<bool> trackClicks;
    std::optional<std::vector<std::string>> tags;
};

struct EmailSearchOptions {
    std::optional<std::string> query;
    std::optional<std::string> from;
    std::optional<std::string> to;
    std::optional<std::string> subject;
    std::optional<std::string> folder;
    std::optional<Date> since;
    std::optional<Date> before;
    std::optional<double> limit;
    std::optional<double> offset;
    std::optional<bool> unread;
    std::optional<bool> flagged;
    std::optional<bool> hasAttachments;
};

struct EmailFolder {
    std::string name;
    std::string path;
    'inbox' | 'sent' | 'drafts' | 'trash' | 'spam' | 'custom' type;
    std::optional<double> messageCount;
    std::optional<double> unreadCount;
    std::optional<std::vector<EmailFolder>> children;
};

struct EmailAccount {
    std::string email;
    std::optional<std::string> name;
    std::optional<std::string> provider;
    std::optional<std::vector<EmailFolder>> folders;
    std::optional<double> quotaUsed;
    std::optional<double> quotaLimit;
};

/**
 * Interface for email services
 */

  /**
   * Send an email
   * @param message - Email message to send
   * @param options - Send options
   * @returns Promise resolving to message ID
   */

  /**
   * Get emails from a folder
   * @param options - Search options
   * @returns Promise resolving to array of emails
   */

  /**
   * Get a specific email by ID
   * @param messageId - Message ID
   * @returns Promise resolving to email message
   */

  /**
   * Delete an email
   * @param messageId - Message ID
   * @returns Promise resolving when deletion completes
   */

  /**
   * Mark an email as read/unread
   * @param messageId - Message ID
   * @param read - True to mark as read, false for unread
   * @returns Promise resolving when operation completes
   */

  /**
   * Flag/unflag an email
   * @param messageId - Message ID
   * @param flagged - True to flag, false to unflag
   * @returns Promise resolving when operation completes
   */

  /**
   * Move email to a different folder
   * @param messageId - Message ID
   * @param folderPath - Destination folder path
   * @returns Promise resolving when move completes
   */

  /**
   * Get available folders
   * @returns Promise resolving to array of folders
   */

  /**
   * Create a new folder
   * @param folderName - Name of the folder
   * @param parentPath - Optional parent folder path
   * @returns Promise resolving when folder is created
   */

  /**
   * Get account information
   * @returns Promise resolving to account details
   */

  /**
   * Search emails
   * @param query - Search query
   * @param options - Search options
   * @returns Promise resolving to search results
   */

} // namespace elizaos
