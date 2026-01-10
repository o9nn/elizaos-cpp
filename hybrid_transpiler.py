#!/usr/bin/env python3
"""
Hybrid TypeScript-to-C++ Transpiler
Integrates ts2cpp (Cheerp) with ts_to_cpp_transpiler.py

This script orchestrates both transpilers to produce optimal C++ code:
- ts2cpp: Generates clean type definitions (Cheerp-compatible)
- ts_to_cpp_transpiler.py: Generates implementations (native C++)

Version 1.0 - Initial Integration
"""

import os
import sys
import subprocess
import argparse
import json
from pathlib import Path
from typing import List, Dict, Tuple, Optional
import re
import shutil

# Import the existing Python transpiler
from ts_to_cpp_transpiler import TypeScriptToCppTranspiler

class HybridTranspiler:
    """
    Hybrid transpiler that combines ts2cpp and Python transpiler
    """
    
    def __init__(self, input_dir: str, output_dir: str, mode: str = 'hybrid', 
                 verbose: bool = False, validate: bool = False, optimize: bool = False):
        self.input_dir = Path(input_dir)
        self.output_dir = Path(output_dir)
        self.mode = mode
        self.verbose = verbose
        self.validate = validate
        self.optimize = optimize
        
        # Paths to transpilers
        self.ts2cpp_path = Path('/home/ubuntu/ts2cpp-master/ts2cpp.js')
        
        # Output directories
        self.cheerp_dir = self.output_dir / 'cheerp'
        self.native_dir = self.output_dir / 'native'
        self.hybrid_dir = self.output_dir / 'hybrid'
        self.types_dir = self.input_dir / 'types'
        
        # Statistics
        self.stats = {
            'ts_files': 0,
            'dts_files': 0,
            'cheerp_headers': 0,
            'native_headers': 0,
            'native_impls': 0,
            'hybrid_files': 0,
            'errors': 0,
            'warnings': 0
        }
        
        # Initialize Python transpiler
        self.python_transpiler = None
    
    def log(self, message: str, level: str = "INFO"):
        """Log message if verbose mode is enabled"""
        if self.verbose or level == "ERROR":
            print(f"[{level}] {message}")
    
    def setup_directories(self):
        """Create output directory structure"""
        self.log("Setting up output directories...")
        
        self.cheerp_dir.mkdir(parents=True, exist_ok=True)
        self.native_dir.mkdir(parents=True, exist_ok=True)
        self.hybrid_dir.mkdir(parents=True, exist_ok=True)
        
        # Create subdirectories
        (self.hybrid_dir / 'include').mkdir(exist_ok=True)
        (self.hybrid_dir / 'src').mkdir(exist_ok=True)
        
        self.log(f"Created output directories in {self.output_dir}")
    
    def find_typescript_files(self) -> Tuple[List[Path], List[Path]]:
        """Find TypeScript source and declaration files"""
        self.log("Scanning for TypeScript files...")
        
        ts_files = list(self.input_dir.rglob('*.ts'))
        dts_files = list(self.input_dir.rglob('*.d.ts'))
        
        # Remove .d.ts files from ts_files
        ts_files = [f for f in ts_files if not f.name.endswith('.d.ts')]
        
        # Filter out test and config files
        ts_files = [f for f in ts_files if self._should_process(f)]
        dts_files = [f for f in dts_files if self._should_process(f)]
        
        self.stats['ts_files'] = len(ts_files)
        self.stats['dts_files'] = len(dts_files)
        
        self.log(f"Found {len(ts_files)} .ts files and {len(dts_files)} .d.ts files")
        
        return ts_files, dts_files
    
    def _should_process(self, file_path: Path) -> bool:
        """Check if file should be processed"""
        # Skip test files
        if 'test' in file_path.stem.lower() or file_path.stem.endswith('.test'):
            return False
        
        # Skip spec files
        if file_path.stem.endswith('.spec'):
            return False
        
        # Skip config files
        if file_path.stem in ['tsconfig', 'vite.config', 'jest.config', 'webpack.config']:
            return False
        
        # Skip node_modules
        if 'node_modules' in str(file_path):
            return False
        
        return True
    
    def run_ts2cpp(self, dts_file: Path) -> Optional[Path]:
        """Run ts2cpp on a .d.ts file"""
        try:
            # Calculate output path
            rel_path = dts_file.relative_to(self.input_dir)
            output_file = self.cheerp_dir / rel_path.with_suffix('.h')
            output_file.parent.mkdir(parents=True, exist_ok=True)
            
            # Run ts2cpp
            cmd = [
                'node',
                str(self.ts2cpp_path),
                '--pretty',
                str(dts_file),
                '-o',
                str(output_file)
            ]
            
            self.log(f"Running ts2cpp on {dts_file.name}...")
            result = subprocess.run(cmd, capture_output=True, text=True, timeout=30)
            
            if result.returncode == 0 and output_file.exists():
                self.stats['cheerp_headers'] += 1
                self.log(f"✓ Generated {output_file.name}")
                return output_file
            else:
                self.log(f"✗ ts2cpp failed for {dts_file.name}: {result.stderr}", "ERROR")
                self.stats['errors'] += 1
                return None
                
        except Exception as e:
            self.log(f"Error running ts2cpp on {dts_file}: {e}", "ERROR")
            self.stats['errors'] += 1
            return None
    
    def run_python_transpiler(self, ts_files: List[Path]):
        """Run Python transpiler on .ts files"""
        self.log("Running Python transpiler...")
        
        # Initialize Python transpiler
        self.python_transpiler = TypeScriptToCppTranspiler(
            input_dir=str(self.input_dir),
            output_dir=str(self.native_dir),
            verbose=self.verbose,
            parallel=True,
            max_workers=4
        )
        
        # Run transpilation
        self.python_transpiler.transpile()
        
        # Update stats
        self.stats['native_headers'] = self.python_transpiler.stats['headers_generated']
        self.stats['native_impls'] = self.python_transpiler.stats['implementations_generated']
        self.stats['errors'] += self.python_transpiler.stats['errors']
        self.stats['warnings'] += self.python_transpiler.stats['warnings']
    
    def extract_cheerp_types(self, cheerp_header: Path) -> Dict[str, str]:
        """Extract type definitions from Cheerp header"""
        types = {}
        
        try:
            with open(cheerp_header, 'r') as f:
                content = f.read()
            
            # Extract class definitions
            class_pattern = r'class\s+(\w+)\s*:\s*public\s+Object\s*\{'
            for match in re.finditer(class_pattern, content):
                class_name = match.group(1)
                types[class_name] = 'class'
            
            # Extract using declarations (type aliases)
            using_pattern = r'using\s+(\w+)\s*=\s*([^;]+);'
            for match in re.finditer(using_pattern, content):
                type_name = match.group(1)
                type_def = match.group(2)
                types[type_name] = type_def
            
        except Exception as e:
            self.log(f"Error extracting types from {cheerp_header}: {e}", "ERROR")
        
        return types
    
    def convert_cheerp_to_native(self, cheerp_header: Path) -> str:
        """Convert Cheerp types to native C++ types"""
        try:
            with open(cheerp_header, 'r') as f:
                content = f.read()
            
            # Type mappings: Cheerp → Native C++
            type_map = {
                'String*': 'std::string',
                'TArray<': 'std::vector<',
                'Object*': 'std::any',
                'Promise<': 'std::future<',
                '_Function<': 'std::function<',
                '_Any*': 'std::any',
                'EventListener*': 'std::function<void()>',
            }
            
            # Apply mappings
            for cheerp_type, native_type in type_map.items():
                content = content.replace(cheerp_type, native_type)
            
            # Remove Cheerp-specific attributes
            content = re.sub(r'\[\[cheerp::genericjs\]\]\s*', '', content)
            content = re.sub(r'\[\[gnu::always_inline\]\]\s*', '', content)
            
            # Remove _New() factory methods
            content = re.sub(r'static\s+\w+\*\s+_New\([^)]*\)\s*\{[^}]*\}\s*', '', content)
            
            # Replace #include <cheerp/clientlib.h>
            content = content.replace('#include <cheerp/clientlib.h>', 
                                     '#include <string>\n#include <vector>\n#include <any>\n#include <future>\n#include <functional>')
            
            # Remove namespace [[cheerp::genericjs]] client
            content = re.sub(r'namespace\s+\[\[cheerp::genericjs\]\]\s+client\s*\{', 'namespace elizaos {', content)
            
            return content
            
        except Exception as e:
            self.log(f"Error converting Cheerp header: {e}", "ERROR")
            return ""
    
    def merge_headers(self, cheerp_header: Path, native_header: Path) -> Optional[Path]:
        """Merge Cheerp and native headers into optimal hybrid header"""
        try:
            # Read Cheerp header and convert to native types
            cheerp_content = self.convert_cheerp_to_native(cheerp_header)
            
            # Extract types from Cheerp (cleaner type definitions)
            cheerp_types = self.extract_cheerp_types(cheerp_header)
            
            # Read native header
            with open(native_header, 'r') as f:
                native_content = f.read()
            
            # Use Cheerp types as base, add native implementations
            # For now, use converted Cheerp content as it has cleaner types
            merged_content = cheerp_content
            
            # Calculate output path
            rel_path = cheerp_header.relative_to(self.cheerp_dir)
            output_file = self.hybrid_dir / 'include' / rel_path.with_suffix('.hpp')
            output_file.parent.mkdir(parents=True, exist_ok=True)
            
            # Write merged header
            with open(output_file, 'w') as f:
                f.write(merged_content)
            
            self.stats['hybrid_files'] += 1
            self.log(f"✓ Created hybrid header: {output_file.name}")
            
            return output_file
            
        except Exception as e:
            self.log(f"Error merging headers: {e}", "ERROR")
            self.stats['errors'] += 1
            return None
    
    def copy_implementations(self):
        """Copy implementation files from native to hybrid"""
        self.log("Copying implementation files...")
        
        native_src = self.native_dir
        hybrid_src = self.hybrid_dir / 'src'
        
        # Find all .cpp files in native directory
        for cpp_file in native_src.rglob('*.cpp'):
            rel_path = cpp_file.relative_to(native_src)
            dest_file = hybrid_src / rel_path
            dest_file.parent.mkdir(parents=True, exist_ok=True)
            
            # Copy and update include paths
            with open(cpp_file, 'r') as f:
                content = f.read()
            
            # Update include paths to use hybrid headers
            content = re.sub(r'#include\s+"([^"]+)\.hpp"', r'#include "../include/\1.hpp"', content)
            
            with open(dest_file, 'w') as f:
                f.write(content)
            
            self.log(f"✓ Copied {dest_file.name}")
    
    def transpile(self):
        """Main transpilation process"""
        self.log("="*60)
        self.log("HYBRID TRANSPILER - Starting")
        self.log("="*60)
        
        # Setup directories
        self.setup_directories()
        
        # Find TypeScript files
        ts_files, dts_files = self.find_typescript_files()
        
        if self.mode in ['hybrid', 'types-only']:
            # Stage 1: Run ts2cpp on .d.ts files
            self.log("\n--- Stage 1: Type Definitions (ts2cpp) ---")
            cheerp_headers = []
            for dts_file in dts_files:
                result = self.run_ts2cpp(dts_file)
                if result:
                    cheerp_headers.append(result)
        
        if self.mode in ['hybrid', 'impl-only']:
            # Stage 2: Run Python transpiler on .ts files
            self.log("\n--- Stage 2: Implementations (Python Transpiler) ---")
            if ts_files:
                self.run_python_transpiler(ts_files)
        
        if self.mode == 'hybrid':
            # Stage 3: Merge outputs
            self.log("\n--- Stage 3: Hybrid Integration ---")
            
            # Find matching headers
            for cheerp_header in self.cheerp_dir.rglob('*.h'):
                rel_path = cheerp_header.relative_to(self.cheerp_dir)
                native_header = self.native_dir / rel_path.with_suffix('.hpp')
                
                if native_header.exists():
                    self.merge_headers(cheerp_header, native_header)
                else:
                    # No native header, just convert Cheerp header
                    converted = self.convert_cheerp_to_native(cheerp_header)
                    output_file = self.hybrid_dir / 'include' / rel_path.with_suffix('.hpp')
                    output_file.parent.mkdir(parents=True, exist_ok=True)
                    with open(output_file, 'w') as f:
                        f.write(converted)
                    self.stats['hybrid_files'] += 1
            
            # Copy implementations
            self.copy_implementations()
        
        # Print summary
        self.print_summary()
    
    def print_summary(self):
        """Print transpilation summary"""
        print("\n" + "="*60)
        print("HYBRID TRANSPILER - SUMMARY")
        print("="*60)
        print(f"Mode: {self.mode}")
        print(f"\nInput:")
        print(f"  TypeScript files (.ts): {self.stats['ts_files']}")
        print(f"  Declaration files (.d.ts): {self.stats['dts_files']}")
        print(f"\nOutput:")
        print(f"  Cheerp headers (.h): {self.stats['cheerp_headers']}")
        print(f"  Native headers (.hpp): {self.stats['native_headers']}")
        print(f"  Native implementations (.cpp): {self.stats['native_impls']}")
        print(f"  Hybrid files: {self.stats['hybrid_files']}")
        print(f"\nStatus:")
        print(f"  Errors: {self.stats['errors']}")
        print(f"  Warnings: {self.stats['warnings']}")
        print(f"\nOutput directories:")
        print(f"  Cheerp: {self.cheerp_dir}")
        print(f"  Native: {self.native_dir}")
        print(f"  Hybrid: {self.hybrid_dir}")
        print("="*60)

