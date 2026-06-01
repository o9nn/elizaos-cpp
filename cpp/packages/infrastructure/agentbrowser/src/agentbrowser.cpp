// HTTP-backed browser automation interface for ElizaOS agents.
//
// This implementation provides real HTTP retrieval, HTML parsing, selector lookup,
// session state, and truthful local interaction semantics. It is not a full
// headless browser: JavaScript
// execution and visual screenshots are represented as explicit best-effort
// diagnostics instead of fabricated browser actions.

#include "elizaos/agentbrowser.hpp"
#include "elizaos/agentlogger.hpp"
#include "elizaos/agentmemory.hpp"

#include <curl/curl.h>
#include <gumbo.h>

#include <algorithm>
#include <cctype>
#include <cstring>
#include <fstream>
#include <iomanip>
#include <optional>
#include <random>
#include <regex>
#include <sstream>
#include <stdexcept>
#include <map>
#include <string>
#include <thread>
#include <unordered_set>
#include <vector>

namespace elizaos {
namespace browser_impl {

static std::string trim(const std::string& value) {
    auto begin = std::find_if_not(value.begin(), value.end(), [](unsigned char c) { return std::isspace(c); });
    auto end = std::find_if_not(value.rbegin(), value.rend(), [](unsigned char c) { return std::isspace(c); }).base();
    if (begin >= end) return "";
    return std::string(begin, end);
}

static std::string toLower(std::string value) {
    std::transform(value.begin(), value.end(), value.begin(), [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
    return value;
}

static std::string normalizeWhitespace(const std::string& value) {
    std::string out;
    out.reserve(value.size());
    bool inSpace = false;
    for (unsigned char c : value) {
        if (std::isspace(c)) {
            if (!inSpace && !out.empty()) out.push_back(' ');
            inSpace = true;
        } else {
            out.push_back(static_cast<char>(c));
            inSpace = false;
        }
    }
    return trim(out);
}

static size_t writeCallback(char* ptr, size_t size, size_t nmemb, void* userdata) {
    auto* target = static_cast<std::string*>(userdata);
    target->append(ptr, size * nmemb);
    return size * nmemb;
}

static bool startsWith(const std::string& value, const std::string& prefix) {
    return value.rfind(prefix, 0) == 0;
}

static std::string urlEncode(const std::string& value) {
    std::ostringstream encoded;
    encoded << std::uppercase << std::hex;
    for (unsigned char c : value) {
        if (std::isalnum(c) || c == '-' || c == '_' || c == '.' || c == '~') {
            encoded << static_cast<char>(c);
        } else if (c == ' ') {
            encoded << '+';
        } else {
            encoded << '%' << std::setw(2) << std::setfill('0') << static_cast<int>(c) << std::setfill(' ');
        }
    }
    return encoded.str();
}

static std::string removeFragment(const std::string& url) {
    auto pos = url.find('#');
    return pos == std::string::npos ? url : url.substr(0, pos);
}

static std::string resolveUrl(const std::string& baseUrl, const std::string& target) {
    const std::string href = trim(target);
    if (href.empty()) return baseUrl;
    if (startsWith(href, "http://") || startsWith(href, "https://")) return href;

    static const std::regex baseRegex(R"(^(https?)://([^/?#]+)([^?#]*)?(\?[^#]*)?(#.*)?$)", std::regex_constants::icase);
    std::smatch match;
    if (!std::regex_match(baseUrl, match, baseRegex)) return href;

    const std::string scheme = match[1].str();
    const std::string authority = match[2].str();
    const std::string rawPath = match[3].matched && !match[3].str().empty() ? match[3].str() : "/";
    const std::string origin = scheme + "://" + authority;

    if (startsWith(href, "//")) return scheme + ":" + href;
    if (href.front() == '/') return origin + href;
    if (href.front() == '?') return origin + rawPath + href;
    if (href.front() == '#') return removeFragment(baseUrl) + href;

    std::string directory = rawPath;
    auto slash = directory.rfind('/');
    directory = (slash == std::string::npos) ? "/" : directory.substr(0, slash + 1);

    std::vector<std::string> segments;
    auto pushSegments = [&](const std::string& path) {
        std::size_t start = 0;
        while (start <= path.size()) {
            const auto end = path.find('/', start);
            std::string part = path.substr(start, end == std::string::npos ? std::string::npos : end - start);
            if (!part.empty() && part != ".") {
                if (part == "..") {
                    if (!segments.empty()) segments.pop_back();
                } else {
                    segments.push_back(part);
                }
            }
            if (end == std::string::npos) break;
            start = end + 1;
        }
    };

    pushSegments(directory);
    pushSegments(href);

    std::ostringstream path;
    path << '/';
    for (std::size_t i = 0; i < segments.size(); ++i) {
        if (i) path << '/';
        path << segments[i];
    }
    return origin + path.str();
}

static std::string appendQuery(const std::string& url, const std::map<std::string, std::string>& params) {
    if (params.empty()) return url;
    std::ostringstream query;
    bool first = true;
    for (const auto& [key, value] : params) {
        if (key.empty()) continue;
        query << (first ? "" : "&") << urlEncode(key) << '=' << urlEncode(value);
        first = false;
    }
    if (first) return url;

    const auto fragmentPos = url.find('#');
    const std::string withoutFragment = fragmentPos == std::string::npos ? url : url.substr(0, fragmentPos);
    const std::string fragment = fragmentPos == std::string::npos ? "" : url.substr(fragmentPos);
    const char separator = withoutFragment.find('?') == std::string::npos ? '?' : '&';
    return withoutFragment + separator + query.str() + fragment;
}

static std::string formFieldName(const std::string& selector, const WebElement& element) {
    auto name = element.attributes.find("name");
    if (name != element.attributes.end() && !name->second.empty()) return name->second;
    if (!element.id.empty()) return element.id;
    std::string cleaned = selector;
    if (!cleaned.empty() && (cleaned.front() == '#' || cleaned.front() == '.')) cleaned.erase(cleaned.begin());
    return cleaned;
}

static std::string formControlValue(const WebElement& element) {
    auto value = element.attributes.find("value");
    if (value != element.attributes.end()) return value->second;
    return element.text;
}

static bool isSuccessfulTextControl(const WebElement& element) {
    const std::string tag = toLower(element.tag);
    const std::string type = [&]() {
        auto it = element.attributes.find("type");
        return it == element.attributes.end() ? std::string{} : toLower(it->second);
    }();
    if (tag == "textarea" || tag == "select") return true;
    if (tag != "input") return false;
    static const std::unordered_set<std::string> skipped = {
        "button", "checkbox", "file", "image", "radio", "reset", "submit"
    };
    return skipped.find(type) == skipped.end();
}

static bool isCheckableControl(const WebElement& element) {
    const std::string tag = toLower(element.tag);
    auto typeIt = element.attributes.find("type");
    const std::string type = typeIt == element.attributes.end() ? std::string{} : toLower(typeIt->second);
    return tag == "input" && (type == "checkbox" || type == "radio");
}

static bool hasHtmlAttribute(const WebElement& element, const std::string& name) {
    return element.attributes.find(name) != element.attributes.end();
}

class HttpClient {
public:
    HttpClient() {
        curl_global_init(CURL_GLOBAL_DEFAULT);
        curl_ = curl_easy_init();
        if (!curl_) throw std::runtime_error("curl_easy_init failed");
    }

    ~HttpClient() {
        if (curl_) curl_easy_cleanup(curl_);
        curl_global_cleanup();
    }

    bool fetch(const std::string& url, const BrowserConfig& config) {
        lastUrl_ = url;
        effectiveUrl_ = url;
        lastHtml_.clear();
        lastError_.clear();
        lastHttpCode_ = 0;

        curl_easy_reset(curl_);
        curl_easy_setopt(curl_, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl_, CURLOPT_WRITEFUNCTION, writeCallback);
        curl_easy_setopt(curl_, CURLOPT_WRITEDATA, &lastHtml_);
        curl_easy_setopt(curl_, CURLOPT_FOLLOWLOCATION, 1L);
        curl_easy_setopt(curl_, CURLOPT_MAXREDIRS, 10L);
        curl_easy_setopt(curl_, CURLOPT_TIMEOUT, std::max(1, config.pageLoadTimeout));
        curl_easy_setopt(curl_, CURLOPT_CONNECTTIMEOUT, std::min(std::max(1, config.pageLoadTimeout), 10));
        curl_easy_setopt(curl_, CURLOPT_USERAGENT, config.userAgent.empty() ? "ElizaOS-AgentBrowser/1.0" : config.userAgent.c_str());
        curl_easy_setopt(curl_, CURLOPT_ACCEPT_ENCODING, "");
        curl_easy_setopt(curl_, CURLOPT_SSL_VERIFYPEER, 1L);
        curl_easy_setopt(curl_, CURLOPT_SSL_VERIFYHOST, 2L);

        CURLcode result = curl_easy_perform(curl_);
        if (result != CURLE_OK) {
            lastError_ = curl_easy_strerror(result);
            return false;
        }

        curl_easy_getinfo(curl_, CURLINFO_RESPONSE_CODE, &lastHttpCode_);
        char* effective = nullptr;
        if (curl_easy_getinfo(curl_, CURLINFO_EFFECTIVE_URL, &effective) == CURLE_OK && effective) {
            effectiveUrl_ = effective;
        }

        if (lastHttpCode_ < 200 || lastHttpCode_ >= 300) {
            lastError_ = "HTTP " + std::to_string(lastHttpCode_);
            return false;
        }
        return true;
    }

    const std::string& html() const { return lastHtml_; }
    const std::string& error() const { return lastError_; }
    const std::string& effectiveUrl() const { return effectiveUrl_; }
    long httpCode() const { return lastHttpCode_; }

private:
    CURL* curl_ = nullptr;
    std::string lastUrl_;
    std::string effectiveUrl_;
    std::string lastHtml_;
    std::string lastError_;
    long lastHttpCode_ = 0;
};

class HtmlParser {
public:
    HtmlParser() = default;

    ~HtmlParser() { reset(); }

    bool parse(const std::string& html) {
        reset();
        html_ = html;
        output_ = gumbo_parse(html.c_str());
        if (!output_ || !output_->root) return false;

        title_.clear();
        text_.clear();
        links_.clear();
        images_.clear();

        findTitle(output_->root);
        extractText(output_->root, text_);
        text_ = normalizeWhitespace(text_);
        findLinks(output_->root);
        findImages(output_->root);
        return true;
    }

    bool isParsed() const { return output_ != nullptr; }
    const std::string& html() const { return html_; }
    const std::string& title() const { return title_; }
    const std::string& text() const { return text_; }
    const std::vector<std::string>& links() const { return links_; }
    const std::vector<std::string>& images() const { return images_; }

    std::vector<WebElement> queryAll(const std::string& selector, SelectorType type) const {
        std::vector<GumboNode*> nodes;
        if (!output_ || !output_->root) return {};
        collectMatching(selector, type, nodes);
        return toElements(nodes);
    }

    std::vector<WebElement> formControls(const std::string& formSelector, SelectorType type) const {
        std::vector<GumboNode*> formNodes;
        if (!output_ || !output_->root) return {};
        collectMatching(formSelector, type, formNodes);
        auto selectedForm = std::find_if(formNodes.begin(), formNodes.end(), [](GumboNode* node) {
            return tagName(node) == "form";
        });
        if (selectedForm == formNodes.end()) return {};

        std::vector<GumboNode*> controls;
        collectByPredicate(*selectedForm, [](GumboNode* node) {
            const std::string tag = tagName(node);
            return tag == "input" || tag == "select" || tag == "textarea" || tag == "button";
        }, controls);
        return toElements(controls);
    }

private:
    using Predicate = std::function<bool(GumboNode*)>;

    void collectMatching(const std::string& selector, SelectorType type, std::vector<GumboNode*>& nodes) const {
        switch (type) {
            case SelectorType::ID:
                collectByPredicate(output_->root, [&](GumboNode* node) { return attr(node, "id") == stripPrefix(selector, '#'); }, nodes);
                break;
            case SelectorType::CLASS_NAME:
                collectByPredicate(output_->root, [&](GumboNode* node) { return hasClass(node, stripPrefix(selector, '.')); }, nodes);
                break;
            case SelectorType::TAG_NAME:
                collectByPredicate(output_->root, [&](GumboNode* node) { return tagName(node) == toLower(selector); }, nodes);
                break;
            case SelectorType::CSS:
                queryCss(selector, nodes);
                break;
            case SelectorType::XPATH:
                queryBasicXPath(selector, nodes);
                break;
        }
    }

    std::vector<WebElement> toElements(const std::vector<GumboNode*>& nodes) const {
        std::vector<WebElement> elements;
        elements.reserve(nodes.size());
        for (auto* node : nodes) elements.push_back(toElement(node));
        return elements;
    }

    void reset() {
        if (output_) {
            gumbo_destroy_output(&kGumboDefaultOptions, output_);
            output_ = nullptr;
        }
    }

    static bool isElement(GumboNode* node) {
        return node && node->type == GUMBO_NODE_ELEMENT;
    }

    static std::string tagName(GumboNode* node) {
        if (!isElement(node)) return "";
        const char* name = gumbo_normalized_tagname(node->v.element.tag);
        return name ? std::string(name) : "";
    }

    static std::string attr(GumboNode* node, const std::string& name) {
        if (!isElement(node)) return "";
        GumboAttribute* value = gumbo_get_attribute(&node->v.element.attributes, name.c_str());
        return (value && value->value) ? std::string(value->value) : "";
    }

    static std::string nodeText(GumboNode* node) {
        if (!node) return "";
        if (node->type == GUMBO_NODE_TEXT || node->type == GUMBO_NODE_WHITESPACE) {
            return node->v.text.text ? std::string(node->v.text.text) : "";
        }
        if (!isElement(node)) return "";
        std::string text;
        GumboVector* children = &node->v.element.children;
        for (unsigned int i = 0; i < children->length; ++i) {
            text += nodeText(static_cast<GumboNode*>(children->data[i]));
            text.push_back(' ');
        }
        return normalizeWhitespace(text);
    }

    static std::string optionValue(GumboNode* option) {
        const std::string explicitValue = attr(option, "value");
        return explicitValue.empty() ? nodeText(option) : explicitValue;
    }

    static std::string selectedOptionValue(GumboNode* select) {
        if (tagName(select) != "select") return "";
        std::optional<std::string> firstOption;
        GumboVector* children = &select->v.element.children;
        for (unsigned int i = 0; i < children->length; ++i) {
            auto* child = static_cast<GumboNode*>(children->data[i]);
            if (tagName(child) != "option") continue;
            const std::string value = optionValue(child);
            if (!firstOption) firstOption = value;
            if (hasAttr(child, "selected")) return value;
        }
        return firstOption.value_or("");
    }

    static bool hasAttr(GumboNode* node, const std::string& name) {
        if (!isElement(node)) return false;
        return gumbo_get_attribute(&node->v.element.attributes, name.c_str()) != nullptr;
    }

    static bool hasClass(GumboNode* node, const std::string& className) {
        std::istringstream classes(attr(node, "class"));
        std::string token;
        while (classes >> token) {
            if (token == className) return true;
        }
        return false;
    }

    static std::string stripPrefix(const std::string& value, char prefix) {
        return (!value.empty() && value.front() == prefix) ? value.substr(1) : value;
    }

    void extractText(GumboNode* node, std::string& target) const {
        if (!node) return;
        if (node->type == GUMBO_NODE_TEXT || node->type == GUMBO_NODE_WHITESPACE) {
            if (node->v.text.text) {
                target += node->v.text.text;
                target.push_back(' ');
            }
            return;
        }
        if (!isElement(node)) return;
        auto tag = node->v.element.tag;
        if (tag == GUMBO_TAG_SCRIPT || tag == GUMBO_TAG_STYLE || tag == GUMBO_TAG_NOSCRIPT) return;
        GumboVector* children = &node->v.element.children;
        for (unsigned int i = 0; i < children->length; ++i) {
            extractText(static_cast<GumboNode*>(children->data[i]), target);
        }
    }

    void findTitle(GumboNode* node) {
        if (!isElement(node)) return;
        if (node->v.element.tag == GUMBO_TAG_TITLE) {
            std::string raw;
            extractText(node, raw);
            title_ = normalizeWhitespace(raw);
            return;
        }
        GumboVector* children = &node->v.element.children;
        for (unsigned int i = 0; i < children->length && title_.empty(); ++i) {
            findTitle(static_cast<GumboNode*>(children->data[i]));
        }
    }

    void findLinks(GumboNode* node) {
        if (!isElement(node)) return;
        if (node->v.element.tag == GUMBO_TAG_A) {
            auto href = attr(node, "href");
            if (!href.empty()) links_.push_back(href);
        }
        GumboVector* children = &node->v.element.children;
        for (unsigned int i = 0; i < children->length; ++i) findLinks(static_cast<GumboNode*>(children->data[i]));
    }

    void findImages(GumboNode* node) {
        if (!isElement(node)) return;
        if (node->v.element.tag == GUMBO_TAG_IMG) {
            auto src = attr(node, "src");
            if (!src.empty()) images_.push_back(src);
        }
        GumboVector* children = &node->v.element.children;
        for (unsigned int i = 0; i < children->length; ++i) findImages(static_cast<GumboNode*>(children->data[i]));
    }

    void collectByPredicate(GumboNode* node, const Predicate& predicate, std::vector<GumboNode*>& out) const {
        if (!node) return;
        if (isElement(node) && predicate(node)) out.push_back(node);
        if (!isElement(node)) return;
        GumboVector* children = &node->v.element.children;
        for (unsigned int i = 0; i < children->length; ++i) {
            collectByPredicate(static_cast<GumboNode*>(children->data[i]), predicate, out);
        }
    }

    void queryCss(const std::string& selector, std::vector<GumboNode*>& out) const {
        const std::string s = trim(selector);
        if (s.empty()) return;

        static const std::regex attrSelector(R"(^([A-Za-z0-9_-]*)?\[([A-Za-z0-9_:-]+)=[\"']?([^\"'\]]+)[\"']?\]$)");
        static const std::regex tagClassSelector(R"(^([A-Za-z0-9_-]+)\.([A-Za-z0-9_-]+)$)");
        static const std::regex tagIdSelector(R"(^([A-Za-z0-9_-]+)#([A-Za-z0-9_-]+)$)");
        std::smatch match;

        if (s.front() == '#') {
            const auto id = s.substr(1);
            collectByPredicate(output_->root, [&](GumboNode* node) { return attr(node, "id") == id; }, out);
        } else if (s.front() == '.') {
            const auto className = s.substr(1);
            collectByPredicate(output_->root, [&](GumboNode* node) { return hasClass(node, className); }, out);
        } else if (std::regex_match(s, match, attrSelector)) {
            const std::string requestedTag = toLower(match[1].str());
            const std::string attrName = match[2].str();
            const std::string attrValue = match[3].str();
            collectByPredicate(output_->root, [&](GumboNode* node) {
                return (requestedTag.empty() || tagName(node) == requestedTag) && attr(node, attrName) == attrValue;
            }, out);
        } else if (std::regex_match(s, match, tagClassSelector)) {
            const std::string requestedTag = toLower(match[1].str());
            const std::string className = match[2].str();
            collectByPredicate(output_->root, [&](GumboNode* node) { return tagName(node) == requestedTag && hasClass(node, className); }, out);
        } else if (std::regex_match(s, match, tagIdSelector)) {
            const std::string requestedTag = toLower(match[1].str());
            const std::string id = match[2].str();
            collectByPredicate(output_->root, [&](GumboNode* node) { return tagName(node) == requestedTag && attr(node, "id") == id; }, out);
        } else {
            const std::string requestedTag = toLower(s);
            collectByPredicate(output_->root, [&](GumboNode* node) { return tagName(node) == requestedTag; }, out);
        }
    }

    void queryBasicXPath(const std::string& selector, std::vector<GumboNode*>& out) const {
        static const std::regex anyTag(R"(^//([A-Za-z0-9_-]+)$)");
        static const std::regex idQuery(R"(^//([A-Za-z0-9_-]+)\[@id=[\"']([^\"']+)[\"']\]$)");
        static const std::regex containsText(R"(^//([A-Za-z0-9_-]+)\[contains\(text\(\),[ ]*[\"']([^\"']+)[\"']\)\]$)");
        std::smatch match;
        if (std::regex_match(selector, match, anyTag)) {
            const std::string requestedTag = toLower(match[1].str());
            collectByPredicate(output_->root, [&](GumboNode* node) { return tagName(node) == requestedTag; }, out);
        } else if (std::regex_match(selector, match, idQuery)) {
            const std::string requestedTag = toLower(match[1].str());
            const std::string id = match[2].str();
            collectByPredicate(output_->root, [&](GumboNode* node) { return tagName(node) == requestedTag && attr(node, "id") == id; }, out);
        } else if (std::regex_match(selector, match, containsText)) {
            const std::string requestedTag = toLower(match[1].str());
            const std::string needle = match[2].str();
            collectByPredicate(output_->root, [&](GumboNode* node) {
                std::string nodeText;
                extractText(node, nodeText);
                return tagName(node) == requestedTag && normalizeWhitespace(nodeText).find(needle) != std::string::npos;
            }, out);
        }
    }

    WebElement toElement(GumboNode* node) const {
        WebElement element;
        if (!isElement(node)) return element;
        element.tag = tagName(node);
        GumboVector* attrs = &node->v.element.attributes;
        for (unsigned int i = 0; i < attrs->length; ++i) {
            auto* gumboAttr = static_cast<GumboAttribute*>(attrs->data[i]);
            if (!gumboAttr || !gumboAttr->name) continue;
            const std::string name = gumboAttr->name;
            const std::string value = gumboAttr->value ? gumboAttr->value : "";
            element.attributes[name] = value;
            if (name == "id") element.id = value;
        }
        if (tagName(node) == "select" && element.attributes.find("value") == element.attributes.end()) {
            element.attributes["value"] = selectedOptionValue(node);
        }
        extractText(node, element.text);
        element.text = normalizeWhitespace(element.text);
        element.innerHTML = element.text;
        element.isEnabled = !hasAttr(node, "disabled") && toLower(attr(node, "aria-disabled")) != "true";
        const std::string style = toLower(attr(node, "style"));
        element.isVisible = !hasAttr(node, "hidden") &&
                            style.find("display:none") == std::string::npos &&
                            style.find("display: none") == std::string::npos &&
                            style.find("visibility:hidden") == std::string::npos &&
                            style.find("visibility: hidden") == std::string::npos;
        element.width = element.isVisible ? 1 : 0;
        element.height = element.isVisible ? 1 : 0;
        return element;
    }

    GumboOutput* output_ = nullptr;
    std::string html_;
    std::string title_;
    std::string text_;
    std::vector<std::string> links_;
    std::vector<std::string> images_;
};

struct BrowserSession {
    HttpClient http;
    HtmlParser parser;
    std::vector<std::string> history;
    std::size_t historyIndex = 0;
    std::unordered_map<std::string, std::string> formValues;
    std::unordered_set<std::string> checkedSelectors;
    std::string lastClickedSelector;
    int scrollX = 0;
    int scrollY = 0;
};

static BrowserSession* asSession(void* ptr) {
    return static_cast<BrowserSession*>(ptr);
}

static const std::vector<uint8_t>& diagnosticPngBytes() {
    static const std::vector<uint8_t> png = {
        0x89, 0x50, 0x4E, 0x47, 0x0D, 0x0A, 0x1A, 0x0A,
        0x00, 0x00, 0x00, 0x0D, 0x49, 0x48, 0x44, 0x52,
        0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x01,
        0x08, 0x06, 0x00, 0x00, 0x00, 0x1F, 0x15, 0xC4,
        0x89, 0x00, 0x00, 0x00, 0x0D, 0x49, 0x44, 0x41,
        0x54, 0x78, 0x9C, 0x63, 0x60, 0x00, 0x02, 0x00,
        0x00, 0x05, 0x00, 0x01, 0xE2, 0x26, 0x05, 0x9B,
        0x00, 0x00, 0x00, 0x00, 0x49, 0x45, 0x4E, 0x44,
        0xAE, 0x42, 0x60, 0x82
    };
    return png;
}

} // namespace browser_impl

AgentBrowser::AgentBrowser(const BrowserConfig& config)
    : config_(config), browserDriver_(nullptr), htmlParser_(nullptr) {
    stats_.sessionStart = std::chrono::system_clock::now();
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(100000, 999999);
    sessionId_ = "browser_session_" + std::to_string(dis(gen));
}

AgentBrowser::~AgentBrowser() {
    if (initialized_.load()) shutdown();
}

BrowserResult AgentBrowser::initialize() {
    std::lock_guard<std::mutex> lock(sessionMutex_);
    if (initialized_.load()) {
        return {BrowserActionResult::SUCCESS, "Browser already initialized", std::nullopt, std::chrono::milliseconds(0)};
    }
    auto start = std::chrono::steady_clock::now();
    auto result = initializeBrowserDriver();
    if (!result) return result;
    initialized_.store(true);
    shouldStop_.store(false);
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - start);
    logAction("initialize", {BrowserActionResult::SUCCESS, "Browser initialized", std::nullopt, duration});
    return {BrowserActionResult::SUCCESS, "Browser initialized", std::nullopt, duration};
}

BrowserResult AgentBrowser::shutdown() {
    std::lock_guard<std::mutex> lock(sessionMutex_);
    if (!initialized_.load()) {
        return {BrowserActionResult::SUCCESS, "Browser not initialized", std::nullopt, std::chrono::milliseconds(0)};
    }
    auto start = std::chrono::steady_clock::now();
    shouldStop_.store(true);
    shutdownBrowserDriver();
    initialized_.store(false);
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - start);
    return {BrowserActionResult::SUCCESS, "Browser shutdown", std::nullopt, duration};
}

BrowserResult AgentBrowser::navigateTo(const std::string& url) {
    if (!initialized_.load()) return {BrowserActionResult::FAILED, "Browser not initialized", std::nullopt, std::chrono::milliseconds(0)};
    if (!browser_utils::isValidUrl(url)) return {BrowserActionResult::FAILED, "Invalid URL: " + url, std::nullopt, std::chrono::milliseconds(0)};

    auto start = std::chrono::steady_clock::now();
    std::string loadedUrl;
    long httpCode = 0;
    {
        std::lock_guard<std::mutex> lock(sessionMutex_);
        auto* session = browser_impl::asSession(browserDriver_);
        if (!session) return {BrowserActionResult::FAILED, "Browser driver not initialized", std::nullopt, std::chrono::milliseconds(0)};
        if (!session->http.fetch(url, config_)) {
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - start);
            return {BrowserActionResult::NAVIGATION_ERROR, "Failed to fetch URL: " + session->http.error(), url, duration};
        }
        if (!session->parser.parse(session->http.html())) {
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - start);
            return {BrowserActionResult::FAILED, "Fetched URL but failed to parse HTML", url, duration};
        }
        loadedUrl = session->http.effectiveUrl();
        httpCode = session->http.httpCode();
        currentUrl_ = loadedUrl;
        if (session->history.empty() || session->history[session->historyIndex] != loadedUrl) {
            if (!session->history.empty() && session->historyIndex + 1 < session->history.size()) {
                session->history.erase(session->history.begin() + static_cast<std::ptrdiff_t>(session->historyIndex + 1), session->history.end());
            }
            session->history.push_back(loadedUrl);
            session->historyIndex = session->history.size() - 1;
        }
        stats_.pagesVisited++;
    }

    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - start);
    updateStatistics("navigation", duration);
    if (memory_) rememberPage(loadedUrl, "navigation");
    BrowserResult result{BrowserActionResult::SUCCESS, "Navigated to " + loadedUrl + " (HTTP " + std::to_string(httpCode) + ")", loadedUrl, duration};
    logAction("navigate_to", result);
    return result;
}

