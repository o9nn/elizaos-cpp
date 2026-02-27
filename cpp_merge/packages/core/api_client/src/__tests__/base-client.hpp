#ifndef _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_ELIZA_PACKAGES_API_CLIENT_SRC___TESTS___BASE_CLIENT_TEST_H
#define _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_ELIZA_PACKAGES_API_CLIENT_SRC___TESTS___BASE_CLIENT_TEST_H
#include "core.hpp"
#include "bun:test.h"
#include "../lib/base-client.h"
#include "../types/base.h"

class TestClient;

class TestClient : public BaseApiClient, public std::enable_shared_from_this<TestClient> {
public:
    using std::enable_shared_from_this<TestClient>::shared_from_this;
    template <typename T>
    any testGet(string path);
    template <typename T>
    any testPost(string path, any body);
    template <typename T>
    any testRequest(string method, string path, any options = std::nullopt);
    TestClient(std::shared_ptr<ApiClientConfig> config);
};

template <typename T>
any TestClient::testGet(string path)
{
    return this->get<T>(path);
}

template <typename T>
any TestClient::testPost(string path, any body)
{
    return this->post<T>(path, body);
}

template <typename T>
any TestClient::testRequest(string method, string path, any options)
{
    return this->request<T>(method, path, options);
}

#endif