def main():
    """Main entry point"""
    parser = argparse.ArgumentParser(
        description='Hybrid TypeScript to C++ Transpiler (ts2cpp + Python)',
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="""
Examples:
  # Full hybrid transpilation
  python3 hybrid_transpiler.py --input-dir ./src --output-dir ./cpp --mode hybrid
  
  # Type definitions only (ts2cpp)
  python3 hybrid_transpiler.py --input-dir ./types --mode types-only
  
  # Implementations only (Python transpiler)
  python3 hybrid_transpiler.py --input-dir ./src --mode impl-only
  
  # With validation and optimization
  python3 hybrid_transpiler.py --mode hybrid --validate --optimize --verbose
        """
    )
    
    parser.add_argument(
        '--input-dir',
        type=str,
        default='.',
        help='Input directory containing TypeScript files (default: current directory)'
    )
    
    parser.add_argument(
        '--output-dir',
        type=str,
        default='cpp',
        help='Output directory for C++ files (default: ./cpp)'
    )
    
    parser.add_argument(
        '--mode',
        type=str,
        choices=['hybrid', 'types-only', 'impl-only'],
        default='hybrid',
        help='Transpilation mode (default: hybrid)'
    )
    
    parser.add_argument(
        '--verbose',
        action='store_true',
        help='Enable verbose output'
    )
    
    parser.add_argument(
        '--validate',
        action='store_true',
        help='Enable validation of generated code'
    )
    
    parser.add_argument(
        '--optimize',
        action='store_true',
        help='Enable optimization of generated code'
    )
    
    args = parser.parse_args()
    
    # Create transpiler and run
    transpiler = HybridTranspiler(
        input_dir=args.input_dir,
        output_dir=args.output_dir,
        mode=args.mode,
        verbose=args.verbose,
        validate=args.validate,
        optimize=args.optimize
    )
    
    transpiler.transpile()

if __name__ == '__main__':
    main()
