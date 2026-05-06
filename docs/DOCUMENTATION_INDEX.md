# ElizaOS C++ Documentation Index

**Version:** 1.0.0  
**Last Updated:** December 12, 2024

## Overview

This document provides a comprehensive index to all ElizaOS C++ documentation, including architecture documentation, formal specifications, implementation guides, and API references.

---

## Quick Links

| Document | Description | Location |
|----------|-------------|----------|
| **Architecture Overview** | Comprehensive system architecture with diagrams | [`architecture_overview.md`](./architecture_overview.md) |
| **Formal Specifications** | Z++ mathematical specifications | [`formal_specs/`](./formal_specs/) |
| **README** | Project overview and quick start | [`../README.md`](../README.md) |
| **Technical Architecture** | Existing technical documentation | [`../TECH_ARCHITECTURE.md`](../TECH_ARCHITECTURE.md) |
| **Implementation Roadmap** | Development status and plans | [`../IMPLEMENTATION_ROADMAP.md`](../IMPLEMENTATION_ROADMAP.md) |

---

## Documentation Categories

### 1. Architecture & Design

#### Primary Documentation

**[Architecture Overview](./architecture_overview.md)** (★ Start Here)
- **What it covers**: Complete system architecture with visual diagrams
- **Key sections**:
  - High-level system architecture (5-layer model)
  - Component architecture and interactions
  - Data flow diagrams
  - Integration boundaries
  - Cognitive processing pipeline
  - Memory and attention systems
  - Concurrency and threading model
  - API surface and interfaces
- **When to use**: 
  - Understanding overall system design
  - Planning new features
  - Integration planning
  - Onboarding new team members
- **Format**: Markdown with Mermaid diagrams
- **Size**: 22KB, comprehensive reference

#### Supporting Documentation

**[Technical Architecture](../TECH_ARCHITECTURE.md)**
- Original technical architecture document
- Cognitive subsystems breakdown
- Meta-cognitive commentary
- Design philosophy

**[README](../README.md)**
- Project overview and status
- Quick start guide
- Feature highlights
- Build instructions
- 80% completion status report

---

### 2. Formal Specifications

#### Overview

The **[Formal Specifications](./formal_specs/)** directory contains rigorous mathematical specifications using Z++ notation. These provide a precise, unambiguous definition of system behavior.

#### Specification Files

**[Data Model](./formal_specs/data_model.zpp)** (14.5KB)
- **Purpose**: Formalize all data structures and types
- **Contents**:
  - Basic types (UUID, timestamps, embeddings)
  - Enumerated types (memory types, statuses)
  - Metadata structures
  - Core data structures (Memory, Message, TruthValue)
  - Hypergraph structures
  - Character and personality models
  - Task and conversation structures
  - Data invariants and constraints
- **Key schemas**: 50+ formal schema definitions
- **Use cases**: 
  - Reference for data structure design
  - Validation of implementations
  - Test case generation

**[System State](./formal_specs/system_state.zpp)** (17KB)
- **Purpose**: Define complete system state
- **Contents**:
  - Agent state and loop state
  - Memory system state
  - Communication system state
  - Knowledge system state
  - Hypergraph state with attention
  - Task management state
  - Global system state
  - System invariants
- **Key schemas**: Agent state composition, system consistency
- **Use cases**:
  - Understanding system state composition
  - State management implementation
  - Debugging state inconsistencies

**[Operations](./formal_specs/operations.zpp)** (19.4KB)
- **Purpose**: Specify all system operations
- **Contents**:
  - Agent lifecycle operations
  - Memory CRUD operations
  - Communication operations
  - Conversation processing
  - Task management
  - Knowledge operations
  - Attention allocation
- **Key specifications**: 15+ operation schemas with pre/postconditions
- **Use cases**:
  - Operation implementation guide
  - API contract reference
  - Correctness verification

**[Integration Contracts](./formal_specs/integrations.zpp)** (17.2KB)
- **Purpose**: Define external system integration contracts
- **Contents**:
  - LLM service integration (OpenAI, Anthropic, etc.)
  - Embedding service contracts
  - Vector database operations
  - Web automation integration
  - Event handlers and webhooks
  - Error handling and retry logic
  - Rate limiting specifications
- **Key specifications**: 20+ integration contracts
- **Use cases**:
  - External API integration
  - Error handling implementation
  - Performance tuning

**[Specification README](./formal_specs/README.md)** (10.9KB)
- **Purpose**: Guide to reading and using formal specifications
- **Contents**:
  - Z++ notation quick reference
  - Reading guide for schemas
  - Usage instructions for different roles
  - Tool support information
  - Mathematical properties
- **Audience**: Developers, architects, testers, researchers
- **Use cases**: Learning Z++ notation, understanding specifications

