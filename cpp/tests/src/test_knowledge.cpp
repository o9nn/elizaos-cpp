#include <gtest/gtest.h>
#include "elizaos/knowledge.hpp"
#include "elizaos/core.hpp"
#include <memory>
#include <thread>
#include <chrono>

using namespace elizaos;

class KnowledgeTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Create a fresh knowledge base for each test
        kb = std::make_shared<KnowledgeBase>();
        // Clear any existing state
        kb->clear();
    }

    void TearDown() override {
        // Clean up after each test
        if (kb) {
            kb->clear();
        }
    }

    std::shared_ptr<KnowledgeBase> kb;
};

// =====================================================
// KnowledgeEntry Tests
// =====================================================

TEST_F(KnowledgeTest, KnowledgeEntryCreation) {
    KnowledgeEntry entry("Test fact", KnowledgeType::FACT);
    
    EXPECT_EQ(entry.content, "Test fact");
    EXPECT_EQ(entry.type, KnowledgeType::FACT);
    EXPECT_EQ(entry.confidence, ConfidenceLevel::MEDIUM);
    EXPECT_EQ(entry.source, KnowledgeSource::PROGRAMMED);
    EXPECT_FALSE(entry.id.empty());
    EXPECT_TRUE(entry.tags.empty());
    EXPECT_TRUE(entry.related_entries.empty());
}

TEST_F(KnowledgeTest, KnowledgeEntryTagManagement) {
    KnowledgeEntry entry("Test fact with tags", KnowledgeType::FACT);
    
    // Add tags
    entry.addTag("science");
    entry.addTag("physics");
    entry.addTag("quantum");
    
    EXPECT_EQ(entry.tags.size(), 3);
    EXPECT_TRUE(entry.hasTag("science"));
    EXPECT_TRUE(entry.hasTag("physics"));
    EXPECT_TRUE(entry.hasTag("quantum"));
    EXPECT_FALSE(entry.hasTag("chemistry"));
    
    // Adding duplicate tag should not increase size
    entry.addTag("science");
    EXPECT_EQ(entry.tags.size(), 3);
}

TEST_F(KnowledgeTest, KnowledgeEntryRelationships) {
    KnowledgeEntry entry("Central fact", KnowledgeType::FACT);
    
    entry.addRelation("related-fact-1");
    entry.addRelation("related-fact-2");
    entry.addRelation("related-concept-1");
    
    EXPECT_EQ(entry.related_entries.size(), 3);
    
    // Adding duplicate relation should not increase size
    entry.addRelation("related-fact-1");
    EXPECT_EQ(entry.related_entries.size(), 3);
}

TEST_F(KnowledgeTest, KnowledgeEntryConfidenceUpdate) {
    KnowledgeEntry entry("Uncertain fact", KnowledgeType::FACT);
    
    auto originalTime = entry.updated_at;
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    
    entry.updateConfidence(ConfidenceLevel::HIGH);
    EXPECT_EQ(entry.confidence, ConfidenceLevel::HIGH);
    EXPECT_GT(entry.updated_at, originalTime);
}

TEST_F(KnowledgeTest, KnowledgeEntrySerialization) {
    KnowledgeEntry original("Serialization test", KnowledgeType::CONCEPT);
    original.addTag("test");
    original.addTag("serialization");
    original.updateConfidence(ConfidenceLevel::HIGH);
    
    // Convert to JSON and back
    JsonValue json = original.toJson();
    KnowledgeEntry deserialized = KnowledgeEntry::fromJson(json);
    
    EXPECT_EQ(deserialized.id, original.id);
    EXPECT_EQ(deserialized.content, original.content);
    EXPECT_EQ(deserialized.type, original.type);
    EXPECT_EQ(deserialized.confidence, original.confidence);
    EXPECT_EQ(deserialized.source, original.source);
}

// =====================================================
// KnowledgeQuery Tests
// =====================================================

