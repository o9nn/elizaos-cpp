#include "elizaos/eliza_3d_hyperfy_starter.hpp"
#include "elizaos/agentlogger.hpp"
#include <iostream>
#include <thread>
#include <chrono>

using namespace elizaos::hyperfy;

int main() {
    std::cout << "=== ElizaOS C++ 3D Hyperfy Starter Demo ===" << std::endl;
    
    // Create a configuration
    HyperfyConfig config;
    config.wsUrl = "wss://chill.hyperfy.xyz/ws";
    config.worldId = "demo-world-123";
    config.connectionTimeoutMs = 5000;
    
    try {
        // Create a Hyperfy service using the factory
        auto service = HyperfyServiceFactory::createService();
        
        std::cout << "Created HyperfyService successfully" << std::endl;
        
        // Start the service
        if (service->start(config)) {
            std::cout << "HyperfyService started successfully" << std::endl;
            
            // Connect to a world
            if (service->connectToWorld("demo-world", config.wsUrl)) {
                std::cout << "Connected to Hyperfy world successfully" << std::endl;
                
                // Test some actions
                std::cout << "\nTesting actions:" << std::endl;
                
                // Test goto action
                std::cout << "Testing goto action..." << std::endl;
                service->executeAction("goto", "10.5,20.0,30.2");
                
                // Test perception action
                std::cout << "Testing perception action..." << std::endl;
                service->executeAction("perception", "");
                
                // Test reply action
                std::cout << "Testing reply action..." << std::endl;
                service->executeAction("reply", "Hello from the C++ Hyperfy starter!");
                
                // Let the service run for a short time
                std::this_thread::sleep_for(std::chrono::seconds(2));
                
                std::cout << "\nDemo completed successfully!" << std::endl;
                
            } else {
                std::cout << "Failed to connect to Hyperfy world" << std::endl;
            }
            
            // Stop the service
            service->stop();
            std::cout << "HyperfyService stopped" << std::endl;
            
        } else {
            std::cout << "Failed to start HyperfyService" << std::endl;
        }
        
    } catch (const std::exception& e) {
        std::cerr << "Exception occurred: " << e.what() << std::endl;
        return 1;
    }
    
    std::cout << "\nDemo finished." << std::endl;
    return 0;
}