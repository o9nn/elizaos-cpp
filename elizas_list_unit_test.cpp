#include "elizaos/elizas_list.hpp"
#include <iostream>
#include <cassert>

using namespace elizaos;

// Simple test assertion helper
void test_assert(bool condition, const std::string& test_name) {
    if (condition) {
        std::cout << "✓ " << test_name << " - PASSED" << std::endl;
    } else {
        std::cout << "✗ " << test_name << " - FAILED" << std::endl;
        exit(1);
    }
}

int main() {
    std::cout << "=== ElizaOS ElizasList Unit Tests ===" << std::endl;
    
    ElizasList elizasList;
    
    // Test initial state
    test_assert(elizasList.getProjectCount() == 0, "Initial project count is zero");
    test_assert(elizasList.getCollectionCount() == 0, "Initial collection count is zero");
    
    // Create test data
    Author author1{"TestUser", "https://github.com/testuser", "https://twitter.com/testuser"};
    Donation donation1{"0x123", "1000", "2024-01-01T00:00:00Z"};
    Metrics metrics1{100, 25};
    
    Project project1{
        "test-project-1",
        "Test Project One",
        "A test project for unit testing",
        "https://example.com/project1",
        "https://github.com/testuser/project1",
        "/images/project1.png",
        author1,
        donation1,
        {"Test", "Unit", "C++"},
        "2024-01-01T00:00:00Z",
        metrics1
    };
    
    // Test project addition
    test_assert(elizasList.addProject(project1), "Add project succeeds");
    test_assert(elizasList.getProjectCount() == 1, "Project count increases after add");
    
    // Test duplicate addition fails
    test_assert(!elizasList.addProject(project1), "Duplicate project addition fails");
    test_assert(elizasList.getProjectCount() == 1, "Project count unchanged after duplicate add");
    
    // Test project retrieval
    auto retrieved = elizasList.getProject("test-project-1");
    test_assert(retrieved.has_value(), "Project retrieval succeeds");
    test_assert(retrieved->name == "Test Project One", "Retrieved project has correct name");
    
    auto nonExistent = elizasList.getProject("non-existent");
    test_assert(!nonExistent.has_value(), "Non-existent project returns nullopt");
    
    // Test search by tag
    auto testProjects = elizasList.getProjectsByTag("Test");
    test_assert(testProjects.size() == 1, "Search by tag finds correct number");
    test_assert(testProjects[0].id == "test-project-1", "Search by tag finds correct project");
    
    // Test search by author
    auto authorProjects = elizasList.getProjectsByAuthor("https://github.com/testuser");
    test_assert(authorProjects.size() == 1, "Search by author finds correct number");
    
    // Test text search
    auto searchResults = elizasList.searchProjects("unit testing");
    test_assert(searchResults.size() == 1, "Text search finds project");
    
    // Test project update
    project1.description = "Updated description";
    test_assert(elizasList.updateProject(project1), "Project update succeeds");
    auto updated = elizasList.getProject("test-project-1");
    test_assert(updated->description == "Updated description", "Project update changes description");
    
    // Add second project for sorting tests
    Author author2{"AnotherUser", "https://github.com/another", std::nullopt};
    Donation donation2{"0x456", "500", "2024-02-01T00:00:00Z"};
    Metrics metrics2{200, 50}; // More stars than first project
    
    Project project2{
        "test-project-2",
        "Test Project Two",
        "Another test project",
        "https://example.com/project2",
        "https://github.com/another/project2",
        "/images/project2.png",
        author2,
        donation2,
        {"Test", "Another"},
        "2024-02-01T00:00:00Z",
        metrics2
    };
    
    elizasList.addProject(project2);
    test_assert(elizasList.getProjectCount() == 2, "Second project added successfully");
    
    // Test sorting by stars
    auto sortedProjects = elizasList.getProjectsSortedByStars();
    test_assert(sortedProjects.size() == 2, "Sorted list has correct size");
    test_assert(sortedProjects[0].id == "test-project-2", "First project has most stars");
    test_assert(sortedProjects[1].id == "test-project-1", "Second project has fewer stars");
    
    // Test recent projects
    auto recentProjects = elizasList.getRecentProjects(1);
    test_assert(recentProjects.size() == 1, "Recent projects limited correctly");
    test_assert(recentProjects[0].id == "test-project-2", "Most recent project returned");
    
    // Test collection functionality
    Curator curator1{"TestCurator", "https://github.com/curator"};
    Collection collection1{
        "test-collection",
        "Test Collection",
        "A collection for testing",
        {"test-project-1", "test-project-2"},
        curator1,
        true
    };
    
    test_assert(elizasList.addCollection(collection1), "Collection addition succeeds");
    test_assert(elizasList.getCollectionCount() == 1, "Collection count increases");
    
    auto featuredCollections = elizasList.getFeaturedCollections();
    test_assert(featuredCollections.size() == 1, "Featured collection found");
    test_assert(featuredCollections[0].name == "Test Collection", "Featured collection has correct name");
    
    // Test JSON export/import
    std::string jsonData = elizasList.exportProjectsToJson();
    test_assert(!jsonData.empty(), "JSON export produces data");
    
    ElizasList newElizasList;
    test_assert(newElizasList.loadProjectsFromJson(jsonData), "JSON import succeeds");
    test_assert(newElizasList.getProjectCount() == 2, "Imported list has correct project count");
    
    // Test tag aggregation
    auto allTags = elizasList.getAllTags();
    test_assert(allTags.size() >= 3, "All tags aggregated correctly"); // At least "Another", "C++", "Test", "Unit"
    
    // Test project removal
    test_assert(elizasList.removeProject("test-project-1"), "Project removal succeeds");
    test_assert(elizasList.getProjectCount() == 1, "Project count decreases after removal");
    test_assert(!elizasList.getProject("test-project-1").has_value(), "Removed project no longer retrievable");
    
    // Test collection removal
    test_assert(elizasList.removeCollection("test-collection"), "Collection removal succeeds");
    test_assert(elizasList.getCollectionCount() == 0, "Collection count decreases after removal");
    
    std::cout << "\n=== All Tests Passed! ✓ ===" << std::endl;
    std::cout << "ElizasList implementation is working correctly." << std::endl;
    
    return 0;
}