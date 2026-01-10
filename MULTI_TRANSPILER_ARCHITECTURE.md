# Multi-Transpiler Architecture for ElizaOS

**Date**: January 10, 2026  
**Version**: 1.0  
**Status**: Design Complete, Ready for Implementation

---

## Executive Summary

This document defines a comprehensive multi-transpiler architecture that integrates **five transpilation backends** to provide maximum flexibility and capabilities for the ElizaOS C++ project:

1. **ts2cpp (Cheerp)** - Type definitions, WebAssembly
2. **Python Transpiler** - Fast approximate implementations
3. **TypeScript2Cxx** - Complete implementations, string interpolation
4. **Compilets** - Native executables, Node.js modules
5. **TSLL** - LLVM optimization, formal verification

---

## Architecture Overview

```
┌─────────────────────────────────────────────────────────────────────┐
│                      TypeScript Source Files                         │
│                         (*.ts, *.d.ts)                              │
└─────────────────────────────────────────────────────────────────────┘
                                  │
                                  ▼
┌─────────────────────────────────────────────────────────────────────┐
│                    Multi-Transpiler Orchestrator                     │
│                                                                      │
│  - Analyzes source files                                            │
│  - Selects optimal transpiler(s) based on:                          │
│    • File characteristics (nested namespaces, string interpolation) │
│    • Target output (headers, implementations, executables)          │
│    • Performance requirements                                        │
│    • User configuration                                              │
└─────────────────────────────────────────────────────────────────────┘
                                  │
                    ┌─────────────┴─────────────┐
                    ▼                           ▼
┌───────────────────────────────┐   ┌───────────────────────────────┐
│   Primary Transpilers         │   │   Specialized Transpilers     │
│   (Always Available)          │   │   (Optional)                  │
├───────────────────────────────┤   ├───────────────────────────────┤
│ 1. ts2cpp (Cheerp)            │   │ 4. Compilets                  │
│    → Type definitions         │   │    → Native executables       │
│    → WebAssembly support      │   │    → Node.js modules          │
│                               │   │                               │
│ 2. Python Transpiler          │   │ 5. TSLL                       │
│    → Fast implementations     │   │    → LLVM IR generation       │
│    → Fallback option          │   │    → Formal verification      │
│                               │   │    → Optimization             │
│ 3. TypeScript2Cxx             │   │                               │
│    → Complete implementations │   │                               │
│    → String interpolation     │   │                               │
│    → Nested namespaces        │   │                               │
└───────────────────────────────┘   └───────────────────────────────┘
                    │                           │
                    └─────────────┬─────────────┘
                                  ▼
┌─────────────────────────────────────────────────────────────────────┐
│                      Type Conversion Layer                           │
│                                                                      │
│  - Cheerp types → Native STL                                        │
│  - js:: types → Native STL                                          │
│  - LLVM IR → C++ (via LLVM backend)                                 │
│  - Oilpan GC types → Native STL (if needed)                         │
└─────────────────────────────────────────────────────────────────────┘
                                  │
                                  ▼
┌─────────────────────────────────────────────────────────────────────┐
│                       Output Merger & Validator                      │
│                                                                      │
│  - Merges headers from multiple sources                             │
│  - Combines implementations                                          │
│  - Validates consistency                                             │
│  - Generates final C++ files                                         │
└─────────────────────────────────────────────────────────────────────┘
                                  │
                                  ▼
┌─────────────────────────────────────────────────────────────────────┐
│                      Final C++ Output                                │
│                                                                      │
│  - include/*.hpp (Native headers)                                   │
│  - src/*.cpp (Complete implementations)                             │
│  - bin/* (Native executables, optional)                             │
│  - lib/*.node (Node.js modules, optional)                           │
└─────────────────────────────────────────────────────────────────────┘
```

---

## Transpiler Capabilities Matrix

