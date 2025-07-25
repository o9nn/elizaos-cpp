#include "elizaos/knowledge.hpp"
#include "elizaos/core.hpp"
#include "elizaos/agentlogger.hpp"
#include <iostream>
#include <thread>
#include <chrono>

using namespace elizaos;

void printSection(const std::string& title) {
    std::cout << "\n" << std::string(60, '=') << std::endl;
    std::cout << " " << title << std::endl;
    std::cout << std::string(60, '=') << std::endl;
}

void printKnowledgeEntry(const KnowledgeEntry& entry) {
    std::cout << "  ID: " << entry.id << std::endl;
    std::cout << "  Content: " << entry.content << std::endl;
    std::cout << "  Type: " << knowledgeTypeToString(entry.type) << std::endl;
    std::cout << "  Confidence: " << confidenceLevelToString(entry.confidence) << std::endl;
    std::cout << "  Source: " << knowledgeSourceToString(entry.source) << std::endl;
    if (!entry.tags.empty()) {
        std::cout << "  Tags: ";
        for (size_t i = 0; i < entry.tags.size(); ++i) {
            std::cout << entry.tags[i];
            if (i < entry.tags.size() - 1) std::cout << ", ";
        }
        std::cout << std::endl;
    }
    if (!entry.related_entries.empty()) {
        std::cout << "  Related: " << entry.related_entries.size() << " entries" << std::endl;
    }
    std::cout << std::endl;
}

