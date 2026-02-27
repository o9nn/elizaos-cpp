#include "utils.hpp"
#include <string>

string characterNameToUrl(string name)
{
    return name->replace((new RegExp(std::string("\s+"))), std::string("-"));
};


string urlToCharacterName(string urlName)
{
    return urlName->replace((new RegExp(std::string("-+"))), std::string(" "));
};


std::shared_ptr<UUID> randomUUID()
{
    return as<std::shared_ptr<UUID>>(URL["createObjectURL"](std::make_shared<Blob>())->split(std::string("/"))->pop());
};


std::shared_ptr<UUID> getEntityId()
{
    auto USER_ID_KEY = std::string("elizaos-client-user-id");
    auto existingUserId = localStorage->getItem(USER_ID_KEY);
    if (existingUserId) {
        return as<std::shared_ptr<UUID>>(existingUserId);
    }
    auto newUserId = as<std::shared_ptr<UUID>>(randomUUID());
    localStorage->setItem(USER_ID_KEY, newUserId);
    return newUserId;
};


any moment = dayjs;
std::function<string(string)> formatAgentName = [=](auto name) mutable
{
    return name->substring(0, 2);
};
std::function<std::shared_ptr<Promise<string>>(std::shared_ptr<File>, any, double)> compressImage = [=](auto file, auto maxSize = AVATAR_IMAGE_MAX_SIZE, auto quality = 0.8) mutable
{
    return std::make_shared<Promise>([=](auto resolve, auto reject) mutable
    {
        auto reader = std::make_shared<FileReader>();
        reader->onload = [=](auto e) mutable
        {
            if (e->target->result) {
                shared img = std::make_shared<Image>();
                img->src = as<string>(e->target->result);
                img->onload = [=]() mutable
                {
                    auto canvas = document->createElement(std::string("canvas"));
                    auto width = img->width;
                    auto height = img->height;
                    if (width > height) {
                        if (width > maxSize) {
                            height *= maxSize / width;
                            width = maxSize;
                        }
                    } else {
                        if (height > maxSize) {
                            width *= maxSize / height;
                            height = maxSize;
                        }
                    }
                    canvas->width = width;
                    canvas->height = height;
                    auto ctx = canvas->getContext(std::string("2d"));
                    ctx->drawImage(img, 0, 0, width, height);
                    auto resizedBase64 = canvas->toDataURL(std::string("image/jpeg"), quality);
                    resolve(resizedBase64);
                };
                img->onerror = reject;
            }
        };
        reader->onerror = reject;
        reader->readAsDataURL(file);
    }
    );
};
array<string> AGENT_AVATAR_PLACEHOLDERS = array<string>{ std::string("/images/agents/agent1.png"), std::string("/images/agents/agent2.png"), std::string("/images/agents/agent3.png"), std::string("/images/agents/agent4.png"), std::string("/images/agents/agent5.png") };
std::function<string(object)> getAgentAvatar = [=](auto agent) mutable
{
    if (agent["settings"]["avatar"]) {
        return agent["settings"]["avatar"];
    }
    if (agent["id"]) {
        auto hash = 0;
        for (auto i = 0; i < agent["id"]->length; i++)
        {
            auto char = agent["id"]->charCodeAt(i);
            hash = (bitwise::lshift(hash, 5)) - hash + char;
            hash |= 0;
        }
        auto index = Math->abs(hash) % AGENT_AVATAR_PLACEHOLDERS->get_length();
        return const_(AGENT_AVATAR_PLACEHOLDERS)[index];
    }
    return std::string("/elizaos-icon.png");
};
std::function<string(any, array<any>, any)> generateGroupName = [=](auto channel, auto participants, P2 currentUserId) mutable
{
    if (AND((channel["name"]), (channel["name"]["trim"]() != string_empty))) {
        return channel["name"];
    }
    if (AND((participants), (participants->get_length() > 0))) {
        auto otherParticipants = participants->filter([=](auto p) mutable
        {
            return AND((p->id != currentUserId), (p->name));
        }
        );
        if (AND((otherParticipants->get_length() == 0), (participants->some([=](auto p) mutable
        {
            return AND((p->id == currentUserId), (p->name));
        }
        )))) {
            auto currentUserParticipant = participants->find([=](auto p) mutable
            {
                return p->id == currentUserId;
            }
            );
            if (currentUserParticipant) return OR((currentUserParticipant->name), (std::string("Unnamed Group")));
            return std::string("Unnamed Group");
        }
        if (otherParticipants->get_length() > 0) {
            return (otherParticipants->map([=](auto p) mutable
            {
                return p->name;
            }
            )->slice(0, 3)->join(std::string(", ")) + ((otherParticipants->get_length() > 3) ? std::string("...") : string_empty));
        }
    }
    return std::string("Unnamed Group");
};

void Main(void)
{
    dayjs->extend(localizedFormat);
}

MAIN