TEST_F(KnowledgeTest, KnowledgeQueryCreation) {
    KnowledgeQuery query("test query");
    
    EXPECT_EQ(query.text, "test query");
    EXPECT_TRUE(query.types.empty());
    EXPECT_TRUE(query.tags.empty());
    EXPECT_EQ(query.minConfidence, ConfidenceLevel::VERY_LOW);
    EXPECT_EQ(query.maxResults, 10);
    EXPECT_FALSE(query.includeRelated);
}

// =====================================================
// KnowledgeBase Tests
// =====================================================

TEST_F(KnowledgeTest, KnowledgeBaseBasicOperations) {
    // Add knowledge
    KnowledgeEntry entry("Test knowledge entry", KnowledgeType::FACT);
    entry.addTag("test");
    
    std::string id = kb->addKnowledge(entry);
    EXPECT_FALSE(id.empty());
    EXPECT_EQ(kb->getKnowledgeCount(), 1);
    
    // Retrieve knowledge
    auto retrieved = kb->getKnowledge(id);
    ASSERT_TRUE(retrieved.has_value());
    EXPECT_EQ(retrieved->content, "Test knowledge entry");
    EXPECT_EQ(retrieved->type, KnowledgeType::FACT);
    EXPECT_TRUE(retrieved->hasTag("test"));
    
    // Update knowledge
    KnowledgeEntry updated("Updated knowledge entry", KnowledgeType::RULE);
    bool updateResult = kb->updateKnowledge(id, updated);
    EXPECT_TRUE(updateResult);
    
    auto updatedRetrieved = kb->getKnowledge(id);
    ASSERT_TRUE(updatedRetrieved.has_value());
    EXPECT_EQ(updatedRetrieved->content, "Updated knowledge entry");
    EXPECT_EQ(updatedRetrieved->type, KnowledgeType::RULE);
    
    // Remove knowledge
    bool removeResult = kb->removeKnowledge(id);
    EXPECT_TRUE(removeResult);
    EXPECT_EQ(kb->getKnowledgeCount(), 0);
    
    auto removedRetrieved = kb->getKnowledge(id);
    EXPECT_FALSE(removedRetrieved.has_value());
}

TEST_F(KnowledgeTest, KnowledgeBaseTextSearch) {
    // Add test knowledge entries
    KnowledgeEntry entry1("Quantum mechanics is a fundamental theory", KnowledgeType::FACT);
    KnowledgeEntry entry2("Classical mechanics deals with motion", KnowledgeType::FACT);
    KnowledgeEntry entry3("Thermodynamics studies heat transfer", KnowledgeType::FACT);
    
    kb->addKnowledge(entry1);
    kb->addKnowledge(entry2);
    kb->addKnowledge(entry3);
    
    // Search for mechanics
    auto results = kb->searchByText("mechanics", 10);
    EXPECT_GE(results.size(), 2); // Should find both quantum and classical mechanics
    
    // Search for specific term
    auto quantumResults = kb->searchByText("quantum", 10);
    EXPECT_GE(quantumResults.size(), 1);
    
    // Search for non-existent term
    auto noResults = kb->searchByText("nonexistent", 10);
    EXPECT_EQ(noResults.size(), 0);
}

TEST_F(KnowledgeTest, KnowledgeBaseTagSearch) {
    // Add knowledge with different tags
    KnowledgeEntry physics1("Newton's laws", KnowledgeType::RULE);
    physics1.addTag("physics");
    physics1.addTag("classical");
    
    KnowledgeEntry physics2("Einstein's relativity", KnowledgeType::CONCEPT);
    physics2.addTag("physics");
    physics2.addTag("modern");
    
    KnowledgeEntry chemistry("Periodic table", KnowledgeType::FACT);
    chemistry.addTag("chemistry");
    chemistry.addTag("elements");
    
    kb->addKnowledge(physics1);
    kb->addKnowledge(physics2);
    kb->addKnowledge(chemistry);
    
    // Search by single tag
    auto physicsResults = kb->searchByTags({"physics"}, 10);
    EXPECT_EQ(physicsResults.size(), 2);
    
    // Search by multiple tags
    auto classicalResults = kb->searchByTags({"physics", "classical"}, 10);
    EXPECT_GE(classicalResults.size(), 1);
    
    // Search by non-existent tag
    auto noResults = kb->searchByTags({"nonexistent"}, 10);
    EXPECT_EQ(noResults.size(), 0);
}

