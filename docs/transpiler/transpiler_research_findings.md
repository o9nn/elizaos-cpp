# TypeScript-to-C++ Transpiler Research Findings

**Date**: January 10, 2026  
**Purpose**: Find tools to address weak points in ElizaOS hybrid transpiler

---

## Tools Discovered

### 1. TypeScript2Cxx (ASDAlexander77)

**Repository**: https://github.com/ASDAlexander77/TypeScript2Cxx  
**Stars**: 703  
**License**: MIT  
**Status**: Active (978 commits, last update 3 years ago)

#### Key Features

- **Full implementation generation** (not just headers)
- **Custom C++ runtime library** (`cpplib/`)
- **Supports C++20**
- **Generates both .h and .cpp files**
- **Custom type system** with `js::` namespace

#### Type System

```cpp
// Uses custom types from cpplib
using namespace js;
- string (custom string type)
- any (custom any type)
- std::shared_ptr<T> for objects
- std::enable_shared_from_this for classes
```

#### Example Output Quality

**Input TypeScript**:
```typescript
class Employee extends Person {
    private department: string;
    constructor(name: string, department: string) {
        super(name);
        this.department = department;
    }
    public get ElevatorPitch() {
        return `Hello, my name is ${this.name} and I work in ${this.department}.`;
    }
}
```

**Output C++**:
```cpp
class Employee : public Person, public std::enable_shared_from_this<Employee> {
public:
    string department;
    Employee(string name, string department);
    virtual any get_ElevatorPitch();
};

any Employee::get_ElevatorPitch() {
    return "Hello, my name is "_S + this->name + " and I work in "_S + this->department + "."_S;
}
```

#### Strengths

✅ **Complete implementations** - Not just type definitions  
✅ **String interpolation** - Handles template literals  
✅ **Inheritance** - Proper class hierarchy  
✅ **Getters/setters** - Property accessors  
✅ **Custom runtime** - Consistent type system  

#### Weaknesses

⚠️ **Custom types** - Not standard C++ STL  
⚠️ **Requires cpplib** - Dependency on custom library  
⚠️ **Limited documentation** - No feature list  
⚠️ **Old commits** - Last update 3 years ago  

#### Potential Integration

**Use Case**: Implementation generation  
**Integration Point**: Stage 2 of hybrid transpiler  
**Benefit**: Better implementation quality than Python transpiler  

---

### 2. Compilets (compilets/compilets)

**Repository**: https://github.com/compilets/compilets  
**Stars**: 126  
**License**: GPLv3 (with linking exception)  
**Status**: Active (243 commits, recent updates)

#### Key Features

- **Native executables** - Produces standalone binaries
- **Node.js modules** - Can create native Node addons
- **Oilpan GC** - Uses Chromium's C++ garbage collector
- **TypeScript compiler API** - Built on official TS compiler
- **GN build system** - Google's build tool
- **Strict null checks required** - Type safety enforced

#### Architecture

```
TypeScript → TypeScript Compiler API → C++ Code → GN Build → Native Binary/Module
```

#### Type System

- Uses **Oilpan (cppgc)** for garbage collection
- Requires `strictNullChecks: true`
- Rejects dynamic JavaScript features (`prototype`, `any`)
- Performance-focused (only efficient C++ representations)

#### CLI Usage

```bash
npm install -g compilets
compilets gen          # Generate C++ project
compilets build        # Build native executable
compilets gn-gen       # Run GN build system
```

#### Project Configuration

**package.json**:
```json
{
  "name": "download",
  "compilets": {
    "main": "lib.ts",      // Creates native module
    "bin": {
      "download": "cli.ts"  // Creates executable
    }
  }
}
```

#### Strengths

✅ **Native executables** - Full compilation to binary  
✅ **Node.js modules** - Can create addons  
✅ **Modern GC** - Oilpan from Chromium  
✅ **Type safety** - Strict null checks enforced  
✅ **Active development** - Recent commits  
✅ **Documentation** - Design docs and roadmap  

#### Weaknesses

⚠️ **GPLv3 license** - More restrictive (though has linking exception)  
⚠️ **Early stage** - "Very early days", fails on most projects  
⚠️ **Limited features** - Rejects dynamic JavaScript  
⚠️ **Complex build** - Requires GN build system  
⚠️ **No feature list** - Unclear what's supported  

#### Potential Integration

**Use Case**: Native module generation for ElizaOS  
**Integration Point**: New compilation target  
**Benefit**: Could create native ElizaOS modules  

---

### 3. tsembind (ted537/tsembind)

**Repository**: https://github.com/ted537/tsembind  
**Purpose**: Generate TypeScript .d.ts files from Emscripten Embind

