// HTML Parser implementation using gumbo-parser for AgentBrowser
// Provides real DOM parsing and element selection

#include "elizaos/agentbrowser.hpp"
#include <gumbo.h>
#include <vector>
#include <string>
#include <algorithm>
#include <cctype>

namespace elizaos {
namespace browser_impl {

class HtmlParser {
private:
    GumboOutput* output_;
    std::string html_;
    std::string title_;
    std::string text_;
    std::vector<std::string> links_;
    std::vector<std::string> images_;
    
    // Helper to extract text from node
    void extractText(GumboNode* node, std::string& text) {
        if (node->type == GUMBO_NODE_TEXT) {
            text += node->v.text.text;
        } else if (node->type == GUMBO_NODE_ELEMENT) {
            GumboVector* children = &node->v.element.children;
            for (unsigned int i = 0; i < children->length; ++i) {
                extractText(static_cast<GumboNode*>(children->data[i]), text);
            }
        }
    }
    
    // Helper to find title tag
    void findTitle(GumboNode* node) {
        if (node->type != GUMBO_NODE_ELEMENT) {
            return;
        }
        
        if (node->v.element.tag == GUMBO_TAG_TITLE) {
            extractText(node, title_);
            return;
        }
        
        GumboVector* children = &node->v.element.children;
        for (unsigned int i = 0; i < children->length; ++i) {
            findTitle(static_cast<GumboNode*>(children->data[i]));
        }
    }
    
    // Helper to find all links
    void findLinks(GumboNode* node) {
        if (node->type != GUMBO_NODE_ELEMENT) {
            return;
        }
        
        if (node->v.element.tag == GUMBO_TAG_A) {
            GumboAttribute* href = gumbo_get_attribute(&node->v.element.attributes, "href");
            if (href && href->value) {
                links_.push_back(href->value);
            }
        }
        
        GumboVector* children = &node->v.element.children;
        for (unsigned int i = 0; i < children->length; ++i) {
            findLinks(static_cast<GumboNode*>(children->data[i]));
        }
    }
    
    // Helper to find all images
    void findImages(GumboNode* node) {
        if (node->type != GUMBO_NODE_ELEMENT) {
            return;
        }
        
        if (node->v.element.tag == GUMBO_TAG_IMG) {
            GumboAttribute* src = gumbo_get_attribute(&node->v.element.attributes, "src");
            if (src && src->value) {
                images_.push_back(src->value);
            }
        }
        
        GumboVector* children = &node->v.element.children;
        for (unsigned int i = 0; i < children->length; ++i) {
            findImages(static_cast<GumboNode*>(children->data[i]));
        }
    }
    
    // Helper to find element by ID
    GumboNode* findById(GumboNode* node, const std::string& id) {
        if (node->type != GUMBO_NODE_ELEMENT) {
            return nullptr;
        }
        
        GumboAttribute* idAttr = gumbo_get_attribute(&node->v.element.attributes, "id");
        if (idAttr && idAttr->value && id == idAttr->value) {
            return node;
        }
        
        GumboVector* children = &node->v.element.children;
        for (unsigned int i = 0; i < children->length; ++i) {
            GumboNode* result = findById(static_cast<GumboNode*>(children->data[i]), id);
            if (result) return result;
        }
        
        return nullptr;
    }
    
    // Helper to find elements by class
    void findByClass(GumboNode* node, const std::string& className, std::vector<GumboNode*>& results) {
        if (node->type != GUMBO_NODE_ELEMENT) {
            return;
        }
        
        GumboAttribute* classAttr = gumbo_get_attribute(&node->v.element.attributes, "class");
        if (classAttr && classAttr->value) {
            std::string classes = classAttr->value;
            if (classes.find(className) != std::string::npos) {
                results.push_back(node);
            }
        }
        
        GumboVector* children = &node->v.element.children;
        for (unsigned int i = 0; i < children->length; ++i) {
            findByClass(static_cast<GumboNode*>(children->data[i]), className, results);
        }
    }
    
    // Helper to find elements by tag name
    void findByTag(GumboNode* node, GumboTag tag, std::vector<GumboNode*>& results) {
        if (node->type != GUMBO_NODE_ELEMENT) {
            return;
        }
        
        if (node->v.element.tag == tag) {
            results.push_back(node);
        }
        
        GumboVector* children = &node->v.element.children;
        for (unsigned int i = 0; i < children->length; ++i) {
            findByTag(static_cast<GumboNode*>(children->data[i]), tag, results);
        }
    }
    
