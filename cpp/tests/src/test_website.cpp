#include <gtest/gtest.h>

#include "elizaos/website.hpp"

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include <atomic>
#include <chrono>
#include <condition_variable>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <future>
#include <iterator>
#include <mutex>
#include <string>
#include <thread>
#include <vector>

namespace elizaos {
namespace {

using namespace std::chrono_literals;

class ScopedSocket {
public:
    explicit ScopedSocket(int descriptor = -1) : descriptor_(descriptor) {}
    ~ScopedSocket() {
        if (descriptor_ >= 0) {
            ::close(descriptor_);
        }
    }
    ScopedSocket(const ScopedSocket&) = delete;
    ScopedSocket& operator=(const ScopedSocket&) = delete;
    ScopedSocket(ScopedSocket&& other) noexcept : descriptor_(other.descriptor_) {
        other.descriptor_ = -1;
    }
    ScopedSocket& operator=(ScopedSocket&& other) noexcept {
        if (this != &other) {
            if (descriptor_ >= 0) {
                ::close(descriptor_);
            }
            descriptor_ = other.descriptor_;
            other.descriptor_ = -1;
        }
        return *this;
    }
    int get() const { return descriptor_; }

private:
    int descriptor_;
};

class TemporaryDirectory {
public:
    TemporaryDirectory() {
        static std::atomic<unsigned long long> sequence{0};
        path_ = std::filesystem::temp_directory_path() /
                ("elizaos-website-e2e-" +
                 std::to_string(std::chrono::steady_clock::now().time_since_epoch().count()) +
                 "-" + std::to_string(sequence.fetch_add(1)));
        std::filesystem::create_directories(path_);
    }
    ~TemporaryDirectory() {
        std::error_code error;
        std::filesystem::remove_all(path_, error);
    }
    const std::filesystem::path& path() const { return path_; }

private:
    std::filesystem::path path_;
};

WebsiteConfig configFor(const std::filesystem::path& root) {
    WebsiteConfig config;
    config.source_dir = root / "source";
    config.output_dir = root / "output";
    config.templates_dir = root / "templates";
    config.assets_dir = root / "assets";
    return config;
}

void writeFile(const std::filesystem::path& path, const std::string& contents) {
    std::filesystem::create_directories(path.parent_path());
    std::ofstream file(path, std::ios::binary | std::ios::trunc);
    ASSERT_TRUE(file.is_open());
    file << contents;
    ASSERT_TRUE(file.good());
}

ScopedSocket connectTo(std::uint16_t port) {
    ScopedSocket socket_handle(::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP));
    if (socket_handle.get() < 0) {
        return socket_handle;
    }
    const timeval timeout{2, 0};
    (void)::setsockopt(socket_handle.get(), SOL_SOCKET, SO_RCVTIMEO, &timeout,
                       static_cast<socklen_t>(sizeof(timeout)));
    (void)::setsockopt(socket_handle.get(), SOL_SOCKET, SO_SNDTIMEO, &timeout,
                       static_cast<socklen_t>(sizeof(timeout)));
    sockaddr_in address{};
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    address.sin_port = htons(port);
    if (::connect(socket_handle.get(), reinterpret_cast<const sockaddr*>(&address),
                  static_cast<socklen_t>(sizeof(address))) != 0) {
        return ScopedSocket{};
    }
    return socket_handle;
}

std::string request(std::uint16_t port, const std::string& wire_request) {
    auto socket_handle = connectTo(port);
    if (socket_handle.get() < 0) {
        return {};
    }
    std::size_t sent = 0;
    while (sent < wire_request.size()) {
        const auto result = ::send(socket_handle.get(), wire_request.data() + sent,
                                   wire_request.size() - sent, MSG_NOSIGNAL);
        if (result <= 0) {
            return {};
        }
        sent += static_cast<std::size_t>(result);
    }
    (void)::shutdown(socket_handle.get(), SHUT_WR);

    std::string response;
    char buffer[4096];
    for (;;) {
        const auto received = ::recv(socket_handle.get(), buffer, sizeof(buffer), 0);
        if (received <= 0) {
            break;
        }
        response.append(buffer, static_cast<std::size_t>(received));
    }
    return response;
}

int statusOf(const std::string& response) {
    const auto first_space = response.find(' ');
    if (first_space == std::string::npos || first_space + 4U > response.size()) {
        return 0;
    }
    return std::stoi(response.substr(first_space + 1U, 3U));
}

std::string bodyOf(const std::string& response) {
    const auto separator = response.find("\r\n\r\n");
    return separator == std::string::npos ? std::string{} : response.substr(separator + 4U);
}

class RunningWebsite {
public:
    RunningWebsite() : config_(configFor(directory_.path())), website_(config_) {
        std::filesystem::create_directories(config_.source_dir);
        std::filesystem::create_directories(config_.output_dir);
        std::filesystem::create_directories(config_.templates_dir);
        std::filesystem::create_directories(config_.assets_dir);
        writeFile(config_.output_dir / "index.html", "<h1>loopback home</h1>");
        writeFile(config_.output_dir / "nested" / "index.html", "nested index");
        writeFile(config_.output_dir / "assets" / "site.css", "body{color:blue}");
        EXPECT_TRUE(website_.initialize());
        EXPECT_TRUE(website_.startDevelopmentServer(0));
        EXPECT_TRUE(website_.isDevelopmentServerRunning());
        EXPECT_NE(website_.getDevelopmentServerPort(), 0U);
    }

