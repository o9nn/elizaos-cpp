# ElizaOS C++ Architecture Overview

**Document Version:** 1.0.0  
**Date:** December 12, 2024  
**Framework Version:** 1.0.0

## Executive Summary

ElizaOS C++ is a high-performance, event-driven cognitive agent framework implementing advanced autonomous agent capabilities through a layered architecture. This document provides comprehensive architectural documentation with formal diagrams and system component analysis.

**Key Characteristics:**
- **Technology Stack:** C++17, CMake, GoogleTest, nlohmann/json
- **Architecture Pattern:** Event-driven, layered cognitive architecture
- **Concurrency Model:** Multi-threaded with thread-safe operations
- **Memory Model:** Embedding-based semantic memory with attention allocation
- **Communication:** Inter-agent messaging with publish-subscribe patterns
- **Deployment:** Cross-platform (Linux, Windows, macOS)

---

## Table of Contents

1. [System Architecture Overview](#system-architecture-overview)
2. [Component Architecture](#component-architecture)
3. [Data Flow Diagrams](#data-flow-diagrams)
4. [Integration Boundaries](#integration-boundaries)
5. [Cognitive Processing Pipeline](#cognitive-processing-pipeline)
6. [Memory and Attention Systems](#memory-and-attention-systems)
7. [Concurrency and Threading Model](#concurrency-and-threading-model)
8. [API Surface and Interfaces](#api-surface-and-interfaces)

---

## System Architecture Overview

### High-Level System Architecture

The ElizaOS C++ framework implements a five-layer cognitive architecture, where each layer builds upon the foundations provided by lower layers.

```mermaid
graph TB
    subgraph "Layer 5: Application Layer"
        APP1[Autonomous Agents]
        APP2[Multi-Agent Swarms]
        APP3[Cognitive Applications]
        APP4[Agent Coordination Systems]
    end
    
    subgraph "Layer 4: Cognitive Layer"
        COG1[Meta-Cognition Engine]
        COG2[Self-Modification System]
        COG3[Attention Allocation ECAN]
        COG4[Pattern Recognition]
        COG5[Emotional State Tracking]
    end
    
    subgraph "Layer 3: Processing Layer"
        PROC1[Agent Loop System<br/>agentloop]
        PROC2[Task Orchestration<br/>agentagenda]
        PROC3[Decision Engine<br/>agentaction]
        PROC4[Response Generation<br/>eliza]
        PROC5[Browser Automation<br/>agentbrowser]
    end
    
    subgraph "Layer 2: Infrastructure Layer"
        INF1[Memory Management<br/>agentmemory]
        INF2[Communication Hub<br/>agentcomms]
        INF3[Character System<br/>characters]
        INF4[Knowledge Base<br/>knowledge]
        INF5[Logging System<br/>agentlogger]
        INF6[Shell Interface<br/>agentshell]
    end
    
    subgraph "Layer 1: Foundation Layer"
        FOUND1[Core Data Structures<br/>core]
        FOUND2[State Management]
        FOUND3[Event System]
        FOUND4[Thread Management]
        FOUND5[UUID Generation]
        FOUND6[JSON Serialization]
    end
    
    %% Layer connections
    APP1 --> COG1
    APP2 --> COG2
    APP3 --> COG4
    APP4 --> COG3
    
    COG1 --> PROC1
    COG2 --> PROC2
    COG3 --> PROC3
    COG4 --> PROC4
    COG5 --> PROC5
    
    PROC1 --> INF1
    PROC2 --> INF2
    PROC3 --> INF3
    PROC4 --> INF4
    PROC5 --> INF5
    
    INF1 --> FOUND1
    INF2 --> FOUND2
    INF3 --> FOUND3
    INF4 --> FOUND4
    INF5 --> FOUND5
    INF6 --> FOUND6
    
    %% Feedback loops (meta-cognitive)
    COG1 -.->|introspection| PROC1
    COG2 -.->|adaptation| INF1
    COG3 -.->|attention| FOUND2
```

**Architectural Principles:**

1. **Layered Abstraction**: Clear separation of concerns with well-defined interfaces
2. **Emergent Intelligence**: Complex behaviors emerge from component interactions
3. **Meta-Cognitive Feedback**: Continuous self-monitoring and adaptation
4. **Thread Safety**: All operations designed for concurrent access
5. **Modular Design**: Components can be developed and tested independently

---

## Component Architecture

### Core Components Interaction

This diagram illustrates the detailed interactions between core subsystems.

```mermaid
graph LR
    subgraph "Memory Subsystem"
        MEM1[AgentMemoryManager]
        MEM2[Embedding Engine]
        MEM3[Semantic Retrieval]
        MEM4[Attention Allocation]
        MEM5[Memory Storage]
    end
    
    subgraph "Agent Loop Subsystem"
        LOOP1[AgentLoop Controller]
        LOOP2[Step Executor]
        LOOP3[State Manager]
        LOOP4[Event Handler]
        LOOP5[Thread Coordinator]
    end
    
    subgraph "Communication Subsystem"
        COMM1[Message Bus]
        COMM2[Channel Manager]
        COMM3[Agent Registry]
        COMM4[Message Validator]
        COMM5[Broadcast System]
    end
    
    subgraph "Cognition Subsystem"
        THINK1[Decision Engine]
        THINK2[Action Validator]
        THINK3[Goal Manager]
        THINK4[Pattern Matcher]
        THINK5[Response Generator]
    end
    
    subgraph "Character Subsystem"
        CHAR1[Personality Matrix]
        CHAR2[Trait Manager]
        CHAR3[Emotional Tracker]
        CHAR4[Profile Manager]
    end
    
    %% Cross-subsystem interactions
    LOOP1 --> MEM1
    LOOP1 --> COMM1
    LOOP1 --> THINK1
    
    MEM1 --> MEM2
    MEM2 --> MEM3
    MEM3 --> MEM4
    MEM4 --> MEM5
    
    COMM1 --> COMM2
    COMM2 --> COMM3
    COMM3 --> COMM4
    COMM4 --> COMM5
    
    THINK1 --> THINK2
    THINK2 --> THINK3
    THINK3 --> THINK4
    THINK4 --> THINK5
    
    CHAR1 --> CHAR2
    CHAR2 --> CHAR3
    CHAR3 --> CHAR4
    
    %% Integration points
    THINK5 --> CHAR1
    MEM3 --> THINK4
    COMM5 --> LOOP4
```

### Module Dependencies

```mermaid
graph TD
    subgraph "Core Modules"
        CORE[core.hpp<br/>State, Memory, Task]
    end
    
    subgraph "Infrastructure Modules"
        LOOP[agentloop.hpp<br/>Event Loop]
        MEM[agentmemory.hpp<br/>Memory System]
        COMM[agentcomms.hpp<br/>Communication]
        LOG[agentlogger.hpp<br/>Logging]
        SHELL[agentshell.hpp<br/>Shell Interface]
    end
    
    subgraph "Application Modules"
        ELIZA[eliza.hpp<br/>Conversation Engine]
        CHAR[characters.hpp<br/>Personality System]
        KNOW[knowledge.hpp<br/>Knowledge Base]
        ACTION[agentaction.hpp<br/>Action Processing]
        AGENDA[agentagenda.hpp<br/>Task Scheduling]
        BROWSER[agentbrowser.hpp<br/>Web Automation]
    end
    
    subgraph "Advanced Modules"
        EVOL[evolutionary.hpp<br/>Learning]
        EMBODY[embodiment.hpp<br/>Sensory/Motor]
        SPARTAN[spartan.hpp<br/>Features]
        REGISTRY[registry.hpp<br/>Registry Service]
    end
    
    %% Dependencies
    LOOP --> CORE
    MEM --> CORE
    COMM --> CORE
    LOG --> CORE
    SHELL --> CORE
    
    ELIZA --> CORE
    ELIZA --> MEM
    ELIZA --> LOG
    ELIZA --> ACTION
    ELIZA --> KNOW
    ELIZA --> CHAR
    
    CHAR --> CORE
    CHAR --> MEM
    CHAR --> LOG
    CHAR --> ACTION
    
    KNOW --> CORE
    KNOW --> MEM
    KNOW --> LOG
    KNOW --> ACTION
    
    ACTION --> CORE
    
    AGENDA --> CORE
    AGENDA --> LOOP
    
    BROWSER --> CORE
    BROWSER --> LOG
    
    EVOL --> CORE
    EMBODY --> CORE
    SPARTAN --> CORE
    REGISTRY --> CORE
```

---

## Data Flow Diagrams

### Message Processing Flow

```mermaid
sequenceDiagram
    participant User
    participant AgentLoop
    participant Eliza
    participant Memory
    participant Character
    participant Knowledge
    participant Response
    
    User->>AgentLoop: Input Message
    AgentLoop->>Memory: Store Input
    Memory-->>AgentLoop: Memory ID
    
    AgentLoop->>Eliza: Process Message
    Eliza->>Character: Get Personality Traits
    Character-->>Eliza: Personality Matrix
    
    Eliza->>Memory: Retrieve Context
    Memory-->>Eliza: Recent History
    
    Eliza->>Knowledge: Query Knowledge Base
    Knowledge-->>Eliza: Relevant Facts
    
    Eliza->>Response: Generate Response
    Response-->>Eliza: Response Text
    
    Eliza->>Memory: Store Response
    Memory-->>Eliza: Confirmation
    
    Eliza-->>AgentLoop: Response
    AgentLoop-->>User: Output Response
```

### Agent Loop Execution Flow

```mermaid
flowchart TD
    START([Agent Loop Start])
    INIT[Initialize State]
    PAUSED{Is Paused?}
    WAIT[Wait for Signal]
    EXECUTE[Execute Step Function]
    NEXT{More Steps?}
    INTERVAL{Has Interval?}
    SLEEP[Sleep for Interval]
    RUNNING{Still Running?}
    STOP([Stop Loop])
    
    START --> INIT
    INIT --> RUNNING
    RUNNING -->|Yes| PAUSED
    RUNNING -->|No| STOP
    
    PAUSED -->|Yes| WAIT
    PAUSED -->|No| EXECUTE
    
    WAIT --> PAUSED
    
    EXECUTE --> NEXT
    NEXT -->|Yes| EXECUTE
    NEXT -->|No| INTERVAL
    
    INTERVAL -->|Yes| SLEEP
    INTERVAL -->|No| RUNNING
    
    SLEEP --> RUNNING
```

### Memory Storage and Retrieval Flow

```mermaid
flowchart LR
    subgraph "Storage Path"
        S1[Input Memory]
        S2[Generate UUID]
        S3[Create Timestamp]
        S4[Compute Embedding]
        S5[Store in Table]
        S6[Update Index]
        S7[Return ID]
    end
    
    subgraph "Retrieval Path"
        R1[Query Parameters]
        R2[Filter by Criteria]
        R3[Compute Similarity]
        R4[Sort by Relevance]
        R5[Apply Limit]
        R6[Return Results]
    end
    
    S1 --> S2 --> S3 --> S4 --> S5 --> S6 --> S7
    R1 --> R2 --> R3 --> R4 --> R5 --> R6
    
    S6 -.->|Index Update| R2
```

---

## Integration Boundaries

### External System Integration

```mermaid
graph TB
    subgraph "ElizaOS C++ Core"
        CORE[Core Framework]
        API[API Layer]
        PLUGIN[Plugin Interface]
    end
    
    subgraph "External Systems"
        LLM[LLM Services<br/>OpenAI, Anthropic]
        DB[Database Systems<br/>Vector DB]
        WEB[Web Services<br/>REST APIs]
        FS[File System<br/>Configuration]
        NET[Network<br/>TCP/WebSocket]
    end
    
    subgraph "Integration Modules"
        HTTP[HTTP Client]
        JSON[JSON Parser]
        CURL[libcurl]
        THREAD[Threading]
    end
    
    CORE --> API
    API --> PLUGIN
    
    PLUGIN --> HTTP
    HTTP --> LLM
    HTTP --> WEB
    
    PLUGIN --> JSON
    JSON --> DB
    
    PLUGIN --> CURL
    CURL --> NET
    
    CORE --> FS
    CORE --> THREAD
```

### Component Boundary Diagram

```mermaid
C4Context
    title System Context Diagram for ElizaOS C++
    
    Person(user, "User", "Interacts with agents")
    Person(developer, "Developer", "Builds agent applications")
    
    System(elizaos, "ElizaOS C++", "Cognitive Agent Framework")
    
    System_Ext(llm, "LLM Services", "AI language models")
    System_Ext(vectordb, "Vector Database", "Embedding storage")
    System_Ext(web, "Web Services", "External APIs")
    
    Rel(user, elizaos, "Interacts with")
    Rel(developer, elizaos, "Develops with")
    Rel(elizaos, llm, "Queries", "HTTP/HTTPS")
    Rel(elizaos, vectordb, "Stores/Retrieves", "Protocol")
    Rel(elizaos, web, "Integrates", "REST/WebSocket")
```

---

## Cognitive Processing Pipeline

### Cognitive Architecture

```mermaid
graph TB
    subgraph "Perception Layer"
        P1[Sensory Input]
        P2[Input Normalization]
        P3[Feature Extraction]
    end
    
    subgraph "Memory Layer"
        M1[Working Memory]
        M2[Long-term Memory]
        M3[Semantic Memory]
        M4[Episodic Memory]
    end
    
    subgraph "Reasoning Layer"
        R1[Pattern Matching]
        R2[Inference Engine]
        R3[Decision Making]
        R4[Goal Selection]
    end
    
    subgraph "Action Layer"
        A1[Action Planning]
        A2[Action Validation]
        A3[Action Execution]
        A4[Response Generation]
    end
    
    subgraph "Meta-Cognitive Layer"
        META1[Self-Monitoring]
        META2[Strategy Selection]
        META3[Performance Evaluation]
        META4[Adaptation]
    end
    
    P1 --> P2 --> P3
    P3 --> M1
    
    M1 --> R1
    M2 --> R2
    M3 --> R1
    M4 --> R3
    
    R1 --> R2 --> R3 --> R4
    
    R4 --> A1 --> A2 --> A3 --> A4
    
    META1 -.->|monitor| R1
    META2 -.->|select| R3
    META3 -.->|evaluate| A3
    META4 -.->|adapt| M2
    
    A4 -.->|feedback| META1
```

### Attention Allocation Mechanism

```mermaid
flowchart TD
    INPUT[Stimulus Input]
    GATE[Attention Gate]
    ALLOC[Attention Allocation]
    PRIORITY[Priority Queue]
    PROCESS[Process Selected]
    UPDATE[Update STI/LTI]
    FORGET[Forget Low Priority]
    
    INPUT --> GATE
    GATE -->|Above Threshold| ALLOC
    GATE -->|Below Threshold| FORGET
    
    ALLOC --> PRIORITY
    PRIORITY --> PROCESS
    PROCESS --> UPDATE
    UPDATE --> ALLOC
    
    FORGET -.->|Remove| PRIORITY
```

**Attention Values:**
- **STI (Short-Term Importance):** Current relevance
- **LTI (Long-Term Importance):** Historical significance
- **VLTI (Very Long-Term Importance):** Core knowledge importance

---

## Memory and Attention Systems

### Memory System Architecture

```mermaid
graph LR
    subgraph "Memory Types"
        DOC[Document Memory]
        FRAG[Fragment Memory]
        MSG[Message Memory]
        DESC[Description Memory]
        CUSTOM[Custom Memory]
    end
    
    subgraph "Memory Operations"
        CREATE[Create Memory]
        RETRIEVE[Retrieve Memory]
        UPDATE[Update Memory]
        DELETE[Delete Memory]
        SEARCH[Search Memory]
    end
    
    subgraph "Storage Layer"
        TABLE1[memories table]
        TABLE2[documents table]
        TABLE3[fragments table]
        EMBED[Embedding Index]
    end
    
    subgraph "Attention System"
        STI[Short-term Importance]
        LTI[Long-term Importance]
        DECAY[Attention Decay]
        SPREAD[Attention Spread]
    end
    
    DOC --> CREATE
    FRAG --> CREATE
    MSG --> CREATE
    DESC --> CREATE
    CUSTOM --> CREATE
    
    CREATE --> TABLE1
    CREATE --> TABLE2
    CREATE --> TABLE3
    CREATE --> EMBED
    
    RETRIEVE --> EMBED
    SEARCH --> EMBED
    
    EMBED --> STI
    STI --> LTI
    LTI --> DECAY
    DECAY --> SPREAD
```

### Hypergraph Knowledge Representation

```mermaid
graph TD
    subgraph "Hypergraph Structure"
        N1((Node 1<br/>Concept))
        N2((Node 2<br/>Entity))
        N3((Node 3<br/>Action))
        N4((Node 4<br/>Relation))
        
        E1[Hyperedge 1<br/>Context]
        E2[Hyperedge 2<br/>Event]
        E3[Hyperedge 3<br/>State]
    end
    
    N1 --> E1
    N2 --> E1
    N3 --> E1
    
    N2 --> E2
    N3 --> E2
    
    N3 --> E3
    N4 --> E3
    N1 --> E3
    
    E1 -.->|Attention| N1
    E2 -.->|Attention| N3
```

**Hypergraph Features:**
- Nodes represent concepts, entities, or actions
- Hyperedges connect multiple nodes (not just pairs)
- Enables representation of complex n-ary relationships
- Supports attention spreading across knowledge structures

---

## Concurrency and Threading Model

### Thread Architecture

```mermaid
graph TB
    MAIN[Main Thread]
    
    subgraph "Agent Loop Threads"
        LOOP1[Loop Thread 1]
        LOOP2[Loop Thread 2]
        LOOP3[Loop Thread N]
    end
    
    subgraph "Communication Threads"
        COMM1[Message Handler]
        COMM2[Broadcast Thread]
        COMM3[Channel Monitor]
    end
    
    subgraph "Processing Threads"
        PROC1[Task Executor 1]
        PROC2[Task Executor 2]
        PROC3[Task Executor N]
    end
    
    subgraph "Background Threads"
        BG1[Attention Decay]
        BG2[Memory Cleanup]
        BG3[Stats Collector]
    end
    
    MAIN --> LOOP1
    MAIN --> LOOP2
    MAIN --> LOOP3
    
    MAIN --> COMM1
    MAIN --> COMM2
    MAIN --> COMM3
    
    LOOP1 --> PROC1
    LOOP2 --> PROC2
    LOOP3 --> PROC3
    
    MAIN --> BG1
    MAIN --> BG2
    MAIN --> BG3
```

### Thread Safety Mechanisms

```mermaid
flowchart LR
    subgraph "Synchronization Primitives"
        MTX[std::mutex]
        LOCK[std::lock_guard]
        ATOMIC[std::atomic]
        COND[std::condition_variable]
    end
    
    subgraph "Protected Resources"
        MEM[Memory Storage]
        TASK[Task Queue]
        STATE[Agent State]
        MSG[Message Queue]
    end
    
    MTX --> LOCK
    LOCK --> MEM
    LOCK --> TASK
    LOCK --> STATE
    
    ATOMIC --> STATE
    
    COND --> TASK
    COND --> MSG
```

---

## API Surface and Interfaces

### Core API Classes

```mermaid
classDiagram
    class State {
        +AgentConfig config
        +vector~Actor~ actors
        +vector~Goal~ goals
        +vector~Memory~ recentMessages
        +addActor(Actor)
        +addGoal(Goal)
        +addRecentMessage(Memory)
    }
    
    class Memory {
        +UUID id
        +string content
        +UUID entityId
        +UUID agentId
        +EmbeddingVector embedding
        +MemoryMetadata metadata
        +Timestamp createdAt
    }
    
    class AgentLoop {
        +AgentLoop(steps, paused, interval)
        +start()
        +stop()
        +pause()
        +unpause()
        +step()
        +isRunning()
        +isPaused()
    }
    
    class AgentMemoryManager {
        +createMemory(memory, table, unique)
        +getMemoryById(id)
        +updateMemory(memory)
        +deleteMemory(id)
        +searchMemories(params)
        +getMemories(params)
    }
    
    class AgentCommsHub {
        +registerAgent(agentId)
        +sendMessage(message)
        +broadcastMessage(message)
        +subscribeToChannel(agentId, channelId)
        +subscribeToServer(agentId, serverId)
    }
    
    class ElizaCore {
        +processMessage(input, context)
        +generateResponse(context)
        +loadPatterns(patterns)
        +saveContext(context)
        +loadContext(sessionId)
    }
    
    class CharacterProfile {
        +string name
        +PersonalityMatrix personality
        +vector~CharacterTrait~ traits
        +EmotionalStateTracker emotionalState
        +addTrait(trait)
        +updatePersonality(matrix)
        +calculateCompatibility(other)
    }
    
    State --> Memory
    AgentLoop --> State
    ElizaCore --> Memory
    ElizaCore --> CharacterProfile
    AgentMemoryManager --> Memory
```

### Interface Contracts

```mermaid
classDiagram
    class IMemoryProvider {
        <<interface>>
        +createMemory(memory)*
        +getMemoryById(id)*
        +searchMemories(params)*
    }
    
    class IMessageHandler {
        <<interface>>
        +handleMessage(message)*
        +validateMessage(message)*
        +routeMessage(message)*
    }
    
    class IDecisionEngine {
        <<interface>>
        +evaluateOptions(state, options)*
        +selectAction(state)*
        +validateAction(action, state)*
    }
    
    class IPatternMatcher {
        <<interface>>
        +matchPattern(input, patterns)*
        +extractFeatures(input)*
        +computeConfidence(match)*
    }
    
    AgentMemoryManager ..|> IMemoryProvider
    AgentCommsHub ..|> IMessageHandler
    ElizaCore ..|> IDecisionEngine
    ElizaCore ..|> IPatternMatcher
```

---

## Build and Deployment Architecture

### Build System

```mermaid
flowchart TD
    CMAKE[CMakeLists.txt]
    CONFIG[Configuration]
    FETCH[Fetch Dependencies]
    
    subgraph "Dependencies"
        GTEST[GoogleTest]
        JSON[nlohmann/json]
        CURL[libcurl]
        THREADS[Threads]
    end
    
    subgraph "Build Targets"
        CORE[Core Libraries]
        INFRA[Infrastructure]
        APP[Applications]
        TEST[Tests]
        EXAMPLES[Examples]
    end
    
    subgraph "Output"
        LIBS[Static/Shared Libraries]
        BINS[Executables]
        TESTS[Test Binaries]
    end
    
    CMAKE --> CONFIG
    CONFIG --> FETCH
    
    FETCH --> GTEST
    FETCH --> JSON
    FETCH --> CURL
    FETCH --> THREADS
    
    GTEST --> TEST
    JSON --> CORE
    CURL --> INFRA
    THREADS --> CORE
    
    CORE --> INFRA --> APP
    APP --> BINS
    CORE --> LIBS
    TEST --> TESTS
```

### Deployment Model

```mermaid
graph LR
    subgraph "Development"
        DEV[Development Build]
        DEBUG[Debug Symbols]
        TEST1[Unit Tests]
    end
    
    subgraph "Staging"
        STAGE[Staging Build]
        RELEASE[Release Config]
        TEST2[Integration Tests]
    end
    
    subgraph "Production"
        PROD[Production Build]
        OPTIM[Optimized]
        PACKAGE[Packages]
    end
    
    subgraph "Packages"
        DEB[DEB Package]
        RPM[RPM Package]
        WIN[Windows Installer]
        TGZ[Archive]
    end
    
    DEV --> DEBUG
    DEBUG --> TEST1
    
    TEST1 --> STAGE
    STAGE --> RELEASE
    RELEASE --> TEST2
    
    TEST2 --> PROD
    PROD --> OPTIM
    OPTIM --> PACKAGE
    
    PACKAGE --> DEB
    PACKAGE --> RPM
    PACKAGE --> WIN
    PACKAGE --> TGZ
```

---

## Summary

This architecture overview documents the ElizaOS C++ framework as a comprehensive, production-ready cognitive agent system. The framework demonstrates:

1. **Layered Architecture**: Five distinct layers from foundation to application
2. **Event-Driven Processing**: Asynchronous, multi-threaded execution model
3. **Cognitive Capabilities**: Memory, attention, reasoning, and meta-cognition
4. **Integration Ready**: Well-defined boundaries for external system integration
5. **Production Quality**: Thread-safe, tested, and optimized for performance

The architecture supports building sophisticated autonomous agents capable of:
- Natural language conversation with personality
- Knowledge management and semantic search
- Multi-agent coordination and communication
- Self-modification and adaptive behavior
- Web automation and external system integration

**Next Steps:**
- Review formal Z++ specifications in accompanying documents
- Explore implementation details in component-specific documentation
- Reference API documentation for integration guidance