BrowserResult AgentBrowser::goBack() {
    if (!initialized_.load()) return {BrowserActionResult::FAILED, "Browser not initialized", std::nullopt, std::chrono::milliseconds(0)};
    std::string target;
    {
        std::lock_guard<std::mutex> lock(sessionMutex_);
        auto* session = browser_impl::asSession(browserDriver_);
        if (!session || session->history.empty() || session->historyIndex == 0) {
            return {BrowserActionResult::FAILED, "No previous page in browser history", std::nullopt, std::chrono::milliseconds(0)};
        }
        target = session->history[--session->historyIndex];
    }
    return navigateTo(target);
}

BrowserResult AgentBrowser::goForward() {
    if (!initialized_.load()) return {BrowserActionResult::FAILED, "Browser not initialized", std::nullopt, std::chrono::milliseconds(0)};
    std::string target;
    {
        std::lock_guard<std::mutex> lock(sessionMutex_);
        auto* session = browser_impl::asSession(browserDriver_);
        if (!session || session->history.empty() || session->historyIndex + 1 >= session->history.size()) {
            return {BrowserActionResult::FAILED, "No next page in browser history", std::nullopt, std::chrono::milliseconds(0)};
        }
        target = session->history[++session->historyIndex];
    }
    return navigateTo(target);
}

