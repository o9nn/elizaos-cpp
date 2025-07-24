#include "elizaos/embodiment.hpp"
#include "elizaos/agentlogger.hpp"
#include <iostream>
#include <thread>
#include <chrono>

namespace elizaos {

// Helper functions for logging
[[maybe_unused]] static void elogInfo(const std::string& message) {
    AgentLogger logger;
    logger.log(message, "", "embodiment", LogLevel::INFO);
}

[[maybe_unused]] static void elogSuccess(const std::string& message) {
    AgentLogger logger;
    logger.log(message, "", "embodiment", LogLevel::SUCCESS);
}

[[maybe_unused]] static void elogError(const std::string& message) {
    AgentLogger logger;
    logger.log(message, "", "embodiment", LogLevel::ERROR);
}

[[maybe_unused]] static void elogSystem(const std::string& message) {
    AgentLogger logger;
    logger.log(message, "", "embodiment", LogLevel::SYSTEM);
}

[[maybe_unused]] static void elogWarning(const std::string& message) {
    AgentLogger logger;
    logger.log(message, "", "embodiment", LogLevel::WARNING);
}

/**
 * Console Text Input Interface Implementation
 */
ConsoleTextInput::ConsoleTextInput() {}

bool ConsoleTextInput::initialize() {
    elogInfo("Initializing Console Text Input Interface");
    
    if (active_) {
        return true; // Already initialized
    }
    
    active_ = true;
    
    // Start input thread for sensory input
    inputThread_ = std::make_unique<std::thread>(&ConsoleTextInput::inputThread, this);
    
    elogSuccess("Console Text Input Interface initialized");
    return true;
}

void ConsoleTextInput::shutdown() {
    if (!active_) {
        return;
    }
    
    elogInfo("Shutting down Console Text Input Interface");
    
    active_ = false;
    
    if (inputThread_ && inputThread_->joinable()) {
        inputThread_->join();
    }
    
    elogInfo("Console Text Input Interface shutdown complete");
}

std::shared_ptr<SensoryData> ConsoleTextInput::readData() {
    std::lock_guard<std::mutex> lock(bufferMutex_);
    
    if (inputBuffer_.empty()) {
        return nullptr;
    }
    
    std::string input = inputBuffer_.front();
    inputBuffer_.erase(inputBuffer_.begin());
    
    auto textData = std::make_shared<TextualData>(input);
    textData->source = "console";
    textData->confidence = 1.0; // Console input is always certain
    
    return textData;
}

std::vector<std::shared_ptr<SensoryData>> ConsoleTextInput::readDataBuffer(size_t maxItems) {
    std::vector<std::shared_ptr<SensoryData>> result;
    
    std::lock_guard<std::mutex> lock(bufferMutex_);
    
    size_t count = std::min(maxItems, inputBuffer_.size());
    for (size_t i = 0; i < count; ++i) {
        auto textData = std::make_shared<TextualData>(inputBuffer_[i]);
        textData->source = "console";
        textData->confidence = 1.0;
        result.push_back(textData);
    }
    
    // Remove processed items
    inputBuffer_.erase(inputBuffer_.begin(), inputBuffer_.begin() + count);
    
    return result;
}

bool ConsoleTextInput::hasData() const {
    std::lock_guard<std::mutex> lock(bufferMutex_);
    return !inputBuffer_.empty();
}

void ConsoleTextInput::setConfiguration(const std::unordered_map<std::string, std::string>& config) {
    std::lock_guard<std::mutex> lock(configMutex_);
    config_ = config;
}

std::unordered_map<std::string, std::string> ConsoleTextInput::getConfiguration() const {
    std::lock_guard<std::mutex> lock(configMutex_);
    return config_;
}

void ConsoleTextInput::setDataCallback(std::function<void(std::shared_ptr<SensoryData>)> callback) {
    dataCallback_ = callback;
}

void ConsoleTextInput::enableRealTimeProcessing(bool enable) {
    realTimeProcessing_ = enable;
}

void ConsoleTextInput::inputThread() {
    elogSystem("Console input thread started");
    
    std::cout << std::endl;
    std::cout << "=== ElizaOS Console Interface ===" << std::endl;
    std::cout << "Type messages to interact with the agent. Type 'quit' to exit." << std::endl;
    std::cout << std::endl;
    
    while (active_) {
        std::cout << "> ";
        std::cout.flush();
        
        std::string input;
        if (std::getline(std::cin, input)) {
            if (!active_) break; // Check if shutdown was requested
            
            if (input == "quit" || input == "exit") {
                elogInfo("User requested exit");
                break;
            }
            
            if (!input.empty()) {
                // Add to input buffer
                {
                    std::lock_guard<std::mutex> lock(bufferMutex_);
                    inputBuffer_.push_back(input);
                }
                
                // If real-time processing is enabled, call callback immediately
                if (realTimeProcessing_ && dataCallback_) {
                    auto textData = std::make_shared<TextualData>(input);
                    textData->source = "console";
                    textData->confidence = 1.0;
                    
                    try {
                        dataCallback_(textData);
                    } catch (const std::exception& e) {
                        elogError("Error in data callback: " + std::string(e.what()));
                    }
                }
            }
        } else {
            // EOF or error condition
            break;
        }
        
        // Small delay to prevent CPU spinning
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    
    elogSystem("Console input thread ended");
}

/**
 * Console Text Output Interface Implementation
 */
ConsoleTextOutput::ConsoleTextOutput() {}

bool ConsoleTextOutput::initialize() {
    elogInfo("Initializing Console Text Output Interface");
    
    if (active_) {
        return true; // Already initialized
    }
    
    active_ = true;
    elogSuccess("Console Text Output Interface initialized");
    return true;
}

void ConsoleTextOutput::shutdown() {
    if (!active_) {
        return;
    }
    
    elogInfo("Shutting down Console Text Output Interface");
    active_ = false;
    elogInfo("Console Text Output Interface shutdown complete");
}

bool ConsoleTextOutput::executeAction(std::shared_ptr<MotorAction> action) {
    if (!active_) {
        return false;
    }
    
    // Handle communication actions (text output)
    if (action->type == MotorActionType::COMMUNICATION) {
        auto commAction = std::dynamic_pointer_cast<CommunicationAction>(action);
        if (commAction) {
            AgentLogger logger;
            
            // Use panel display for agent responses
            logger.log(commAction->message, "", "Agent Response", LogLevel::INFO, LogColor::GREEN, true, true);
            
            return true;
        }
    }
    
    // Handle display actions
    if (action->type == MotorActionType::DISPLAY) {
        auto displayAction = std::dynamic_pointer_cast<DisplayAction>(action);
        if (displayAction) {
            std::cout << "[DISPLAY] " << displayAction->content << std::endl;
            return true;
        }
    }
    
    // Handle speech actions (as text output)
    if (action->type == MotorActionType::SPEECH) {
        auto speechAction = std::dynamic_pointer_cast<SpeechAction>(action);
        if (speechAction) {
            std::cout << "[SPEECH] " << speechAction->text << std::endl;
            return true;
        }
    }
    
    return false;
}

bool ConsoleTextOutput::canExecute(std::shared_ptr<MotorAction> action) const {
    if (!action) return false;
    
    // Console interface can handle communication, display, and speech actions
    return (action->type == MotorActionType::COMMUNICATION ||
            action->type == MotorActionType::DISPLAY ||
            action->type == MotorActionType::SPEECH);
}

void ConsoleTextOutput::stopAction(const std::string& actionId) {
    // Console actions are immediate, no stopping needed
    (void)actionId; // Suppress unused parameter warning
}

void ConsoleTextOutput::stopAllActions() {
    // Console actions are immediate, no stopping needed
}

bool ConsoleTextOutput::isActionComplete(const std::string& actionId) const {
    // Console actions complete immediately
    (void)actionId; // Suppress unused parameter warning
    return true;
}

std::vector<std::string> ConsoleTextOutput::getActiveActions() const {
    // Console actions don't remain active
    return {};
}

double ConsoleTextOutput::getActionProgress(const std::string& actionId) const {
    // Console actions complete immediately
    (void)actionId; // Suppress unused parameter warning
    return 1.0;
}

void ConsoleTextOutput::setConfiguration(const std::unordered_map<std::string, std::string>& config) {
    std::lock_guard<std::mutex> lock(configMutex_);
    config_ = config;
}

std::unordered_map<std::string, std::string> ConsoleTextOutput::getConfiguration() const {
    std::lock_guard<std::mutex> lock(configMutex_);
    return config_;
}

} // namespace elizaos