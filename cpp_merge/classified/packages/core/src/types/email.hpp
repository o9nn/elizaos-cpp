#ifndef _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_CLASSIFIED_PACKAGES_CORE_SRC_TYPES_EMAIL_H
#define _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_CLASSIFIED_PACKAGES_CORE_SRC_TYPES_EMAIL_H
#include "core.hpp"
#include "./service.h"

class EmailAddress;
class EmailAttachment;
class EmailMessage;
class EmailSendOptions;
class EmailSearchOptions;
class EmailFolder;
class EmailAccount;
class IEmailService;

class EmailAddress : public object, public std::enable_shared_from_this<EmailAddress> {
public:
    using std::enable_shared_from_this<EmailAddress>::shared_from_this;
    string email;

    string name;
};

class EmailAttachment : public object, public std::enable_shared_from_this<EmailAttachment> {
public:
    using std::enable_shared_from_this<EmailAttachment>::shared_from_this;
    string filename;

    any content;

    string contentType;

    any contentDisposition;

    string cid;
};

class EmailMessage : public object, public std::enable_shared_from_this<EmailMessage> {
public:
    using std::enable_shared_from_this<EmailMessage>::shared_from_this;
    std::shared_ptr<EmailAddress> from;

    array<std::shared_ptr<EmailAddress>> to;

    array<std::shared_ptr<EmailAddress>> cc;

    array<std::shared_ptr<EmailAddress>> bcc;

    string subject;

    string text;

    string html;

    array<std::shared_ptr<EmailAttachment>> attachments;

    std::shared_ptr<EmailAddress> replyTo;

    std::shared_ptr<Date> date;

    string messageId;

    array<string> references;

    string inReplyTo;

    any priority;
};

class EmailSendOptions : public object, public std::enable_shared_from_this<EmailSendOptions> {
public:
    using std::enable_shared_from_this<EmailSendOptions>::shared_from_this;
    double retry;

    double timeout;

    boolean trackOpens;

    boolean trackClicks;

    array<string> tags;
};

class EmailSearchOptions : public object, public std::enable_shared_from_this<EmailSearchOptions> {
public:
    using std::enable_shared_from_this<EmailSearchOptions>::shared_from_this;
    string query;

    string from;

    string to;

    string subject;

    string folder;

    std::shared_ptr<Date> since;

    std::shared_ptr<Date> before;

    double limit;

    double offset;

    boolean unread;

    boolean flagged;

    boolean hasAttachments;
};

class EmailFolder : public object, public std::enable_shared_from_this<EmailFolder> {
public:
    using std::enable_shared_from_this<EmailFolder>::shared_from_this;
    string name;

    string path;

    any type;

    double messageCount;

    double unreadCount;

    array<std::shared_ptr<EmailFolder>> children;
};

class EmailAccount : public object, public std::enable_shared_from_this<EmailAccount> {
public:
    using std::enable_shared_from_this<EmailAccount>::shared_from_this;
    string email;

    string name;

    string provider;

    array<std::shared_ptr<EmailFolder>> folders;

    double quotaUsed;

    double quotaLimit;
};

class IEmailService : public Service, public std::enable_shared_from_this<IEmailService> {
public:
    using std::enable_shared_from_this<IEmailService>::shared_from_this;
    static any override;

    string serviceType = ServiceType["EMAIL"];

    string capabilityDescription = std:("Email sending, receiving, and management capabilities");

    virtual std::shared_ptr<Promise<string>> sendEmail(std::shared_ptr<EmailMessage> message, std::shared_ptr<EmailSendOptions> options = undefined) = 0;
    virtual std::shared_ptr<Promise<array<std::shared_ptr<EmailMessage>>>> getEmails(std::shared_ptr<EmailSearchOptions> options = undefined) = 0;
    virtual std::shared_ptr<Promise<std::shared_ptr<EmailMessage>>> getEmail(string messageId) = 0;
    virtual std::shared_ptr<Promise<void>> deleteEmail(string messageId) = 0;
    virtual std::shared_ptr<Promise<void>> markEmailAsRead(string messageId, boolean read) = 0;
    virtual std::shared_ptr<Promise<void>> flagEmail(string messageId, boolean flagged) = 0;
    virtual std::shared_ptr<Promise<void>> moveEmail(string messageId, string folderPath) = 0;
    virtual std::shared_ptr<Promise<array<std::shared_ptr<EmailFolder>>>> getFolders() = 0;
    virtual std::shared_ptr<Promise<void>> createFolder(string folderName, string parentPath = undefined) = 0;
    virtual std::shared_ptr<Promise<std::shared_ptr<EmailAccount>>> getAccountInfo() = 0;
    virtual std::shared_ptr<Promise<array<std::shared_ptr<EmailMessage>>>> searchEmails(string query, std::shared_ptr<EmailSearchOptions> options = undefined) = 0;
    IEmailService(std::shared_ptr<IAgentRuntime> runtime = undefined);
};

#endif
