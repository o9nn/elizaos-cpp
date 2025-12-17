#pragma once
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>
#include "service.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



struct EmailAddress {
    std::string email;
    std::optional<std::string> name;
};

struct EmailAttachment {
    std::string filename;
    std::variant<Buffer, std::string> content;
    std::optional<std::string> contentType;
    std::optional<std::string> contentDisposition;
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
    std::optional<std::string> priority;
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
    std::string type;
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


} // namespace elizaos