BrowserResult AgentBrowser::refresh() {
    if (!initialized_.load()) return {BrowserActionResult::FAILED, "Browser not initialized", std::nullopt, std::chrono::milliseconds(0)};
    std::string url;
    {
        std::lock_guard<std::mutex> lock(sessionMutex_);
        url = currentUrl_;
    }
    if (url.empty()) return {BrowserActionResult::FAILED, "No page loaded", std::nullopt, std::chrono::milliseconds(0)};
    return navigateTo(url);
}

BrowserResult AgentBrowser::waitForPageLoad(int timeoutSec) {
    if (!initialized_.load()) return {BrowserActionResult::FAILED, "Browser not initialized", std::nullopt, std::chrono::milliseconds(0)};
    auto start = std::chrono::steady_clock::now();
    while (std::chrono::duration_cast<std::chrono::seconds>(std::chrono::steady_clock::now() - start).count() <= timeoutSec) {
        std::lock_guard<std::mutex> lock(sessionMutex_);
        auto* session = browser_impl::asSession(browserDriver_);
        if (session && session->parser.isParsed()) {
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - start);
            return {BrowserActionResult::SUCCESS, "Page loaded", currentUrl_, duration};
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - start);
    return {BrowserActionResult::TIMEOUT, "Timed out waiting for page load", std::nullopt, duration};
}