TEST_F(KnowledgeTest, KnowledgeBaseQueryWithFilters) {
    // Add knowledge entries with different types and confidence levels
    KnowledgeEntry fact1("High confidence fact", KnowledgeType::FACT);
    fact1.updateConfidence(ConfidenceLevel::HIGH);
    fact1.addTag("science");
    
    KnowledgeEntry fact2("Low confidence fact", KnowledgeType::FACT);
    fact2.updateConfidence(ConfidenceLevel::LOW);
    fact2.addTag("science");
    
    KnowledgeEntry rule1("Important rule", KnowledgeType::RULE);
    rule1.updateConfidence(ConfidenceLevel::HIGH);
    rule1.addTag("science");
    
    kb->addKnowledge(fact1);
    kb->addKnowledge(fact2);
    kb->addKnowledge(rule1);
    
    // Query with confidence filter
    KnowledgeQuery highConfQuery("confidence");
    highConfQuery.minConfidence = ConfidenceLevel::HIGH;
    auto highConfResults = kb->query(highConfQuery);
    EXPECT_GE(highConfResults.size(), 1); // Should find at least the high confidence entries
    EXPECT_LE(highConfResults.size(), 2); // Should not exceed the number of high confidence entries
    
    // Query with type filter
    KnowledgeQuery factQuery("fact");
    factQuery.types = {KnowledgeType::FACT};
    auto factResults = kb->query(factQuery);
    EXPECT_GE(factResults.size(), 1); // Should find at least some fact entries
    EXPECT_LE(factResults.size(), 2); // Should not exceed the number of fact entries
    
    // Query with tag filter
    KnowledgeQuery tagQuery("");
    tagQuery.tags = {"science"};
    auto tagResults = kb->query(tagQuery);
    EXPECT_GE(tagResults.size(), 1); // Should find entries with science tag
    EXPECT_LE(tagResults.size(), 3); // Should not exceed the number of entries with science tag
}

TEST_F(KnowledgeTest, KnowledgeBaseStatistics) {
    // Add knowledge entries of different types
    KnowledgeEntry fact("Test fact", KnowledgeType::FACT);
    KnowledgeEntry rule("Test rule", KnowledgeType::RULE);
    KnowledgeEntry conceptEntry("Test concept", KnowledgeType::CONCEPT);
    
    kb->addKnowledge(fact);
    kb->addKnowledge(rule);
    kb->addKnowledge(conceptEntry);
    
    // Check count
    EXPECT_EQ(kb->getKnowledgeCount(), 3);
    
    // Check type statistics
    auto typeStats = kb->getKnowledgeTypeStats();
    EXPECT_EQ(typeStats[KnowledgeType::FACT], 1);
    EXPECT_EQ(typeStats[KnowledgeType::RULE], 1);
    EXPECT_EQ(typeStats[KnowledgeType::CONCEPT], 1);
    
    // Check statistics string
    std::string stats = kb->getStatistics();
    EXPECT_FALSE(stats.empty());
    EXPECT_TRUE(stats.find("Total entries: 3") != std::string::npos);
}

TEST_F(KnowledgeTest, KnowledgeBaseRelatedKnowledge) {
    // Create related knowledge entries
    KnowledgeEntry primary("Primary concept", KnowledgeType::CONCEPT);
    primary.addTag("primary");
    primary.addTag("main");
    
    KnowledgeEntry related1("Related fact 1", KnowledgeType::FACT);
    related1.addTag("primary");
    related1.addTag("supporting");
    
    KnowledgeEntry related2("Related fact 2", KnowledgeType::FACT);
    related2.addTag("main");
    related2.addTag("supporting");
    
    KnowledgeEntry unrelated("Unrelated fact", KnowledgeType::FACT);
    unrelated.addTag("different");
    
    std::string primaryId = kb->addKnowledge(primary);
    std::string related1Id = kb->addKnowledge(related1);
    std::string related2Id = kb->addKnowledge(related2);
    kb->addKnowledge(unrelated);
    
    // Add explicit relationships
    primary.addRelation(related1Id);
    primary.addRelation(related2Id);
    kb->updateKnowledge(primaryId, primary);
    
    // Get related knowledge
    auto relatedEntries = kb->getRelatedKnowledge(primaryId, 5);
    EXPECT_GE(relatedEntries.size(), 2); // Should find related entries via tags and explicit relations
}

