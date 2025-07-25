#include "elizaos/characters.hpp"
#include "elizaos/character_json_loader.hpp"
#include "elizaos/agentlogger.hpp"
#include <iostream>
#include <iomanip>
#include <thread>
#include <chrono>

using namespace elizaos;

void demonstrateCharacterCreation() {
    std::cout << "\n=== Character Creation Demo ===" << std::endl;
    
    // Create a custom character
    CharacterProfile scientist("Dr. Emma Watson", "A brilliant AI researcher");
    scientist.personality.openness = 0.9f;
    scientist.personality.curiosity = 0.95f;
    scientist.personality.conscientiousness = 0.8f;
    
    // Add traits
    CharacterTrait analyticalTrait("analytical", "Strong analytical thinking", 
                                  TraitCategory::COGNITIVE, TraitValueType::NUMERIC);
    analyticalTrait.setNumericValue(0.9f);
    scientist.addTrait(analyticalTrait);
    
    CharacterTrait creativeTrait("creative", "Creative problem solving", 
                                TraitCategory::PERSONALITY, TraitValueType::BOOLEAN);
    creativeTrait.setBooleanValue(true);
    scientist.addTrait(creativeTrait);
    
    // Set background
    scientist.background.occupation = "AI Researcher";
    scientist.background.experiences.push_back("Published 50+ papers on neural networks");
    scientist.background.experiences.push_back("Led breakthrough research in AGI");
    scientist.background.goals.push_back("Develop safe and beneficial AI");
    
    // Set communication style
    scientist.communicationStyle.tone = "professional";
    scientist.communicationStyle.vocabulary = "technical";
    scientist.communicationStyle.formality = 0.7f;
    scientist.communicationStyle.verbosity = 0.8f;
    
    std::cout << "Created character: " << scientist.name << std::endl;
    std::cout << "Description: " << scientist.description << std::endl;
    std::cout << "Personality Type: " << scientist.personality.getPersonalityType() << std::endl;
    std::cout << "Emotional State: " << scientist.getEmotionalState() << std::endl;
    std::cout << "Traits: " << scientist.traits.size() << std::endl;
    
    // Register with manager
    std::string scientistId = globalCharacterManager->registerCharacter(scientist);
    std::cout << "Registered with ID: " << scientistId << std::endl;
}

void demonstrateCharacterTemplates() {
    std::cout << "\n=== Character Templates Demo ===" << std::endl;
    
    // Register archetype templates
    globalCharacterManager->registerTemplate(CharacterArchetypes::createScientist());
    globalCharacterManager->registerTemplate(CharacterArchetypes::createArtist());
    globalCharacterManager->registerTemplate(CharacterArchetypes::createLeader());
    globalCharacterManager->registerTemplate(CharacterArchetypes::createHelper());
    
    std::cout << "Registered " << globalCharacterManager->getAllTemplates().size() << " templates" << std::endl;
    
    // Create characters from templates
    CharacterProfile aiScientist = globalCharacterManager->createFromTemplate("Scientist", "Dr. Alan Turing");
    CharacterProfile digitalArtist = globalCharacterManager->createFromTemplate("Artist", "Ada Lovelace");
    CharacterProfile teamLeader = globalCharacterManager->createFromTemplate("Leader", "Grace Hopper");
    CharacterProfile aiAssistant = globalCharacterManager->createFromTemplate("Helper", "ELIZA Assistant");
    
    // Register all characters
    globalCharacterManager->registerCharacter(aiScientist);
    globalCharacterManager->registerCharacter(digitalArtist);
    globalCharacterManager->registerCharacter(teamLeader);
    globalCharacterManager->registerCharacter(aiAssistant);
    
    std::cout << "Created 4 characters from templates" << std::endl;
    std::cout << "Total characters: " << globalCharacterManager->getCharacterCount() << std::endl;
}