std::optional<PageInfo> AgentBrowser::getCurrentPageInfo() {
    if (!initialized_.load()) return std::nullopt;
    std::lock_guard<std::mutex> lock(sessionMutex_);
    auto* session = browser_impl::asSession(browserDriver_);
    if (!session || !session->parser.isParsed()) return std::nullopt;
    PageInfo info;
    info.url = currentUrl_;
    info.title = session->parser.title();
    info.html = session->parser.html();
    info.links = session->parser.links();
    info.images = session->parser.images();
    info.loadTime = std::chrono::system_clock::now();
    info.isLoaded = true;
    return info;
}

std::optional<std::string> AgentBrowser::getPageTitle() {
    auto info = getCurrentPageInfo();
    return info ? std::optional<std::string>(info->title) : std::nullopt;
}

std::optional<std::string> AgentBrowser::getPageText() {
    if (!initialized_.load()) return std::nullopt;
    std::lock_guard<std::mutex> lock(sessionMutex_);
    auto* session = browser_impl::asSession(browserDriver_);
    if (!session || !session->parser.isParsed()) return std::nullopt;
    return session->parser.text();
}

std::optional<std::string> AgentBrowser::getPageHTML() {
    auto info = getCurrentPageInfo();
    return info ? std::optional<std::string>(info->html) : std::nullopt;
}

