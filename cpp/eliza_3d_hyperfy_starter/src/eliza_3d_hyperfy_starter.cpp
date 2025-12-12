#include "elizaos/eliza_3d_hyperfy_starter.hpp"
#include "elizaos/agentlogger.hpp"
#include <sstream>
#include <chrono>
#include <iostream>

namespace elizaos {
    namespace hyperfy {
        
        // HyperfyWorld implementation
        HyperfyWorld::HyperfyWorld(const std::string& worldId, const std::string& wsUrl) 
            : worldId_(worldId), wsUrl_(wsUrl), connected_(false) {
        }
        
        HyperfyWorld::~HyperfyWorld() {
            disconnect();
        }
        
        bool HyperfyWorld::connect(const std::string& authToken) {
            (void)authToken; // Suppress unused parameter warning
            std::lock_guard<std::mutex> lock(worldMutex_);
            
            if (connected_.load()) {
                return true;
            }
            
            // TODO: Implement actual WebSocket connection to Hyperfy world
            // For now, simulate connection
            logInfo("Connecting to Hyperfy world: " + worldId_ + " at " + wsUrl_, "HyperfyWorld");
            
            // Simulate connection delay
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            
            connected_.store(true);
            logInfo("Successfully connected to Hyperfy world: " + worldId_, "HyperfyWorld");
            
            return true;
        }
        
        void HyperfyWorld::disconnect() {
            std::lock_guard<std::mutex> lock(worldMutex_);
            
            if (!connected_.load()) {
                return;
            }
            
            logInfo("Disconnecting from Hyperfy world: " + worldId_, "HyperfyWorld");
            connected_.store(false);
            worldState_.clear();
            
            logInfo("Disconnected from Hyperfy world: " + worldId_, "HyperfyWorld");
        }
        
        bool HyperfyWorld::isConnected() const {
            return connected_.load();
        }
        
        void HyperfyWorld::updateState(const std::string& key, const std::string& value) {
            std::lock_guard<std::mutex> lock(worldMutex_);
            worldState_[key] = value;
        }
        
        std::string HyperfyWorld::getState(const std::string& key) const {
            std::lock_guard<std::mutex> lock(worldMutex_);
            auto it = worldState_.find(key);
            return (it != worldState_.end()) ? it->second : "";
        }
        
        bool HyperfyWorld::sendMessage(const std::string& message) {
            if (!connected_.load()) {
                logWarning("Cannot send message: not connected to world", "HyperfyWorld");
                return false;
            }
            
            logInfo("Sending message to world " + worldId_ + ": " + message, "HyperfyWorld");
            // TODO: Implement actual message sending via WebSocket
            return true;
        }
        
        bool HyperfyWorld::moveToPosition(double x, double y, double z) {
            if (!connected_.load()) {
                logWarning("Cannot move: not connected to world", "HyperfyWorld");
                return false;
            }
            
            std::ostringstream oss;
            oss << "Moving to position (" << x << ", " << y << ", " << z << ") in world " << worldId_;
            logInfo(oss.str(), "HyperfyWorld");
            
            // TODO: Implement actual position movement via WebSocket
            updateState("position_x", std::to_string(x));
            updateState("position_y", std::to_string(y));
            updateState("position_z", std::to_string(z));
            
            return true;
        }
        
        bool HyperfyWorld::performAction(const std::string& action, const std::string& parameters) {
            if (!connected_.load()) {
                logWarning("Cannot perform action: not connected to world", "HyperfyWorld");
                return false;
            }
            
            logInfo("Performing action '" + action + "' with parameters: " + parameters, "HyperfyWorld");
            // TODO: Implement actual action execution via WebSocket
            return true;
        }
        
        // HyperfyService implementation
        HyperfyService::HyperfyService() : running_(false) {
        }
        
        HyperfyService::~HyperfyService() {
            stop();
        }
        
        bool HyperfyService::start(const HyperfyConfig& config) {
            std::lock_guard<std::mutex> lock(serviceMutex_);
            
            if (running_.load()) {
                logWarning("HyperfyService is already running", "HyperfyService");
                return true;
            }
            
            config_ = config;
            logInfo("Starting HyperfyService with URL: " + config_.wsUrl, "HyperfyService");
            
            running_.store(true);
            serviceThread_ = std::thread(&HyperfyService::serviceLoop, this);
            
            logInfo("HyperfyService started successfully", "HyperfyService");
            return true;
        }
        
        void HyperfyService::stop() {
            {
                std::lock_guard<std::mutex> lock(serviceMutex_);
                if (!running_.load()) {
                    return;
                }
                
                logInfo("Stopping HyperfyService", "HyperfyService");
                running_.store(false);
            }
            
            // Wait for service thread to complete
            if (serviceThread_.joinable()) {
                serviceThread_.join();
            }
            
            // Disconnect from world
            disconnectFromWorld();
            
            // Cleanup managers
            for (auto& [name, manager] : managers_) {
                manager->cleanup();
            }
            managers_.clear();
            actions_.clear();
            
            logInfo("HyperfyService stopped", "HyperfyService");
        }
        
        void HyperfyService::serviceLoop() {
            while (running_.load()) {
                // Service heartbeat and maintenance tasks
                if (world_ && world_->isConnected()) {
                    // TODO: Implement periodic world state updates, heartbeat, etc.
                }
                
                // Sleep for a short interval
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
            }
        }
        