#### Key Features

- **Reverse direction** - C++ → TypeScript (not TS → C++)
- **Embind integration** - Works with Emscripten
- **No recompilation** - Works with existing WASM
- **TypeScript definitions** - Generates .d.ts files

#### Strengths

✅ **Complementary tool** - Solves opposite problem  
✅ **Embind support** - Works with existing Emscripten projects  
✅ **Type safety** - Provides TypeScript types for C++  

#### Potential Integration

**Use Case**: Bidirectional type checking  
**Integration Point**: Validation layer  
**Benefit**: Verify generated C++ matches TypeScript types  

---

## Comparison with Current Hybrid Transpiler

### Current Hybrid Transpiler

| Feature | ts2cpp (Cheerp) | Python Transpiler | Hybrid |
|---------|-----------------|-------------------|--------|
| Type definitions | ✅ Excellent | ⚠️ Approximate | ✅ Excellent |
| Implementations | ❌ None | ⚠️ Approximate | ⚠️ Approximate |
| Type system | Cheerp types | Native STL | Native STL |
| Speed | ~630ms/file | 197 files/sec | Fast |
| Nested namespaces | ⚠️ Limited | ⚠️ Limited | ⚠️ Limited |

### TypeScript2Cxx

| Feature | TypeScript2Cxx |
|---------|----------------|
| Type definitions | ✅ Good |
| Implementations | ✅ Complete |
| Type system | Custom (js::) |
| Speed | Unknown |
| Nested namespaces | ✅ Likely supported |
| String interpolation | ✅ Supported |
| Inheritance | ✅ Supported |

### Compilets

| Feature | Compilets |
|---------|-----------|
| Type definitions | ✅ Good |
| Implementations | ✅ Complete |
| Type system | Oilpan GC |
| Speed | Unknown |
| Native executables | ✅ Yes |
| Node.js modules | ✅ Yes |
| Maturity | ⚠️ Early stage |

---

## Weak Points in Current Transpiler

### Identified Weak Points

1. **Implementation Quality** ⚠️
   - Python transpiler generates approximate implementations
   - Requires manual refinement
   - Function bodies often incomplete

2. **Nested Namespaces** ⚠️
   - Limited support in both ts2cpp and Python transpiler
   - Flat namespace structures work better

3. **Complex Generics** ⚠️
   - Basic support only
   - Advanced type manipulation not handled

4. **String Interpolation** ❌
   - Not supported in current transpilers
   - Template literals not converted

5. **Dynamic Features** ❌
   - `any` type handling limited
   - Prototype manipulation not supported

---

## How New Tools Address Weak Points

### TypeScript2Cxx Addresses

✅ **Implementation Quality** - Generates complete implementations  
✅ **String Interpolation** - Handles template literals with `"text"_S` syntax  
✅ **Inheritance** - Proper class hierarchy with `std::enable_shared_from_this`  
✅ **Getters/Setters** - Property accessors work correctly  

❌ **Nested Namespaces** - Unknown (needs testing)  
❌ **Complex Generics** - Unknown (needs testing)  

### Compilets Addresses

✅ **Implementation Quality** - Complete compilation to native code  
✅ **Type Safety** - Enforces strict null checks  
✅ **Native Performance** - Produces optimized binaries  
✅ **Modern GC** - Oilpan garbage collection  

❌ **Dynamic Features** - Explicitly rejects them (by design)  
❌ **Maturity** - Early stage, limited feature support  

---

## Integration Recommendations

### Recommendation 1: Integrate TypeScript2Cxx for Implementations

**Priority**: High  
**Effort**: Medium  
**Impact**: High

**Strategy**:
1. Add TypeScript2Cxx as Stage 2 alternative
2. Use for implementation generation instead of Python transpiler
3. Convert custom `js::` types to native STL types
4. Keep ts2cpp for type definitions

**Benefits**:
- ✅ Complete implementations with function bodies
- ✅ String interpolation support
- ✅ Better inheritance handling
- ✅ Reduced manual refinement

**Challenges**:
- ⚠️ Custom type system needs conversion
- ⚠️ Requires cpplib dependency
- ⚠️ Integration complexity

### Recommendation 2: Use Compilets for Native Modules

**Priority**: Medium  
**Effort**: High  
**Impact**: Medium

**Strategy**:
1. Add compilets as optional compilation target
2. Use for performance-critical ElizaOS modules
3. Create Node.js native addons
4. Separate workflow from main transpiler

**Benefits**:
- ✅ Native performance for critical code
- ✅ Node.js module integration
- ✅ Modern garbage collection
- ✅ Type safety enforcement