std::vector<std::string> AgentBrowser::getLinks() {
    auto info = getCurrentPageInfo();
    return info ? info->links : std::vector<std::string>{};
}

std::vector<std::string> AgentBrowser::getImages() {
    auto info = getCurrentPageInfo();
    return info ? info->images : std::vector<std::string>{};
}

std::optional<WebElement> AgentBrowser::findElement(const std::string& selector, SelectorType type) {
    auto elements = findElements(selector, type);
    if (elements.empty()) return std::nullopt;
    return elements.front();
}

std::vector<WebElement> AgentBrowser::findElements(const std::string& selector, SelectorType type) {
    if (!initialized_.load()) return {};
    auto validation = validateSelector(selector, type);
    if (!validation) return {};
    std::lock_guard<std::mutex> lock(sessionMutex_);
    auto* session = browser_impl::asSession(browserDriver_);
    if (!session || !session->parser.isParsed()) return {};
    return session->parser.queryAll(selector, type);
}

BrowserResult AgentBrowser::clickElement(const std::string& selector, SelectorType type) {
    auto start = std::chrono::steady_clock::now();
    auto element = findElement(selector, type);
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - start);
    if (!element) return {BrowserActionResult::ELEMENT_NOT_FOUND, "Element not found: " + selector, selector, duration};
    if (!element->isVisible || !element->isEnabled) return {BrowserActionResult::FAILED, "Element is not interactable: " + selector, selector, duration};

    std::optional<std::string> href;
    if (browser_impl::toLower(element->tag) == "a") {
        auto it = element->attributes.find("href");
        if (it != element->attributes.end() && !browser_impl::trim(it->second).empty()) href = it->second;
    }

    std::string baseUrl;
    {
        std::lock_guard<std::mutex> lock(sessionMutex_);
        auto* session = browser_impl::asSession(browserDriver_);
        if (session) session->lastClickedSelector = selector;
        baseUrl = currentUrl_;
        stats_.elementsClicked++;
    }

    if (href && !baseUrl.empty()) {
        const std::string targetUrl = browser_impl::resolveUrl(baseUrl, *href);
        auto nav = navigateTo(targetUrl);
        if (nav) {
            nav.message = "Clicked link " + selector + " and navigated to " + targetUrl;
            logAction("click_element", nav);
        }
        return nav;
    }

    BrowserResult result{BrowserActionResult::SUCCESS, "Clicked element: " + selector, selector, duration};
    logAction("click_element", result);
    return result;
}

