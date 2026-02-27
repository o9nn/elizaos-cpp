#ifndef _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_DISCRUB_EXT_SRC_SERVICES_GITHUB_SERVICE_H
#define _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_DISCRUB_EXT_SRC_SERVICES_GITHUB_SERVICE_H
#include "core.hpp"

typedef object Announcement;

typedef object Donation;


extern string GITHUB_GIST_URL;
extern string ANNOUNCEMENT_ENDPOINT;
extern string DONATION_ENDPOINT;
extern std::function<std::shared_ptr<Promise<std::shared_ptr<Announcement>>>()> fetchAnnouncementData;
extern std::function<std::shared_ptr<Promise<array<std::shared_ptr<Donation>>>>()> fetchDonationData;
#endif