int main() {
    std::cout << "ElizaOS C++ Knowledge Management System Demo" << std::endl;
    std::cout << "=============================================" << std::endl;
    
    // Create a knowledge base
    auto kb = std::make_shared<KnowledgeBase>();
    
    printSection("1. Creating Knowledge Entries");
    
    // Create various types of knowledge
    KnowledgeEntry fact1("The Earth orbits the Sun", KnowledgeType::FACT);
    fact1.addTag("astronomy");
    fact1.addTag("solar_system");
    fact1.updateConfidence(ConfidenceLevel::VERY_HIGH);
    
    KnowledgeEntry fact2("Python is a programming language", KnowledgeType::FACT);
    fact2.addTag("programming");
    fact2.addTag("language");
    fact2.updateConfidence(ConfidenceLevel::HIGH);
    
    KnowledgeEntry rule1("If it rains, then the ground gets wet", KnowledgeType::RULE);
    rule1.addTag("physics");
    rule1.addTag("weather");
    rule1.updateConfidence(ConfidenceLevel::HIGH);
    
    KnowledgeEntry concept1("Artificial Intelligence", KnowledgeType::CONCEPT);
    concept1.addTag("AI");
    concept1.addTag("technology");
    concept1.addTag("programming");
    concept1.updateConfidence(ConfidenceLevel::MEDIUM);
    
    KnowledgeEntry relationship1("Python is used in AI development", KnowledgeType::RELATIONSHIP);
    relationship1.addTag("programming");
    relationship1.addTag("AI");
    relationship1.updateConfidence(ConfidenceLevel::HIGH);
    
    // Add knowledge to the base
    std::string id1 = kb->addKnowledge(fact1);
    std::string id2 = kb->addKnowledge(fact2);
    std::string id3 = kb->addKnowledge(rule1);
    std::string id4 = kb->addKnowledge(concept1);
    std::string id5 = kb->addKnowledge(relationship1);
    
    // Create relationships between entries
    concept1.addRelation(id2); // AI relates to Python
    concept1.addRelation(id5); // AI relates to the relationship
    kb->updateKnowledge(id4, concept1);
    
    relationship1.addRelation(id2); // Relationship relates to Python fact
    relationship1.addRelation(id4); // Relationship relates to AI concept
    kb->updateKnowledge(id5, relationship1);
    
    std::cout << "Created " << kb->getKnowledgeCount() << " knowledge entries:" << std::endl;
    printKnowledgeEntry(fact1);
    printKnowledgeEntry(fact2);
    printKnowledgeEntry(rule1);
    printKnowledgeEntry(concept1);
    printKnowledgeEntry(relationship1);
    
    printSection("2. Knowledge Base Statistics");
    std::cout << kb->getStatistics() << std::endl;
    
    printSection("3. Text-based Search");
    std::cout << "Searching for 'programming':" << std::endl;
    auto searchResults = kb->searchByText("programming", 5);
    for (const auto& entry : searchResults) {
        std::cout << "  - " << entry.content << " (" << knowledgeTypeToString(entry.type) << ")" << std::endl;
    }
    
    std::cout << "\nSearching for 'AI':" << std::endl;
    auto aiResults = kb->searchByText("AI", 5);
    for (const auto& entry : aiResults) {
        std::cout << "  - " << entry.content << " (" << knowledgeTypeToString(entry.type) << ")" << std::endl;
    }
    
    printSection("4. Tag-based Search");
    std::cout << "Searching for entries with 'programming' tag:" << std::endl;
    auto tagResults = kb->searchByTags({"programming"}, 5);
    for (const auto& entry : tagResults) {
        std::cout << "  - " << entry.content << " (" << knowledgeTypeToString(entry.type) << ")" << std::endl;
    }
    
    std::cout << "\nAll available tags:" << std::endl;
    auto allTags = kb->getAllTags();
    std::cout << "  ";
    for (size_t i = 0; i < allTags.size(); ++i) {
        std::cout << allTags[i];
        if (i < allTags.size() - 1) std::cout << ", ";
    }
    std::cout << std::endl;
    
    printSection("5. Advanced Query with Filters");
    KnowledgeQuery complexQuery("programming");
    complexQuery.types = {KnowledgeType::FACT, KnowledgeType::CONCEPT};
    complexQuery.minConfidence = ConfidenceLevel::MEDIUM;
    complexQuery.includeRelated = true;
    complexQuery.maxResults = 10;
    
    auto complexResults = kb->query(complexQuery);
    std::cout << "Query for programming-related facts and concepts (medium+ confidence, with related):" << std::endl;
    for (const auto& entry : complexResults) {
        std::cout << "  - " << entry.content << " (" << knowledgeTypeToString(entry.type) 
                  << ", " << confidenceLevelToString(entry.confidence) << ")" << std::endl;
    }
    
    printSection("6. Knowledge by Type");
    auto typeStats = kb->getKnowledgeTypeStats();
    for (const auto& pair : typeStats) {
        std::cout << knowledgeTypeToString(pair.first) << ": " << pair.second << " entries" << std::endl;
        
        auto entriesOfType = kb->getKnowledgeByType(pair.first);
        for (const auto& entry : entriesOfType) {
            std::cout << "  - " << entry.content << std::endl;
        }
        std::cout << std::endl;
    }
    
    printSection("7. Related Knowledge Discovery");
    std::cout << "Finding knowledge related to AI concept:" << std::endl;
    auto relatedEntries = kb->getRelatedKnowledge(id4, 5);
    for (const auto& entry : relatedEntries) {
        std::cout << "  - " << entry.content << " (" << knowledgeTypeToString(entry.type) << ")" << std::endl;
    }
    
    printSection("8. Knowledge Inference");
    std::cout << "Performing inference on programming-related knowledge:" << std::endl;
    KnowledgeQuery inferenceQuery("programming");
    auto inferredKnowledge = kb->performInference(inferenceQuery);
    std::cout << "Generated " << inferredKnowledge.size() << " inferred knowledge entries:" << std::endl;
    for (const auto& entry : inferredKnowledge) {
        std::cout << "  - " << entry.content << " (" << knowledgeSourceToString(entry.source) << ")" << std::endl;
    }
    
    printSection("9. Knowledge Serialization");
    std::cout << "Exporting knowledge to JSON format:" << std::endl;
    JsonValue exported = kb->exportToJson();
    std::cout << "Export contains " << exported.size() << " fields" << std::endl;
    
    // Test file export
    std::string exportFile = "/tmp/knowledge_demo_export.txt";
    bool exportSuccess = kb->exportToFile(exportFile);
    std::cout << "File export to " << exportFile << ": " 
              << (exportSuccess ? "SUCCESS" : "FAILED") << std::endl;
    
    printSection("10. Knowledge Base Maintenance");
    std::cout << "Running knowledge validation..." << std::endl;
    kb->validateKnowledge();
    
    std::cout << "Before consolidation: " << kb->getKnowledgeCount() << " entries" << std::endl;
    kb->consolidateKnowledge();
    std::cout << "After consolidation: " << kb->getKnowledgeCount() << " entries" << std::endl;
    
    printSection("11. Real-time Knowledge Updates");
    std::cout << "Adding new knowledge about machine learning..." << std::endl;
    
    KnowledgeEntry ml_fact("Machine learning is a subset of AI", KnowledgeType::FACT);
    ml_fact.addTag("AI");
    ml_fact.addTag("machine_learning");
    ml_fact.addTag("technology");
    ml_fact.updateConfidence(ConfidenceLevel::HIGH);
    ml_fact.addRelation(id4); // Relate to AI concept
    
    std::string mlId = kb->addKnowledge(ml_fact);
    std::cout << "Added new knowledge with ID: " << mlId << std::endl;
    
    // Update the AI concept to include this relationship
    auto aiConcept = kb->getKnowledge(id4);
    if (aiConcept) {
        aiConcept->addRelation(mlId);
        kb->updateKnowledge(id4, *aiConcept);
        std::cout << "Updated AI concept to include machine learning relationship" << std::endl;
    }
    
    // Show updated related knowledge
    std::cout << "\nUpdated related knowledge for AI concept:" << std::endl;
    auto updatedRelated = kb->getRelatedKnowledge(id4, 10);
    for (const auto& entry : updatedRelated) {
        std::cout << "  - " << entry.content << std::endl;
    }
    
    printSection("Demo Complete!");
    std::cout << "Final knowledge base statistics:" << std::endl;
    std::cout << kb->getStatistics() << std::endl;
    
    std::cout << "\nThe ElizaOS Knowledge Management System successfully demonstrated:" << std::endl;
    std::cout << "  ✓ Creating and managing different types of knowledge" << std::endl;
    std::cout << "  ✓ Text and tag-based search capabilities" << std::endl;
    std::cout << "  ✓ Advanced filtering and querying" << std::endl;
    std::cout << "  ✓ Knowledge relationships and discovery" << std::endl;
    std::cout << "  ✓ Automated inference generation" << std::endl;
    std::cout << "  ✓ Serialization and data export" << std::endl;
    std::cout << "  ✓ Knowledge base maintenance operations" << std::endl;
    std::cout << "  ✓ Real-time knowledge updates and relationship management" << std::endl;
    
    std::cout << "\nKnowledge base ready for production use!" << std::endl;
    
    return 0;
}