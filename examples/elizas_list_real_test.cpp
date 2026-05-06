#include "elizaos/elizas_list.hpp"
#include <iostream>
#include <fstream>

using namespace elizaos;

int main() {
    std::cout << "=== ElizaOS ElizasList Real Data Test ===" << std::endl;
    
    ElizasList elizasList;
    
    // Try to load the actual projects.json from the elizas-list directory
    std::string projectsJsonPath = "/home/runner/work/elizaos-cpp/elizaos-cpp/elizas-list/src/data/projects.json";
    
    if (elizasList.loadFromJson(projectsJsonPath)) {
        std::cout << "Successfully loaded real data from projects.json" << std::endl;
        
        std::cout << "Total projects loaded: " << elizasList.getProjectCount() << std::endl;
        std::cout << "Total collections loaded: " << elizasList.getCollectionCount() << std::endl;
        
        // Display all projects
        auto projects = elizasList.getAllProjects();
        std::cout << "\n=== Loaded Projects ===" << std::endl;
        for (const auto& project : projects) {
            std::cout << "- " << project.name << " by " << project.author.name << std::endl;
            std::cout << "  GitHub: " << project.github << std::endl;
            std::cout << "  Tags: ";
            for (const auto& tag : project.tags) {
                std::cout << tag << " ";
            }
            std::cout << std::endl;
            if (project.metrics.has_value()) {
                std::cout << "  Stars: " << project.metrics->stars << ", Forks: " << project.metrics->forks << std::endl;
            }
            std::cout << std::endl;
        }
        
        // Show all available tags
        auto tags = elizasList.getAllTags();
        std::cout << "=== Available Tags ===" << std::endl;
        for (const auto& tag : tags) {
            std::cout << tag << " ";
        }
        std::cout << std::endl;
        
        // Test search functionality with real data
        std::cout << "\n=== Search Tests ===" << std::endl;
        auto aiProjects = elizasList.getProjectsByTag("AI");
        std::cout << "Projects with 'AI' tag: " << aiProjects.size() << std::endl;
        
        auto searchResults = elizasList.searchProjects("eliza");
        std::cout << "Search results for 'eliza': " << searchResults.size() << std::endl;
        
        auto sortedProjects = elizasList.getProjectsSortedByStars();
        std::cout << "Top project by stars: ";
        if (!sortedProjects.empty()) {
            int stars = sortedProjects[0].metrics ? sortedProjects[0].metrics->stars : 0;
            std::cout << sortedProjects[0].name << " (" << stars << " stars)";
        }
        std::cout << std::endl;
        
    } else {
        std::cout << "Failed to load projects.json - this is expected if the file format differs" << std::endl;
        
        // Try to manually check what's in the file
        std::ifstream file(projectsJsonPath);
        if (file.is_open()) {
            std::string firstLine;
            std::getline(file, firstLine);
            std::cout << "First line of projects.json: " << firstLine << std::endl;
            file.close();
        } else {
            std::cout << "Could not open projects.json file" << std::endl;
        }
    }
    
    std::cout << "\n=== Real Data Test Complete ===" << std::endl;
    return 0;
}