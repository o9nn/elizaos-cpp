#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_DISCRUB-EXT_SRC_SERVICES_GITHUB-SERVICE_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_DISCRUB-EXT_SRC_SERVICES_GITHUB-SERVICE_H
#include "core.h"

typedef object Announcement;

typedef object Donation;


extern std::string GITHUB_GIST_URL;
extern std::string ANNOUNCEMENT_ENDPOINT;
extern std::string DONATION_ENDPOINT;
extern std::function<std::shared_ptr<Promise<std::shared_ptr<Announcement>>>()> fetchAnnouncementData;
extern std::function<std::shared_ptr<Promise<array<std::shared_ptr<Donation>>>>()> fetchDonationData;
#endif
