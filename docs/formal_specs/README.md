# ElizaOS C++ Formal Specifications

## Overview

This directory contains rigorous formal specifications for the ElizaOS C++ framework using the Z++ specification language. These specifications provide a mathematically precise model of the system's behavior, data structures, operations, and integration contracts.

## Purpose

The formal specifications serve multiple purposes:

1. **Precise Documentation**: Unambiguous description of system behavior
2. **Verification**: Formal basis for proving correctness properties
3. **Implementation Guide**: Reference for developers implementing features
4. **Contract Definition**: Clear API contracts and invariants
5. **Testing Reference**: Foundation for comprehensive test suites

## Specification Files

### 1. Data Model (`data_model.zpp`)

Formalizes the core data structures and types:

- **Basic Types**: UUIDs, timestamps, embeddings, content
- **Enumerated Types**: Memory types, message types, task statuses
- **Metadata Structures**: Base metadata, specialized metadata variants
- **Core Structures**: Memory, Message, TruthValue, Hypergraph nodes/edges
- **Character System**: Personality matrix, emotional state, traits, profiles
- **Task System**: Actors, goals, tasks
- **Conversation System**: Turns, contexts, response patterns
- **Data Invariants**: Uniqueness, referential integrity, temporal consistency

**Key Features:**
- Complete type definitions with constraints
- Invariants for data integrity
- Support for hypergraph knowledge representation
- ECAN-inspired attention allocation

### 2. System State (`system_state.zpp`)

Formalizes the complete system state:

- **Agent State**: Individual agent configuration and runtime state
- **Agent Loop State**: Execution state for event loops
- **Memory System State**: Storage, indexing, and retrieval state
- **Communication System State**: Channels, servers, agents, messages
- **Knowledge System State**: Knowledge base entries and indices
- **Hypergraph State**: Knowledge graph with attention values
- **Task Management State**: Task queue, dependencies, execution
- **Global System State**: Complete ElizaOS system state
- **System Invariants**: Consistency, resource constraints, thread safety

**Key Features:**
- Layered state composition
- Cross-subsystem consistency invariants
- Resource management constraints
- Initial state definition

### 3. Operations (`operations.zpp`)

Formalizes system operations as state transformations:

**Agent Lifecycle:**
- `CreateAgent`: Initialize new agent
- `StartAgentLoop`: Begin event loop execution
- `StopAgentLoop`: Halt event loop

**Memory Operations:**
- `CreateMemory`: Store new memory with metadata
- `GetMemoryById`: Retrieve specific memory
- `SearchMemoriesByEmbedding`: Semantic similarity search
- `DeleteMemory`: Remove memory from system

**Communication Operations:**
- `RegisterAgent`: Add agent to communication system
- `SendMessage`: Direct agent-to-agent messaging
- `BroadcastMessage`: Channel-wide messaging

**Conversation Operations:**
- `ProcessMessage`: Eliza conversation processing

**Task Operations:**
- `CreateTask`: Define new task
- `StartTask`: Begin task execution
- `CompleteTask`: Mark task as completed

**Knowledge Operations:**
- `AddKnowledgeEntry`: Store knowledge
- `QueryKnowledgeByCategory`: Retrieve knowledge

**Attention Operations:**
- `UpdateAttention`: ECAN attention spreading
- `ForgetLowAttention`: Prune low-importance items

**Key Features:**
- Preconditions and postconditions
- State transformation specifications
- Helper function definitions
- Error handling specifications

### 4. Integration Contracts (`integrations.zpp`)

Formalizes external system integration:

**LLM Service Integration:**
- Request/response structures for OpenAI, Anthropic, etc.
- Prompt engineering parameters
- Token usage tracking

**Embedding Service Integration:**
- Batch embedding generation
- Dimension consistency
- Model specification

**Vector Database Integration:**
- Insert, search, update, delete operations
- Similarity search with filters
- Index management

**Web Automation Integration:**
- Browser action execution
- Content extraction
- Error handling

**Event Handler Integration:**
- Webhook processing
- Event validation
- Signature verification

**Error Handling:**
- Retry policies with exponential backoff
- Rate limiting (token bucket algorithm)
- Circuit breaker patterns
- Health monitoring

**Key Features:**
- Complete API contracts
- Error recovery strategies
- Performance constraints
- Security considerations

## Reading the Specifications

### Z++ Notation Quick Reference

**Basic Symbols:**
- `ℙ A`: Power set of A (set of all subsets)
- `A ⇸ B`: Partial function from A to B
- `A ↔ B`: Relation between A and B
- `seq A`: Sequence of elements from A
- `A ⊎ B`: Disjoint union (sum type)
- `ℕ`: Natural numbers (0, 1, 2, ...)
- `ℤ`: Integers
- `ℝ`: Real numbers
- `𝔹`: Booleans (true, false)

**Logical Symbols:**
- `∧`: Logical AND
- `∨`: Logical OR
- `⇒`: Implies
- `⇔`: If and only if
- `¬`: Logical NOT
- `∀`: For all (universal quantifier)
- `∃`: There exists (existential quantifier)

**Set Operations:**
- `∈`: Element of
- `∉`: Not element of
- `⊆`: Subset
- `∪`: Union
- `∩`: Intersection
- `\`: Set difference
- `#A`: Cardinality (size) of set A
- `∅`: Empty set