    // Convert GumboNode to WebElement
    WebElement nodeToElement(GumboNode* node) {
        WebElement element;
        
        if (node->type == GUMBO_NODE_ELEMENT) {
            element.tag = gumbo_normalized_tagname(node->v.element.tag);
            
            // Get ID
            GumboAttribute* idAttr = gumbo_get_attribute(&node->v.element.attributes, "id");
            if (idAttr && idAttr->value) {
                element.id = idAttr->value;
            }
            
            // Get all attributes
            GumboVector* attrs = &node->v.element.attributes;
            for (unsigned int i = 0; i < attrs->length; ++i) {
                GumboAttribute* attr = static_cast<GumboAttribute*>(attrs->data[i]);
                element.attributes[attr->name] = attr->value;
            }
            
            // Extract text content
            extractText(node, element.text);
            
            // Get innerHTML (simplified)
            element.innerHTML = "<" + element.tag + ">" + element.text + "</" + element.tag + ">";
            
            // Default visibility and enabled state
            element.isVisible = true;
            element.isEnabled = true;
            
            // Position and size (not available from static HTML)
            element.x = 0;
            element.y = 0;
            element.width = 0;
            element.height = 0;
        }
        
        return element;
    }
    
public:
    HtmlParser() : output_(nullptr) {}
    
    ~HtmlParser() {
        if (output_) {
            gumbo_destroy_output(&kGumboDefaultOptions, output_);
        }
    }
    
    bool parse(const std::string& html) {
        if (output_) {
            gumbo_destroy_output(&kGumboDefaultOptions, output_);
        }
        
        html_ = html;
        output_ = gumbo_parse(html.c_str());
        
        if (!output_) {
            return false;
        }
        
        // Extract metadata
        title_.clear();
        text_.clear();
        links_.clear();
        images_.clear();
        
        findTitle(output_->root);
        extractText(output_->root, text_);
        findLinks(output_->root);
        findImages(output_->root);
        
        return true;
    }
    
    const std::string& getTitle() const {
        return title_;
    }
    
    const std::string& getText() const {
        return text_;
    }
    
    const std::string& getHtml() const {
        return html_;
    }
    
    const std::vector<std::string>& getLinks() const {
        return links_;
    }
    
    const std::vector<std::string>& getImages() const {
        return images_;
    }
    
    std::optional<WebElement> findElementById(const std::string& id) {
        if (!output_) {
            return std::nullopt;
        }
        
        GumboNode* node = findById(output_->root, id);
        if (!node) {
            return std::nullopt;
        }
        
        return nodeToElement(node);
    }
    
    std::vector<WebElement> findElementsByClass(const std::string& className) {
        std::vector<WebElement> elements;
        if (!output_) {
            return elements;
        }
        
        std::vector<GumboNode*> nodes;
        findByClass(output_->root, className, nodes);
        
        for (GumboNode* node : nodes) {
            elements.push_back(nodeToElement(node));
        }
        
        return elements;
    }
    
    std::vector<WebElement> findElementsByTag(const std::string& tagName) {
        std::vector<WebElement> elements;
        if (!output_) {
            return elements;
        }
        
        // Convert tag name to GumboTag
        GumboTag tag = gumbo_tag_enum(tagName.c_str());
        if (tag == GUMBO_TAG_UNKNOWN) {
            return elements;
        }
        
        std::vector<GumboNode*> nodes;
        findByTag(output_->root, tag, nodes);
        
        for (GumboNode* node : nodes) {
            elements.push_back(nodeToElement(node));
        }
        
        return elements;
    }
    
    // Simple CSS selector support (ID, class, tag)
    std::optional<WebElement> querySelector(const std::string& selector) {
        if (selector.empty()) {
            return std::nullopt;
        }
        
        // ID selector (#id)
        if (selector[0] == '#') {
            return findElementById(selector.substr(1));
        }
        
        // Class selector (.class)
        if (selector[0] == '.') {
            auto elements = findElementsByClass(selector.substr(1));
            if (!elements.empty()) {
                return elements[0];
            }
            return std::nullopt;
        }
        
        // Tag selector (tag)
        auto elements = findElementsByTag(selector);
        if (!elements.empty()) {
            return elements[0];
        }
        
        return std::nullopt;
    }
    
    std::vector<WebElement> querySelectorAll(const std::string& selector) {
        if (selector.empty()) {
            return {};
        }
        
        // ID selector (#id)
        if (selector[0] == '#') {
            auto element = findElementById(selector.substr(1));
            if (element) {
                return {*element};
            }
            return {};
        }
        
        // Class selector (.class)
        if (selector[0] == '.') {
            return findElementsByClass(selector.substr(1));
        }
        
        // Tag selector (tag)
        return findElementsByTag(selector);
    }
};

} // namespace browser_impl
} // namespace elizaos