void demonstrateCharacterInteractions() {
    std::cout << "\n=== Character Interactions Demo ===" << std::endl;
    
    auto allCharacters = globalCharacterManager->getAllCharacters();
    
    if (allCharacters.size() >= 2) {
        const auto& char1 = allCharacters[0];
        const auto& char2 = allCharacters[1];
        
        std::cout << "Analyzing interaction between " << char1.name << " and " << char2.name << std::endl;
        
        float compatibility = char1.calculateCompatibility(char2);
        std::cout << "Compatibility: " << compatibility << std::endl;
        
        std::string interactionStyle = char1.predictInteractionStyle(char2);
        std::cout << "Predicted interaction style: " << interactionStyle << std::endl;
        
        auto commonTraits = char1.findCommonTraits(char2);
        std::cout << "Common traits: ";
        for (const auto& trait : commonTraits) {
            std::cout << trait << " ";
        }
        std::cout << std::endl;
        
        // Generate sample responses
        std::string response1 = char1.generateResponse("What do you think about AI development?", "research discussion");
        std::string response2 = char2.generateResponse("How can we collaborate effectively?", "team planning");
        
        std::cout << "\nSample responses:" << std::endl;
        std::cout << char1.name << ": " << response1 << std::endl;
        std::cout << char2.name << ": " << response2 << std::endl;
    }
}

void demonstrateCharacterSearch() {
    std::cout << "\n=== Character Search Demo ===" << std::endl;
    
    // Search by name/description
    auto searchResults = globalCharacterManager->searchCharacters("scientist");
    std::cout << "Search for 'scientist': " << searchResults.size() << " results" << std::endl;
    
    // Find compatible characters
    auto allCharacters = globalCharacterManager->getAllCharacters();
    if (!allCharacters.empty()) {
        auto compatible = globalCharacterManager->findCompatibleCharacters(allCharacters[0].id, 0.6f);
        std::cout << "Compatible characters with " << allCharacters[0].name << ": " << compatible.size() << std::endl;
    }
    
    // Get analytics
    std::string analytics = globalCharacterManager->getCharacterAnalytics();
    std::cout << "\nCharacter Analytics:" << std::endl;
    std::cout << analytics << std::endl;
}

void demonstrateJsonLoading() {
    std::cout << "\n=== JSON Character Loading Demo ===" << std::endl;
    
    // Try to load characters from the /characters directory
    std::string charactersDir = "/home/runner/work/elizaos-cpp/elizaos-cpp/characters";
    
    auto loadedCharacters = CharacterJsonLoader::loadFromDirectory(charactersDir);
    std::cout << "Loaded " << loadedCharacters.size() << " characters from JSON files" << std::endl;
    
    // Register loaded characters
    int registered = 0;
    for (const auto& character : loadedCharacters) {
        std::string id = globalCharacterManager->registerCharacter(character);
        if (!id.empty()) {
            registered++;
            std::cout << "Loaded: " << character.name << " (ID: " << id.substr(0, 8) << "...)" << std::endl;
        }
    }
    
    std::cout << "Successfully registered " << registered << " characters from JSON files" << std::endl;
    std::cout << "Total characters now: " << globalCharacterManager->getCharacterCount() << std::endl;
    
    // Show a few loaded characters
    if (registered > 0) {
        auto allChars = globalCharacterManager->getAllCharacters();
        std::cout << "\nSample loaded characters:" << std::endl;
        int shown = 0;
        for (const auto& character : allChars) {
            if (shown >= 3) break; // Show max 3
            if (character.traits.size() > 0) { // Only show characters with traits (likely loaded from JSON)
                std::cout << "- " << character.name << ": " << character.description.substr(0, 50) << "..." << std::endl;
                std::cout << "  Traits: " << character.traits.size() << ", Experiences: " << character.background.experiences.size() << std::endl;
                shown++;
            }
        }
    }
}

