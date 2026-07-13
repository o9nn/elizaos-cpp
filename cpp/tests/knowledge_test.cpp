// knowledge_test.cpp - E2E tests for elizaos::KnowledgeBase / KnowledgeEntry / KnowledgeQuery.
#include <gtest/gtest.h>
#include "elizaos/knowledge.hpp"
#include "elizaos/knowledge_helpers.hpp"

using namespace elizaos;

TEST(KnowledgeEntry, DefaultConstruction) {
    KnowledgeEntry e;
    EXPECT_EQ(e.type, KnowledgeType::FACT);
    EXPECT_EQ(e.confidence, ConfidenceLevel::MEDIUM);
}

TEST(KnowledgeEntry, ContentConstructor) {
    KnowledgeEntry e("water boils at 100C", KnowledgeType::FACT);
    EXPECT_EQ(e.content, "water boils at 100C");
    EXPECT_EQ(e.type, KnowledgeType::FACT);
}

TEST(KnowledgeEntry, TagsAndRelations) {
    KnowledgeEntry e("x");
    e.addTag("physics");
    e.addTag("temperature");
    EXPECT_TRUE(e.hasTag("physics"));
    EXPECT_FALSE(e.hasTag("missing"));
    e.addRelation("rel-1");
    EXPECT_FALSE(e.related_entries.empty());
}

TEST(KnowledgeEntry, UpdateConfidence) {
    KnowledgeEntry e("x");
    e.updateConfidence(ConfidenceLevel::VERY_HIGH);
    EXPECT_EQ(e.confidence, ConfidenceLevel::VERY_HIGH);
}

TEST(KnowledgeEntry, JsonRoundtrip) {
    KnowledgeEntry e("roundtrip", KnowledgeType::CONCEPT);
    e.addTag("a");
    auto j = e.toJson();
    auto back = KnowledgeEntry::fromJson(j);
    EXPECT_EQ(back.content, "roundtrip");
    EXPECT_EQ(back.type, KnowledgeType::CONCEPT);
}

TEST(KnowledgeQuery, ConstructionDefaults) {
    KnowledgeQuery q("temperature");
    EXPECT_EQ(q.text, "temperature");
    EXPECT_EQ(q.maxResults, 10);
    EXPECT_FALSE(q.includeRelated);
}

class KnowledgeBaseTest : public ::testing::Test {
protected:
    KnowledgeBase kb;

    void TearDown() override { kb.clear(); }
};

TEST_F(KnowledgeBaseTest, AddAndGetKnowledge) {
    KnowledgeEntry e("hello world");
    auto id = kb.addKnowledge(e);
    EXPECT_FALSE(id.empty());
    auto got = kb.getKnowledge(id);
    ASSERT_TRUE(got.has_value());
    EXPECT_EQ(got->content, "hello world");
}

TEST_F(KnowledgeBaseTest, UpdateKnowledge) {
    KnowledgeEntry e("v1");
    auto id = kb.addKnowledge(e);
    KnowledgeEntry updated("v2");
    EXPECT_TRUE(kb.updateKnowledge(id, updated));
    auto got = kb.getKnowledge(id);
    ASSERT_TRUE(got.has_value());
    EXPECT_EQ(got->content, "v2");
}

TEST_F(KnowledgeBaseTest, RemoveKnowledge) {
    auto id = kb.addKnowledge(KnowledgeEntry("doomed"));
    EXPECT_TRUE(kb.removeKnowledge(id));
    EXPECT_FALSE(kb.getKnowledge(id).has_value());
}

TEST_F(KnowledgeBaseTest, SearchByText) {
    kb.addKnowledge(KnowledgeEntry("the cat sat"));
    kb.addKnowledge(KnowledgeEntry("dogs bark loudly"));
    auto results = kb.searchByText("cat", 5);
    EXPECT_GE(results.size(), 1u);
}

TEST_F(KnowledgeBaseTest, SearchByTags) {
    KnowledgeEntry a("a");
    a.addTag("animal");
    KnowledgeEntry b("b");
    b.addTag("plant");
    kb.addKnowledge(a);
    kb.addKnowledge(b);
    auto results = kb.searchByTags({"animal"}, 5);
    EXPECT_GE(results.size(), 1u);
}