| Feature | ts2cpp | Python | TypeScript2Cxx | Compilets | TSLL |
|---------|--------|--------|----------------|-----------|------|
| **Type Definitions** | ⭐⭐⭐⭐⭐ | ⭐⭐⭐ | ⭐⭐⭐⭐ | ⭐⭐⭐⭐ | ⭐⭐⭐⭐ |
| **Implementations** | ❌ None | ⭐⭐⭐ Approx | ⭐⭐⭐⭐⭐ Complete | ⭐⭐⭐⭐⭐ Complete | ⭐⭐⭐⭐ LLVM IR |
| **Nested Namespaces** | ⭐⭐ Limited | ⭐⭐ Limited | ⭐⭐⭐⭐⭐ Full | ⭐⭐⭐⭐ Good | ⭐⭐⭐ Unknown |
| **String Interpolation** | ❌ No | ❌ No | ⭐⭐⭐⭐⭐ Yes | ⭐⭐⭐ Unknown | ⭐⭐⭐ Unknown |
| **Generics** | ⭐⭐⭐⭐ Good | ⭐⭐⭐ Basic | ⭐⭐⭐⭐ Good | ⭐⭐⭐⭐ Good | ⭐⭐⭐⭐ Good |
| **Speed** | ⭐⭐⭐⭐ Fast | ⭐⭐⭐⭐⭐ Very Fast | ⭐⭐⭐ Medium | ⭐⭐ Slow | ⭐⭐ Slow |
| **Native Executables** | ❌ No | ❌ No | ❌ No | ⭐⭐⭐⭐⭐ Yes | ⭐⭐⭐⭐⭐ Yes (via LLVM) |
| **Node.js Modules** | ❌ No | ❌ No | ❌ No | ⭐⭐⭐⭐⭐ Yes | ❌ No |
| **WebAssembly** | ⭐⭐⭐⭐⭐ Yes | ❌ No | ❌ No | ❌ No | ❌ No |
| **Optimization** | ⭐⭐⭐ Good | ⭐⭐ Basic | ⭐⭐⭐ Good | ⭐⭐⭐⭐ Good | ⭐⭐⭐⭐⭐ LLVM |
| **Formal Verification** | ❌ No | ❌ No | ❌ No | ❌ No | ⭐⭐⭐⭐⭐ Yes |
| **Setup Complexity** | ⭐⭐⭐⭐ Easy | ⭐⭐⭐⭐⭐ Very Easy | ⭐⭐⭐⭐ Easy | ⭐⭐ Complex | ⭐ Very Complex |
| **Maturity** | ⭐⭐⭐⭐ Stable | ⭐⭐⭐ Good | ⭐⭐⭐ Good | ⭐⭐ Early | ⭐⭐ Alpha |

---

## Transpiler Selection Strategy

### Decision Tree

```
Input: TypeScript file(s)
│
├─ Need type definitions only?
│  └─ YES → Use ts2cpp (Cheerp)
│
├─ Need WebAssembly output?
│  └─ YES → Use ts2cpp (Cheerp)
│
├─ Need native executable?
│  └─ YES → Use Compilets or TSLL
│
├─ Need Node.js module?
│  └─ YES → Use Compilets
│
├─ Need formal verification?
│  └─ YES → Use TSLL
│
├─ Need LLVM optimization?
│  └─ YES → Use TSLL
│
├─ Has nested namespaces?
│  └─ YES → Use TypeScript2Cxx
│
├─ Has string interpolation?
│  └─ YES → Use TypeScript2Cxx
│
├─ Need fast processing?
│  └─ YES → Use Python Transpiler
│
└─ Default → Use Enhanced Hybrid (ts2cpp + TypeScript2Cxx)
```

### Configuration-Based Selection

