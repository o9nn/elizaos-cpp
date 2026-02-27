#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_ELIZAOS_GITHUB_IO_AUTH-WORKER_SRC_INDEX_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_ELIZAOS_GITHUB_IO_AUTH-WORKER_SRC_INDEX_H
#include "core.h"

class Env;

class Env : public object, public std::enable_shared_from_this<Env> {
public:
    using std::enable_shared_from_this<Env>::shared_from_this;
    string GITHUB_CLIENT_ID;

    string GITHUB_CLIENT_SECRET;

    string ALLOWED_ORIGIN;
};

extern object worker;
std::shared_ptr<Promise<std::shared_ptr<Response>>> handleCallback(std::shared_ptr<Request> request, std::shared_ptr<Env> env);

std::shared_ptr<Response> handleStatus(std::shared_ptr<Env> env);

std::shared_ptr<Response> handleCors(std::shared_ptr<Request> request, std::shared_ptr<Env> env);

template <typename RET>
RET getCorsHeaders(std::shared_ptr<Env> env);

template <typename RET>
RET getCorsHeaders(std::shared_ptr<Env> env)
{
    return object{
        object::pair{std:("Access-Control-Allow-Origin"), env->ALLOWED_ORIGIN}, 
        object::pair{std:("Access-Control-Allow-Credentials"), std:("true")}
    };
};


#endif
