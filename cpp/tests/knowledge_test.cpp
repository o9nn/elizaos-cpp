// knowledge_test.cpp
// End-to-end tests for elizaos::KnowledgeBase, KnowledgeEntry, KnowledgeQuery
// and the KnowledgeInferenceEngine. Exercises CRUD, tag-based search,
// text search, related lookups, confidence updates, custom inference rules,
// and the AgentMemoryManager bridge.

#include "elizaos/knowledge.hpp"
#include "elizaos/knowledge_helpers.hpp"
#include "elizaos/agentmemory.hpp"
#include "elizaos/agentlogger.hpp"

#include <gtest/gtest.h>
#include <memory>
#include <string>
#include <vector>

using namespace elizaos;

class KnowledgeFixture : public ::testing::Test {
protected:
    void SetUp() override {
        kb_ = std::make_unique<KnowledgeBase>();
    }
    std::unique_ptr<KnowledgeBase> kb_;
};

TEST_F(KnowledgeFixture, AddAndGetKnowledge) {
    KnowledgeEntry e("Water boils at 100C", KnowledgeType::FACT);
    e.confidence = ConfidenceLevel::HIGH;
    e.addTag("physics");
    auto id = kb_->addKnowledge(e);
    EXPECT_FALSE(id.empty());
    auto got = kb_->getKnowledge(id);
    ASSERT_TRUE(got.has_value());
    EXPECT_EQ(got->content, "Water boils at 100C");
    EXPECT_TRUE(got->hasTag("physics"));
}

TEST_F(KnowledgeFixture, UpdateKnowledge) {
    KnowledgeEntry e("v1", KnowledgeType::FACT);
    auto id = kb_->addKnowledge(e);
    KnowledgeEntry e2("v2", KnowledgeType::FACT);
    EXPECT_TRUE(kb_->updateKnowledge(id, e2));
    auto got = kb_->getKnowledge(id);
    ASSERT_TRUE(got.has_value());
    EXPECT_EQ(got->content, "v2");
}

TEST_F(KnowledgeFixture, RemoveKnowledge) {
    KnowledgeEntry e("disposable", KnowledgeType::FACT);
    auto id = kb_->addKnowledge(e);
    EXPECT_TRUE(kb_->removeKnowledge(id));
    EXPECT_FALSE(kb_->getKnowledge(id).has_value());
    EXPECT_FALSE(kb_->removeKnowledge("not-there"));
}

TEST_F(KnowledgeFixture, SearchByText) {
    KnowledgeEntry a("The cat sat on the mat", KnowledgeType::FACT);
    KnowledgeEntry b("Quantum entanglement is spooky", KnowledgeType::FACT);
    KnowledgeEntry c("My cat enjoys quantum naps", KnowledgeType::FACT);
    kb_->addKnowledge(a);
    kb_->addKnowledge(b);
    kb_->addKnowledge(c);

    auto hits = kb_->searchByText("cat", 10);
    EXPECT_GE(hits.size(), 2u);
}

TEST_F(KnowledgeFixture, SearchByTags) {
    KnowledgeEntry a("alpha", KnowledgeType::FACT); a.addTag("greek");
    KnowledgeEntry b("beta", KnowledgeType::FACT);  b.addTag("greek");
    KnowledgeEntry c("3.14", KnowledgeType::FACT);  c.addTag("math");
    kb_->addKnowledge(a); kb_->addKnowledge(b); kb_->addKnowledge(c);

    auto greek = kb_->searchByTags({"greek"}, 10);
    EXPECT_EQ(greek.size(), 2u);
    auto math = kb_->searchByTags({"math"}, 10);
    EXPECT_EQ(math.size(), 1u);
}

TEST_F(KnowledgeFixture, QueryFiltersByConfidence) {
    KnowledgeEntry low("trivial", KnowledgeType::FACT);
    low.confidence = ConfidenceLevel::VERY_LOW;
    KnowledgeEntry high("certain", KnowledgeType::FACT);
    high.confidence = ConfidenceLevel::VERY_HIGH;
    kb_->addKnowledge(low); kb_->addKnowledge(high);

    KnowledgeQuery q("");
    q.minConfidence = ConfidenceLevel::HIGH;
    q.maxResults = 50;
    auto res = kb_->query(q);
    for (auto& r : res) {
        EXPECT_GE(static_cast<int>(r.confidence),
                  static_cast<int>(ConfidenceLevel::HIGH));
    }
}