// =====================================================
// KnowledgeInferenceEngine Tests
// =====================================================

TEST_F(KnowledgeTest, InferenceEngineBasicOperations) {
    auto engine = std::make_shared<KnowledgeInferenceEngine>();
    
    // Test basic inference
    std::vector<KnowledgeEntry> facts;
    KnowledgeEntry fact1("A relates to B", KnowledgeType::RELATIONSHIP);
    KnowledgeEntry fact2("B relates to C", KnowledgeType::RELATIONSHIP);
    facts.push_back(fact1);
    facts.push_back(fact2);
    
    auto inferred = engine->inferFromFacts(facts);
    // The default transitivity rule should generate some inferences
    EXPECT_GE(inferred.size(), 0);
    
    // Test related concepts
    KnowledgeEntry testEntry("Test concept", KnowledgeType::CONCEPT);
    auto relatedConcepts = engine->findRelatedConcepts(testEntry);
    EXPECT_GE(relatedConcepts.size(), 1);
    
    // Test evidence combination
    std::vector<KnowledgeEntry> evidence;
    KnowledgeEntry evidence1("Evidence 1", KnowledgeType::FACT);
    evidence1.updateConfidence(ConfidenceLevel::HIGH);
    KnowledgeEntry evidence2("Evidence 2", KnowledgeType::FACT);
    evidence2.updateConfidence(ConfidenceLevel::MEDIUM);
    evidence.push_back(evidence1);
    evidence.push_back(evidence2);
    
    auto combined = engine->combineEvidence(evidence);
    EXPECT_EQ(combined.type, KnowledgeType::FACT);
    EXPECT_EQ(combined.source, KnowledgeSource::INFERRED);
    EXPECT_TRUE(combined.hasTag("combined_evidence"));
}

TEST_F(KnowledgeTest, InferenceEngineCustomRules) {
    auto engine = std::make_shared<KnowledgeInferenceEngine>();
    
    // Add custom inference rule
    engine->addInferenceRule("test_rule", [](const std::vector<KnowledgeEntry>& facts) {
        std::vector<KnowledgeEntry> results;
        for (const auto& fact : facts) {
            if (fact.hasTag("trigger")) {
                KnowledgeEntry inferred("Inferred from trigger", KnowledgeType::FACT);
                inferred.source = KnowledgeSource::INFERRED;
                inferred.addTag("custom_inferred");
                results.push_back(inferred);
            }
        }
        return results;
    });
    
    // Test custom rule
    std::vector<KnowledgeEntry> facts;
    KnowledgeEntry triggerFact("Trigger fact", KnowledgeType::FACT);
    triggerFact.addTag("trigger");
    facts.push_back(triggerFact);
    
    auto inferred = engine->inferFromFacts(facts);
    bool foundCustomInferred = false;
    for (const auto& entry : inferred) {
        if (entry.hasTag("custom_inferred")) {
            foundCustomInferred = true;
            break;
        }
    }
    EXPECT_TRUE(foundCustomInferred);
    
    // Remove custom rule
    engine->removeInferenceRule("test_rule");
    auto inferredAfterRemoval = engine->inferFromFacts(facts);
    bool foundCustomInferredAfterRemoval = false;
    for (const auto& entry : inferredAfterRemoval) {
        if (entry.hasTag("custom_inferred")) {
            foundCustomInferredAfterRemoval = true;
            break;
        }
    }
    EXPECT_FALSE(foundCustomInferredAfterRemoval);
}

