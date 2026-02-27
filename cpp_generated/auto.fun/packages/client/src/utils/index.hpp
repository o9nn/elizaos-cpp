#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_AUTO_FUN_PACKAGES_CLIENT_SRC_UTILS_INDEX_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_AUTO_FUN_PACKAGES_CLIENT_SRC_UTILS_INDEX_H
#include "core.h"
#include "@/types.h"
#include "dayjs.h"
using dayjs = _default;
#include "dayjs/plugin/relativeTime.h"
using relativeTime = _default;
#include "./env.h"

extern any moment;
extern double LAMPORTS_PER_SOL;
extern std::function<double(double)> normalizedProgress;
extern std::function<string(string)> shortenAddress;
extern std::function<string(double, boolean)> abbreviateNumber;
extern std::function<string(double, boolean, boolean)> formatNumber;
extern std::function<string(any, boolean)> fromNow;
string toSubscript(double num);

extern std::function<string(double, double)> formatNumberSubscript;
extern std::function<string(double, double)> formatNumberSubscriptSmart;
extern std::function<boolean(string, string)> isFromDomain;
extern std::function<string(string, double, double)> resizeImage;
extern double networkId;
extern std::function<boolean(any)> useCodex;
extern std::function<any(string)> sanitizeCheckmark;
#endif