BrowserResult AgentBrowser::typeText(const std::string& selector, const std::string& text, SelectorType type) {
    auto start = std::chrono::steady_clock::now();
    auto element = findElement(selector, type);
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - start);
    if (!element) return {BrowserActionResult::ELEMENT_NOT_FOUND, "Element not found: " + selector, selector, duration};
    if (!element->isEnabled) return {BrowserActionResult::FAILED, "Element is disabled: " + selector, selector, duration};
    std::lock_guard<std::mutex> lock(sessionMutex_);
    auto* session = browser_impl::asSession(browserDriver_);
    if (session) session->formValues[selector] += text;
    return {BrowserActionResult::SUCCESS, "Typed text into " + selector, text, duration};
}

BrowserResult AgentBrowser::clearText(const std::string& selector, SelectorType type) {
    auto start = std::chrono::steady_clock::now();
    auto element = findElement(selector, type);
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - start);
    if (!element) return {BrowserActionResult::ELEMENT_NOT_FOUND, "Element not found: " + selector, selector, duration};
    std::lock_guard<std::mutex> lock(sessionMutex_);
    auto* session = browser_impl::asSession(browserDriver_);
    if (session) session->formValues[selector].clear();
    return {BrowserActionResult::SUCCESS, "Cleared text from " + selector, selector, duration};
}

BrowserResult AgentBrowser::fillForm(const std::unordered_map<std::string, std::string>& formData) {
    auto start = std::chrono::steady_clock::now();
    for (const auto& [selector, value] : formData) {
        auto result = clearText(selector);
        if (!result) return result;
        result = typeText(selector, value);
        if (!result) return result;
    }
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - start);
    return {BrowserActionResult::SUCCESS, "Form filled with " + std::to_string(formData.size()) + " fields", std::nullopt, duration};
}

BrowserResult AgentBrowser::submitForm(const std::string& formSelector) {
    auto start = std::chrono::steady_clock::now();
    auto element = findElement(formSelector);
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - start);
    if (!element) return {BrowserActionResult::ELEMENT_NOT_FOUND, "Form not found: " + formSelector, formSelector, duration};
    if (browser_impl::toLower(element->tag) != "form") return {BrowserActionResult::FAILED, "Selector is not a form: " + formSelector, formSelector, duration};

    std::unordered_map<std::string, std::string> formValues;
    std::unordered_set<std::string> checkedSelectors;
    std::string baseUrl;
    {
        std::lock_guard<std::mutex> lock(sessionMutex_);
        auto* session = browser_impl::asSession(browserDriver_);
        if (session) {
            formValues = session->formValues;
            checkedSelectors = session->checkedSelectors;
        }
        baseUrl = currentUrl_;
        stats_.formsSubmitted++;
    }

    const auto methodIt = element->attributes.find("method");
    const std::string method = methodIt == element->attributes.end() ? "get" : browser_impl::toLower(methodIt->second);
    const auto actionIt = element->attributes.find("action");
    const std::string action = actionIt == element->attributes.end() || browser_impl::trim(actionIt->second).empty() ? baseUrl : actionIt->second;

    if (method == "get") {
        std::map<std::string, std::string> queryParams;
        std::vector<WebElement> scopedControls;
        {
            std::lock_guard<std::mutex> lock(sessionMutex_);
            auto* session = browser_impl::asSession(browserDriver_);
            if (session) scopedControls = session->parser.formControls(formSelector, SelectorType::CSS);
        }

        auto sameControl = [](const WebElement& lhs, const WebElement& rhs) {
            if (!lhs.id.empty() && lhs.id == rhs.id) return true;
            auto lhsName = lhs.attributes.find("name");
            auto rhsName = rhs.attributes.find("name");
            if (lhsName != lhs.attributes.end() && rhsName != rhs.attributes.end() &&
                !lhsName->second.empty() && lhsName->second == rhsName->second && lhs.tag == rhs.tag) return true;
            return false;
        };
        auto isScopedControl = [&](const WebElement& field) {
            return std::any_of(scopedControls.begin(), scopedControls.end(), [&](const WebElement& control) {
                return sameControl(control, field);
            });
        };

        for (const auto& control : scopedControls) {
            if (!control.isEnabled) continue;
            const std::string name = browser_impl::formFieldName("", control);
            if (name.empty()) continue;
            if (browser_impl::isSuccessfulTextControl(control)) {
                queryParams.emplace(name, browser_impl::formControlValue(control));
            } else if (browser_impl::isCheckableControl(control) && browser_impl::hasHtmlAttribute(control, "checked")) {
                const std::string value = browser_impl::formControlValue(control);
                queryParams.emplace(name, value.empty() ? "on" : value);
            }
        }
        for (const auto& [selector, value] : formValues) {
            auto field = findElement(selector);
            if (!field || !field->isEnabled || !isScopedControl(*field)) continue;
            queryParams[browser_impl::formFieldName(selector, *field)] = value;
        }
        for (const auto& selector : checkedSelectors) {
            auto field = findElement(selector);
            if (!field || !field->isEnabled || !isScopedControl(*field)) continue;
            auto valueIt = field->attributes.find("value");
            queryParams[browser_impl::formFieldName(selector, *field)] = valueIt == field->attributes.end() || valueIt->second.empty() ? "on" : valueIt->second;
        }
        const std::string target = browser_impl::appendQuery(browser_impl::resolveUrl(baseUrl, action), queryParams);
        auto nav = navigateTo(target);
        if (nav) nav.message = "Submitted GET form " + formSelector + " to " + target;
        return nav;
    }

    return {BrowserActionResult::SUCCESS, "Form submission prepared for unsupported HTTP method: " + method, formSelector, duration};
}

BrowserResult AgentBrowser::selectOption(const std::string& selector, const std::string& value) {
    auto start = std::chrono::steady_clock::now();
    auto element = findElement(selector);
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - start);
    if (!element) return {BrowserActionResult::ELEMENT_NOT_FOUND, "Select element not found: " + selector, selector, duration};
    std::lock_guard<std::mutex> lock(sessionMutex_);
    auto* session = browser_impl::asSession(browserDriver_);
    if (session) session->formValues[selector] = value;
    return {BrowserActionResult::SUCCESS, "Selected option '" + value + "' in " + selector, value, duration};
}

BrowserResult AgentBrowser::checkCheckbox(const std::string& selector, bool checked) {
    auto start = std::chrono::steady_clock::now();
    auto element = findElement(selector);
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - start);
    if (!element) return {BrowserActionResult::ELEMENT_NOT_FOUND, "Checkbox not found: " + selector, selector, duration};
    std::lock_guard<std::mutex> lock(sessionMutex_);
    auto* session = browser_impl::asSession(browserDriver_);
    if (session) {
        if (checked) session->checkedSelectors.insert(selector);
        else session->checkedSelectors.erase(selector);
    }
    return {BrowserActionResult::SUCCESS, std::string(checked ? "Checked" : "Unchecked") + " checkbox: " + selector, selector, duration};
}

BrowserResult AgentBrowser::executeJavaScript(const std::string& script) {
    if (!initialized_.load()) return {BrowserActionResult::FAILED, "Browser not initialized", std::nullopt, std::chrono::milliseconds(0)};
    auto start = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - start);
    return {BrowserActionResult::FAILED, "JavaScript execution requires a real browser engine; HTTP parser mode cannot execute scripts", script, duration};
}

