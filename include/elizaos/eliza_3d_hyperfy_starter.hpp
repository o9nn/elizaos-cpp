#pragma once

#include "elizaos/core.hpp"
#include <string>
#include <memory>
#include <map>
#include <functional>
#include <thread>
#include <atomic>
#include <mutex>

namespace elizaos {
    namespace hyperfy {
        
        // Forward declarations
        class HyperfyService;
        class HyperfyWorld;
        class HyperfyAction;
        class HyperfyManager;
        
        /**
         * @brief Configuration for Hyperfy world connection
         */
        struct HyperfyConfig {
            std::string wsUrl = "wss://chill.hyperfy.xyz/ws";
            std::string worldId;
            std::string authToken;
            int connectionTimeoutMs = 10000;
            int heartbeatIntervalMs = 30000;
        };
        
        /**
         * @brief Base class for Hyperfy actions that can be performed in the 3D world
         */
        class HyperfyAction {
        public:
            virtual ~HyperfyAction() = default;
            virtual std::string getName() const = 0;
            virtual std::string getDescription() const = 0;
            virtual bool execute(const std::string& parameters = "") = 0;
        };
        
        /**
         * @brief Manages specific aspects of Hyperfy world interaction
         */
        class HyperfyManager {
        public:
            virtual ~HyperfyManager() = default;
            virtual bool initialize(std::shared_ptr<HyperfyService> service) = 0;
            virtual void cleanup() = 0;
        };
        
        /**
         * @brief Represents a connection to a Hyperfy 3D world
         */
        class HyperfyWorld {
        private:
            std::string worldId_;
            std::string wsUrl_;
            std::atomic<bool> connected_;
            mutable std::mutex worldMutex_;
            std::map<std::string, std::string> worldState_;
            
        public:
            HyperfyWorld(const std::string& worldId, const std::string& wsUrl);
            ~HyperfyWorld();
            
            bool connect(const std::string& authToken = "");
            void disconnect();
            bool isConnected() const;
            
            std::string getWorldId() const { return worldId_; }
            std::string getWsUrl() const { return wsUrl_; }
            
            // World state management
            void updateState(const std::string& key, const std::string& value);
            std::string getState(const std::string& key) const;
            
            // Basic world interaction methods
            bool sendMessage(const std::string& message);
            bool moveToPosition(double x, double y, double z);
            bool performAction(const std::string& action, const std::string& parameters = "");
        };
        
        /**
         * @brief Main service for managing Hyperfy world integration
         * Based on the TypeScript HyperfyService implementation
         */
        class HyperfyService : public std::enable_shared_from_this<HyperfyService> {
        private:
            std::shared_ptr<HyperfyWorld> world_;
            std::map<std::string, std::shared_ptr<HyperfyAction>> actions_;
            std::map<std::string, std::shared_ptr<HyperfyManager>> managers_;
            std::atomic<bool> running_;
            std::thread serviceThread_;
            std::mutex serviceMutex_;
            HyperfyConfig config_;
            
            // Service loop for handling background tasks
            void serviceLoop();
            
        public:
            HyperfyService();
            ~HyperfyService();
            
            // Service lifecycle
            bool start(const HyperfyConfig& config);
            void stop();
            bool isRunning() const { return running_.load(); }
            
            // World management
            std::shared_ptr<HyperfyWorld> getWorld() const { return world_; }
            bool connectToWorld(const std::string& worldId, const std::string& wsUrl, const std::string& authToken = "");
            void disconnectFromWorld();
            
            // Action management
            void registerAction(const std::string& name, std::shared_ptr<HyperfyAction> action);
            std::shared_ptr<HyperfyAction> getAction(const std::string& name) const;
            bool executeAction(const std::string& name, const std::string& parameters = "");
            
            // Manager registration
            void registerManager(const std::string& name, std::shared_ptr<HyperfyManager> manager);
            std::shared_ptr<HyperfyManager> getManager(const std::string& name) const;
            
            // Configuration
            const HyperfyConfig& getConfig() const { return config_; }
        };
        
        // Basic action implementations
        
        /**
         * @brief Action for moving to a specific location in the 3D world
         */
        class GotoAction : public HyperfyAction {
        private:
            std::shared_ptr<HyperfyService> service_;
            
        public:
            GotoAction(std::shared_ptr<HyperfyService> service);
            std::string getName() const override { return "goto"; }
            std::string getDescription() const override { return "Move to a specific location in the 3D world"; }
            bool execute(const std::string& parameters) override;
        };
        
        /**
         * @brief Action for sending messages in the 3D world
         */
        class ReplyAction : public HyperfyAction {
        private:
            std::shared_ptr<HyperfyService> service_;
            
        public:
            ReplyAction(std::shared_ptr<HyperfyService> service);
            std::string getName() const override { return "reply"; }
            std::string getDescription() const override { return "Send a message in the 3D world"; }
            bool execute(const std::string& parameters) override;
        };
        
        /**
         * @brief Action for perceiving the current 3D scene
         */
        class PerceptionAction : public HyperfyAction {
        private:
            std::shared_ptr<HyperfyService> service_;
            
        public:
            PerceptionAction(std::shared_ptr<HyperfyService> service);
            std::string getName() const override { return "perception"; }
            std::string getDescription() const override { return "Analyze and describe the current 3D scene"; }
            bool execute(const std::string& parameters) override;
        };
        
        /**
         * @brief Factory class for creating and managing Hyperfy service instances
         */
        class HyperfyServiceFactory {
        public:
            static std::shared_ptr<HyperfyService> createService();
            static std::shared_ptr<HyperfyService> createServiceWithConfig(const HyperfyConfig& config);
        };
        
    } // namespace hyperfy
} // namespace elizaos