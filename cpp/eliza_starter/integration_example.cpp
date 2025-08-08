/**
 * Integration Example: Simple Chat Server using ElizaOS C++ Starter
 * 
 * This example demonstrates how the ElizaOS C++ Starter could be integrated
 * into a larger application, such as a simple chat server.
 * 
 * Note: This is a demonstration example and would need additional 
 * networking code for a real chat server implementation.
 */

#include "elizaos/eliza_starter.hpp"
#include <iostream>
#include <thread>
#include <chrono>
#include <vector>
#include <string>
#include <map>

class SimpleChatServer {
private:
    std::unique_ptr<elizaos::ElizaStarterAgent> agent_;
    std::map<std::string, std::vector<std::string>> userSessions_;
    bool running_;

public:
    SimpleChatServer() : running_(false) {
        // Create a specialized chat agent
        agent_ = elizaos::createElizaStarterAgent("ChatServer", "chat-server-001");
        
        // Configure for customer service
        agent_->setCharacter(
            "ChatBot Assistant",
            "A helpful customer service agent for online chat support",
            "Designed to provide friendly and efficient customer service"
        );
        
        // Add customer service personality traits
        agent_->addPersonalityTrait("professionalism", "Maintains professional demeanor", 0.95);
        agent_->addPersonalityTrait("patience", "Patient with all customer inquiries", 0.9);
        agent_->addPersonalityTrait("helpfulness", "Eager to solve customer problems", 0.92);
        agent_->addPersonalityTrait("empathy", "Understanding of customer needs", 0.85);
    }

    bool start() {
        std::cout << "Starting ChatBot Server..." << std::endl;
        
        if (!agent_->initialize()) {
            std::cerr << "Failed to initialize chat agent!" << std::endl;
            return false;
        }
        
        agent_->start();
        running_ = true;
        
        std::cout << "ChatBot Server is now running!" << std::endl;
        return true;
    }

    void stop() {
        if (running_) {
            std::cout << "Stopping ChatBot Server..." << std::endl;
            agent_->stop();
            running_ = false;
        }
    }

    std::string handleUserMessage(const std::string& userId, const std::string& message) {
        if (!running_) {
            return "Server is not running";
        }

        // Store user session
        userSessions_[userId].push_back(message);

        // Process with agent
        std::string response = agent_->processMessage(message, userId);

        // Store agent response
        userSessions_[userId].push_back("Agent: " + response);

        return response;
    }

    void showUserSession(const std::string& userId) {
        std::cout << "\n=== Session History for " << userId << " ===" << std::endl;
        
        if (userSessions_.find(userId) != userSessions_.end()) {
            for (const auto& msg : userSessions_[userId]) {
                std::cout << msg << std::endl;
            }
        } else {
            std::cout << "No session found for user: " << userId << std::endl;
        }
        std::cout << "=== End Session ===" << std::endl << std::endl;
    }

    void showServerStats() {
        std::cout << "\n=== Server Statistics ===" << std::endl;
        std::cout << "Active users: " << userSessions_.size() << std::endl;
        std::cout << "Agent status: " << (agent_->isRunning() ? "Running" : "Stopped") << std::endl;
        
        auto memories = agent_->getRecentMemories(10);
        std::cout << "Recent memories: " << memories.size() << std::endl;
        std::cout << "========================" << std::endl << std::endl;
    }
};

int main() {
    std::cout << "ElizaOS C++ Starter Integration Example" << std::endl;
    std::cout << "Simple Chat Server Demonstration" << std::endl;
    std::cout << "=================================" << std::endl << std::endl;

    SimpleChatServer server;
    
    if (!server.start()) {
        return 1;
    }

    // Simulate multiple users chatting
    std::cout << "Simulating customer interactions..." << std::endl << std::endl;

    struct ChatInteraction {
        std::string userId;
        std::string message;
    };

    std::vector<ChatInteraction> interactions = {
        {"customer_001", "Hi, I need help with my order"},
        {"customer_002", "Hello! I have a question about your services"},
        {"customer_001", "My order number is #12345 and it hasn't arrived"},
        {"customer_003", "Good morning! Can you tell me about your return policy?"},
        {"customer_002", "I'm having trouble logging into my account"},
        {"customer_001", "Thank you for your help!"},
        {"customer_002", "That solved my problem. Thanks!"},
        {"customer_003", "Perfect, that answers my question."}
    };

    // Process each interaction
    for (const auto& interaction : interactions) {
        std::cout << "User " << interaction.userId << ": " << interaction.message << std::endl;
        
        std::string response = server.handleUserMessage(interaction.userId, interaction.message);
        std::cout << "Agent: " << response << std::endl << std::endl;
        
        // Small delay to simulate real conversation timing
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }

    // Show session histories
    std::cout << "\n" << std::string(60, '=') << std::endl;
    std::cout << "SESSION SUMMARIES" << std::endl;
    std::cout << std::string(60, '=') << std::endl;

    server.showUserSession("customer_001");
    server.showUserSession("customer_002");
    server.showUserSession("customer_003");

    // Show server statistics
    server.showServerStats();

    // Cleanup
    server.stop();
    
    std::cout << "Chat server demonstration completed!" << std::endl;
    std::cout << "\nThis example shows how the ElizaOS C++ Starter can be" << std::endl;
    std::cout << "integrated into real applications like chat servers," << std::endl;
    std::cout << "customer service systems, or interactive applications." << std::endl;

    return 0;
}