std::optional<std::string> AgentBrowser::evaluateJavaScript(const std::string& expression) {
    if (!initialized_.load()) return std::nullopt;
    const std::string expr = browser_impl::trim(expression);
    if (expr == "document.title") return getPageTitle();
    if (expr == "document.body.innerText" || expr == "document.body.textContent") return getPageText();
    if (expr == "document.documentElement.outerHTML") return getPageHTML();
    if (expr == "window.location.href" || expr == "location.href") {
        std::lock_guard<std::mutex> lock(sessionMutex_);
        return currentUrl_;
    }
    return std::nullopt;
}

BrowserResult AgentBrowser::scrollToElement(const std::string& selector) {
    auto start = std::chrono::steady_clock::now();
    auto element = findElement(selector);
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - start);
    if (!element) return {BrowserActionResult::ELEMENT_NOT_FOUND, "Element not found: " + selector, selector, duration};
    std::lock_guard<std::mutex> lock(sessionMutex_);
    auto* session = browser_impl::asSession(browserDriver_);
    if (session) { session->scrollX = element->x; session->scrollY = element->y; }
    return {BrowserActionResult::SUCCESS, "Scrolled to element: " + selector, selector, duration};
}

BrowserResult AgentBrowser::scrollBy(int x, int y) {
    if (!initialized_.load()) return {BrowserActionResult::FAILED, "Browser not initialized", std::nullopt, std::chrono::milliseconds(0)};
    auto start = std::chrono::steady_clock::now();
    std::lock_guard<std::mutex> lock(sessionMutex_);
    auto* session = browser_impl::asSession(browserDriver_);
    if (session) { session->scrollX += x; session->scrollY += y; }
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - start);
    std::string data = "(" + std::to_string(x) + ", " + std::to_string(y) + ")";
    return {BrowserActionResult::SUCCESS, "Scrolled by " + data, data, duration};
}

BrowserResult AgentBrowser::captureScreenshot(const std::string& filename) {
    if (!initialized_.load()) return {BrowserActionResult::FAILED, "Browser not initialized", std::nullopt, std::chrono::milliseconds(0)};
    auto start = std::chrono::steady_clock::now();
    std::string actualFilename = filename.empty() ? generateScreenshotFilename() : filename;
    std::ofstream file(actualFilename, std::ios::binary);
    if (!file) return {BrowserActionResult::FAILED, "Could not save diagnostic screenshot to " + actualFilename, std::nullopt, std::chrono::milliseconds(0)};
    const auto& png = browser_impl::diagnosticPngBytes();
    file.write(reinterpret_cast<const char*>(png.data()), static_cast<std::streamsize>(png.size()));
    file.close();
    {
        std::lock_guard<std::mutex> lock(sessionMutex_);
        stats_.screenshotsTaken++;
    }
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - start);
    return {BrowserActionResult::SUCCESS, "Saved diagnostic PNG artifact: " + actualFilename, actualFilename, duration};
}

std::vector<uint8_t> AgentBrowser::getScreenshotData() {
    if (!initialized_.load()) return {};
    return browser_impl::diagnosticPngBytes();
}

BrowserResult AgentBrowser::savePageHTML(const std::string& filename) {
    auto html = getPageHTML();
    if (!html) return {BrowserActionResult::FAILED, "No page HTML available", std::nullopt, std::chrono::milliseconds(0)};
    auto start = std::chrono::steady_clock::now();
    std::ofstream file(filename);
    if (!file) return {BrowserActionResult::FAILED, "Could not save HTML to " + filename, std::nullopt, std::chrono::milliseconds(0)};
    file << *html;
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - start);
    return {BrowserActionResult::SUCCESS, "HTML saved: " + filename, filename, duration};
}

BrowserResult AgentBrowser::waitForElement(const std::string& selector, int timeoutSec) {
    auto start = std::chrono::steady_clock::now();
    while (std::chrono::duration_cast<std::chrono::seconds>(std::chrono::steady_clock::now() - start).count() <= timeoutSec) {
        if (findElement(selector)) {
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - start);
            return {BrowserActionResult::SUCCESS, "Element found: " + selector, selector, duration};
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - start);
    return {BrowserActionResult::TIMEOUT, "Element wait timeout: " + selector, selector, duration};
}

BrowserResult AgentBrowser::waitForElementVisible(const std::string& selector, int timeoutSec) {
    auto start = std::chrono::steady_clock::now();
    auto result = waitForElement(selector, timeoutSec);
    if (!result) return result;
    auto element = findElement(selector);
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - start);
    return (element && element->isVisible)
        ? BrowserResult{BrowserActionResult::SUCCESS, "Element visible: " + selector, selector, duration}
        : BrowserResult{BrowserActionResult::TIMEOUT, "Element visibility wait timeout: " + selector, selector, duration};
}

BrowserResult AgentBrowser::waitForElementClickable(const std::string& selector, int timeoutSec) {
    auto start = std::chrono::steady_clock::now();
    auto result = waitForElement(selector, timeoutSec);
    if (!result) return result;
    auto element = findElement(selector);
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - start);
    return (element && element->isVisible && element->isEnabled)
        ? BrowserResult{BrowserActionResult::SUCCESS, "Element clickable: " + selector, selector, duration}
        : BrowserResult{BrowserActionResult::TIMEOUT, "Element clickable wait timeout: " + selector, selector, duration};
}

BrowserResult AgentBrowser::waitForText(const std::string& text, int timeoutSec) {
    auto start = std::chrono::steady_clock::now();
    while (std::chrono::duration_cast<std::chrono::seconds>(std::chrono::steady_clock::now() - start).count() <= timeoutSec) {
        auto pageText = getPageText();
        if (pageText && pageText->find(text) != std::string::npos) {
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - start);
            return {BrowserActionResult::SUCCESS, "Text found: " + text, text, duration};
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - start);
    return {BrowserActionResult::TIMEOUT, "Text wait timeout: " + text, text, duration};
}

void AgentBrowser::setMemory(std::shared_ptr<AgentMemoryManager> memory) { memory_ = std::move(memory); }

void AgentBrowser::setLogger(std::shared_ptr<AgentLogger> logger) { logger_ = std::move(logger); }

void AgentBrowser::rememberPage(const std::string& url, const std::string& purpose) {
    if (!memory_) return;
    CustomMetadata metadata;
    metadata.scope = MemoryScope::PRIVATE;
    metadata.source = "agentbrowser";
    metadata.timestamp = std::chrono::system_clock::now();
    metadata.tags = {"browser", "page", purpose};
    metadata.customData["url"] = url;
    metadata.customData["purpose"] = purpose;

    const UUID id = "browser_" + std::to_string(std::hash<std::string>{}(url + "|" + purpose));
    const UUID entityId = "browser_entity";
    const UUID agentId = "browser_agent";
    std::string title;
    if (auto pageTitle = getPageTitle()) title = *pageTitle;
    const std::string content = "Visited URL: " + url + "\nPurpose: " + purpose + (title.empty() ? "" : "\nTitle: " + title);
    auto memory = std::make_shared<Memory>(id, content, entityId, agentId, metadata);
    memory->setRoomId("browser_room");
    memory->setUnique(true);
    memory_->createMemory(memory, "browser_pages", true);
}