        bool HyperfyService::connectToWorld(const std::string& worldId, const std::string& wsUrl, const std::string& authToken) {
            std::lock_guard<std::mutex> lock(serviceMutex_);
            
            if (world_ && world_->isConnected()) {
                disconnectFromWorld();
            }
            
            world_ = std::make_shared<HyperfyWorld>(worldId, wsUrl);
            bool connected = world_->connect(authToken);
            
            if (connected) {
                logInfo("Successfully connected to Hyperfy world: " + worldId, "HyperfyService");
            } else {
                logError("Failed to connect to Hyperfy world: " + worldId, "HyperfyService");
                world_.reset();
            }
            
            return connected;
        }
        
        void HyperfyService::disconnectFromWorld() {
            if (world_) {
                world_->disconnect();
                world_.reset();
            }
        }
        
        void HyperfyService::registerAction(const std::string& name, std::shared_ptr<HyperfyAction> action) {
            std::lock_guard<std::mutex> lock(serviceMutex_);
            actions_[name] = action;
            logInfo("Registered Hyperfy action: " + name, "HyperfyService");
        }
        
        std::shared_ptr<HyperfyAction> HyperfyService::getAction(const std::string& name) const {
            auto it = actions_.find(name);
            return (it != actions_.end()) ? it->second : nullptr;
        }
        
        bool HyperfyService::executeAction(const std::string& name, const std::string& parameters) {
            auto action = getAction(name);
            if (!action) {
                logWarning("Action not found: " + name, "HyperfyService");
                return false;
            }
            
            logInfo("Executing action: " + name, "HyperfyService");
            return action->execute(parameters);
        }
        
        void HyperfyService::registerManager(const std::string& name, std::shared_ptr<HyperfyManager> manager) {
            std::lock_guard<std::mutex> lock(serviceMutex_);
            managers_[name] = manager;
            manager->initialize(shared_from_this());
            logInfo("Registered Hyperfy manager: " + name, "HyperfyService");
        }
        
        std::shared_ptr<HyperfyManager> HyperfyService::getManager(const std::string& name) const {
            auto it = managers_.find(name);
            return (it != managers_.end()) ? it->second : nullptr;
        }
        
        // Action implementations
        GotoAction::GotoAction(std::shared_ptr<HyperfyService> service) : service_(service) {
        }
        
        bool GotoAction::execute(const std::string& parameters) {
            // Parse parameters for x, y, z coordinates
            // Simple parsing for "x,y,z" format
            std::stringstream ss(parameters);
            std::string item;
            std::vector<std::string> coords;
            
            while (std::getline(ss, item, ',')) {
                coords.push_back(item);
            }
            
            if (coords.size() != 3) {
                logError("Invalid goto parameters. Expected format: x,y,z", "GotoAction");
                return false;
            }
            
            try {
                double x = std::stod(coords[0]);
                double y = std::stod(coords[1]);
                double z = std::stod(coords[2]);
                
                auto world = service_->getWorld();
                if (world) {
                    return world->moveToPosition(x, y, z);
                } else {
                    logError("No world connected for goto action", "GotoAction");
                    return false;
                }
            } catch (const std::exception& e) {
                logError("Failed to parse goto coordinates: " + std::string(e.what()), "GotoAction");
                return false;
            }
        }
        
        ReplyAction::ReplyAction(std::shared_ptr<HyperfyService> service) : service_(service) {
        }
        
        bool ReplyAction::execute(const std::string& parameters) {
            auto world = service_->getWorld();
            if (world) {
                return world->sendMessage(parameters);
            } else {
                logError("No world connected for reply action", "ReplyAction");
                return false;
            }
        }
        
        PerceptionAction::PerceptionAction(std::shared_ptr<HyperfyService> service) : service_(service) {
        }
        
        bool PerceptionAction::execute(const std::string& parameters) {
            (void)parameters; // Suppress unused parameter warning
            
            auto world = service_->getWorld();
            if (!world) {
                logError("No world connected for perception action", "PerceptionAction");
                return false;
            }
            
            logInfo("Analyzing 3D scene...", "PerceptionAction");
            // TODO: Implement actual scene perception
            // For now, just log that perception is happening
            
            std::ostringstream perception;
            perception << "Scene perception at position ("
                      << world->getState("position_x") << ", "
                      << world->getState("position_y") << ", "
                      << world->getState("position_z") << ")";
            
            logInfo(perception.str(), "PerceptionAction");
            return true;
        }
        
        // Factory implementation
        std::shared_ptr<HyperfyService> HyperfyServiceFactory::createService() {
            auto service = std::make_shared<HyperfyService>();
            
            // Register default actions
            service->registerAction("goto", std::make_shared<GotoAction>(service));
            service->registerAction("reply", std::make_shared<ReplyAction>(service));
            service->registerAction("perception", std::make_shared<PerceptionAction>(service));
            
            return service;
        }
        
        std::shared_ptr<HyperfyService> HyperfyServiceFactory::createServiceWithConfig(const HyperfyConfig& config) {
            auto service = createService();
            service->start(config);
            return service;
        }
        
    } // namespace hyperfy
} // namespace elizaos