    Website& website() { return website_; }
    const WebsiteConfig& config() const { return config_; }

private:
    TemporaryDirectory directory_;
    WebsiteConfig config_;
    Website website_;
};

TEST(WebsiteDevelopmentServerE2E, ServesGetHeadDirectoryIndexAndMimeTypes) {
    RunningWebsite running;
    const auto port = running.website().getDevelopmentServerPort();

    const auto get = request(port, "GET / HTTP/1.1\r\nHost: localhost\r\n\r\n");
    EXPECT_EQ(statusOf(get), 200);
    EXPECT_EQ(bodyOf(get), "<h1>loopback home</h1>");
    EXPECT_NE(get.find("Content-Type: text/html; charset=utf-8"), std::string::npos);
    EXPECT_NE(get.find("Content-Length: 22"), std::string::npos);

    const auto head = request(port, "HEAD / HTTP/1.1\r\nHost: localhost\r\n\r\n");
    EXPECT_EQ(statusOf(head), 200);
    EXPECT_TRUE(bodyOf(head).empty());
    EXPECT_NE(head.find("Content-Length: 22"), std::string::npos);

    const auto nested = request(port, "GET /nested/ HTTP/1.0\r\n\r\n");
    EXPECT_EQ(statusOf(nested), 200);
    EXPECT_EQ(bodyOf(nested), "nested index");

    const auto css = request(port, "GET /assets/site.css?cache=1 HTTP/1.1\r\nHost: x\r\n\r\n");
    EXPECT_EQ(statusOf(css), 200);
    EXPECT_NE(css.find("Content-Type: text/css; charset=utf-8"), std::string::npos);
}

TEST(WebsiteDevelopmentServerE2E, ReturnsMissingMethodAndMalformedErrors) {
    RunningWebsite running;
    const auto port = running.website().getDevelopmentServerPort();

    EXPECT_EQ(statusOf(request(port, "GET /missing HTTP/1.1\r\nHost: x\r\n\r\n")), 404);
    const auto post = request(port, "POST / HTTP/1.1\r\nHost: x\r\nContent-Length: 0\r\n\r\n");
    EXPECT_EQ(statusOf(post), 405);
    EXPECT_NE(post.find("Allow: GET, HEAD"), std::string::npos);
    EXPECT_EQ(statusOf(request(port, "GET / HTTP/9.9\r\n\r\n")), 400);
    EXPECT_EQ(statusOf(request(port, "GET / HTTP/1.1\r\nBadHeader\r\n\r\n")), 400);
    EXPECT_EQ(statusOf(request(port, "GET / HTTP/1.1\r\nTransfer-Encoding: chunked\r\n\r\n")), 400);

    std::string oversized = "GET / HTTP/1.1\r\nX-Fill: ";
    oversized.append(17U * 1024U, 'x');
    oversized += "\r\n\r\n";
    EXPECT_EQ(statusOf(request(port, oversized)), 431);
}

TEST(WebsiteDevelopmentServerE2E, BlocksPlainEncodedAndSymlinkEscapes) {
    TemporaryDirectory outside;
    writeFile(outside.path() / "secret.txt", "never expose this");

    RunningWebsite running;
    std::error_code symlink_error;
    std::filesystem::create_symlink(outside.path() / "secret.txt",
                                    running.config().output_dir / "escape.txt", symlink_error);
    ASSERT_FALSE(symlink_error);
    const auto port = running.website().getDevelopmentServerPort();

    for (const std::string path : {"/../secret.txt", "/%2e%2e/secret.txt", "/escape.txt"}) {
        const auto response = request(port, "GET " + path + " HTTP/1.1\r\nHost: x\r\n\r\n");
        EXPECT_EQ(statusOf(response), 404) << path;
        EXPECT_EQ(response.find("never expose this"), std::string::npos) << path;
    }
    EXPECT_EQ(statusOf(request(port, "GET /%zz HTTP/1.1\r\nHost: x\r\n\r\n")), 400);
}

TEST(WebsiteDevelopmentServerE2E, ReportsInvalidAndOccupiedPortFailuresOutsideLocks) {
    TemporaryDirectory temporary;
    Website website(configFor(temporary.path()));
    ASSERT_TRUE(website.initialize());

    std::atomic<int> callback_count{0};
    website.setDevelopmentServerErrorCallback([&](const std::string&) {
        ++callback_count;
        EXPECT_FALSE(website.getLastDevelopmentServerError().empty());
        website.setDevelopmentServerErrorCallback(nullptr);
    });
    EXPECT_FALSE(website.startDevelopmentServer(70000));
    EXPECT_EQ(callback_count.load(), 1);

    ScopedSocket occupied(::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP));
    ASSERT_GE(occupied.get(), 0);
    sockaddr_in address{};
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    address.sin_port = 0;
    ASSERT_EQ(::bind(occupied.get(), reinterpret_cast<const sockaddr*>(&address),
                     static_cast<socklen_t>(sizeof(address))), 0);
    ASSERT_EQ(::listen(occupied.get(), 1), 0);
    socklen_t size = static_cast<socklen_t>(sizeof(address));
    ASSERT_EQ(::getsockname(occupied.get(), reinterpret_cast<sockaddr*>(&address), &size), 0);
    EXPECT_FALSE(website.startDevelopmentServer(ntohs(address.sin_port)));
    EXPECT_NE(website.getLastDevelopmentServerError().find("bind"), std::string::npos);
}