std::vector<std::string> AgentBrowser::getSimilarPages(const std::string& purpose) {
    if (!memory_) return {};
    MemorySearchParams params;
    params.tableName = "browser_pages";
    params.count = 50;
    auto memories = memory_->getMemories(params);
    std::vector<std::string> urls;
    std::regex urlRegex(R"(Visited URL: ([^\n]+))");
    for (const auto& memory : memories) {
        if (!memory) continue;
        const auto& content = memory->getContent();
        if (!purpose.empty() && content.find(purpose) == std::string::npos) continue;
        std::smatch match;
        if (std::regex_search(content, match, urlRegex) && match.size() > 1) urls.push_back(match[1].str());
    }
    return urls;
}

void AgentBrowser::setConfig(const BrowserConfig& config) { config_ = config; }

void AgentBrowser::resetStatistics() {
    std::lock_guard<std::mutex> lock(statsMutex_);
    stats_ = Statistics{};
    stats_.sessionStart = std::chrono::system_clock::now();
}

BrowserResult AgentBrowser::validateSelector(const std::string& selector, SelectorType type) {
    if (browser_impl::trim(selector).empty()) return {BrowserActionResult::FAILED, "Empty selector", std::nullopt, std::chrono::milliseconds(0)};
    if (type == SelectorType::CSS && selector.find_first_of("{}") != std::string::npos) {
        return {BrowserActionResult::FAILED, "Invalid CSS selector", std::nullopt, std::chrono::milliseconds(0)};
    }
    if (type == SelectorType::XPATH && selector.rfind("/", 0) != 0) {
        return {BrowserActionResult::FAILED, "Invalid XPath selector", std::nullopt, std::chrono::milliseconds(0)};
    }
    return {BrowserActionResult::SUCCESS, "Selector valid", std::nullopt, std::chrono::milliseconds(0)};
}

std::string AgentBrowser::generateScreenshotFilename() {
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    auto tm = *std::localtime(&time);
    std::ostringstream oss;
    oss << "screenshot_" << std::put_time(&tm, "%Y%m%d_%H%M%S") << ".png";
    return oss.str();
}

void AgentBrowser::logAction(const std::string& action, const BrowserResult& result) {
    if (!logger_) return;
    logger_->log(result.message + " (" + action + ")", "agentbrowser", "Browser Action", result ? LogLevel::SUCCESS : LogLevel::ERROR);
}

void AgentBrowser::updateStatistics(const std::string& action, std::chrono::milliseconds duration) {
    std::lock_guard<std::mutex> lock(statsMutex_);
    if (action == "navigation") stats_.totalNavigationTime += duration;
}

BrowserResult AgentBrowser::initializeBrowserDriver() {
    try {
        auto* session = new browser_impl::BrowserSession();
        browserDriver_ = session;
        htmlParser_ = &session->parser;
        return {BrowserActionResult::SUCCESS, "Browser driver initialized with libcurl and gumbo", std::nullopt, std::chrono::milliseconds(0)};
    } catch (const std::exception& e) {
        browserDriver_ = nullptr;
        htmlParser_ = nullptr;
        return {BrowserActionResult::FAILED, std::string("Failed to initialize browser driver: ") + e.what(), std::nullopt, std::chrono::milliseconds(0)};
    }
}

void AgentBrowser::shutdownBrowserDriver() {
    auto* session = browser_impl::asSession(browserDriver_);
    delete session;
    browserDriver_ = nullptr;
    htmlParser_ = nullptr;
}

BrowserResult AgentBrowser::sendBrowserCommand(const std::string& command, const std::unordered_map<std::string, std::string>& params) {
    if (!initialized_.load()) return {BrowserActionResult::FAILED, "Browser not initialized", std::nullopt, std::chrono::milliseconds(0)};
    if (command == "getTitle") {
        auto title = getPageTitle();
        return title ? BrowserResult{BrowserActionResult::SUCCESS, "Command executed: getTitle", *title, std::chrono::milliseconds(0)}
                     : BrowserResult{BrowserActionResult::FAILED, "No page title available", std::nullopt, std::chrono::milliseconds(0)};
    }
    if (command == "getText") {
        auto text = getPageText();
        return text ? BrowserResult{BrowserActionResult::SUCCESS, "Command executed: getText", *text, std::chrono::milliseconds(0)}
                    : BrowserResult{BrowserActionResult::FAILED, "No page text available", std::nullopt, std::chrono::milliseconds(0)};
    }
    if (command == "navigate") {
        auto it = params.find("url");
        if (it == params.end()) return {BrowserActionResult::FAILED, "navigate command requires url", std::nullopt, std::chrono::milliseconds(0)};
        return navigateTo(it->second);
    }
    return {BrowserActionResult::FAILED, "Unsupported browser command: " + command, std::nullopt, std::chrono::milliseconds(0)};
}

namespace browser_utils {

std::string cssSelector(const std::string& element, const std::string& attribute, const std::string& value) {
    std::string selector = element;
    if (!attribute.empty() && !value.empty()) selector += "[" + attribute + "='" + value + "']";
    return selector;
}

std::string xpathSelector(const std::string& element, const std::string& text) {
    return text.empty() ? "//" + element : "//" + element + "[contains(text(), '" + text + "')]";
}

bool isValidUrl(const std::string& url) {
    static const std::regex urlRegex(R"(^https?://[^\s/$.?#][^\s]*$)", std::regex_constants::icase);
    return std::regex_match(url, urlRegex);
}

std::string extractDomain(const std::string& url) {
    static const std::regex domainRegex(R"(^https?://([^/:?#]+))", std::regex_constants::icase);
    std::smatch match;
    return (std::regex_search(url, match, domainRegex) && match.size() > 1) ? match[1].str() : "";
}

std::vector<std::string> extractEmails(const std::string& text) {
    std::vector<std::string> emails;
    static const std::regex emailRegex(R"([A-Za-z0-9._%+-]+@[A-Za-z0-9.-]+\.[A-Za-z]{2,})");
    for (auto it = std::sregex_iterator(text.begin(), text.end(), emailRegex); it != std::sregex_iterator(); ++it) emails.push_back(it->str());
    return emails;
}

std::vector<std::string> extractPhoneNumbers(const std::string& text) {
    std::vector<std::string> phones;
    static const std::regex phoneRegex(R"((?:\+?[0-9]|\([0-9]{2,4}\))[0-9\s().-]{7,}[0-9])");
    for (auto it = std::sregex_iterator(text.begin(), text.end(), phoneRegex); it != std::sregex_iterator(); ++it) phones.push_back(browser_impl::trim(it->str()));
    return phones;
}

} // namespace browser_utils

} // namespace elizaos
