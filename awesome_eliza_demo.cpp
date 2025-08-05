#include <iostream>
#include <iomanip>
#include "elizaos/awesome_eliza.hpp"

void printSeparator(const std::string& title) {
    std::cout << "\n" << std::string(60, '=') << std::endl;
    std::cout << "  " << title << std::endl;
    std::cout << std::string(60, '=') << std::endl;
}

void printResources(const std::vector<elizaos::AwesomeResource>& resources, const std::string& title) {
    printSeparator(title);
    
    if (resources.empty()) {
        std::cout << "No resources found." << std::endl;
        return;
    }
    
    for (size_t i = 0; i < resources.size(); ++i) {
        const auto& resource = resources[i];
        std::cout << "[" << std::setw(2) << (i + 1) << "] " << resource.name << std::endl;
        std::cout << "     URL: " << resource.url << std::endl;
        std::cout << "     Description: " << resource.description << std::endl;
        if (!resource.subcategory.empty()) {
            std::cout << "     Subcategory: " << resource.subcategory << std::endl;
        }
        std::cout << std::endl;
    }
}

int main() {
    std::cout << "🌟 Awesome Eliza C++ Demo 🌟" << std::endl;
    std::cout << "This demo shows the awesome eliza resource management capabilities" << std::endl;
    
    // Create the awesome eliza manager
    elizaos::AwesomeElizaManager manager;
    
    // Show overview statistics
    printSeparator("Resource Overview");
    std::cout << "Total resources: " << manager.getResourceCount() << std::endl;
    
    auto counts = manager.getCategoryCounts();
    std::cout << "\nBreakdown by category:" << std::endl;
    for (const auto& pair : counts) {
        std::cout << "  " << std::setw(15) << std::left << pair.first << ": " << pair.second << " resources" << std::endl;
    }
    
    // Show official resources
    auto official = manager.getOfficialResources();
    printResources(official, "Official Developer Resources");
    
    // Show some tutorials (limit to first 3)
    auto tutorials = manager.getTutorials();
    if (tutorials.size() > 3) {
        tutorials.resize(3);
    }
    printResources(tutorials, "Tutorials (First 3)");
    
    // Show blockchain plugins
    auto blockchain_plugins = manager.getBlockchainPlugins();
    if (blockchain_plugins.size() > 5) {
        blockchain_plugins.resize(5);
    }
    printResources(blockchain_plugins, "Blockchain Plugins (First 5)");
    
    // Show some clients
    auto clients = manager.getClients();
    if (clients.size() > 3) {
        clients.resize(3);
    }
    printResources(clients, "Client Integrations (First 3)");
    
    // Demonstrate search functionality
    printSeparator("Search Functionality Demo");
    
    std::cout << "Searching for 'GitHub'..." << std::endl;
    auto github_results = manager.searchResources("github");
    std::cout << "Found " << github_results.size() << " resources:" << std::endl;
    for (const auto& resource : github_results) {
        std::cout << "  - " << resource.name << " (" << resource.category << ")" << std::endl;
    }
    
    std::cout << "\nSearching for 'Solana'..." << std::endl;
    auto solana_results = manager.searchResources("solana");
    std::cout << "Found " << solana_results.size() << " resources:" << std::endl;
    for (const auto& resource : solana_results) {
        std::cout << "  - " << resource.name << " (" << resource.category;
        if (!resource.subcategory.empty()) {
            std::cout << " - " << resource.subcategory;
        }
        std::cout << ")" << std::endl;
    }
    
    std::cout << "\n" << std::string(60, '=') << std::endl;
    std::cout << "✨ Demo complete! The awesome eliza ecosystem is at your fingertips! ✨" << std::endl;
    std::cout << std::string(60, '=') << std::endl;
    
    return 0;
}