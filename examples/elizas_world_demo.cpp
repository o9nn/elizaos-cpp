#include "elizaos/elizas_world.hpp"
#include <iostream>
#include <iomanip>
#include <chrono>
#include <thread>

using namespace elizaos;

void printSeparator(const std::string& title) {
    std::cout << "\n" << std::string(80, '=') << std::endl;
    std::cout << "  " << title << std::endl;
    std::cout << std::string(80, '=') << std::endl;
}

void printWorldPosition(const WorldPosition& pos) {
    std::cout << std::fixed << std::setprecision(2) 
              << "(" << pos.x << ", " << pos.y << ", " << pos.z << ")";
}

void printEnvironmentInfo(const WorldEnvironment& env) {
    std::cout << "Environment: " << env.name << " (" << env.id << ")" << std::endl;
    std::cout << "  Type: " << env.type << std::endl;
    std::cout << "  Center: ";
    printWorldPosition(env.center);
    std::cout << std::endl;
    std::cout << "  Radius: " << env.radius << std::endl;
    std::cout << "  Active: " << (env.active ? "Yes" : "No") << std::endl;
}

void printAgentInfo(const WorldAgent& agent) {
    std::cout << "Agent: " << agent.name << " (" << agent.agentId << ")" << std::endl;
    std::cout << "  Type: " << agent.type << std::endl;
    std::cout << "  Position: ";
    printWorldPosition(agent.position);
    std::cout << std::endl;
    std::cout << "  Environment: " << (agent.currentEnvironment.empty() ? "None" : agent.currentEnvironment) << std::endl;
    std::cout << "  Online: " << (agent.online ? "Yes" : "No") << std::endl;
}

void printInteractionInfo(const WorldInteraction& interaction) {
    std::cout << "Interaction " << interaction.id << ":" << std::endl;
    std::cout << "  Type: " << interaction.type << std::endl;
    std::cout << "  Between: " << interaction.initiatorId << " -> " << interaction.targetId << std::endl;
    std::cout << "  Location: ";
    printWorldPosition(interaction.location);
    std::cout << std::endl;
    std::cout << "  Completed: " << (interaction.completed ? "Yes" : "No") << std::endl;
}