TEST(WebsiteDevelopmentServerE2E, StopRestartAndConfigMutationAreDeterministic) {
    RunningWebsite running;
    Website& website = running.website();
    const auto first_port = website.getDevelopmentServerPort();
    EXPECT_FALSE(website.startDevelopmentServer(0));

    WebsiteConfig changed = website.getConfig();
    changed.site_title = "changed";
    EXPECT_FALSE(website.updateConfig(changed));

    website.stopDevelopmentServer();
    EXPECT_FALSE(website.isDevelopmentServerRunning());
    EXPECT_EQ(website.getDevelopmentServerPort(), 0U);
    EXPECT_TRUE(website.getDevelopmentServerDocumentRoot().empty());
    EXPECT_TRUE(request(first_port, "GET / HTTP/1.0\r\n\r\n").empty());

    EXPECT_TRUE(website.updateConfig(changed));
    EXPECT_TRUE(website.serveDevelopmentSite(0));
    EXPECT_TRUE(website.isDevelopmentServerRunning());
    EXPECT_NE(website.getDevelopmentServerPort(), 0U);
    EXPECT_EQ(statusOf(request(website.getDevelopmentServerPort(),
                               "GET / HTTP/1.0\r\n\r\n")), 200);
    website.stopDevelopmentServer();
    website.stopDevelopmentServer();
}

TEST(WebsiteDevelopmentServerE2E, ErrorCallbackCanStopFromAcceptThreadWithoutDetach) {
    RunningWebsite running;
    Website& website = running.website();
    const auto first_port = website.getDevelopmentServerPort();

    std::mutex callback_mutex;
    std::condition_variable callback_changed;
    bool callback_finished = false;
    std::atomic<int> callback_count{0};
    website.setDevelopmentServerErrorCallback([&](const std::string&) {
        ++callback_count;
        website.stopDevelopmentServer();
        {
            std::lock_guard<std::mutex> lock(callback_mutex);
            callback_finished = true;
        }
        callback_changed.notify_one();
    });

    // Four clients occupy the workers and 64 fill the bounded queue. The next
    // accepted client reports queue saturation from the accept thread itself.
    std::vector<ScopedSocket> stalled_clients;
    stalled_clients.reserve(96U);
    for (std::size_t index = 0; index < 96U && callback_count.load() == 0; ++index) {
        auto client = connectTo(first_port);
        if (client.get() >= 0) {
            stalled_clients.emplace_back(std::move(client));
        }
    }

    {
        std::unique_lock<std::mutex> lock(callback_mutex);
        ASSERT_TRUE(callback_changed.wait_for(lock, 3s, [&]() { return callback_finished; }));
    }
    EXPECT_EQ(callback_count.load(), 1);
    EXPECT_FALSE(website.isDevelopmentServerRunning());
    EXPECT_EQ(website.getDevelopmentServerPort(), 0U);

    // An external stop reaps the callback's own completed thread. A restart
    // proves no thread was detached and no stale generation remains.
    website.setDevelopmentServerErrorCallback(nullptr);
    website.stopDevelopmentServer();
    EXPECT_TRUE(website.startDevelopmentServer(0));
    EXPECT_EQ(statusOf(request(website.getDevelopmentServerPort(),
                               "GET / HTTP/1.0\r\n\r\n")), 200);
}