**Challenges**:
- ⚠️ GPLv3 license (though has linking exception)
- ⚠️ Early stage maturity
- ⚠️ Complex build system (GN)
- ⚠️ Limited feature support

### Recommendation 3: Use tsembind for Validation

**Priority**: Low  
**Effort**: Low  
**Impact**: Low

**Strategy**:
1. Add tsembind to validation pipeline
2. Generate .d.ts from C++ output
3. Compare with original TypeScript types
4. Catch type mismatches early

**Benefits**:
- ✅ Bidirectional type checking
- ✅ Early error detection
- ✅ Type consistency verification

**Challenges**:
- ⚠️ Requires Emscripten/Embind
- ⚠️ Additional build step

---

## Next Steps

### Immediate (This Week)

1. [ ] Clone and test TypeScript2Cxx locally
2. [ ] Run ElizaOS test files through TypeScript2Cxx
3. [ ] Compare output quality with current transpiler
4. [ ] Analyze cpplib dependencies

### Short-term (This Month)

1. [ ] Design integration architecture for TypeScript2Cxx
2. [ ] Create type conversion layer (js:: → std::)
3. [ ] Implement hybrid mode with TypeScript2Cxx
4. [ ] Test on full ElizaOS codebase

### Medium-term (Next Quarter)

1. [ ] Evaluate compilets for native modules
2. [ ] Create proof-of-concept native ElizaOS module
3. [ ] Benchmark performance improvements
4. [ ] Consider tsembind for validation

---

## Conclusion

**TypeScript2Cxx** is the most promising tool for addressing implementation quality weak points. It generates complete implementations with proper string interpolation and inheritance support.

**Compilets** offers a different approach for native module generation, but is less mature and more complex to integrate.

**Recommended Action**: Integrate TypeScript2Cxx as Stage 2 alternative in hybrid transpiler for improved implementation generation.

---

**Research Completed**: January 10, 2026  
**Next Action**: Test TypeScript2Cxx on ElizaOS files


---

### 4. TSLL (sbip-sg/tsll)

**Repository**: https://github.com/sbip-sg/tsll  
**Stars**: 13  
**License**: Unknown  
**Status**: Active (204 commits, v1.0.0-alpha.11)

#### Key Features

- **LLVM IR generation** - TypeScript → LLVM IR
- **Smart contract verification** - Blockchain focus
- **TypeScript Compiler API** - Built on official TS compiler
- **LLVM TypeScript bindings** - Direct LLVM integration
- **AST visitor pattern** - Clean architecture

#### Architecture

```
TypeScript → TS Compiler API → AST → Visitor → LLVM Builder → LLVM IR → C++ (via LLVM backend)
```

#### Design Approach

- Extracts information from TypeScript AST
- Visits each node with Visitor pattern
- Creates LLVM instructions with Builder
- Generates LLVM IR
- Can use LLVM backend to generate C++

#### Components

```
src/
├── core/
│   ├── Visitor.ts    # AST traversal
│   ├── Builder.ts    # LLVM IR construction
│   └── Debugger.ts   # Debug support
├── common/          # Utilities
├── cli.ts           # CLI entry point
└── converter.ts     # Conversion logic
```

#### Strengths

✅ **LLVM backend** - Can generate optimized C++ via LLVM  
✅ **Clean architecture** - Visitor/Builder pattern  
✅ **TypeScript Compiler API** - Official TS support  
✅ **Active development** - Recent updates  
✅ **Documentation** - Wiki pages available  
✅ **Verification focus** - Smart contract analysis  

#### Weaknesses

⚠️ **LLVM dependency** - Requires LLVM 13.0.0  
⚠️ **Complex setup** - Build LLVM from source  
⚠️ **Limited stars** - Small community (13 stars)  
⚠️ **Alpha stage** - v1.0.0-alpha.11  
⚠️ **Blockchain focus** - May not support all TypeScript features  

#### Potential Integration

**Use Case**: LLVM-based C++ generation  
**Integration Point**: Alternative backend  
**Benefit**: Optimized C++ via LLVM, formal verification  

---

## Additional Tools Discovered

### 5. llvm-bindings (JavaScript/TypeScript LLVM bindings)

**Purpose**: Use LLVM from JavaScript/TypeScript  
**Repository**: https://www.npmjs.com/package/llvm-bindings  

#### Features

- JavaScript/TypeScript bindings for LLVM
- Programmatic LLVM IR generation
- Can be used to build custom transpilers

#### Potential Use

- Build custom LLVM-based transpiler
- Generate optimized C++ via LLVM backend
- Integrate with existing TypeScript tooling

