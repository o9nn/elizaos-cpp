// HTTP Client implementation using libcurl for AgentBrowser
// Replaces mock HTTP functionality with real web requests

#include "elizaos/agentbrowser.hpp"
#include <curl/curl.h>
#include <sstream>
#include <iostream>

namespace elizaos {
namespace browser_impl {

// Callback function for libcurl to write response data
static size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp) {
    ((std::string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}

class HttpClient {
private:
    CURL* curl_;
    std::string lastUrl_;
    std::string lastHtml_;
    long lastHttpCode_;
    std::string lastError_;
    
public:
    HttpClient() : curl_(nullptr), lastHttpCode_(0) {
        curl_global_init(CURL_GLOBAL_DEFAULT);
        curl_ = curl_easy_init();
    }
    
    ~HttpClient() {
        if (curl_) {
            curl_easy_cleanup(curl_);
        }
        curl_global_cleanup();
    }
    
    bool fetch(const std::string& url, int timeoutSeconds = 30) {
        if (!curl_) {
            lastError_ = "CURL not initialized";
            return false;
        }
        
        lastUrl_ = url;
        lastHtml_.clear();
        lastError_.clear();
        
        curl_easy_setopt(curl_, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl_, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl_, CURLOPT_WRITEDATA, &lastHtml_);
        curl_easy_setopt(curl_, CURLOPT_TIMEOUT, timeoutSeconds);
        curl_easy_setopt(curl_, CURLOPT_FOLLOWLOCATION, 1L);
        curl_easy_setopt(curl_, CURLOPT_MAXREDIRS, 5L);
        curl_easy_setopt(curl_, CURLOPT_USERAGENT, "ElizaOS-AgentBrowser/1.0");
        curl_easy_setopt(curl_, CURLOPT_SSL_VERIFYPEER, 1L);
        curl_easy_setopt(curl_, CURLOPT_SSL_VERIFYHOST, 2L);
        
        CURLcode res = curl_easy_perform(curl_);
        
        if (res != CURLE_OK) {
            lastError_ = curl_easy_strerror(res);
            return false;
        }
        
        curl_easy_getinfo(curl_, CURLINFO_RESPONSE_CODE, &lastHttpCode_);
        
        return lastHttpCode_ >= 200 && lastHttpCode_ < 300;
    }
    
    const std::string& getHtml() const {
        return lastHtml_;
    }
    
    const std::string& getUrl() const {
        return lastUrl_;
    }
    
    long getHttpCode() const {
        return lastHttpCode_;
    }
    
    const std::string& getError() const {
        return lastError_;
    }
    
    bool isSuccess() const {
        return lastHttpCode_ >= 200 && lastHttpCode_ < 300;
    }
};

} // namespace browser_impl
} // namespace elizaos
