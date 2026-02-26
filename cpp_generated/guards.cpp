#include "/home/runner/work/elizaos-cpp/elizaos-cpp/discrub-ext/src/app/guards.h"

std::function<any(any)> isMessage = [=](auto entity) mutable
{
    if (!entity) return false;
    auto message = as<std::shared_ptr<Message>>(entity);
    return (AND((AND((in(std::string("content"), message)), (in(std::string("attachments"), message)))), (in(std::string("embeds"), message))));
};
std::function<any(any)> isGuild = [=](auto entity) mutable
{
    if (!entity) return false;
    auto guild = as<std::shared_ptr<Guild>>(entity);
    return AND((in(std::string("emojis"), guild)), (in(std::string("roles"), guild)));
};
std::function<any(any)> isRole = [=](auto entity) mutable
{
    if (!entity) return false;
    auto role = as<std::shared_ptr<Role>>(entity);
    return AND((in(std::string("color"), role)), (in(std::string("hoist"), role)));
};
std::function<any(any)> isAttachment = [=](auto entity) mutable
{
    if (!entity) return false;
    auto attachment = as<std::shared_ptr<Attachment>>(entity);
    return in(std::string("filename"), attachment);
};

void Main(void)
{
}

MAIN