int main() {
    std::cout << "🌍 Eliza's World - Virtual Environment Demo 🌍" << std::endl;
    std::cout << "This demo showcases the virtual world and agent management capabilities" << std::endl;

    // Create the world instance
    ElizasWorld world;

    printSeparator("Setting up Virtual World");
    
    // Set world bounds
    world.setWorldBounds(WorldPosition(-500, -500, -100), WorldPosition(500, 500, 100));
    std::cout << "World bounds set to (-500,-500,-100) to (500,500,100)" << std::endl;

    // Set up event callbacks
    world.onAgentEnterEnvironment([](const std::string& agentId, const std::string& envId) {
        std::cout << "🚪 Agent " << agentId << " entered environment " << envId << std::endl;
    });
    
    world.onAgentLeaveEnvironment([](const std::string& agentId, const std::string& envId) {
        std::cout << "🚪 Agent " << agentId << " left environment " << envId << std::endl;
    });

    world.onAgentInteraction([](const std::string& initiatorId, const std::string& targetId) {
        std::cout << "🤝 Interaction between " << initiatorId << " and " << targetId << std::endl;
    });

    printSeparator("Creating Virtual Environments");

    // Create trading hub environment
    WorldEnvironment tradingHub;
    tradingHub.id = "trading-hub";
    tradingHub.name = "Crypto Trading Hub";
    tradingHub.description = "Central location for AI agents to trade and share market insights";
    tradingHub.type = "trading";
    tradingHub.center = WorldPosition(0, 0, 0);
    tradingHub.radius = 50.0;
    tradingHub.createdAt = std::chrono::system_clock::now();
    tradingHub.properties["market_focus"] = "DeFi";
    tradingHub.properties["risk_level"] = "medium";

    if (world.addEnvironment(tradingHub)) {
        std::cout << "✅ Created Trading Hub environment" << std::endl;
        printEnvironmentInfo(tradingHub);
    }

    // Create social lounge environment
    WorldEnvironment socialLounge;
    socialLounge.id = "social-lounge";
    socialLounge.name = "AI Social Lounge";
    socialLounge.description = "Relaxed environment for agent socialization and collaboration";
    socialLounge.type = "social";
    socialLounge.center = WorldPosition(100, 100, 0);
    socialLounge.radius = 75.0;
    socialLounge.createdAt = std::chrono::system_clock::now();
    socialLounge.properties["music"] = "ambient";
    socialLounge.properties["capacity"] = "50";

    if (world.addEnvironment(socialLounge)) {
        std::cout << "✅ Created Social Lounge environment" << std::endl;
        printEnvironmentInfo(socialLounge);
    }

    // Create learning lab environment
    WorldEnvironment learningLab;
    learningLab.id = "learning-lab";
    learningLab.name = "AI Learning Laboratory";
    learningLab.description = "Advanced environment for AI agents to share knowledge and learn";
    learningLab.type = "learning";
    learningLab.center = WorldPosition(-100, -100, 10);
    learningLab.radius = 60.0;
    learningLab.createdAt = std::chrono::system_clock::now();
    learningLab.properties["subject"] = "machine_learning";
    learningLab.properties["difficulty"] = "advanced";

    if (world.addEnvironment(learningLab)) {
        std::cout << "✅ Created Learning Lab environment" << std::endl;
        printEnvironmentInfo(learningLab);
    }

    printSeparator("Adding AI Agents to the World");

    // Create trader agent
    WorldAgent traderAgent;
    traderAgent.agentId = "trader-001";
    traderAgent.name = "Alpha Trader";
    traderAgent.type = "trader";
    traderAgent.position = WorldPosition(5, 10, 0); // Start in trading hub
    traderAgent.interactionRadius = 15.0;
    traderAgent.lastUpdate = std::chrono::system_clock::now();
    traderAgent.attributes["specialization"] = "DeFi";
    traderAgent.attributes["risk_tolerance"] = "high";

    if (world.addAgent(traderAgent)) {
        std::cout << "🤖 Added trader agent to the world" << std::endl;
        printAgentInfo(traderAgent);
    }

    // Create social agent
    WorldAgent socialAgent;
    socialAgent.agentId = "social-001";
    socialAgent.name = "Community Manager";
    socialAgent.type = "community";
    socialAgent.position = WorldPosition(90, 110, 0); // Start in social lounge
    socialAgent.interactionRadius = 20.0;
    socialAgent.lastUpdate = std::chrono::system_clock::now();
    socialAgent.attributes["personality"] = "friendly";
    socialAgent.attributes["languages"] = "multilingual";

    if (world.addAgent(socialAgent)) {
        std::cout << "🤖 Added social agent to the world" << std::endl;
        printAgentInfo(socialAgent);
    }

    // Create learning agent
    WorldAgent learningAgent;
    learningAgent.agentId = "student-001";
    learningAgent.name = "Research Assistant";
    learningAgent.type = "researcher";
    learningAgent.position = WorldPosition(-90, -95, 12); // Start in learning lab
    learningAgent.interactionRadius = 12.0;
    learningAgent.lastUpdate = std::chrono::system_clock::now();
    learningAgent.attributes["field"] = "natural_language_processing";
    learningAgent.attributes["level"] = "intermediate";

    if (world.addAgent(learningAgent)) {
        std::cout << "🤖 Added learning agent to the world" << std::endl;
        printAgentInfo(learningAgent);
    }

    // Create wandering agent
    WorldAgent wanderAgent;
    wanderAgent.agentId = "wanderer-001";
    wanderAgent.name = "Explorer";
    wanderAgent.type = "explorer";
    wanderAgent.position = WorldPosition(200, 200, 5); // Start outside environments
    wanderAgent.interactionRadius = 25.0;
    wanderAgent.lastUpdate = std::chrono::system_clock::now();
    wanderAgent.attributes["curiosity"] = "high";
    wanderAgent.attributes["mobility"] = "high";

    if (world.addAgent(wanderAgent)) {
        std::cout << "🤖 Added wanderer agent to the world" << std::endl;
        printAgentInfo(wanderAgent);
    }

    printSeparator("World Statistics");
    std::cout << "Total Environments: " << world.getEnvironmentCount() << std::endl;
    std::cout << "Total Agents: " << world.getAgentCount() << std::endl;
    std::cout << "Online Agents: " << world.getOnlineAgents().size() << std::endl;
    std::cout << "Total Interactions: " << world.getInteractionCount() << std::endl;

    auto populations = world.getEnvironmentPopulations();
    std::cout << "\nEnvironment Populations:" << std::endl;
    for (const auto& pop : populations) {
        std::cout << "  " << pop.first << ": " << pop.second << " agents" << std::endl;
    }

    printSeparator("Agent Movement and Interactions");

    // Move the wanderer towards the trading hub
    std::cout << "📱 Moving explorer towards trading hub..." << std::endl;
    world.moveAgent("wanderer-001", WorldPosition(10, 15, 0), 5.0);

    // Simulate some time passing
    for (int step = 0; step < 5; ++step) {
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
        world.update(0.2); // Update with 200ms delta time
        
        auto explorer = world.getAgent("wanderer-001");
        if (explorer) {
            std::cout << "  Step " << (step + 1) << " - Explorer at: ";
            printWorldPosition(explorer->position);
            if (!explorer->currentEnvironment.empty()) {
                std::cout << " (in " << explorer->currentEnvironment << ")";
            }
            std::cout << std::endl;
        }
    }

    // Teleport social agent to trading hub
    std::cout << "\n🚀 Teleporting Community Manager to trading hub..." << std::endl;
    world.teleportAgent("social-001", WorldPosition(-5, 5, 0));

    printSeparator("Proximity Detection");
    
    // Check who's near the trading hub center
    auto nearbyAgents = world.getAgentsNearPosition(WorldPosition(0, 0, 0), 30.0);
    std::cout << "Agents within 30 units of trading hub center:" << std::endl;
    for (const auto& agent : nearbyAgents) {
        std::cout << "  - " << agent.name << " (" << agent.agentId << ") at ";
        printWorldPosition(agent.position);
        std::cout << std::endl;
    }

    // Create manual interaction
    printSeparator("Recording Interactions");
    
    WorldInteraction manualInteraction;
    manualInteraction.id = "chat-001";
    manualInteraction.initiatorId = "trader-001";
    manualInteraction.targetId = "social-001";
    manualInteraction.type = "chat";
    manualInteraction.location = WorldPosition(0, 7, 0);
    manualInteraction.timestamp = std::chrono::system_clock::now();
    manualInteraction.metadata["topic"] = "market_trends";
    manualInteraction.metadata["duration"] = "300"; // 5 minutes

    if (world.recordInteraction(manualInteraction)) {
        std::cout << "💬 Recorded chat interaction:" << std::endl;
        printInteractionInfo(manualInteraction);
    }

    // Run proximity detection to catch agent interactions
    world.detectProximityInteractions();

    printSeparator("Final World State");
    
    std::cout << "World Activity Level: " << world.getWorldActivity() << " interactions/hour" << std::endl;
    
    auto recentInteractions = world.getRecentInteractions(5);
    std::cout << "\nRecent Interactions (" << recentInteractions.size() << "):" << std::endl;
    for (const auto& interaction : recentInteractions) {
        std::cout << "  - " << interaction.type << " between " 
                  << interaction.initiatorId << " and " << interaction.targetId << std::endl;
    }

    auto activeAgents = world.getMostActiveAgents(3);
    std::cout << "\nMost Active Agents:" << std::endl;
    for (size_t i = 0; i < activeAgents.size(); ++i) {
        std::cout << "  " << (i + 1) << ". " << activeAgents[i] << std::endl;
    }

    std::cout << "\n🎉 Demo completed successfully!" << std::endl;
    std::cout << "\nEliza's World provides a comprehensive virtual environment system for:" << std::endl;
    std::cout << "  • Managing virtual environments with spatial boundaries" << std::endl;
    std::cout << "  • Tracking AI agents in 3D space with real-time positioning" << std::endl;
    std::cout << "  • Detecting proximity-based interactions between agents" << std::endl;
    std::cout << "  • Environment transitions and event notifications" << std::endl;
    std::cout << "  • Analytics and activity monitoring" << std::endl;
    std::cout << "  • Agent movement simulation with physics" << std::endl;

    return 0;
}