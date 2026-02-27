#include "/home/runner/work/elizaos-cpp/elizaos-cpp/discrub-ext/src/services/github-service.h"

string GITHUB_GIST_URL = std:("https://api.github.com/gists");
string ANNOUNCEMENT_ENDPOINT = string_empty + GITHUB_GIST_URL + std:("/912abef788d8f6dc25b3f4e0fa5e6371");
string DONATION_ENDPOINT = string_empty + GITHUB_GIST_URL + std:("/2aca7928b4db1ab84eac3720ac8e8559");
std::function<std::shared_ptr<Promise<std::shared_ptr<Announcement>>>()> fetchAnnouncementData = [=]() mutable
{
    return fetch(ANNOUNCEMENT_ENDPOINT, object{
        object::pair{std:("method"), std:("GET")}, 
        object::pair{std:("headers"), object{
            object::pair{std:("Content-Type"), std:("application/json")}
        }}
    })->then([=](auto resp) mutable
    {
        auto gistData = std::async([=]() { resp->json(); });
        auto announcement = JSON->parse(const_(gistData["files"])[std:("announcement.json")]["content"]);
        return announcement;
    }
    )->_catch([=](auto e) mutable
    {
        return console->error(std:("Error fetching announcement"), e);
    }
    );
};
std::function<std::shared_ptr<Promise<array<std::shared_ptr<Donation>>>>()> fetchDonationData = [=]() mutable
{
    return fetch(DONATION_ENDPOINT, object{
        object::pair{std:("method"), std:("GET")}, 
        object::pair{std:("headers"), object{
            object::pair{std:("Content-Type"), std:("application/json")}
        }}
    })->then([=](auto resp) mutable
    {
        auto gistData = std::async([=]() { resp->json(); });
        auto donation = JSON->parse(const_(gistData["files"])[std:("donation.json")]["content"]);
        return donation;
    }
    )->_catch([=](auto e) mutable
    {
        return console->error(std:("Error fetching donations"), e);
    }
    );
};

void Main(void)
{
}

MAIN