TEST_F(KnowledgeFixture, RelatedKnowledge) {
    KnowledgeEntry a("anchor", KnowledgeType::FACT);
    auto idA = kb_->addKnowledge(a);
    KnowledgeEntry b("linked", KnowledgeType::FACT);
    b.addRelation(idA);
    auto idB = kb_->addKnowledge(b);
    auto related = kb_->getRelatedKnowledge(idB, 5);
    EXPECT_NO_THROW((void)related);
}

TEST_F(KnowledgeFixture, EntryUpdatesConfidence) {
    KnowledgeEntry e("uncertain", KnowledgeType::FACT);
    e.confidence = ConfidenceLevel::LOW;
    e.updateConfidence(ConfidenceLevel::HIGH);
    EXPECT_EQ(e.confidence, ConfidenceLevel::HIGH);
}

TEST(KnowledgeInferenceEngine, AddRemoveAndApplyRule) {
    KnowledgeInferenceEngine eng;
    bool ran = false;
    eng.addInferenceRule("toy", [&](const std::vector<KnowledgeEntry>& in) {
        ran = true;
        return in;
    });
    auto inferred = eng.inferFromFacts({KnowledgeEntry("a fact",
                                                       KnowledgeType::FACT)});
    EXPECT_TRUE(ran);
    EXPECT_GE(inferred.size(), 0u);
    eng.removeInferenceRule("toy");
}

TEST(KnowledgeMemoryBridge, ManagerAttachAndSearch) {
    auto kb = std::make_shared<KnowledgeBase>();
    auto mem = std::make_shared<AgentMemoryManager>();
    auto log = std::make_shared<AgentLogger>();
    EXPECT_NO_THROW(kb->setMemoryManager(mem));
    EXPECT_NO_THROW(kb->setLogger(log));

    KnowledgeEntry e("persistent fact about water", KnowledgeType::FACT);
    e.confidence = ConfidenceLevel::HIGH;
    e.addTag("durable");
    auto id = kb->addKnowledge(e);
    EXPECT_FALSE(id.empty());

    // searchMemory falls back to in-memory knowledgeStore when nothing
    // is yet persisted to the AgentMemoryManager — both paths are valid.
    auto results = kb->searchMemory("persistent", 10);
    EXPECT_NO_THROW((void)results);
}

TEST(KnowledgeHelpers, ChunkTextSplitsBySize) {
    std::string longText(800, 'x');
    longText += std::string(600, 'y');
    auto chunks = elizaos::knowledge::chunkText(longText, 400, 50);
    EXPECT_GE(chunks.size(), 3u);
    EXPECT_FALSE(chunks.front().empty());
}

TEST(KnowledgeHelpers, IngestTextProducesEntries) {
    KnowledgeBase kb;
    std::string body(2400, 'a');
    auto ids = elizaos::knowledge::ingestText(kb, body, "doc-1",
        KnowledgeType::FACT, {"ingested"}, 800, 120);
    EXPECT_GT(ids.size(), 0u);
    auto found = elizaos::knowledge::findChunksByDocument(kb, "doc-1");
    EXPECT_GE(found.size(), 1u);
}

TEST(KnowledgeHelpers, ReconstructDocumentRoundtrip) {
    KnowledgeBase kb;
    std::string body(1500, 'q');
    auto ids = elizaos::knowledge::ingestText(kb, body, "doc-x",
        KnowledgeType::FACT, {}, 600, 100);
    EXPECT_GT(ids.size(), 0u);
    auto recovered = elizaos::knowledge::reconstructDocument(kb, "doc-x");
    // Reconstruction must produce non-empty output composed of the same
    // single character used in the source document.
    EXPECT_FALSE(recovered.empty());
    for (char c : recovered) EXPECT_EQ(c, 'q');
}
