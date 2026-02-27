#include "/home/runner/work/elizaos-cpp/elizaos-cpp/otaku/src/plugins/plugin-bootstrap/src/utils/media.h"

std::shared_ptr<Promise<array<MediaData>>> fetchMediaData(array<std::shared_ptr<Media>> attachments)
{
    return Promise->all(attachments->map([=](auto attachment) mutable
    {
        if (OR((attachment->url->startsWith(std::string("http://"))), (attachment->url->startsWith(std::string("https://"))))) {
            auto response = std::async([=]() { fetch(attachment->url); });
            auto mediaBuffer = Buffer::from(std::async([=]() { response->arrayBuffer(); }));
            auto mediaType = OR((attachment->contentType), (std::string("image/png")));
            return object{
                object::pair{std::string("data"), mediaBuffer}, 
                object::pair{std::string("mediaType"), std::string("mediaType")}
            };
        }
        throw any(std::make_shared<Error>(std::string("Local file paths are not supported yet")));
    }
    ));
};


std::shared_ptr<Promise<array<std::shared_ptr<Media>>>> processAttachments(array<std::shared_ptr<Media>> attachments, std::shared_ptr<IAgentRuntime> runtime)
{
    if (OR((!attachments), (attachments->get_length() == 0))) {
        return array<any>();
    }
    runtime->logger->debug(std::string("[Bootstrap] Processing ") + attachments->get_length() + std::string(" attachment(s)"));
    auto processedAttachments = array<std::shared_ptr<Media>>();
    for (auto& attachment : attachments)
    {
        if (OR((attachment->contentType->startsWith(std::string("image/"))), (attachment->contentType->startsWith(std::string("application/pdf"))))) {
            auto processedAttachment = utils::assign(object{
            }, attachment);
            if (!processedAttachment->description) {
                try
                {
                    auto base64Data = string_empty;
                    auto mimeType = attachment->contentType;
                    if (AND((!attachment->url->startsWith(std::string("http://"))), (!attachment->url->startsWith(std::string("https://"))))) {
                        runtime->logger->debug(std::string("[Bootstrap] Skipping local file processing:"), attachment->url);
                        processedAttachments->push(attachment);
                        continue;
                    } else {
                        auto response = std::async([=]() { fetch(attachment->url); });
                        auto buffer = Buffer::from(std::async([=]() { response->arrayBuffer(); }));
                        base64Data = buffer->toString(std::string("base64"));
                        mimeType = OR((OR((attachment->contentType), (response->headers->get(std::string("content-type"))))), (std::string("image/png")));
                    }
                    auto descriptionPrompt = std::string("Describe this ") + (mimeType->startsWith(std::string("image/"))) ? std::string("image") : std::string("document") + std::string(" in detail. Include:\
1. What you see in the content\
2. Any text visible in the content\
3. The overall context and purpose\
4. Any notable details or important information\
\
Be concise but thorough.");
                    auto description = std::async([=]() { runtime->useModel(ModelType->TEXT_SMALL, object{
                        object::pair{std::string("prompt"), descriptionPrompt}, 
                        object::pair{std::string("attachments"), array<any>{ utils::assign(object{
                            , 
                            object::pair{std::string("data"), base64Data}, 
                            object::pair{std::string("contentType"), mimeType}
                        }, attachment) }}
                    }); });
                    processedAttachment->description = description;
                    processedAttachment->title = OR((attachment->title), (string_empty + mimeType + std::string(" attachment")));
                    processedAttachment->text = description;
                    runtime->logger->debug(std::string("[Bootstrap] Generated description for attachment: ") + attachment->url + string_empty);
                }
                catch (const any& error)
                {
                    runtime->logger->error(std::string("[Bootstrap] Error processing attachment: ") + attachment->url + std::string(" - ") + (is<Error>(error)) ? error->message : String(error) + string_empty);
                }
            }
            processedAttachments->push(processedAttachment);
        } else {
            processedAttachments->push(attachment);
        }
    }
    return processedAttachments;
};


