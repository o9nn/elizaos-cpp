// knowledge_test.cpp - E2E tests for elizaos::KnowledgeBase / KnowledgeEntry / KnowledgeQuery.
#include <gtest/gtest.h>
#include "elizaos/knowledge.hpp"
#include "elizaos/knowledge_helpers.hpp"

#include <any>

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

TEST_F(KnowledgeBaseTest, ImportReportsMalformedSiblingWithoutDiscardingValidEntry) {
    KnowledgeEntry valid("validated import", KnowledgeType::CONCEPT);
    valid.id = "valid-import";
    JsonValue payload;
    payload["valid"] = std::any(valid.toJson());
    payload["malformed"] = std::any(std::string("not a JsonValue"));

    EXPECT_FALSE(kb.importFromJson(payload));
    EXPECT_EQ(kb.getKnowledgeCount(), 1u);
    const auto imported = kb.getKnowledge("valid-import");
    ASSERT_TRUE(imported.has_value());
    EXPECT_EQ(imported->content, "validated import");
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

namespace {

size_t clusterContaining(const std::vector<std::vector<std::string>>& clusters,
                         const std::string& id) {
    for (size_t index = 0; index < clusters.size(); ++index) {
        if (std::find(clusters[index].begin(), clusters[index].end(), id) !=
            clusters[index].end()) {
            return index;
        }
    }
    return clusters.size();
}

} // namespace

TEST(EnhancedKnowledgeBase, ClustersEntriesBySemanticSimilarityDeterministically) {
    EnhancedKnowledgeBase knowledge;

    KnowledgeEntry catOne("cat feline whiskers animal", KnowledgeType::CONCEPT);
    catOne.addTag("animals");
    KnowledgeEntry catTwo("cat feline purr whiskers", KnowledgeType::CONCEPT);
    catTwo.addTag("animals");
    KnowledgeEntry rocketOne("rocket orbit space launch", KnowledgeType::CONCEPT);
    rocketOne.addTag("space");
    KnowledgeEntry rocketTwo("space rocket orbital launch", KnowledgeType::CONCEPT);
    rocketTwo.addTag("space");

    const auto catOneId = knowledge.addKnowledge(catOne);
    const auto catTwoId = knowledge.addKnowledge(catTwo);
    const auto rocketOneId = knowledge.addKnowledge(rocketOne);
    const auto rocketTwoId = knowledge.addKnowledge(rocketTwo);

    const auto clusters = knowledge.clusterKnowledge(2);
    ASSERT_EQ(clusters.size(), 2u);
    EXPECT_EQ(clusterContaining(clusters, catOneId), clusterContaining(clusters, catTwoId));
    EXPECT_EQ(clusterContaining(clusters, rocketOneId), clusterContaining(clusters, rocketTwoId));
    EXPECT_NE(clusterContaining(clusters, catOneId), clusterContaining(clusters, rocketOneId));
    EXPECT_EQ(clusters, knowledge.clusterKnowledge(2));

    EXPECT_TRUE(knowledge.clusterKnowledge(0).empty());
    EXPECT_EQ(knowledge.clusterKnowledge(20).size(), 4u);
}

TEST(KnowledgeHypergraph, ReportsNodeIdsAndAverageShortestPathLength) {
    KnowledgeHypergraph graph;
    KnowledgeEntry a("node a");
    KnowledgeEntry b("node b");
    KnowledgeEntry c("node c");
    a.id = "a";
    b.id = "b";
    c.id = "c";
    graph.addNode(a);
    graph.addNode(b);
    graph.addNode(c);
    graph.addEdge(Hyperedge("connects", {"a", "b"}));
    graph.addEdge(Hyperedge("connects", {"b", "c"}));

    EXPECT_EQ(graph.getNodeIds(), (std::vector<std::string>{"a", "b", "c"}));
    EXPECT_NEAR(graph.averageShortestPathLength(), 4.0 / 3.0, 1e-9);
}

TEST(EnhancedKnowledgeBase, EnhancedStatsReflectGraphInferenceAndPendingConflicts) {
    EnhancedKnowledgeBase knowledge;
    KnowledgeEntry a("shared semantic value", KnowledgeType::FACT);
    KnowledgeEntry b("shared semantic value", KnowledgeType::FACT);
    KnowledgeEntry c("derived result", KnowledgeType::CONCEPT);
    a.confidence = ConfidenceLevel::HIGH;
    b.confidence = ConfidenceLevel::LOW;
    c.source = KnowledgeSource::INFERRED;

    const auto aId = knowledge.addKnowledge(a);
    const auto bId = knowledge.addKnowledge(b);
    const auto cId = knowledge.addKnowledge(c);
    knowledge.enableHypergraph();
    ASSERT_FALSE(knowledge.addRelationship("supports", {aId, bId}).empty());
    ASSERT_FALSE(knowledge.addRelationship("supports", {bId, cId}).empty());

    a.id = aId;
    b.id = bId;
    const auto conflicts = knowledge.getFusionEngine().detectConflicts({a, b});
    ASSERT_EQ(conflicts.size(), 1u);

    const auto stats = knowledge.getEnhancedStats();
    EXPECT_EQ(stats.totalEntries, 3u);
    EXPECT_EQ(stats.totalRelationships, 2u);
    EXPECT_EQ(stats.inferredEntries, 1u);
    EXPECT_EQ(stats.conflictsPending, 1u);
    EXPECT_NEAR(stats.graphDensity, 2.0 / 3.0, 1e-9);
    EXPECT_EQ(stats.averagePathLength, 1);
}

TEST(KnowledgeBase, InstancesOwnIndependentStores) {
    KnowledgeBase first;
    KnowledgeBase second;
    const auto id = first.addKnowledge(KnowledgeEntry("private to first"));

    EXPECT_EQ(first.getKnowledgeCount(), 1u);
    EXPECT_EQ(second.getKnowledgeCount(), 0u);
    EXPECT_TRUE(first.getKnowledge(id).has_value());
    EXPECT_FALSE(second.getKnowledge(id).has_value());
}
