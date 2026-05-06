// ElizaOS Agent Type Definitions
declare namespace ElizaOS {
    interface AgentConfig {
        agentId: string;
        agentName: string;
        bio?: string;
        lore?: string[];
    }

    interface Memory {
        id: string;
        content: string;
        entityId: string;
        agentId: string;
        timestamp: number;
    }

    class AgentMemoryManager {
        constructor();
        addMemory(memory: Memory): void;
        getMemory(id: string): Memory | null;
        searchMemories(query: string, limit: number): Memory[];
        clearMemories(): void;
    }

    class AgentLogger {
        constructor();
        log(level: string, message: string): void;
        info(message: string): void;
        warn(message: string): void;
        error(message: string): void;
    }
}
