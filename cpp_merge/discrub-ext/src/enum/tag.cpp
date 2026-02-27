#include "tag.hpp"
#include <string>

std::function<string(Tag)> getTagName = [=](auto type) mutable
{
    switch (type)
    {
    case Tag::TAGS_MADE_BY_USER:
        return std::string("Tags Sent By User");
    case Tag::TAGGED_USERS:
        return std::string("Tags Received For User");
    default:
        return string_empty;
    }
};

void Main(void)
{
}

MAIN