**Schema Notation:**
- `schema Name`: Defines a state schema
- `ΔName`: Schema with before/after states (name and name')
- `ΞName`: Schema with unchanged state
- `name?`: Input parameter
- `name!`: Output parameter
- `name'`: After-state value

### Reading a Schema

```zpp
schema CreateMemory
  ΔElizaOSSystemState     -- State changes
  memory?: Memory          -- Input parameter
  tableName?: Content      -- Input parameter
  memoryId!: UUID          -- Output parameter
where
  -- Preconditions (must be true before)
  tableName? ∈ dom (memorySystem.tables)
  
  -- Postconditions (must be true after)
  memorySystem' = memorySystem⟨
    totalMemories := memorySystem.totalMemories + 1
  ⟩
end
```

**Interpretation:**
1. Operation name: `CreateMemory`
2. State change: System state changes (Δ)
3. Inputs: memory data, table name
4. Output: new memory ID
5. Preconditions: table must exist
6. Postconditions: memory count increases

## Using the Specifications

### For Developers

1. **Implementation Reference**: Use specifications to understand exact semantics
2. **Test Generation**: Derive test cases from preconditions and postconditions
3. **Bug Analysis**: Check if implementation violates invariants
4. **API Design**: Ensure APIs match specified contracts

### For Architects

1. **System Design**: Use as blueprint for system architecture
2. **Integration Planning**: Reference integration contracts
3. **Consistency Verification**: Check cross-component invariants
4. **Performance Analysis**: Identify complexity from formal definitions

### For Testers

1. **Test Case Design**: Derive boundary conditions from constraints
2. **Invariant Testing**: Verify state invariants hold
3. **Integration Testing**: Validate external service contracts
4. **Property-Based Testing**: Use specifications for property definitions

### For Researchers

1. **Formal Verification**: Use as input for theorem provers
2. **Model Checking**: Verify system properties mechanically
3. **Refinement**: Show implementation refines specifications
4. **Correctness Proofs**: Prove system properties formally

## Validation and Consistency

### Internal Consistency

All specifications have been validated for:
- **Type consistency**: All types are well-defined and used correctly
- **Reference consistency**: All referenced schemas exist
- **Invariant feasibility**: Invariants can be satisfied
- **Operation completeness**: All state changes are specified

### Correspondence with Implementation

The specifications correspond to the C++ implementation in:
- `include/elizaos/core.hpp`: Data model structures
- `include/elizaos/agentloop.hpp`: Agent loop operations
- `include/elizaos/agentmemory.hpp`: Memory operations
- `include/elizaos/agentcomms.hpp`: Communication operations
- `include/elizaos/eliza.hpp`: Conversation processing
- `include/elizaos/characters.hpp`: Character system
- `cpp/*/src/*.cpp`: Operation implementations

## Mathematical Properties

The specifications define several important properties:

### Safety Properties

1. **Type Safety**: All operations preserve type invariants
2. **Memory Safety**: No dangling references or leaks (modeled)
3. **Temporal Safety**: Time ordering always preserved
4. **Uniqueness Safety**: IDs remain unique across system

### Liveness Properties

1. **Progress**: Operations eventually complete or fail
2. **Fairness**: All agents eventually execute
3. **Responsiveness**: Messages eventually delivered
4. **Termination**: Loops can always be stopped

### Consistency Properties

1. **State Consistency**: Invariants maintained across operations
2. **Referential Integrity**: All references valid
3. **Temporal Consistency**: Timestamps properly ordered
4. **Resource Consistency**: Resource limits enforced

## Tool Support

The Z++ specifications can be processed by:

1. **Z/EVES**: Interactive theorem prover for Z specifications
2. **CZT (Community Z Tools)**: Type checking and animation
3. **ProofPower-Z**: Formal proof development
4. **Fuzz**: Type checker and animator

## Future Work

Planned extensions to the specifications:

1. **Performance Specifications**: Add timing and resource bounds
2. **Concurrency Refinement**: Detailed thread synchronization
3. **Security Properties**: Formal security guarantees
4. **Probabilistic Extensions**: Model uncertainty and learning
5. **Proof Development**: Mechanized correctness proofs

## References

### Z++ and Formal Methods

- **Z Notation**: J.M. Spivey, "The Z Notation: A Reference Manual"
- **Z++**: Object-oriented extensions to Z
- **Formal Methods**: Woodcock & Davies, "Using Z"

### Related Specifications

- **OpenCog**: AtomSpace formal semantics
- **ECAN**: Economic Attention Networks
- **PLN**: Probabilistic Logic Networks

### ElizaOS Resources

- **Architecture Documentation**: `../architecture_overview.md`
- **Implementation Guide**: `../IMPLEMENTATION_GUIDE.md`
- **Technical Architecture**: `../../TECH_ARCHITECTURE.md`

## Contact and Contributions

For questions about the formal specifications:

1. **Issues**: Open issue on GitHub repository
2. **Discussions**: Use GitHub Discussions
3. **Email**: Contact maintainers
4. **Wiki**: Check project wiki for additional examples

Contributions to the formal specifications are welcome! Please:
- Maintain consistency with existing notation
- Add comprehensive comments
- Validate against implementation
- Update this README for significant changes

---

**Last Updated**: December 12, 2024  
**Specification Version**: 1.0.0  
**Framework Version**: ElizaOS C++ 1.0.0