#### Using the Formal Specifications

**For Developers:**
- Reference for exact semantics during implementation
- Source for test case generation
- Debugging guide for invariant violations

**For Architects:**
- Blueprint for system design decisions
- Integration contract reference
- Consistency verification tool

**For Testers:**
- Test case design from constraints
- Invariant testing scenarios
- Property-based testing foundation

**For Researchers:**
- Input for formal verification
- Model checking specifications
- Correctness proof basis

---

### 3. Implementation Guides

**[Implementation Roadmap](../IMPLEMENTATION_ROADMAP.md)**
- Current implementation status (80% complete)
- Module-by-module breakdown
- Priority recommendations
- Next steps

**[Completeness Report](../COMPLETENESS_REPORT.md)**
- Detailed analysis of 44 modules
- 35/44 modules fully implemented
- Code metrics and test coverage
- What works today

**[Status Report](../STATUS_REPORT.md)**
- Build and test status
- Current capabilities
- Known issues

---

### 4. Code-Level Documentation

#### Header Files

All public APIs are documented in header files under `include/elizaos/`:

**Core Infrastructure:**
- `core.hpp` - Core data structures (State, Memory, Task, Hypergraph)
- `agentloop.hpp` - Event-driven agent loop system
- `agentmemory.hpp` - Memory management with semantic search
- `agentcomms.hpp` - Inter-agent communication
- `agentlogger.hpp` - Logging and monitoring
- `agentshell.hpp` - Shell interface

**Application Components:**
- `eliza.hpp` - Conversation engine with pattern matching
- `characters.hpp` - Personality and trait system
- `knowledge.hpp` - Knowledge base management
- `agentaction.hpp` - Action processing and validation
- `agentagenda.hpp` - Task scheduling and orchestration
- `agentbrowser.hpp` - Web automation capabilities

**Advanced Features:**
- `evolutionary.hpp` - Evolutionary learning algorithms
- `embodiment.hpp` - Sensory/motor interfaces
- `attention.hpp` - Attention allocation mechanisms

---

## Documentation by Use Case

### Getting Started

1. **First-time setup**: Start with [`README.md`](../README.md)
   - Project overview
   - Prerequisites
   - Build instructions
   - Quick start example

2. **Understanding the architecture**: Read [`architecture_overview.md`](./architecture_overview.md)
   - System layers and components
   - Data flow
   - Key design patterns

3. **Exploring capabilities**: Check [`COMPLETENESS_REPORT.md`](../COMPLETENESS_REPORT.md)
   - What's implemented
   - What works today
   - Module descriptions

### Development

1. **Implementing a new feature**:
   - Check formal specifications for relevant operations
   - Review architecture to find appropriate layer
   - Examine related header files
   - Follow implementation patterns

2. **Debugging an issue**:
   - Review system state specifications
   - Check invariants in formal specs
   - Examine operation preconditions/postconditions
   - Use logging system

3. **Integration work**:
   - Start with integration contracts specification
   - Review error handling patterns
   - Check rate limiting specifications
   - Examine existing integration code

### Architecture & Design

1. **Planning architectural changes**:
   - Review architecture overview
   - Check system state specifications
   - Validate against formal invariants
   - Update documentation

2. **API design**:
   - Reference operations specification
   - Check data model schemas
   - Ensure consistency with existing APIs
   - Document contracts

3. **Performance optimization**:
   - Review concurrency model
   - Check resource constraints in specifications
   - Examine threading architecture
   - Profile and measure

### Testing & Verification

1. **Writing tests**:
   - Derive test cases from formal specifications
   - Check preconditions for boundary conditions
   - Validate postconditions
   - Test invariants

2. **Integration testing**:
   - Use integration contracts as reference
   - Test error conditions
   - Verify retry logic
   - Check rate limiting

3. **Formal verification**:
   - Use Z++ specifications as input
   - Define properties to prove
   - Apply theorem provers
   - Document results

---

## Diagram Index

### Architecture Overview Diagrams

1. **High-Level System Architecture** - 5-layer cognitive architecture
2. **Core Components Interaction** - Subsystem communication patterns
3. **Module Dependencies** - Dependency graph of all modules
4. **Message Processing Flow** - Sequence diagram for message handling
5. **Agent Loop Execution Flow** - Control flow in agent loop
6. **Memory Storage and Retrieval** - Data flow in memory system
7. **External System Integration** - Integration boundaries
8. **Component Boundary Diagram** - C4 context diagram
9. **Cognitive Architecture** - Cognitive processing layers
10. **Attention Allocation Mechanism** - ECAN-inspired attention flow
11. **Memory System Architecture** - Memory subsystem components
12. **Hypergraph Knowledge Representation** - Knowledge graph structure
13. **Thread Architecture** - Threading and concurrency model
14. **Thread Safety Mechanisms** - Synchronization primitives
15. **Core API Classes** - Class diagram for public APIs
16. **Interface Contracts** - Interface definitions
17. **Build System** - CMake build flow
18. **Deployment Model** - Development to production pipeline

