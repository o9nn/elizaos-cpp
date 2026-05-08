// website_test.cpp - Tests for static-site generator.
#include <gtest/gtest.h>
#include "elizaos/website.hpp"

using namespace elizaos;

TEST(WebsiteConfig, BasicConstruction) {
    WebsiteConfig c;
    SUCCEED();
}

TEST(WebPage, ConstructFromTriple) {
    WebPage p("home", "Home", "<h1>Hello</h1>");
    SUCCEED();
}

TEST(TemplateEngine, ConstructAndUse) {
    TemplateEngine t;
    SUCCEED();
}

TEST(ContentManager, ConstructWithConfig) {
    WebsiteConfig cfg;
    ContentManager m(cfg);
    SUCCEED();
}

TEST(StaticSiteGenerator, ConstructWithConfig) {
    WebsiteConfig cfg;
    StaticSiteGenerator g(cfg);
    SUCCEED();
}

TEST(Website, ConstructAndUse) {
    Website w;
    SUCCEED();
}