---

### 6. Awesome TypeScript Compilers List

**Repository**: https://github.com/JohnDeved/awesome-typescript-compilers  

A curated list of TypeScript compilers, transpilers, and runtimes including:
- Various TypeScript-to-X transpilers
- Runtime environments
- Compiler tools

Worth exploring for additional options.

---

## Updated Comparison Table

| Tool | Stars | License | Type Defs | Implementations | Type System | Maturity | Special Features |
|------|-------|---------|-----------|-----------------|-------------|----------|------------------|
| **ts2cpp (Cheerp)** | N/A | Commercial | ✅ Excellent | ❌ None | Cheerp | ⭐⭐⭐⭐ | WebAssembly |
| **Python Transpiler** | N/A | MIT | ⚠️ Approximate | ⚠️ Approximate | Native STL | ⭐⭐⭐ | Fast |
| **Hybrid (Current)** | N/A | MIT | ✅ Excellent | ⚠️ Approximate | Native STL | ⭐⭐⭐⭐ | Best of both |
| **TypeScript2Cxx** | 703 | MIT | ✅ Good | ✅ Complete | Custom (js::) | ⭐⭐⭐ | String interpolation |
| **Compilets** | 126 | GPLv3 | ✅ Good | ✅ Complete | Oilpan GC | ⭐⭐ | Native executables |
| **TSLL** | 13 | Unknown | ✅ Good | ✅ LLVM IR | LLVM | ⭐⭐ | LLVM optimization |

---

## Updated Weak Point Analysis

### How TSLL Addresses Weak Points

✅ **Implementation Quality** - Generates LLVM IR (can compile to C++)  
✅ **Optimization** - LLVM backend provides world-class optimization  
✅ **Formal Verification** - Designed for smart contract verification  
✅ **Clean Architecture** - Visitor/Builder pattern  

❌ **Complexity** - Requires LLVM build (complex setup)  
❌ **Maturity** - Alpha stage  
❌ **Direct C++** - Generates IR, not C++ directly  

---

## Updated Integration Recommendations

### Recommendation 4: Explore TSLL for Advanced Use Cases

**Priority**: Low  
**Effort**: High  
**Impact**: Medium-High

**Strategy**:
1. Evaluate TSLL for formal verification needs
2. Use LLVM backend to generate optimized C++
3. Consider for performance-critical modules
4. Leverage for security analysis

**Benefits**:
- ✅ LLVM optimization passes
- ✅ Formal verification capabilities
- ✅ Clean architecture
- ✅ TypeScript Compiler API

**Challenges**:
- ⚠️ Complex LLVM setup
- ⚠️ Alpha stage maturity
- ⚠️ Indirect C++ generation (via LLVM)
- ⚠️ Small community

**When to Use**:
- Performance-critical code
- Security-sensitive modules
- Formal verification needed
- Optimization important

---

## Final Recommendations (Updated)

### Priority 1: TypeScript2Cxx Integration (High Priority)

**Best for**: Immediate improvement of implementation quality

**Pros**:
- ✅ Complete implementations
- ✅ String interpolation
- ✅ 703 stars (proven)
- ✅ MIT license

**Cons**:
- ⚠️ Custom type system
- ⚠️ Requires cpplib

**Action**: Integrate as Stage 2 alternative

---

### Priority 2: Compilets Evaluation (Medium Priority)

**Best for**: Native module generation

**Pros**:
- ✅ Native executables
- ✅ Node.js modules
- ✅ Modern GC

**Cons**:
- ⚠️ GPLv3 license
- ⚠️ Early stage
- ⚠️ Complex build

**Action**: Evaluate for specific use cases

---

### Priority 3: TSLL Exploration (Low Priority)

**Best for**: Advanced optimization and verification

**Pros**:
- ✅ LLVM optimization
- ✅ Formal verification
- ✅ Clean architecture

**Cons**:
- ⚠️ Complex setup
- ⚠️ Alpha stage
- ⚠️ Indirect C++

**Action**: Explore for future enhancements

---

## Conclusion (Updated)

After comprehensive research, **TypeScript2Cxx** remains the top recommendation for addressing weak points in the current hybrid transpiler. It provides:

1. **Complete implementations** (addresses main weak point)
2. **String interpolation** (new capability)
3. **Proven track record** (703 stars)
4. **MIT license** (compatible)
5. **Reasonable complexity** (manageable integration)

**TSLL** offers an interesting alternative for advanced use cases requiring LLVM optimization or formal verification, but the complexity and maturity concerns make it a lower priority.

**Next immediate action**: Clone and test TypeScript2Cxx on ElizaOS files.