---

## Documentation Standards

### Writing New Documentation

When adding new documentation to ElizaOS C++, follow these standards:

**Structure:**
- Use clear hierarchical headings (H1, H2, H3)
- Include table of contents for documents > 1000 lines
- Add "last updated" date and version
- Cross-reference related documents

**Diagrams:**
- Use Mermaid for all diagrams when possible
- Include alt text and descriptions
- Keep diagrams focused and readable
- Update diagrams when architecture changes

**Formal Specifications:**
- Follow Z++ notation conventions
- Include natural language descriptions
- Document all invariants
- Add examples for complex schemas

**Code Documentation:**
- Document all public APIs in headers
- Include usage examples
- Document preconditions and postconditions
- Note thread safety requirements

**Maintenance:**
- Update documentation with code changes
- Version documentation with releases
- Archive obsolete documentation
- Link to related issues/PRs

---

## Contribution Guidelines

### Documentation Contributions

We welcome documentation contributions! Please:

1. **Check existing docs** before creating new ones
2. **Follow the standards** outlined above
3. **Test examples** to ensure they work
4. **Update this index** when adding new docs
5. **Get review** from maintainers
6. **Update formal specs** if changing APIs

### Review Process

Documentation changes undergo review for:
- Technical accuracy
- Completeness
- Clarity and readability
- Consistency with existing docs
- Format compliance

---

## Tools and Resources

### Documentation Tools

**Diagram Creation:**
- [Mermaid Live Editor](https://mermaid.live) - Interactive Mermaid editor
- [PlantUML](https://plantuml.com) - Alternative diagramming tool

**Formal Methods:**
- [CZT (Community Z Tools)](http://czt.sourceforge.net) - Z type checker
- [Z/EVES](https://www.orcacontrols.com/zeves/) - Z theorem prover
- [ProofPower-Z](http://www.lemma-one.com/ProofPower) - Formal proof tool

**Markdown Tools:**
- [Markdown Preview Enhanced](https://shd101wyy.github.io/markdown-preview-enhanced/) - VS Code extension
- [grip](https://github.com/joeyespo/grip) - GitHub-flavored Markdown preview

### External References

**C++ Best Practices:**
- [C++ Core Guidelines](https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines)
- [Google C++ Style Guide](https://google.github.io/styleguide/cppguide.html)

**Formal Methods:**
- "The Z Notation: A Reference Manual" by J.M. Spivey
- "Using Z: Specification, Refinement, and Proof" by Woodcock & Davies

**Cognitive Architecture:**
- [OpenCog](https://opencog.org) - AGI research platform
- [ECAN](https://wiki.opencog.org/w/ECAN) - Economic Attention Networks

---

## FAQ

**Q: Where do I start if I'm new to the project?**  
A: Start with the [README](../README.md) for overview, then read [Architecture Overview](./architecture_overview.md) to understand the system design.

**Q: Do I need to understand Z++ to use ElizaOS?**  
A: No, but the formal specifications provide precise definitions useful for complex features. The [Specification README](./formal_specs/README.md) has a quick reference guide.

**Q: How do I know which layer my feature belongs in?**  
A: Review the [Architecture Overview](./architecture_overview.md) section on the 5-layer model. Features generally go in the Processing or Application layers.

**Q: Where are the API docs?**  
A: API documentation is in the header files under `include/elizaos/`. Each public class and function is documented inline.

**Q: How do I update the formal specifications?**  
A: Modify the relevant `.zpp` file in `docs/formal_specs/` and ensure consistency across all specs. Get review from maintainers.

**Q: What's the difference between TECH_ARCHITECTURE.md and architecture_overview.md?**  
A: TECH_ARCHITECTURE.md is the original design document. architecture_overview.md is the new comprehensive reference with detailed diagrams.

---

## Version History

| Version | Date | Changes |
|---------|------|---------|
| 1.0.0 | 2024-12-12 | Initial comprehensive documentation package |
|  |  | - Added architecture_overview.md with 15+ diagrams |
|  |  | - Added complete Z++ formal specifications |
|  |  | - Created documentation index and guides |

---

## Contact

For questions about documentation:
- **Issues**: [GitHub Issues](https://github.com/o9nn/elizaos-cpp/issues)
- **Discussions**: [GitHub Discussions](https://github.com/o9nn/elizaos-cpp/discussions)
- **Email**: Contact repository maintainers

---

**Maintained by**: ElizaOS C++ Contributors  
**License**: MIT  
**Repository**: https://github.com/o9nn/elizaos-cpp