void demonstrateCharacterEvolution() {
    std::cout << "\n=== Character Evolution Demo ===" << std::endl;
    
    auto allCharacters = globalCharacterManager->getAllCharacters();
    if (!allCharacters.empty()) {
        const auto& character = allCharacters[0];
        
        std::cout << "Character before evolution:" << std::endl;
        std::cout << "- Openness: " << character.personality.openness << std::endl;
        std::cout << "- Extraversion: " << character.personality.extraversion << std::endl;
        std::cout << "- Emotional State: " << character.getEmotionalState() << std::endl;
        
        // Simulate character learning from interactions
        auto characterCopy = character;
        characterCopy.learnFromInteraction("Had a successful presentation", "positive");
        characterCopy.learnFromInteraction("Collaborated well with team", "positive");
        characterCopy.learnFromInteraction("Received constructive feedback", "positive");
        
        std::cout << "\nAfter positive interactions:" << std::endl;
        std::cout << "- Openness: " << characterCopy.personality.openness << std::endl;
        std::cout << "- Extraversion: " << characterCopy.personality.extraversion << std::endl;
        std::cout << "- Emotional State: " << characterCopy.getEmotionalState() << std::endl;
        std::cout << "- Experiences: " << characterCopy.background.experiences.size() << std::endl;
        
        // Simulate time-based evolution
        characterCopy.evolvePersonality(30.0f); // 30 days
        
        std::cout << "\nAfter 30 days of evolution:" << std::endl;
        std::cout << "- Openness: " << characterCopy.personality.openness << std::endl;
        std::cout << "- Extraversion: " << characterCopy.personality.extraversion << std::endl;
        std::cout << "- Emotional State: " << characterCopy.getEmotionalState() << std::endl;
    }
}

void demonstrateCharacterCompatibility() {
    std::cout << "\n=== Character Compatibility Analysis ===" << std::endl;
    
    auto allCharacters = globalCharacterManager->getAllCharacters();
    
    if (allCharacters.size() >= 3) {
        // Analyze compatibility between different character pairs
        std::cout << "Compatibility Matrix (top 3 characters):" << std::endl;
        std::cout << "Character\\Character";
        for (size_t i = 0; i < 3 && i < allCharacters.size(); i++) {
            std::cout << "\\t" << allCharacters[i].name.substr(0, 8);
        }
        std::cout << std::endl;
        
        for (size_t i = 0; i < 3 && i < allCharacters.size(); i++) {
            std::cout << allCharacters[i].name.substr(0, 12);
            for (size_t j = 0; j < 3 && j < allCharacters.size(); j++) {
                if (i == j) {
                    std::cout << "\\t1.00";
                } else {
                    float compatibility = allCharacters[i].calculateCompatibility(allCharacters[j]);
                    std::cout << "\\t" << std::fixed << std::setprecision(2) << compatibility;
                }
            }
            std::cout << std::endl;
        }
        
        // Find best matches
        auto bestMatches = globalCharacterManager->findBestMatches();
        std::cout << "\nBest character matches (>0.8 compatibility):" << std::endl;
        for (const auto& match : bestMatches) {
            std::cout << "- " << match.first << " & " << match.second << std::endl;
        }
    }
}

int main() {
    std::cout << "ElizaOS C++ Characters System Demo" << std::endl;
    std::cout << "=================================" << std::endl;
    
    // Initialize logger
    auto logger = std::make_shared<AgentLogger>();
    logger->setConsoleEnabled(true);
    
    try {
        // Clear any existing characters
        globalCharacterManager->clear();
        
        // Run demonstrations
        demonstrateCharacterCreation();
        demonstrateCharacterTemplates();
        demonstrateJsonLoading();
        demonstrateCharacterInteractions();
        demonstrateCharacterSearch();
        demonstrateCharacterEvolution();
        demonstrateCharacterCompatibility();
        
        std::cout << "\n=== Final Statistics ===" << std::endl;
        std::cout << "Total characters: " << globalCharacterManager->getCharacterCount() << std::endl;
        std::cout << "Total templates: " << globalCharacterManager->getAllTemplates().size() << std::endl;
        
        auto stats = globalCharacterManager->getTraitCategoryStats();
        std::cout << "Trait category distribution:" << std::endl;
        for (const auto& [category, count] : stats) {
            std::cout << "- " << traitCategoryToString(category) << ": " << count << std::endl;
        }
        
        std::cout << "\nDemo completed successfully!" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "Error during demo: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}