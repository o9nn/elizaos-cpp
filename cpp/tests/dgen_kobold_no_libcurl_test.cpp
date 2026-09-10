#include <gtest/gtest.h>

#include "http_client.hpp"

TEST(DgenKoboldNoLibcurl, CapabilityFailsTruthfullyWithoutAdapter) {
    EXPECT_FALSE(elizaos::HttpClient::libcurlAvailable());
    try {
        static_cast<void>(elizaos::HttpClient::createLibcurlTransport());
        FAIL() << "Expected disabled libcurl adapter construction to fail";
    } catch (const elizaos::TransportError& error) {
        EXPECT_EQ(error.code(), elizaos::TransportErrorCode::Unavailable);
        EXPECT_STREQ(error.what(), "HTTP transport is unavailable");
    }
}

TEST(DgenKoboldNoLibcurl, InjectedTransportRemainsIndependentOfLibcurl) {
    class Fake final : public elizaos::HttpTransport {
    public:
        elizaos::HttpResponse execute(const elizaos::HttpRequest&) const override {
            return {200L, "application/json", "{\"ok\":true}"};
        }
    };
    const elizaos::HttpClient client(std::make_shared<Fake>());
    EXPECT_TRUE(client.getJson("http://localhost/")["ok"].get<bool>());
}