TEST(WebsiteDevelopmentServerE2E, ConcurrentStopsCancelBlockedClientsAndRemainRestartable) {
    RunningWebsite running;
    Website& website = running.website();
    const auto first_port = website.getDevelopmentServerPort();

    std::vector<ScopedSocket> stalled_clients;
    for (int index = 0; index < 12; ++index) {
        auto client = connectTo(first_port);
        ASSERT_GE(client.get(), 0);
        const std::string partial = "GET / HTTP/1.1\r\nHost: x\r\n";
        ASSERT_GT(::send(client.get(), partial.data(), partial.size(), MSG_NOSIGNAL), 0);
        stalled_clients.emplace_back(std::move(client));
    }

    std::vector<std::thread> stoppers;
    for (int index = 0; index < 8; ++index) {
        stoppers.emplace_back([&website]() { website.stopDevelopmentServer(); });
    }
    for (auto& stopper : stoppers) {
        stopper.join();
    }

    EXPECT_FALSE(website.isDevelopmentServerRunning());
    EXPECT_TRUE(request(first_port, "GET / HTTP/1.0\r\n\r\n").empty());
    EXPECT_TRUE(website.startDevelopmentServer(0));
    EXPECT_EQ(statusOf(request(website.getDevelopmentServerPort(),
                               "GET / HTTP/1.0\r\n\r\n")), 200);
}

TEST(WebsiteDevelopmentServerE2E, HandlesConcurrentClients) {
    RunningWebsite running;
    const auto port = running.website().getDevelopmentServerPort();
    constexpr int client_count = 48;
    std::vector<std::future<bool>> clients;
    clients.reserve(client_count);
    for (int index = 0; index < client_count; ++index) {
        clients.emplace_back(std::async(std::launch::async, [port]() {
            return statusOf(request(port, "GET / HTTP/1.1\r\nHost: x\r\n\r\n")) == 200;
        }));
    }
    for (auto& client : clients) {
        EXPECT_TRUE(client.get());
    }
}

TEST(WebsiteDevelopmentServerE2E, DestructorCancelsBlockedClientsAndDoesNoWorkAfterTeardown) {
    TemporaryDirectory temporary;
    const WebsiteConfig config = configFor(temporary.path());
    std::filesystem::create_directories(config.output_dir);
    writeFile(config.output_dir / "index.html", "alive");

    std::uint16_t old_port = 0;
    ScopedSocket stalled;
    const auto start = std::chrono::steady_clock::now();
    {
        Website website(config);
        ASSERT_TRUE(website.initialize());
        ASSERT_TRUE(website.startDevelopmentServer(0));
        old_port = website.getDevelopmentServerPort();
        stalled = connectTo(old_port);
        ASSERT_GE(stalled.get(), 0);
        const std::string partial = "GET / HTTP/1.1\r\nHost: x\r\n";
        ASSERT_GT(::send(stalled.get(), partial.data(), partial.size(), MSG_NOSIGNAL), 0);
    }
    EXPECT_LT(std::chrono::steady_clock::now() - start, 2s);

    char byte = 0;
    EXPECT_LE(::recv(stalled.get(), &byte, 1, 0), 0);
    EXPECT_TRUE(request(old_port, "GET / HTTP/1.0\r\n\r\n").empty());
}

TEST(WebsiteDevelopmentServerStress, RepeatedStartTrafficStopAndDestruction) {
    for (int iteration = 0; iteration < 40; ++iteration) {
        TemporaryDirectory temporary;
        const WebsiteConfig config = configFor(temporary.path());
        std::filesystem::create_directories(config.output_dir);
        writeFile(config.output_dir / "index.html", "stress");
        Website website(config);
        ASSERT_TRUE(website.initialize()) << iteration;
        ASSERT_TRUE(website.startDevelopmentServer(0)) << iteration;
        const auto port = website.getDevelopmentServerPort();
        EXPECT_EQ(statusOf(request(port, "GET / HTTP/1.0\r\n\r\n")), 200) << iteration;
        website.stopDevelopmentServer();
        EXPECT_FALSE(website.isDevelopmentServerRunning()) << iteration;
    }
}

} // namespace
} // namespace elizaos
