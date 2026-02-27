#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_ELIZAS-LIST_SRC_APP_API_AUTH_[___NEXTAUTH]_ROUTE_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_ELIZAS-LIST_SRC_APP_API_AUTH_[___NEXTAUTH]_ROUTE_H
#include "core.h"
#include "next-auth.h"
using NextAuth = _default;
#include "@next-auth/prisma-adapter.h"
#include "@/lib/prisma.h"
#include "next-auth/providers/github.h"
using GithubProvider = _default;

extern std::shared_ptr<AuthOptions> authOptions;
extern any handler;
#endif
