# Hybrid TypeScript-to-C++ Transpiler Architecture

**Date**: January 10, 2026  
**Purpose**: Integrate ts2cpp (Cheerp-based) with ts_to_cpp_transpiler.py for optimal C++ code generation

---

## Executive Summary

This document outlines the architecture for integrating two complementary TypeScript-to-C++ transpilation tools:

1. **ts2cpp** (Leaningtech) - Generates Cheerp-compatible C++ headers from TypeScript declarations
2. **ts_to_cpp_transpiler.py** - Python-based transpiler that converts TypeScript implementations to C++

The hybrid system leverages the strengths of both tools to produce optimal C++ code for the ElizaOS project.

---

## Tool Comparison

### ts2cpp (Leaningtech)

**Strengths**:
- ✅ Generates clean, standards-compliant C++ headers
- ✅ Excellent type mapping (TS → C++)
- ✅ Cheerp integration for WebAssembly
- ✅ Fast compilation (~630ms)
- ✅ Handles interfaces, classes, enums perfectly

**Limitations**:
- ❌ Only generates declarations (no implementations)
- ❌ Limited nested namespace support
- ❌ No logic translation
- ❌ Cheerp-specific output (requires Cheerp runtime)

**Best For**:
- Type definition files (.d.ts)
- API interfaces
- WebAssembly builds
- Type consistency checking

### ts_to_cpp_transpiler.py

**Strengths**:
- ✅ Generates both headers (.hpp) and implementations (.cpp)
- ✅ Converts function bodies and logic
- ✅ Handles statements, expressions, control flow
- ✅ Pattern recognition for TypeScript idioms
- ✅ Parallel processing support
- ✅ Validation and error checking

**Limitations**:
- ❌ Generated code requires manual fixes
- ❌ Experimental/approximate conversion
- ❌ May leak TypeScript patterns
- ❌ Less polished type mappings

**Best For**:
- Full TypeScript file conversion
- Implementation logic translation
- Rapid prototyping
- Native C++ builds (no Cheerp)

---

## Hybrid Architecture Design

### Strategy: Two-Stage Transpilation Pipeline

```
Stage 1: Type Definitions (ts2cpp)
  Input: TypeScript declarations (.d.ts)
  Output: Cheerp C++ headers (.h)
  Purpose: Generate clean type definitions

Stage 2: Implementations (ts_to_cpp_transpiler.py)
  Input: TypeScript implementations (.ts)
  Output: Native C++ headers (.hpp) + implementations (.cpp)
  Purpose: Convert logic and implementations

Stage 3: Integration & Optimization
  Merge: Combine best of both outputs
  Validate: Check consistency
  Optimize: Apply best practices
```

### Workflow

```
TypeScript Source Files
         │
         ├─────────────────┬─────────────────┐
         │                 │                 │
         ▼                 ▼                 ▼
    .d.ts files       .ts files      .tsx files
         │                 │                 │
         │                 │                 │
    [ts2cpp]         [ts_to_cpp]      [ts_to_cpp]
         │                 │                 │
         ▼                 ▼                 ▼
   Cheerp .h         Native .hpp        Native .cpp
         │                 │                 │
         └─────────────────┴─────────────────┘
                           │
                           ▼
              [Hybrid Integration Layer]
                           │
                           ▼
                 Optimal C++ Output
                    (.hpp + .cpp)
```

---

## Integration Points

### 1. Type Definition Generation

**Use ts2cpp for**:
- Core type definitions
- Interface contracts
- API boundaries
- Shared type libraries

**Process**:
```bash
# Generate .d.ts from TypeScript
tsc --declaration --emitDeclarationOnly

# Convert to Cheerp headers
ts2cpp --pretty types.d.ts -o types.h

# Extract type definitions for native C++
python extract_types.py types.h > types.hpp
```

### 2. Implementation Generation

**Use ts_to_cpp_transpiler.py for**:
- Function implementations
- Class method bodies
- Logic translation
- Native C++ builds

**Process**:
```bash
# Convert TypeScript implementations
python ts_to_cpp_transpiler.py \
  --input-dir ./src \
  --output-dir ./cpp \
  --parallel \
  --max-workers 8
```

### 3. Hybrid Integration

**Combine outputs**:
- Use ts2cpp types as reference
- Apply to ts_to_cpp_transpiler.py output
- Validate consistency
- Optimize generated code

---

## Implementation Plan

### Phase 1: Wrapper Script

Create `hybrid_transpiler.py` that orchestrates both tools:

```python
class HybridTranspiler:
    def __init__(self, input_dir, output_dir):
        self.ts2cpp_path = "/path/to/ts2cpp"
        self.python_transpiler = TypeScriptToCppTranspiler(...)
    
    def transpile(self):
        # Stage 1: Generate type definitions with ts2cpp
        self.generate_type_definitions()
        
        # Stage 2: Generate implementations with Python transpiler
        self.generate_implementations()
        
        # Stage 3: Merge and optimize
        self.merge_outputs()
```

### Phase 2: Type Consistency Checker

Ensure types match between both outputs:

```python
def validate_type_consistency(ts2cpp_header, python_header):
    """Compare type definitions from both transpilers"""
    ts2cpp_types = extract_types(ts2cpp_header)
    python_types = extract_types(python_header)
    
    return compare_types(ts2cpp_types, python_types)
```

### Phase 3: Optimization Layer

Apply best practices from both tools:

```python
def optimize_output(ts2cpp_types, python_impl):
    """Combine best type definitions with best implementations"""
    # Use ts2cpp types (cleaner)
    # Use Python transpiler implementations
    # Merge into optimal C++
    return optimized_cpp
```

---

## File Organization

```
elizaos-cpp/
├── types/                    # TypeScript type definitions
│   ├── agent.d.ts
│   ├── memory.d.ts
│   └── knowledge.d.ts
├── cpp/                      # Generated C++ (hybrid output)
│   ├── include/
│   │   ├── agent.hpp        # Optimized headers
│   │   ├── memory.hpp
│   │   └── knowledge.hpp
│   └── src/
│       ├── agent.cpp        # Implementations
│       ├── memory.cpp
│       └── knowledge.cpp
├── cheerp/                   # Cheerp-specific output
│   ├── agent.h              # ts2cpp output
│   ├── memory.h
│   └── knowledge.h
├── excpp/                    # Python transpiler output
│   ├── agent.hpp
│   ├── agent.cpp
│   └── ...
└── tools/
    ├── hybrid_transpiler.py  # Main integration script
    ├── type_extractor.py     # Extract types from Cheerp headers
    ├── consistency_checker.py # Validate type consistency
    └── optimizer.py          # Optimize merged output
```

---

## Usage Examples

### Basic Usage

```bash
# Run hybrid transpiler
python hybrid_transpiler.py \
  --input-dir ./src \
  --output-dir ./cpp \
  --mode hybrid \
  --verbose
```

### Type Definitions Only

```bash
# Generate only type definitions (ts2cpp)
python hybrid_transpiler.py \
  --input-dir ./types \
  --output-dir ./cpp/include \
  --mode types-only
```

### Implementations Only

```bash
# Generate only implementations (Python transpiler)
python hybrid_transpiler.py \
  --input-dir ./src \
  --output-dir ./cpp \
  --mode impl-only
```

### Full Pipeline with Validation

```bash
# Full pipeline with consistency checking
python hybrid_transpiler.py \
  --input-dir ./src \
  --output-dir ./cpp \
  --mode hybrid \
  --validate \
  --optimize \
  --parallel
```

---

## Benefits of Hybrid Approach

### 1. Best of Both Worlds

- ✅ Clean type definitions from ts2cpp
- ✅ Full implementation conversion from Python transpiler
- ✅ Cheerp compatibility for WebAssembly
- ✅ Native C++ for performance-critical code

### 2. Flexibility

- Choose transpiler based on use case
- Generate Cheerp or native C++
- Incremental adoption
- Fallback options

### 3. Quality Assurance

- Cross-validate types between transpilers
- Detect inconsistencies early
- Automated optimization
- Validation warnings

### 4. Maintainability

- Centralized transpilation logic
- Consistent output structure
- Easy to update/extend
- Well-documented process

---

## Challenges and Solutions

### Challenge 1: Type Mapping Differences

**Problem**: ts2cpp uses Cheerp types, Python transpiler uses STL types

**Solution**: 
- Create type mapping layer
- Convert Cheerp types to STL equivalents
- Maintain compatibility matrix

### Challenge 2: Namespace Handling

**Problem**: ts2cpp has limited nested namespace support

**Solution**:
- Flatten namespaces for ts2cpp
- Use Python transpiler for complex namespaces
- Merge with proper namespace structure

### Challenge 3: Implementation Quality

**Problem**: Python transpiler generates approximate code

**Solution**:
- Use ts2cpp types as reference
- Apply validation rules
- Manual review for critical code
- Iterative refinement

---

## Roadmap

### Short-term (1-2 weeks)

- [x] Analyze both transpilers
- [x] Design hybrid architecture
- [ ] Implement wrapper script
- [ ] Test on ElizaOS types
- [ ] Document usage

### Medium-term (1 month)

- [ ] Type consistency checker
- [ ] Optimization layer
- [ ] Automated testing
- [ ] CI/CD integration
- [ ] Performance benchmarks

### Long-term (3 months)

- [ ] Full ElizaOS conversion
- [ ] WebAssembly builds
- [ ] Plugin architecture
- [ ] Documentation generation
- [ ] Community adoption

---

## Conclusion

The hybrid transpiler architecture combines the strengths of ts2cpp (clean type definitions) with ts_to_cpp_transpiler.py (full implementation conversion) to produce optimal C++ code for ElizaOS.

This approach provides:
- **Flexibility** - Choose the right tool for each task
- **Quality** - Leverage best features of both transpilers
- **Maintainability** - Centralized, well-documented process
- **Scalability** - Supports both Cheerp and native C++ builds

**Next Steps**: Implement the hybrid wrapper script and test on ElizaOS core types.

---

**Document Version**: 1.0  
**Author**: Manus AI  
**Status**: Design Complete, Ready for Implementation
