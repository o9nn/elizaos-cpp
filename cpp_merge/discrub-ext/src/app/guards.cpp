#include "/home/runner/work/elizaos-cpp/elizaos-cpp/discrub-ext/src/app/guards.h"

std::function<any(any)> isMessage = [=](auto entity) mutable
{
    if (!entity) return false;
    auto message = as<std::shared_ptr<Message>>(entity);
    return (AND((AND((in(std:("content"), message)), (in(std:("attachments"), message)))), (in(std:("embeds"), message))));
};
std::function<any(any)> isGuild = [=](auto entity) mutable
{
    if (!entity) return false;
    auto guild = as<std::shared_ptr<Guild>>(entity);
    return AND((in(std:("emojis"), guild)), (in(std:("roles"), guild)));
};
std::function<any(any)> isRole = [=](auto entity) mutable
{
    if (!entity) return false;
    auto role = as<std::shared_ptr<Role>>(entity);
    return AND((in(std:("color"), role)), (in(std:("hoist"), role)));
};
std::function<any(any)> isAttachment = [=](auto entity) mutable
{
    if (!entity) return false;
    auto attachment = as<std::shared_ptr<Attachment>>(entity);
    return in(std:("filename"), attachment);
};

void Main(void)
{
}

MAIN
