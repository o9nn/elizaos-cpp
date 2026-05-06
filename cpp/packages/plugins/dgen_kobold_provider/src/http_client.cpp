#include "http_client.hpp"
#include <stdexcept>
#include <sstream>

#if HAS_LIBCURL
#include <curl/curl.h>

namespace {

size_t write_callback(char* ptr, size_t size, size_t nmemb, void* userdata) {
    auto* response = static_cast<std::string*>(userdata);
    response->append(ptr, size * nmemb);
    return size * nmemb;
}

} // anonymous namespace

namespace elizaos {

json HttpClient::post(const std::string& url, const json& body, const std::string& api_key) {
    CURL* curl = curl_easy_init();
    if (!curl) {
        throw std::runtime_error("Failed to initialize libcurl");
    }

    std::string response_body;
    std::string request_body = body.dump();

    struct curl_slist* headers = nullptr;
    headers = curl_slist_append(headers, "Content-Type: application/json");
    if (!api_key.empty()) {
        std::string auth_header = "Authorization: Bearer " + api_key;
        headers = curl_slist_append(headers, auth_header.c_str());
    }

    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, request_body.c_str());
    curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, static_cast<long>(request_body.size()));
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_body);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 120L);
    curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 10L);

    CURLcode res = curl_easy_perform(curl);

    long http_code = 0;
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);

    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);

    if (res != CURLE_OK) {
        throw std::runtime_error(std::string("HTTP request failed: ") + curl_easy_strerror(res));
    }

    if (http_code >= 400) {
        std::ostringstream oss;
        oss << "HTTP Error " << http_code << ": " << response_body;
        throw std::runtime_error(oss.str());
    }

    return json::parse(response_body);
}

} // namespace elizaos

#else // !HAS_LIBCURL

namespace elizaos {

json HttpClient::post(const std::string& /*url*/, const json& /*body*/, const std::string& /*api_key*/) {
    throw std::runtime_error(
        "HTTP client not available: libcurl was not found at build time. "
        "Install libcurl-dev and rebuild to enable DreamGen/KoboldCpp providers."
    );
}

} // namespace elizaos

#endif // HAS_LIBCURL