```yaml
# multi_transpiler_config.yaml

default_strategy: enhanced_hybrid

modules:
  # Core modules - highest quality
  - pattern: "src/core/**/*.ts"
    primary: typescript2cxx
    fallback: python
    reason: "Core modules need complete implementations"
  
  # Type definitions - best type quality
  - pattern: "**/*.d.ts"
    primary: ts2cpp
    reason: "Type definitions only, ts2cpp excels here"
  
  # Performance-critical - native compilation
  - pattern: "src/performance/**/*.ts"
    primary: compilets
    fallback: tsll
    reason: "Performance-critical, need native code"
  
  # Security-critical - formal verification
  - pattern: "src/security/**/*.ts"
    primary: tsll
    fallback: typescript2cxx
    reason: "Security-critical, need verification"
  
  # WebAssembly modules
  - pattern: "src/wasm/**/*.ts"
    primary: ts2cpp
    reason: "WebAssembly target"
  
  # Node.js addons
  - pattern: "src/addons/**/*.ts"
    primary: compilets
    reason: "Node.js native modules"
  
  # Everything else - enhanced hybrid
  - pattern: "**/*.ts"
    primary: enhanced_hybrid
    reason: "Default strategy"

strategies:
  enhanced_hybrid:
    stage1: ts2cpp          # Type definitions
    stage2: typescript2cxx  # Implementations
    stage3: type_conversion # Convert to native STL
  
  fast_approximate:
    stage1: ts2cpp          # Type definitions
    stage2: python          # Fast implementations
    stage3: type_conversion # Convert to native STL
  
  native_executable:
    stage1: compilets       # Generate C++ and compile
    stage2: type_conversion # Convert Oilpan types if needed
  
  llvm_optimized:
    stage1: tsll            # Generate LLVM IR
    stage2: llvm_backend    # Compile to C++
    stage3: type_conversion # Convert LLVM types
  
  typescript2cxx_only:
    stage1: typescript2cxx  # Complete solution
    stage2: type_conversion # Convert js:: to std::
```

---

## Implementation Architecture

### Multi-Transpiler Orchestrator