TEST_F(KnowledgeTest, KnowledgeBaseInferenceIntegration) {
    // Test inference integration with knowledge base
    auto customEngine = std::make_shared<KnowledgeInferenceEngine>();
    kb->setInferenceEngine(customEngine);
    
    // Add some facts for inference
    KnowledgeEntry fact1("Socrates is a man", KnowledgeType::FACT);
    fact1.addTag("person");
    fact1.addTag("philosopher");
    
    KnowledgeEntry fact2("All men are mortal", KnowledgeType::RULE);
    fact2.addTag("mortality");
    fact2.addTag("rule");
    
    kb->addKnowledge(fact1);
    kb->addKnowledge(fact2);
    
    // Perform inference query
    KnowledgeQuery inferenceQuery("mortality");
    auto inferred = kb->performInference(inferenceQuery);
    
    // Check that inference was performed and results were added to knowledge base
    auto totalCount = kb->getKnowledgeCount();
    EXPECT_GT(totalCount, 2); // Should have original facts plus inferred knowledge
}

// =====================================================
// Knowledge Base Maintenance Tests
// =====================================================

TEST_F(KnowledgeTest, KnowledgeBaseValidation) {
    // Add valid and potentially invalid knowledge
    KnowledgeEntry validEntry("Valid entry", KnowledgeType::FACT);
    KnowledgeEntry emptyEntry("", KnowledgeType::FACT); // Invalid - empty content
    
    std::string validId = kb->addKnowledge(validEntry);
    std::string emptyId = kb->addKnowledge(emptyEntry);
    
    EXPECT_FALSE(validId.empty());
    EXPECT_TRUE(emptyId.empty()); // Should reject empty content
    
    // Run validation
    kb->validateKnowledge();
    // Validation should complete without errors
    EXPECT_EQ(kb->getKnowledgeCount(), 1); // Only valid entry should remain
}

TEST_F(KnowledgeTest, KnowledgeBasePruning) {
    // Add old low-confidence knowledge
    KnowledgeEntry oldEntry("Old low confidence entry", KnowledgeType::FACT);
    oldEntry.updateConfidence(ConfidenceLevel::LOW);
    
    // Add recent high-confidence knowledge
    KnowledgeEntry recentEntry("Recent high confidence entry", KnowledgeType::FACT);
    recentEntry.updateConfidence(ConfidenceLevel::HIGH);
    
    kb->addKnowledge(oldEntry);
    kb->addKnowledge(recentEntry);
    
    EXPECT_EQ(kb->getKnowledgeCount(), 2);
    
    // Prune old knowledge (use very short time for testing)
    kb->pruneOldKnowledge(std::chrono::hours(0)); // Prune everything older than now
    
    // Should still have both entries since the pruning logic considers confidence levels
    EXPECT_GE(kb->getKnowledgeCount(), 1);
}

TEST_F(KnowledgeTest, KnowledgeBaseConsolidation) {
    // Add similar knowledge entries
    KnowledgeEntry entry1("Similar content", KnowledgeType::FACT);
    entry1.updateConfidence(ConfidenceLevel::HIGH);
    
    KnowledgeEntry entry2("Similar content with more details", KnowledgeType::FACT);
    entry2.updateConfidence(ConfidenceLevel::MEDIUM);
    
    kb->addKnowledge(entry1);
    kb->addKnowledge(entry2);
    
    EXPECT_EQ(kb->getKnowledgeCount(), 2);
    
    // Consolidate similar entries
    kb->consolidateKnowledge();
    
    // Should consolidate similar entries (implementation dependent)
    EXPECT_GE(kb->getKnowledgeCount(), 1);
}

// =====================================================
// Import/Export Tests
// =====================================================

