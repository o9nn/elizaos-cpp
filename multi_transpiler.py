#!/usr/bin/env python3
"""
Multi-Transpiler Orchestrator for ElizaOS C++
Integrates 5 transpiler backends with intelligent strategy selection
"""

from pathlib import Path
from typing import List, Dict, Optional, Any
from enum import Enum
from dataclasses import dataclass
import yaml
import subprocess
import re
import sys
import argparse
import json

# Import existing transpiler
sys.path.append(str(Path(__file__).parent))
from ts_to_cpp_transpiler import TypeScriptToCppTranspiler

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
    TS2CPP_ONLY = "ts2cpp_only"

@dataclass
class TranspilationResult:
    """Result of transpilation"""
    header: Optional[str] = None
    implementation: Optional[str] = None
    type_system: str = "unknown"
    backend: Optional[TranspilerBackend] = None
    success: bool = False
    error: Optional[str] = None
    metadata: Dict[str, Any] = None

class TypeConverter:
    """Converts between different type systems"""
    
    def __init__(self):
        self.cheerp_to_std = {
            'String*': 'std::string',
            'TArray<': 'std::vector<',
            'Object*': 'std::any',
            'Promise<': 'std::future<',
            '_Function<': 'std::function<',
        }
        
        self.js_to_std = {
            'js::string': 'std::string',
            'js::array<': 'std::vector<',
            'js::object': 'std::map<std::string, std::any>',
            'js::number': 'double',
            'js::boolean': 'bool',
            'js::any': 'std::any',
            'STR(': 'std::string(',
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
        result = content
        
        # Replace js:: types
        for js_type, std_type in self.js_to_std.items():
            result = result.replace(js_type, std_type)
        
        # Remove js:: namespace declarations
        result = re.sub(r'using namespace js;\s*', '', result)
        result = re.sub(r'namespace js\s*\{', '', result)
        
        # Remove extra closing braces from removed namespace
        # Count opening and closing braces to balance
        open_count = result.count('{')
        close_count = result.count('}')
        if close_count > open_count:
            # Remove extra closing braces from end
            for _ in range(close_count - open_count):
                result = result.rstrip()
                if result.endswith('}'):
                    result = result[:-1]
        
        return result
    
    def convert_oilpan_to_std(self, content: str) -> str:
        """Convert Oilpan GC types to std::"""
        for oilpan, std in self.oilpan_to_std.items():
            content = content.replace(oilpan, std)
        return content

class Ts2CppBackend:
    """ts2cpp (Cheerp) backend"""
    
    def __init__(self, path: Path):
        self.path = path
        self.ts2cpp_bin = path / 'ts2cpp.js'
    
    def is_available(self) -> bool:
        return self.ts2cpp_bin.exists()
    
    def transpile(self, ts_file: Path) -> TranspilationResult:
        """Transpile using ts2cpp"""
        try:
            # Use absolute path for ts_file
            abs_ts_file = ts_file.resolve()
            cmd = ['node', str(self.ts2cpp_bin), str(abs_ts_file), '--pretty']
            result = subprocess.run(cmd, capture_output=True, text=True, cwd=self.path)
            
            if result.returncode != 0:
                return TranspilationResult(
                    success=False,
                    error=f"ts2cpp failed: {result.stderr}",
                    backend=TranspilerBackend.TS2CPP
                )
            
            # ts2cpp outputs to stdout
            header_content = result.stdout
            
            return TranspilationResult(
                header=header_content,
                implementation=None,  # ts2cpp only generates headers
                type_system='cheerp',
                backend=TranspilerBackend.TS2CPP,
                success=True
            )
        except Exception as e:
            return TranspilationResult(
                success=False,
                error=str(e),
                backend=TranspilerBackend.TS2CPP
            )

class PythonTranspilerBackend:
    """Python transpiler backend"""
    
    def __init__(self, transpiler_path: Path):
        # Create temporary directories for transpiler
        import tempfile
        self.temp_dir = Path(tempfile.mkdtemp())
        self.transpiler = TypeScriptToCppTranspiler(
            input_dir=str(self.temp_dir / 'input'),
            output_dir=str(self.temp_dir / 'output')
        )
    
    def is_available(self) -> bool:
        return True  # Always available
    
    def transpile(self, ts_file: Path) -> TranspilationResult:
        """Transpile using Python transpiler"""
        try:
            ts_content = ts_file.read_text()
            
            # Transpile (use transpile_content method)
            cpp_code = self.transpiler.transpile_content(ts_content)
            
            # Split into header and implementation
            # For now, put everything in implementation
            return TranspilationResult(
                header=None,
                implementation=cpp_code,
                type_system='std',
                backend=TranspilerBackend.PYTHON,
                success=True
            )
        except Exception as e:
            return TranspilationResult(
                success=False,
                error=str(e),
                backend=TranspilerBackend.PYTHON
            )

class TypeScript2CxxBackend:
    """TypeScript2Cxx backend"""
    
    def __init__(self, path: Path):
        self.path = path
        self.main_js = path / '__out' / 'main.js'
    
    def is_available(self) -> bool:
        return self.main_js.exists()
    
    def transpile(self, ts_file: Path) -> TranspilationResult:
        """Transpile using TypeScript2Cxx"""
        try:
            # Use absolute path
            abs_ts_file = ts_file.resolve()
            cmd = ['node', str(self.main_js), str(abs_ts_file)]
            result = subprocess.run(cmd, capture_output=True, text=True, cwd=self.path)
            
            if result.returncode != 0:
                return TranspilationResult(
                    success=False,
                    error=f"TypeScript2Cxx failed: {result.stderr}",
                    backend=TranspilerBackend.TYPESCRIPT2CXX
                )
            
            # TypeScript2Cxx generates files in the same directory as input
            # Look for generated files
            h_file = abs_ts_file.with_suffix('.h')
            cpp_file = abs_ts_file.with_suffix('.cpp')
            
            header_content = None
            impl_content = None
            
            if h_file.exists():
                header_content = h_file.read_text()
                print(f"  TypeScript2Cxx generated header: {h_file} ({len(header_content)} bytes)")
            
            if cpp_file.exists():
                impl_content = cpp_file.read_text()
                print(f"  TypeScript2Cxx generated implementation: {cpp_file} ({len(impl_content)} bytes)")
            
            if not header_content and not impl_content:
                print(f"  Warning: TypeScript2Cxx did not generate any files")
                print(f"  Expected: {h_file} or {cpp_file}")
            
            return TranspilationResult(
                header=header_content,
                implementation=impl_content,
                type_system='js',
                backend=TranspilerBackend.TYPESCRIPT2CXX,
                success=True
            )
        except Exception as e:
            return TranspilationResult(
                success=False,
                error=str(e),
                backend=TranspilerBackend.TYPESCRIPT2CXX
            )

class CompiletsBackend:
    """Compilets backend"""
    
    def __init__(self, path: Path):
        self.path = path
        self.cli_js = path / 'dist' / 'cli.js'
    
    def is_available(self) -> bool:
        return self.cli_js.exists()
    
    def transpile(self, ts_file: Path) -> TranspilationResult:
        """Transpile using Compilets (code generation only)"""
        try:
            # Note: Full compilation requires GN build system
            # For now, we'll just mark it as available for future use
            return TranspilationResult(
                success=False,
                error="Compilets requires GN build system (not yet integrated)",
                backend=TranspilerBackend.COMPILETS
            )
        except Exception as e:
            return TranspilationResult(
                success=False,
                error=str(e),
                backend=TranspilerBackend.COMPILETS
            )

class TsllBackend:
    """TSLL backend"""
    
    def __init__(self, path: Path):
        self.path = path
        self.cli_js = path / 'lib' / 'cli.js'
    
    def is_available(self) -> bool:
        return self.cli_js.exists()
    
    def transpile(self, ts_file: Path) -> TranspilationResult:
        """Transpile using TSLL"""
        try:
            # Note: TSLL requires LLVM 13.0.0
            return TranspilationResult(
                success=False,
                error="TSLL requires LLVM 13.0.0 (not yet built)",
                backend=TranspilerBackend.TSLL
            )
        except Exception as e:
            return TranspilationResult(
                success=False,
                error=str(e),
                backend=TranspilerBackend.TSLL
            )

class MultiTranspiler:
    """Main multi-transpiler orchestrator"""
    
    def __init__(self, config_path: Optional[Path] = None):
        self.config = self.load_config(config_path) if config_path else self.default_config()
        self.type_converter = TypeConverter()
        self.backends = self.initialize_backends()
    
    def default_config(self) -> Dict:
        """Default configuration"""
        return {
            'default_strategy': 'enhanced_hybrid',
            'modules': [
                {
                    'pattern': '**/*.d.ts',
                    'primary': 'ts2cpp_only',
                    'reason': 'Type definitions only'
                },
                {
                    'pattern': '**/*.ts',
                    'primary': 'enhanced_hybrid',
                    'reason': 'Default strategy'
                }
            ],
            'strategies': {
                'enhanced_hybrid': {
                    'stage1': 'ts2cpp',
                    'stage2': 'typescript2cxx',
                    'stage3': 'type_conversion'
                },
                'fast_approximate': {
                    'stage1': 'ts2cpp',
                    'stage2': 'python',
                    'stage3': 'type_conversion'
                },
                'typescript2cxx_only': {
                    'stage1': 'typescript2cxx',
                    'stage2': 'type_conversion'
                },
                'ts2cpp_only': {
                    'stage1': 'ts2cpp'
                }
            }
        }
    
    def load_config(self, config_path: Path) -> Dict:
        """Load configuration from YAML file"""
        with open(config_path) as f:
            return yaml.safe_load(f)
    
    def initialize_backends(self) -> Dict[TranspilerBackend, Any]:
        """Initialize all available transpiler backends"""
        backends = {}
        
        # ts2cpp (Cheerp)
        ts2cpp_path = Path('/home/ubuntu/ts2cpp-master')
        if ts2cpp_path.exists():
            backend = Ts2CppBackend(ts2cpp_path)
            if backend.is_available():
                backends[TranspilerBackend.TS2CPP] = backend
                print(f"✓ ts2cpp backend available")
        
        # Python transpiler (always available)
        transpiler_path = Path(__file__).parent / 'ts_to_cpp_transpiler.py'
        backends[TranspilerBackend.PYTHON] = PythonTranspilerBackend(transpiler_path)
        print(f"✓ Python transpiler backend available")
        
        # TypeScript2Cxx
        ts2cxx_path = Path('/home/ubuntu/typescript2cxx')
        if ts2cxx_path.exists():
            backend = TypeScript2CxxBackend(ts2cxx_path)
            if backend.is_available():
                backends[TranspilerBackend.TYPESCRIPT2CXX] = backend
                print(f"✓ TypeScript2Cxx backend available")
        
        # Compilets (optional)
        compilets_path = Path('/home/ubuntu/compilets')
        if compilets_path.exists():
            backend = CompiletsBackend(compilets_path)
            if backend.is_available():
                backends[TranspilerBackend.COMPILETS] = backend
                print(f"✓ Compilets backend available")
        
        # TSLL (optional)
        tsll_path = Path('/home/ubuntu/tsll')
        if tsll_path.exists():
            backend = TsllBackend(tsll_path)
            if backend.is_available():
                backends[TranspilerBackend.TSLL] = backend
                print(f"✓ TSLL backend available")
        
        return backends
    
    def select_strategy(self, ts_file: Path) -> str:
        """Select optimal transpilation strategy for a file"""
        # Check configuration patterns
        for module_config in self.config['modules']:
            pattern = module_config['pattern']
            if ts_file.match(pattern):
                return module_config['primary']
        
        # Analyze file characteristics
        try:
            content = ts_file.read_text()
            
            # Check for nested namespaces
            if self.has_nested_namespaces(content):
                print(f"  Detected nested namespaces → typescript2cxx_only")
                return 'typescript2cxx_only'
            
            # Check for string interpolation
            if self.has_string_interpolation(content):
                print(f"  Detected string interpolation → typescript2cxx_only")
                return 'typescript2cxx_only'
        except Exception as e:
            print(f"  Warning: Could not analyze file: {e}")
        
        # Default to enhanced hybrid
        return self.config['default_strategy']
    
    def has_nested_namespaces(self, content: str) -> bool:
        """Check if content has nested namespaces"""
        pattern = r'namespace\s+\w+\s*\{[^}]*namespace\s+\w+'
        return bool(re.search(pattern, content))
    
    def has_string_interpolation(self, content: str) -> bool:
        """Check if content has template literals"""
        return '`' in content and '${' in content
    
    def execute_strategy(self, ts_file: Path, strategy_name: str) -> TranspilationResult:
        """Execute transpilation strategy"""
        if strategy_name not in self.config['strategies']:
            return TranspilationResult(
                success=False,
                error=f"Unknown strategy: {strategy_name}"
            )
        
        strategy_config = self.config['strategies'][strategy_name]
        results = {}
        
        # Stage 1
        stage1_name = strategy_config['stage1']
        stage1_backend = TranspilerBackend(stage1_name)
        
        if stage1_backend not in self.backends:
            return TranspilationResult(
                success=False,
                error=f"Backend not available: {stage1_name}"
            )
        
        print(f"  Stage 1: {stage1_name}")
        results['stage1'] = self.backends[stage1_backend].transpile(ts_file)
        print(f"  Stage 1 result: success={results['stage1'].success}, header={results['stage1'].header is not None}, impl={results['stage1'].implementation is not None}")
        
        if not results['stage1'].success:
            print(f"  Stage 1 failed: {results['stage1'].error}")
            return results['stage1']
        
        # Stage 2
        if 'stage2' in strategy_config:
            stage2_name = strategy_config['stage2']
            
            if stage2_name == 'type_conversion':
                print(f"  Stage 2: type_conversion")
                results['stage2'] = self.apply_type_conversion(results['stage1'])
            else:
                stage2_backend = TranspilerBackend(stage2_name)
                if stage2_backend in self.backends:
                    print(f"  Stage 2: {stage2_name}")
                    results['stage2'] = self.backends[stage2_backend].transpile(ts_file)
                    print(f"  Stage 2 result: success={results['stage2'].success}, header={results['stage2'].header is not None}, impl={results['stage2'].implementation is not None}")
                    
                    if not results['stage2'].success:
                        print(f"  Stage 2 failed: {results['stage2'].error}")
                        return results['stage2']
        
        # Stage 3
        if 'stage3' in strategy_config:
            stage3_name = strategy_config['stage3']
            
            if stage3_name == 'type_conversion':
                print(f"  Stage 3: type_conversion")
                source_result = results.get('stage2', results['stage1'])
                results['stage3'] = self.apply_type_conversion(source_result)
        
        # Return final result
        return results.get('stage3', results.get('stage2', results['stage1']))
    
    def apply_type_conversion(self, result: TranspilationResult) -> TranspilationResult:
        """Apply type conversion to result"""
        # If no content, return original result
        if not result.header and not result.implementation:
            return result
        
        converted_header = None
        converted_impl = None
        
        if result.header:
            converted_header = self.type_converter.convert(result.header, result.type_system)
            print(f"  Type conversion: header {len(result.header)} → {len(converted_header)} bytes")
        
        if result.implementation:
            converted_impl = self.type_converter.convert(result.implementation, result.type_system)
            print(f"  Type conversion: impl {len(result.implementation)} → {len(converted_impl)} bytes")
        
        return TranspilationResult(
            header=converted_header,
            implementation=converted_impl,
            type_system='std',
            backend=result.backend,
            success=True
        )
    
    def write_output(self, result: TranspilationResult, output_dir: Path, ts_file: Path):
        """Write transpilation result to output directory"""
        output_dir.mkdir(parents=True, exist_ok=True)
        
        # Determine output file names
        base_name = ts_file.stem
        
        files_written = []
        
        if result.header:
            header_file = output_dir / f"{base_name}.hpp"
            header_file.write_text(result.header)
            files_written.append(str(header_file))
            print(f"  → {header_file}")
        
        if result.implementation:
            impl_file = output_dir / f"{base_name}.cpp"
            impl_file.write_text(result.implementation)
            files_written.append(str(impl_file))
            print(f"  → {impl_file}")
        
        if not files_written:
            print(f"  Warning: No output files generated")
    
    def transpile(self, input_path: Path, output_dir: Path, strategy: Optional[str] = None):
        """Transpile TypeScript files"""
        # Collect TypeScript files
        if input_path.is_file():
            ts_files = [input_path]
        else:
            ts_files = list(input_path.rglob('*.ts'))
        
        print(f"\nFound {len(ts_files)} TypeScript files")
        print(f"Output directory: {output_dir}\n")
        
        success_count = 0
        failure_count = 0
        
        for ts_file in ts_files:
            print(f"Processing: {ts_file.name}")
            
            # Select strategy
            if strategy:
                selected_strategy = strategy
            else:
                selected_strategy = self.select_strategy(ts_file)
            
            print(f"  Strategy: {selected_strategy}")
            
            # Execute strategy
            result = self.execute_strategy(ts_file, selected_strategy)
            
            if result.success:
                # Debug: check result content
                print(f"  Debug: header={result.header is not None}, impl={result.implementation is not None}")
                if result.header:
                    print(f"  Debug: header length={len(result.header)}")
                if result.implementation:
                    print(f"  Debug: impl length={len(result.implementation)}")
                
                # Write output
                self.write_output(result, output_dir, ts_file)
                success_count += 1
                print(f"  ✓ Success\n")
            else:
                print(f"  ✗ Failed: {result.error}\n")
                failure_count += 1
        
        print(f"\n{'='*60}")
        print(f"Transpilation complete:")
        print(f"  Success: {success_count}/{len(ts_files)}")
        print(f"  Failed:  {failure_count}/{len(ts_files)}")
        print(f"{'='*60}\n")

def main():
    parser = argparse.ArgumentParser(description='Multi-Transpiler for ElizaOS C++')
    parser.add_argument('input', type=Path, help='Input TypeScript file or directory')
    parser.add_argument('-o', '--output', type=Path, required=True, help='Output directory')
    parser.add_argument('-s', '--strategy', type=str, help='Transpilation strategy')
    parser.add_argument('-c', '--config', type=Path, help='Configuration file')
    parser.add_argument('-v', '--verbose', action='store_true', help='Verbose output')
    
    args = parser.parse_args()
    
    # Create multi-transpiler
    transpiler = MultiTranspiler(config_path=args.config)
    
    # Transpile
    transpiler.transpile(args.input, args.output, strategy=args.strategy)

if __name__ == '__main__':
    main()
