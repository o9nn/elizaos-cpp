#include "elizaos/eliza.hpp"
#include "elizaos/agentlogger.hpp"
#include <iostream>
#include <string>
#include <memory>

using namespace elizaos;

void printWelcome() {
    std::cout << "\n";
    std::cout << "┌─────────────────────────────────────┐\n";
    std::cout << "│          ElizaOS C++                │\n";
    std::cout << "│    Conversational AI Agent          │\n";
    std::cout << "└─────────────────────────────────────┘\n";
    std::cout << "\n";
    std::cout << "Welcome! I'm Eliza, your conversational AI assistant.\n";
    std::cout << "Type 'help' for commands, or just start chatting.\n";
    std::cout << "Type 'quit' or 'exit' to end our conversation.\n";
    std::cout << "\n";
}

void printHelp() {
    std::cout << "\nEliza Commands:\n";
    std::cout << "  help     - Show this help message\n";
    std::cout << "  clear    - Clear conversation history\n";
    std::cout << "  stats    - Show conversation statistics\n";
    std::cout << "  quit     - Exit the program\n";
    std::cout << "  exit     - Exit the program\n";
    std::cout << "\nOr just type naturally to chat with me!\n\n";
}

int main(int argc, char* argv[]) {
    // Parse command line arguments
    bool showHelp = false;
    bool verbose = false;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "--help" || arg == "-h") {
            showHelp = true;
        } else if (arg == "--verbose" || arg == "-v") {
            verbose = true;
        }
    }
    
    if (showHelp) {
        std::cout << "ElizaOS C++ - Conversational AI Agent\n\n";
        std::cout << "Usage: " << argv[0] << " [options]\n\n";
        std::cout << "Options:\n";
        std::cout << "  -h, --help     Show this help message\n";
        std::cout << "  -v, --verbose  Enable verbose logging\n";
        std::cout << "\n";
        std::cout << "Examples:\n";
        std::cout << "  " << argv[0] << "              # Start interactive chat\n";
        std::cout << "  " << argv[0] << " --verbose    # Start with detailed logging\n";
        return 0;
    }
    
    try {
        // Initialize logger (verbose mode just enables more detailed output)
        if (verbose) {
            globalLogger->log("Verbose mode enabled", "eliza-main", "Eliza CLI", LogLevel::INFO);
        }
        
        // Create Eliza core instance
        auto eliza = std::make_shared<ElizaCore>();
        
        // Enable features
        eliza->enableEmotionalTracking(true);
        eliza->enableKnowledgeIntegration(true);
        
        // Create a session for this interaction
        std::string userId = "user";
        std::string sessionId = eliza->createSession(userId);
        
        printWelcome();
        
        std::string input;
        bool running = true;
        
        while (running) {
            std::cout << "You: ";
            std::getline(std::cin, input);
            
            // Handle empty input
            if (input.empty()) {
                continue;
            }
            
            // Handle special commands
            if (input == "quit" || input == "exit") {
                running = false;
                std::cout << "\nEliza: Goodbye! It was nice talking with you.\n";
                break;
            } else if (input == "help") {
                printHelp();
                continue;
            } else if (input == "clear") {
                // End current session and start new one
                eliza->endSession(sessionId);
                sessionId = eliza->createSession(userId);
                std::cout << "\nEliza: I've cleared our conversation history. Let's start fresh!\n\n";
                continue;
            } else if (input == "stats") {
                auto session = eliza->getSession(sessionId);
                if (session) {
                    std::cout << "\nConversation Statistics:\n";
                    std::cout << "  Session ID: " << session->sessionId << "\n";
                    std::cout << "  Turn count: " << session->history.size() << "\n";
                    
                    auto duration = std::chrono::duration_cast<std::chrono::minutes>(
                        session->lastActivity - session->startTime
                    ).count();
                    std::cout << "  Duration: " << duration << " minutes\n";
                }
                std::cout << "\n" << eliza->getConversationAnalytics() << "\n";
                continue;
            }
            
            // Process input with Eliza
            try {
                std::string response = eliza->processInput(input, sessionId, userId);
                std::cout << "Eliza: " << response << "\n\n";
            } catch (const std::exception& e) {
                std::cout << "Eliza: I'm sorry, I encountered an error: " << e.what() << "\n\n";
                if (verbose) {
                    globalLogger->log("Error processing input: " + std::string(e.what()), "error", "eliza-main");
                }
            }
        }
        
        // Clean up session
        eliza->endSession(sessionId);
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}