```python
# multi_transpiler.py

from pathlib import Path
from typing import List, Dict, Optional
from enum import Enum
import yaml

class TranspilerBackend(Enum):
    TS2CPP = "ts2cpp"
    PYTHON = "python"
    TYPESCRIPT2CXX = "typescript2cxx"
    COMPILETS = "compilets"
    TSLL = "tsll"

class TranspilerStrategy(Enum):
    ENHANCED_HYBRID = "enhanced_hybrid"
    FAST_APPROXIMATE = "fast_approximate"
    NATIVE_EXECUTABLE = "native_executable"
    LLVM_OPTIMIZED = "llvm_optimized"
    TYPESCRIPT2CXX_ONLY = "typescript2cxx_only"

class MultiTranspiler:
    def __init__(self, config_path: Path):
        self.config = self.load_config(config_path)
        self.backends = self.initialize_backends()
        self.type_converter = TypeConverter()
    
    def load_config(self, config_path: Path) -> Dict:
        """Load configuration from YAML file"""
        with open(config_path) as f:
            return yaml.safe_load(f)
    
    def initialize_backends(self) -> Dict[TranspilerBackend, object]:
        """Initialize all available transpiler backends"""
        backends = {}
        
        # Always available
        backends[TranspilerBackend.TS2CPP] = Ts2CppBackend(
            path=Path('/home/ubuntu/ts2cpp-master')
        )
        backends[TranspilerBackend.PYTHON] = PythonTranspilerBackend(
            path=Path('/home/ubuntu/elizaos-cpp/ts_to_cpp_transpiler.py')
        )
        backends[TranspilerBackend.TYPESCRIPT2CXX] = TypeScript2CxxBackend(
            path=Path('/home/ubuntu/typescript2cxx')
        )
        
        # Optional backends
        try:
            backends[TranspilerBackend.COMPILETS] = CompiletsBackend(
                path=Path('/home/ubuntu/compilets')
            )
        except Exception as e:
            print(f"Compilets not available: {e}")
        
        try:
            backends[TranspilerBackend.TSLL] = TsllBackend(
                path=Path('/home/ubuntu/tsll')
            )
        except Exception as e:
            print(f"TSLL not available: {e}")
        
        return backends
    
    def select_strategy(self, ts_file: Path) -> TranspilerStrategy:
        """Select optimal transpilation strategy for a file"""
        # Check configuration patterns
        for module_config in self.config['modules']:
            if self.matches_pattern(ts_file, module_config['pattern']):
                strategy_name = module_config.get('primary')
                if strategy_name in [s.value for s in TranspilerStrategy]:
                    return TranspilerStrategy(strategy_name)
        
        # Analyze file characteristics
        content = ts_file.read_text()
        
        # Check for nested namespaces
        if self.has_nested_namespaces(content):
            return TranspilerStrategy.TYPESCRIPT2CXX_ONLY
        
        # Check for string interpolation
        if self.has_string_interpolation(content):
            return TranspilerStrategy.TYPESCRIPT2CXX_ONLY
        
        # Default to enhanced hybrid
        return TranspilerStrategy.ENHANCED_HYBRID
    
    def transpile(self, input_dir: Path, output_dir: Path):
        """Transpile all TypeScript files in input directory"""
        ts_files = list(input_dir.rglob('*.ts'))
        
        for ts_file in ts_files:
            print(f"Processing {ts_file}...")
            
            # Select strategy
            strategy = self.select_strategy(ts_file)
            print(f"  Strategy: {strategy.value}")
            
            # Execute strategy
            result = self.execute_strategy(ts_file, strategy, output_dir)
            
            # Validate result
            if not self.validate_output(result):
                print(f"  Warning: Validation failed for {ts_file}")
    
    def execute_strategy(self, ts_file: Path, strategy: TranspilerStrategy, output_dir: Path):
        """Execute transpilation strategy"""
        strategy_config = self.config['strategies'][strategy.value]
        
        results = {}
        
        # Stage 1
        stage1_backend = TranspilerBackend(strategy_config['stage1'])
        if stage1_backend in self.backends:
            results['stage1'] = self.backends[stage1_backend].transpile(ts_file)
        
        # Stage 2
        if 'stage2' in strategy_config:
            stage2_name = strategy_config['stage2']
            if stage2_name == 'type_conversion':
                results['stage2'] = self.type_converter.convert(results['stage1'])
            elif stage2_name == 'llvm_backend':
                results['stage2'] = self.compile_llvm_ir(results['stage1'])
            else:
                stage2_backend = TranspilerBackend(stage2_name)
                if stage2_backend in self.backends:
                    results['stage2'] = self.backends[stage2_backend].transpile(ts_file)
        
        # Stage 3
        if 'stage3' in strategy_config:
            stage3_name = strategy_config['stage3']
            if stage3_name == 'type_conversion':
                results['stage3'] = self.type_converter.convert(results['stage2'])
        
        # Merge and write output
        final_output = self.merge_results(results)
        self.write_output(final_output, output_dir, ts_file)
        
        return final_output
    
    def has_nested_namespaces(self, content: str) -> bool:
        """Check if content has nested namespaces"""
        import re
        # Look for patterns like: namespace A { namespace B { ... } }
        pattern = r'namespace\s+\w+\s*\{[^}]*namespace\s+\w+'
        return bool(re.search(pattern, content))
    
    def has_string_interpolation(self, content: str) -> bool:
        """Check if content has template literals"""
        return '`' in content and '${' in content
```

---

## Type Conversion Layer

### Unified Type Converter

```python
# type_converter.py

class TypeConverter:
    def __init__(self):
        self.cheerp_to_std = {
            'String*': 'std::string',
            'TArray<': 'std::vector<',
            'Object*': 'std::any',
            'Promise<': 'std::future<',
            '_Function<': 'std::function<',
            'double': 'double',
            'int': 'int',
            'bool': 'bool',
        }
        
        self.js_to_std = {
            'js::string': 'std::string',
            'js::array<': 'std::vector<',
            'js::object': 'std::map<std::string, std::any>',
            'js::number': 'double',
            'js::boolean': 'bool',
            'js::any': 'std::any',
        }
        
        self.oilpan_to_std = {
            'Member<': 'std::shared_ptr<',
            'Persistent<': 'std::shared_ptr<',
            'GarbageCollected': 'std::enable_shared_from_this',
        }
    
    def convert(self, content: str, source_type: str = 'auto') -> str:
        """Convert types from various sources to native STL"""
        if source_type == 'auto':
            source_type = self.detect_source_type(content)
        
        if source_type == 'cheerp':
            return self.convert_cheerp_to_std(content)
        elif source_type == 'js':
            return self.convert_js_to_std(content)
        elif source_type == 'oilpan':
            return self.convert_oilpan_to_std(content)
        else:
            return content
    
    def detect_source_type(self, content: str) -> str:
        """Detect the source type system"""
        if 'namespace js' in content or 'js::' in content:
            return 'js'
        elif 'String*' in content or 'TArray<' in content:
            return 'cheerp'
        elif 'Member<' in content or 'GarbageCollected' in content:
            return 'oilpan'
        else:
            return 'unknown'
    
    def convert_cheerp_to_std(self, content: str) -> str:
        """Convert Cheerp types to std::"""
        for cheerp, std in self.cheerp_to_std.items():
            content = content.replace(cheerp, std)
        return content
    
    def convert_js_to_std(self, content: str) -> str:
        """Convert js:: types to std::"""
        for js, std in self.js_to_std.items():
            content = content.replace(js, std)
        # Remove js:: namespace
        content = re.sub(r'using namespace js;', '', content)
        content = re.sub(r'namespace js\s*\{', '', content)
        return content
    
    def convert_oilpan_to_std(self, content: str) -> str:
        """Convert Oilpan GC types to std::"""
        for oilpan, std in self.oilpan_to_std.items():
            content = content.replace(oilpan, std)
        return content
```

---

## Backend Implementations

### TypeScript2Cxx Backend

```python
# backends/typescript2cxx_backend.py

class TypeScript2CxxBackend:
    def __init__(self, path: Path):
        self.path = path
        self.main_js = path / '__out' / 'main.js'
    
    def transpile(self, ts_file: Path) -> Dict:
        """Transpile TypeScript file using TypeScript2Cxx"""
        cmd = ['node', str(self.main_js), str(ts_file)]
        result = subprocess.run(cmd, capture_output=True, text=True)
        
        if result.returncode != 0:
            raise Exception(f"TypeScript2Cxx failed: {result.stderr}")
        
        # Read generated files
        h_file = ts_file.with_suffix('.h')
        cpp_file = ts_file.with_suffix('.cpp')
        
        return {
            'header': h_file.read_text() if h_file.exists() else None,
            'implementation': cpp_file.read_text() if cpp_file.exists() else None,
            'type_system': 'js',
        }
```

### Compilets Backend

```python
# backends/compilets_backend.py

class CompiletsBackend:
    def __init__(self, path: Path):
        self.path = path
        self.cli_js = path / 'dist' / 'cli.js'
    
    def transpile(self, ts_file: Path) -> Dict:
        """Transpile TypeScript file using Compilets"""
        # Generate C++ project
        cmd = ['node', str(self.cli_js), 'gen', '--root', str(ts_file.parent)]
        result = subprocess.run(cmd, capture_output=True, text=True)
        
        if result.returncode != 0:
            raise Exception(f"Compilets gen failed: {result.stderr}")
        
        # Read generated C++ files
        cpp_project_dir = ts_file.parent / 'cpp-project'
        
        return {
            'project_dir': cpp_project_dir,
            'type_system': 'oilpan',
        }
    
    def build_executable(self, project_dir: Path) -> Path:
        """Build native executable from C++ project"""
        cmd = ['node', str(self.cli_js), 'build', '--config', 'Release']
        result = subprocess.run(cmd, cwd=project_dir, capture_output=True, text=True)
        
        if result.returncode != 0:
            raise Exception(f"Compilets build failed: {result.stderr}")
        
        # Return path to executable
        return project_dir / 'out' / 'Release' / project_dir.name
```

### TSLL Backend

```python
# backends/tsll_backend.py

class TsllBackend:
    def __init__(self, path: Path):
        self.path = path
        self.cli_js = path / 'lib' / 'cli.js'
    
    def transpile(self, ts_file: Path) -> Dict:
        """Transpile TypeScript file to LLVM IR using TSLL"""
        cmd = ['node', str(self.cli_js), str(ts_file)]
        result = subprocess.run(cmd, capture_output=True, text=True)
        
        if result.returncode != 0:
            raise Exception(f"TSLL failed: {result.stderr}")
        
        # Read generated LLVM IR
        ll_file = ts_file.with_suffix('.ll')
        
        return {
            'llvm_ir': ll_file.read_text() if ll_file.exists() else None,
            'type_system': 'llvm',
        }
    
    def compile_to_cpp(self, llvm_ir_file: Path) -> Dict:
        """Compile LLVM IR to C++ using LLVM backend"""
        # Use llc to compile LLVM IR to C++
        cmd = ['llc', '-march=cpp', str(llvm_ir_file)]
        result = subprocess.run(cmd, capture_output=True, text=True)
        
        if result.returncode != 0:
            raise Exception(f"LLVM compilation failed: {result.stderr}")
        
        cpp_file = llvm_ir_file.with_suffix('.cpp')
        
        return {
            'implementation': cpp_file.read_text() if cpp_file.exists() else None,
            'type_system': 'llvm',
        }
```

---

## Usage Examples

### Example 1: Enhanced Hybrid (Default)

```bash
# Use enhanced hybrid strategy (ts2cpp + TypeScript2Cxx)
python3 multi_transpiler.py \
    --input-dir /home/ubuntu/elizaos-cpp/test_hybrid \
    --output-dir /home/ubuntu/elizaos-cpp/output \
    --strategy enhanced_hybrid \
    --verbose
```

### Example 2: Native Executable

```bash
# Generate native executable using Compilets
python3 multi_transpiler.py \
    --input-dir /home/ubuntu/elizaos-cpp/src/performance \
    --output-dir /home/ubuntu/elizaos-cpp/bin \
    --strategy native_executable \
    --build-executable
```

### Example 3: LLVM Optimized

```bash
# Generate LLVM-optimized C++ using TSLL
python3 multi_transpiler.py \
    --input-dir /home/ubuntu/elizaos-cpp/src/security \
    --output-dir /home/ubuntu/elizaos-cpp/output_llvm \
    --strategy llvm_optimized \
    --optimization-level 3
```

### Example 4: Configuration-Based

```bash
# Use configuration file for module-specific strategies
python3 multi_transpiler.py \
    --input-dir /home/ubuntu/elizaos-cpp/src \
    --output-dir /home/ubuntu/elizaos-cpp/output \
    --config multi_transpiler_config.yaml
```

---

## Benefits of Multi-Transpiler Architecture

### Flexibility

✅ **Choose the right tool for each job**
- Type definitions → ts2cpp
- Complete implementations → TypeScript2Cxx
- Native executables → Compilets
- LLVM optimization → TSLL

### Quality

✅ **Best-in-class output for each use case**
- Highest quality type definitions
- Complete implementations with string interpolation
- Native performance when needed
- Formal verification for security-critical code

### Backward Compatibility

✅ **Gradual adoption**
- Existing Python transpiler still available as fallback
- Can mix strategies within same project
- No breaking changes to existing workflow

### Future-Proofing

✅ **Ready for new requirements**
- WebAssembly support (ts2cpp)
- Node.js modules (Compilets)
- Formal verification (TSLL)
- LLVM optimization (TSLL)

---

## Implementation Timeline

### Phase 1: Core Integration (Week 1)

- [x] TypeScript2Cxx built and tested
- [x] Compilets built and tested
- [ ] Multi-transpiler orchestrator implementation
- [ ] Type converter implementation
- [ ] Configuration system

### Phase 2: Backend Integration (Week 2)

- [ ] TypeScript2Cxx backend
- [ ] Compilets backend (code generation only)
- [ ] Enhanced hybrid strategy
- [ ] Fast approximate strategy
- [ ] Unit tests

### Phase 3: Testing & Validation (Week 3)

- [ ] Test on ElizaOS codebase
- [ ] Performance benchmarking
- [ ] Output validation
- [ ] Documentation

### Phase 4: Advanced Features (Week 4)

- [ ] TSLL backend (when LLVM available)
- [ ] Native executable compilation
- [ ] LLVM optimization
- [ ] Formal verification workflows

---

## Success Criteria

### Code Quality

- **Implementation completeness**: 100% (vs 70% current)
- **Nested namespace support**: 100% (vs 30% current)
- **String interpolation**: 100% (vs 0% current)
- **Manual refinement needed**: <10% (vs 30% current)

### Performance

- **Transpilation speed**: Within 2x of current for enhanced hybrid
- **Native executable performance**: 5-10x faster than interpreted
- **Compilation success**: >95%

### Flexibility

- **Multiple strategies available**: 5 strategies
- **Configuration-based selection**: Yes
- **Fallback support**: Yes

---

## Conclusion

The multi-transpiler architecture provides **maximum flexibility and capabilities** for the ElizaOS C++ project by integrating five complementary transpilation backends. Each backend excels in specific use cases, and the orchestrator automatically selects the optimal strategy based on file characteristics and configuration.

**Status**: Ready for implementation  
**Priority**: HIGH  
**Timeline**: 4 weeks for full implementation

---

**Document Created**: January 10, 2026  
**Next Review**: After Phase 1 completion  
**Owner**: Manus AI / ElizaOS Team