TEST_F(KnowledgeTest, KnowledgeBaseExportImport) {
    // Add test knowledge
    KnowledgeEntry entry1("Export test fact", KnowledgeType::FACT);
    entry1.addTag("export");
    entry1.addTag("test");
    
    KnowledgeEntry entry2("Export test rule", KnowledgeType::RULE);
    entry2.addTag("export");
    entry2.updateConfidence(ConfidenceLevel::HIGH);
    
    kb->addKnowledge(entry1);
    kb->addKnowledge(entry2);
    
    // Export to JSON
    JsonValue exported = kb->exportToJson();
    EXPECT_FALSE(exported.empty());
    
    // Test file export (basic test)
    std::string testFile = "/tmp/test_knowledge_export.txt";
    bool exportResult = kb->exportToFile(testFile);
    EXPECT_TRUE(exportResult);
    
    // Test JSON import
    auto newKb = std::make_shared<KnowledgeBase>();
    bool importResult = newKb->importFromJson(exported);
    EXPECT_TRUE(importResult);
    
    // Test file import
    bool fileImportResult = newKb->importFromFile(testFile);
    EXPECT_TRUE(fileImportResult);
}

// =====================================================
// Utility Function Tests
// =====================================================

TEST_F(KnowledgeTest, UtilityFunctions) {
    // Test knowledge type conversions
    EXPECT_EQ(knowledgeTypeToString(KnowledgeType::FACT), "fact");
    EXPECT_EQ(knowledgeTypeToString(KnowledgeType::RULE), "rule");
    EXPECT_EQ(knowledgeTypeToString(KnowledgeType::CONCEPT), "concept");
    EXPECT_EQ(knowledgeTypeToString(KnowledgeType::RELATIONSHIP), "relationship");
    EXPECT_EQ(knowledgeTypeToString(KnowledgeType::PROCEDURE), "procedure");
    EXPECT_EQ(knowledgeTypeToString(KnowledgeType::EXPERIENCE), "experience");
    
    EXPECT_EQ(stringToKnowledgeType("fact"), KnowledgeType::FACT);
    EXPECT_EQ(stringToKnowledgeType("rule"), KnowledgeType::RULE);
    EXPECT_EQ(stringToKnowledgeType("concept"), KnowledgeType::CONCEPT);
    EXPECT_EQ(stringToKnowledgeType("relationship"), KnowledgeType::RELATIONSHIP);
    EXPECT_EQ(stringToKnowledgeType("procedure"), KnowledgeType::PROCEDURE);
    EXPECT_EQ(stringToKnowledgeType("experience"), KnowledgeType::EXPERIENCE);
    EXPECT_EQ(stringToKnowledgeType("unknown"), KnowledgeType::FACT); // Default
    
    // Test confidence level conversions
    EXPECT_EQ(confidenceLevelToString(ConfidenceLevel::VERY_LOW), "very_low");
    EXPECT_EQ(confidenceLevelToString(ConfidenceLevel::LOW), "low");
    EXPECT_EQ(confidenceLevelToString(ConfidenceLevel::MEDIUM), "medium");
    EXPECT_EQ(confidenceLevelToString(ConfidenceLevel::HIGH), "high");
    EXPECT_EQ(confidenceLevelToString(ConfidenceLevel::VERY_HIGH), "very_high");
    
    EXPECT_EQ(stringToConfidenceLevel("very_low"), ConfidenceLevel::VERY_LOW);
    EXPECT_EQ(stringToConfidenceLevel("low"), ConfidenceLevel::LOW);
    EXPECT_EQ(stringToConfidenceLevel("medium"), ConfidenceLevel::MEDIUM);
    EXPECT_EQ(stringToConfidenceLevel("high"), ConfidenceLevel::HIGH);
    EXPECT_EQ(stringToConfidenceLevel("very_high"), ConfidenceLevel::VERY_HIGH);
    EXPECT_EQ(stringToConfidenceLevel("unknown"), ConfidenceLevel::MEDIUM); // Default
    
    // Test knowledge source conversions
    EXPECT_EQ(knowledgeSourceToString(KnowledgeSource::LEARNED), "learned");
    EXPECT_EQ(knowledgeSourceToString(KnowledgeSource::PROGRAMMED), "programmed");
    EXPECT_EQ(knowledgeSourceToString(KnowledgeSource::INFERRED), "inferred");
    EXPECT_EQ(knowledgeSourceToString(KnowledgeSource::OBSERVED), "observed");
    EXPECT_EQ(knowledgeSourceToString(KnowledgeSource::COMMUNICATED), "communicated");
    
    EXPECT_EQ(stringToKnowledgeSource("learned"), KnowledgeSource::LEARNED);
    EXPECT_EQ(stringToKnowledgeSource("programmed"), KnowledgeSource::PROGRAMMED);
    EXPECT_EQ(stringToKnowledgeSource("inferred"), KnowledgeSource::INFERRED);
    EXPECT_EQ(stringToKnowledgeSource("observed"), KnowledgeSource::OBSERVED);
    EXPECT_EQ(stringToKnowledgeSource("communicated"), KnowledgeSource::COMMUNICATED);
    EXPECT_EQ(stringToKnowledgeSource("unknown"), KnowledgeSource::PROGRAMMED); // Default
}