TEST_F(KnowledgeBaseTest, GetKnowledgeByType) {
    KnowledgeEntry r("r", KnowledgeType::RULE);
    kb.addKnowledge(r);
    auto rules = kb.getKnowledgeByType(KnowledgeType::RULE);
    EXPECT_EQ(rules.size(), 1u);
}

TEST_F(KnowledgeBaseTest, KnowledgeCount) {
    EXPECT_EQ(kb.getKnowledgeCount(), 0u);
    kb.addKnowledge(KnowledgeEntry("a"));
    kb.addKnowledge(KnowledgeEntry("b"));
    EXPECT_EQ(kb.getKnowledgeCount(), 2u);
}

TEST_F(KnowledgeBaseTest, AllTags) {
    KnowledgeEntry e("tagged");
    e.addTag("alpha");
    e.addTag("beta");
    kb.addKnowledge(e);
    auto tags = kb.getAllTags();
    EXPECT_GE(tags.size(), 2u);
}

TEST_F(KnowledgeBaseTest, ClearEmpties) {
    kb.addKnowledge(KnowledgeEntry("a"));
    kb.clear();
    EXPECT_EQ(kb.getKnowledgeCount(), 0u);
}

TEST_F(KnowledgeBaseTest, HelpersIngestReconstructAndSelfCheck) {
    const std::string text = "alpha beta gamma delta epsilon zeta eta theta iota kappa lambda mu";
    auto chunks = knowledge::chunkText(text, 24, 6);
    ASSERT_GE(chunks.size(), 2u);

    auto ids = knowledge::ingestText(kb, "  " + text + "  ", "helpers_e2e.rule",
                                     KnowledgeType::RULE, {"helpers", "e2e"}, 24, 6);
    EXPECT_EQ(ids.size(), chunks.size());

    auto storedChunks = knowledge::findChunksByDocument(kb, "helpers_e2e.rule");
    ASSERT_EQ(storedChunks.size(), ids.size());
    EXPECT_EQ(storedChunks.front().type, KnowledgeType::RULE);
    EXPECT_TRUE(storedChunks.front().hasTag("helpers"));
    EXPECT_EQ(knowledge::reconstructDocument(kb, "helpers_e2e.rule"), text);
    EXPECT_TRUE(knowledge::knowledge_helpers_self_check());
    EXPECT_NO_THROW(knowledge::knowledge_helpers_placeholder());
}

TEST_F(KnowledgeBaseTest, QueryWithFilter) {
    kb.addKnowledge(KnowledgeEntry("water", KnowledgeType::FACT));
    kb.addKnowledge(KnowledgeEntry("if rain then wet", KnowledgeType::RULE));
    KnowledgeQuery q("water");
    q.types = {KnowledgeType::FACT};
    auto r = kb.query(q);
    EXPECT_GE(r.size(), 1u);
}

TEST_F(KnowledgeBaseTest, StatisticsString) {
    auto s = kb.getStatistics();
    EXPECT_FALSE(s.empty());
}

TEST(KnowledgeUtils, EnumStringRoundtrip) {
    for (auto t : {KnowledgeType::FACT, KnowledgeType::RULE, KnowledgeType::CONCEPT,
                   KnowledgeType::RELATIONSHIP, KnowledgeType::PROCEDURE,
                   KnowledgeType::EXPERIENCE}) {
        EXPECT_EQ(stringToKnowledgeType(knowledgeTypeToString(t)), t);
    }
    for (auto c : {ConfidenceLevel::VERY_LOW, ConfidenceLevel::LOW,
                   ConfidenceLevel::MEDIUM, ConfidenceLevel::HIGH,
                   ConfidenceLevel::VERY_HIGH}) {
        EXPECT_EQ(stringToConfidenceLevel(confidenceLevelToString(c)), c);
    }
    for (auto s : {KnowledgeSource::LEARNED, KnowledgeSource::PROGRAMMED,
                   KnowledgeSource::INFERRED, KnowledgeSource::OBSERVED,
                   KnowledgeSource::COMMUNICATED}) {
        EXPECT_EQ(stringToKnowledgeSource(knowledgeSourceToString(s)), s);
    }
}

TEST(KnowledgeInference, AddRule) {
    KnowledgeInferenceEngine eng;
    eng.addInferenceRule("identity", [](const std::vector<KnowledgeEntry>& in) {
        return in;
    });
    auto out = eng.inferFromFacts({KnowledgeEntry("seed")});
    SUCCEED() << "produced " << out.size();
    eng.removeInferenceRule("identity");
}
