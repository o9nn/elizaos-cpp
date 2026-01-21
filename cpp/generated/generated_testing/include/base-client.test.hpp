#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_ELIZA_PACKAGES_API-CLIENT_SRC___TESTS___BASE-CLIENT_TEST_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_ELIZA_PACKAGES_API-CLIENT_SRC___TESTS___BASE-CLIENT_TEST_H
#include "core.h"
#include "bun:test.h"
#include "../lib/base-client.h"
#include "../types/base.h"

class TestClient;

class TestClient : public BaseApiClient, public std::enable_shared_from_this<TestClient> {
public:
    using std::enable_shared_from_this<TestClient>::shared_from_this;
    template <typename T>
    std::any testGet(std::string path);
    template <typename T>
    std::any testPost(std::string path, std::any body);
    template <typename T>
    std::any testRequest(std::string method, std::string path, std::any options = undefined);
    TestClient(std::shared_ptr<ApiClientConfig> config);
};

template <typename T>
std::any TestClient::testGet(std::string path)
{
    return this->get<T>(path);
}

template <typename T>
std::any TestClient::testPost(std::string path, std::any body)
{
    return this->post<T>(path, body);
}

template <typename T>
std::any TestClient::testRequest(std::string method, std::string path, std::any options)
{
    return this->request<T>(method, path, options);
}

#endif