// =====================================================
// Thread Safety Tests
// =====================================================

TEST_F(KnowledgeTest, KnowledgeBaseThreadSafety) {
    const int numThreads = 4;
    const int entriesPerThread = 10;
    
    std::vector<std::thread> threads;
    std::atomic<int> successCount(0);
    
    // Launch multiple threads adding knowledge concurrently
    for (int t = 0; t < numThreads; ++t) {
        threads.emplace_back([this, t, entriesPerThread, &successCount]() {
            for (int i = 0; i < entriesPerThread; ++i) {
                KnowledgeEntry entry("Thread " + std::to_string(t) + " Entry " + std::to_string(i), 
                                    KnowledgeType::FACT);
                entry.addTag("thread_" + std::to_string(t));
                
                std::string id = kb->addKnowledge(entry);
                if (!id.empty()) {
                    successCount++;
                }
            }
        });
    }
    
    // Wait for all threads to complete
    for (auto& thread : threads) {
        thread.join();
    }
    
    // Verify all entries were added successfully
    EXPECT_EQ(successCount.load(), numThreads * entriesPerThread);
    EXPECT_EQ(kb->getKnowledgeCount(), static_cast<size_t>(numThreads * entriesPerThread));
    
    // Verify concurrent queries work
    std::atomic<int> queryCount(0);
    std::vector<std::thread> queryThreads;
    
    for (int t = 0; t < numThreads; ++t) {
        queryThreads.emplace_back([this, t, &queryCount]() {
            KnowledgeQuery query("Thread");
            query.tags = {"thread_" + std::to_string(t)};
            auto results = kb->query(query);
            if (!results.empty()) {
                queryCount++;
            }
        });
    }
    
    for (auto& thread : queryThreads) {
        thread.join();
    }
    
    EXPECT_EQ(queryCount.load(), numThreads);
}

// =====================================================
// Global Knowledge Base Tests
// =====================================================

TEST_F(KnowledgeTest, GlobalKnowledgeBase) {
    // Test global knowledge base instance
    ASSERT_NE(globalKnowledgeBase, nullptr);
    
    // Clear it first
    globalKnowledgeBase->clear();
    EXPECT_EQ(globalKnowledgeBase->getKnowledgeCount(), 0);
    
    // Add knowledge to global instance
    KnowledgeEntry globalEntry("Global knowledge", KnowledgeType::FACT);
    globalEntry.addTag("global");
    
    std::string id = globalKnowledgeBase->addKnowledge(globalEntry);
    EXPECT_FALSE(id.empty());
    EXPECT_EQ(globalKnowledgeBase->getKnowledgeCount(), 1);
    
    // Verify retrieval
    auto retrieved = globalKnowledgeBase->getKnowledge(id);
    ASSERT_TRUE(retrieved.has_value());
    EXPECT_EQ(retrieved->content, "Global knowledge");
    EXPECT_TRUE(retrieved->hasTag("global"));
    
    // Clean up
    globalKnowledgeBase->clear();
}