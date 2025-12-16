#!/usr/bin/env python3
"""
TypeScript to C++ Experimental Transpiler

This script uses known code patterns and separation of concerns to convert
TypeScript implementations into approximate C++ equivalents. The goal is to
rapidly generate approximate .cpp and .hpp files that maintain relative paths
of the original TypeScript file structure.

This is an EXPERIMENTAL method for evaluating rapid transpiling effectiveness.
The generated code will likely need manual fixes and is intended for comparison
against a 2-step TS -> Z++ -> C++ approach.

Usage:
    python3 ts_to_cpp_transpiler.py [--input-dir DIR] [--output-dir DIR] [--verbose]
"""

import os
import sys
import re
import argparse
from pathlib import Path
from typing import List, Dict, Tuple, Optional
import json

class TypeScriptToCppTranspiler:
    """Main transpiler class for converting TypeScript to C++"""
    
    def __init__(self, input_dir: str, output_dir: str, verbose: bool = False):
        self.input_dir = Path(input_dir)
        self.output_dir = Path(output_dir)
        self.verbose = verbose
        self.stats = {
            'files_processed': 0,
            'files_skipped': 0,
            'errors': 0
        }
        
        # TypeScript to C++ type mappings
        self.type_mappings = {
            'string': 'std::string',
            'number': 'double',
            'boolean': 'bool',
            'any': 'std::any',
            'void': 'void',
            'null': 'nullptr',
            'undefined': 'std::nullopt',
            'Array': 'std::vector',
            'Map': 'std::unordered_map',
            'Set': 'std::unordered_set',
            'Promise': 'std::future',
            'Function': 'std::function',
            'Record': 'std::unordered_map',
            'Partial': 'std::optional',
            'readonly': 'const',
        }
        
        # Common import patterns to C++ includes
        self.import_to_include = {
            '@elizaos/core': '#include "elizaos/core.hpp"',
            'elizaos': '#include "elizaos/core.hpp"',
        }
    
    def log(self, message: str, level: str = "INFO"):
        """Log message if verbose mode is enabled"""
        if self.verbose or level == "ERROR":
            print(f"[{level}] {message}")
    
    def convert_type(self, ts_type: str) -> str:
        """Convert TypeScript type to C++ type"""
        # Handle array types like string[] or Array<string>
        if ts_type.endswith('[]'):
            inner_type = ts_type[:-2].strip()
            return f'std::vector<{self.convert_type(inner_type)}>'
        
        # Handle generic types like Array<T>, Map<K,V>
        generic_match = re.match(r'(\w+)<(.+)>', ts_type)
        if generic_match:
            base_type = generic_match.group(1)
            params = generic_match.group(2)
            
            if base_type in self.type_mappings:
                cpp_base = self.type_mappings[base_type]
                # Split params by comma (but respect nested generics)
                param_parts = self.split_generic_params(params)
                cpp_params = ', '.join([self.convert_type(p.strip()) for p in param_parts])
                return f'{cpp_base}<{cpp_params}>'
        
        # Direct type mapping
        if ts_type in self.type_mappings:
            return self.type_mappings[ts_type]
        
        return ts_type
    
    def split_generic_params(self, params: str) -> List[str]:
        """Split generic parameters respecting nested generics"""
        parts = []
        current = []
        depth = 0
        
        for char in params:
            if char == '<':
                depth += 1
            elif char == '>':
                depth -= 1
            elif char == ',' and depth == 0:
                parts.append(''.join(current))
                current = []
                continue
            current.append(char)
        
        if current:
            parts.append(''.join(current))
        
        return parts
    
    def convert_imports(self, ts_content: str) -> Tuple[List[str], str]:
        """Convert TypeScript imports to C++ includes"""
        includes = set()
        includes.add('#pragma once')  # For header files
        includes.add('#include <string>')
        includes.add('#include <vector>')
        includes.add('#include <memory>')
        includes.add('#include <optional>')
        includes.add('#include <functional>')
        includes.add('#include <unordered_map>')
        
        # Extract import statements
        import_pattern = r'import\s+(?:{([^}]+)}|(\w+)|\*\s+as\s+(\w+))\s+from\s+["\']([^"\']+)["\']'
        
        for match in re.finditer(import_pattern, ts_content):
            module_path = match.group(4)
            
            # Convert module path to include
            if module_path.startswith('@elizaos/'):
                module_name = module_path.split('/')[-1]
                includes.add(f'#include "elizaos/{module_name}.hpp"')
            elif module_path.startswith('.'):
                # Relative import
                clean_path = module_path.replace('./', '').replace('../', '')
                if not clean_path.endswith('.hpp'):
                    clean_path += '.hpp'
                includes.add(f'#include "{clean_path}"')
        
        # Remove import statements from content (including any trailing semicolons)
        content_no_imports = re.sub(import_pattern + r';?', '', ts_content)
        
        # Clean up any remaining stray semicolons after import removal
        content_no_imports = re.sub(r'^\s*;\s*$', '', content_no_imports, flags=re.MULTILINE)
        
        return sorted(list(includes)), content_no_imports
    
    def convert_interface(self, interface_match: re.Match) -> str:
        """Convert TypeScript interface to C++ struct"""
        interface_name = interface_match.group(1)
        interface_body = interface_match.group(2)
        
        cpp_struct = f'struct {interface_name} {{\n'
        
        # Parse interface members (handle multiline)
        member_pattern = r'(\w+)(\?)?:\s*([^;,}]+)[;,]?'
        for member_match in re.finditer(member_pattern, interface_body):
            member_name = member_match.group(1)
            is_optional = member_match.group(2) == '?'
            member_type = member_match.group(3).strip()
            
            # Skip keywords like 'true' or 'false' which aren't types
            if member_type in ['true', 'false']:
                continue
            
            cpp_type = self.convert_type(member_type)
            
            if is_optional:
                cpp_struct += f'    std::optional<{cpp_type}> {member_name};\n'
            else:
                cpp_struct += f'    {cpp_type} {member_name};\n'
        
        cpp_struct += '};\n'
        return cpp_struct
    
    def convert_function(self, func_match: re.Match, is_async: bool = False) -> str:
        """Convert TypeScript function to C++ function"""
        func_name = func_match.group(1)
        params = func_match.group(2)
        return_type = func_match.group(3) if func_match.lastindex >= 3 else 'void'
        
        # Convert parameters
        cpp_params = []
        if params.strip():
            param_list = params.split(',')
            for param in param_list:
                param = param.strip()
                if ':' in param:
                    param_name, param_type = param.split(':', 1)
                    param_name = param_name.strip()
                    param_type = param_type.strip()
                    cpp_type = self.convert_type(param_type)
                    
                    # Use const reference for complex types
                    if cpp_type.startswith('std::'):
                        cpp_params.append(f'const {cpp_type}& {param_name}')
                    else:
                        cpp_params.append(f'{cpp_type} {param_name}')
                else:
                    cpp_params.append(f'auto {param.strip()}')
        
        # Convert return type
        cpp_return = self.convert_type(return_type) if return_type else 'void'
        
        # Handle async functions
        if is_async and cpp_return != 'void':
            cpp_return = f'std::future<{cpp_return}>'
        
        params_str = ', '.join(cpp_params)
        return f'{cpp_return} {func_name}({params_str})'
    
    def convert_class(self, class_match: re.Match) -> str:
        """Convert TypeScript class to C++ class"""
        class_name = class_match.group(1)
        class_body = class_match.group(2)
        
        cpp_class = f'class {class_name} {{\npublic:\n'
        
        # Extract constructor
        constructor_pattern = r'constructor\s*\(([^)]*)\)\s*{([^}]*)}'
        constructor_match = re.search(constructor_pattern, class_body)
        
        if constructor_match:
            params = constructor_match.group(1)
            cpp_class += f'    {class_name}({params});\n'
        
        # Extract methods
        method_pattern = r'(?:async\s+)?(\w+)\s*\(([^)]*)\)\s*(?::\s*([^{]+))?\s*{'
        for method_match in re.finditer(method_pattern, class_body):
            method_name = method_match.group(1)
            if method_name != 'constructor':
                is_async = 'async' in class_body[:method_match.start()]
                func_sig = self.convert_function(method_match, is_async)
                cpp_class += f'    {func_sig};\n'
        
        # Extract properties
        property_pattern = r'(?:private|public|protected)?\s+(\w+):\s*([^;=]+)[;=]'
        cpp_class += '\nprivate:\n'
        for prop_match in re.finditer(property_pattern, class_body):
            prop_name = prop_match.group(1)
            prop_type = prop_match.group(2).strip()
            cpp_type = self.convert_type(prop_type)
            cpp_class += f'    {cpp_type} {prop_name}_;\n'
        
        cpp_class += '};\n'
        return cpp_class
    
    def convert_export(self, ts_content: str) -> str:
        """Handle export statements (mostly remove them for C++)"""
        # Remove export keywords
        content = re.sub(r'\bexport\s+', '', ts_content)
        # Remove export default
        content = re.sub(r'export\s+default\s+', '', content)
        return content
    
    def convert_const_enum(self, ts_content: str) -> str:
        """Convert const/let/var to appropriate C++ equivalents"""
        # const -> const
        # let/var -> auto (in implementation) or member variable
        content = re.sub(r'\bconst\s+(\w+):\s*(\w+)\s*=', r'const \2 \1 =', ts_content)
        content = re.sub(r'\b(?:let|var)\s+(\w+):\s*(\w+)\s*=', r'\2 \1 =', content)
        
        # Handle const without type
        content = re.sub(r'\bconst\s+(\w+)\s*=', r'const auto \1 =', content)
        content = re.sub(r'\b(?:let|var)\s+(\w+)\s*=', r'auto \1 =', content)
        
        return content
    
    def generate_header(self, ts_content: str, namespace: str = 'elizaos') -> str:
        """Generate C++ header file from TypeScript content"""
        includes, content = self.convert_imports(ts_content)
        
        # Remove export statements first
        content = self.convert_export(content)
        
        # Convert interfaces (use more sophisticated regex to handle nested braces)
        interface_converted = []
        interface_pattern = r'interface\s+(\w+)\s*\{'
        pos = 0
        while True:
            match = re.search(interface_pattern, content[pos:])
            if not match:
                break
            
            start = pos + match.start()
            name = match.group(1)
            brace_start = pos + match.end() - 1
            
            # Find matching closing brace
            brace_count = 1
            i = brace_start + 1
            while i < len(content) and brace_count > 0:
                if content[i] == '{':
                    brace_count += 1
                elif content[i] == '}':
                    brace_count -= 1
                i += 1
            
            if brace_count == 0:
                interface_body = content[brace_start+1:i-1]
                interface_str = self.convert_interface_body(name, interface_body)
                interface_converted.append((start, i, interface_str))
            
            pos = i
        
        # Apply replacements in reverse order
        for start, end, replacement in reversed(interface_converted):
            content = content[:start] + replacement + content[end:]
        
        # Convert classes (similar approach)
        class_pattern = r'class\s+(\w+)(?:\s+extends\s+\w+)?(?:\s+implements\s+[\w,\s]+)?\s*\{'
        # Similar nested brace handling for classes...
        
        # Convert type aliases - handle union types
        type_pattern = r'type\s+(\w+)\s*=\s*([^;]+);'
        def convert_type_alias(m):
            name = m.group(1)
            type_def = m.group(2).strip()
            # Convert union types A | B to std::variant<A, B>
            if '|' in type_def:
                types = [self.convert_type(t.strip()) for t in type_def.split('|')]
                return f'using {name} = std::variant<{", ".join(types)}>;'
            else:
                return f'using {name} = {self.convert_type(type_def)};'
        content = re.sub(type_pattern, convert_type_alias, content)
        
        # Extract and convert function declarations properly
        # First, handle async functions with proper brace matching
        content = self._extract_function_declarations(content)
        
        # Keep only declaration lines (structs, using, function signatures, comments)
        lines = content.split('\n')
        filtered_lines = []
        in_struct = False
        struct_brace_count = 0
        
        for line in lines:
            stripped = line.strip()
            
            # Keep comments
            if stripped.startswith('//') or stripped.startswith('/*') or stripped.startswith('*'):
                filtered_lines.append(line)
                continue
            
            # Track struct/class blocks
            if re.match(r'^\s*(?:struct|class|enum)\s+\w+', line):
                in_struct = True
                struct_brace_count = 0
                filtered_lines.append(line)
                continue
            
            if in_struct:
                # Count braces
                struct_brace_count += line.count('{') - line.count('}')
                filtered_lines.append(line)
                if struct_brace_count <= 0 and '}' in line:
                    in_struct = False
                continue
            
            # Keep using statements (type aliases)
            if re.match(r'^\s*using\s+\w+\s*=', line):
                filtered_lines.append(line)
                continue
            
            # Keep function declarations (end with ;)
            if re.match(r'^\s*(?:std::)?\w+(?:<[^>]+>)?\s+\w+\s*\([^)]*\)\s*;', line):
                filtered_lines.append(line)
                continue
            
            # Keep blank lines for readability
            if not stripped:
                filtered_lines.append(line)
                continue
        
        content = '\n'.join(filtered_lines)
        
        # Clean up extra blank lines
        content = re.sub(r'\n\s*\n\s*\n+', '\n\n', content)
        
        # Build header
        header = '\n'.join(includes) + '\n\n'
        header += f'namespace {namespace} {{\n\n'
        header += '// NOTE: This is auto-generated approximate C++ code\n'
        header += '// Manual refinement required for production use\n\n'
        header += content
        header += f'\n}} // namespace {namespace}\n'
        
        return header
    
    def _extract_function_declarations(self, content: str) -> str:
        """Extract function declarations while removing implementations.
        Uses proper brace matching to avoid code leakage."""
        result = []
        pos = 0
        
        # Pattern for function declaration start
        func_pattern = r'(export\s+)?(async\s+)?function\s+(\w+)\s*\(([^)]*)\)\s*(?::\s*([^{]+))?\s*\{'
        
        while pos < len(content):
            match = re.search(func_pattern, content[pos:])
            if not match:
                # No more functions, append remaining content
                result.append(content[pos:])
                break
            
            # Append content before function
            result.append(content[pos:pos + match.start()])
            
            # Extract function info
            is_async = match.group(2) is not None
            func_name = match.group(3)
            params = match.group(4)
            return_type = match.group(5).strip() if match.group(5) else None
            
            # Convert to C++ declaration
            cpp_params = self.convert_params(params)
            
            # Handle return type
            if return_type:
                # Remove Promise< > wrapper if async
                if is_async and return_type.startswith('Promise<'):
                    inner_type = return_type[8:-1]  # Remove 'Promise<' and '>'
                    cpp_return = self.convert_type(inner_type)
                    cpp_return = f'std::future<{cpp_return}>'
                else:
                    cpp_return = self.convert_type(return_type)
                    if is_async:
                        cpp_return = f'std::future<{cpp_return}>'
            else:
                cpp_return = 'void'
                if is_async:
                    cpp_return = 'std::future<void>'
            
            # Generate declaration
            result.append(f'{cpp_return} {func_name}({cpp_params});')
            
            # Find matching closing brace to skip function body
            brace_start = pos + match.end() - 1  # Position of opening {
            brace_count = 1
            i = brace_start + 1
            
            while i < len(content) and brace_count > 0:
                if content[i] == '{':
                    brace_count += 1
                elif content[i] == '}':
                    brace_count -= 1
                i += 1
            
            # Move past the function body
            pos = i
        
        return ''.join(result)
    
    def convert_interface_body(self, name: str, body: str) -> str:
        """Convert interface body to C++ struct"""
        cpp_struct = f'struct {name} {{\n'
        
        # Parse members more carefully
        lines = body.split('\n')
        for line in lines:
            line = line.strip()
            if not line or line.startswith('//'):
                continue
            
            # Match property: type pattern
            match = re.match(r'(\w+)(\?)?:\s*(.+?)[;,]?$', line)
            if match:
                prop_name = match.group(1)
                is_optional = match.group(2) == '?'
                prop_type = match.group(3).strip()
                
                # Skip invalid types
                if prop_type in ['true', 'false']:
                    continue
                
                cpp_type = self.convert_type(prop_type)
                
                if is_optional:
                    cpp_struct += f'    std::optional<{cpp_type}> {prop_name};\n'
                else:
                    cpp_struct += f'    {cpp_type} {prop_name};\n'
        
        cpp_struct += '};\n'
        return cpp_struct
    
    def convert_params(self, params: str) -> str:
        """Convert TypeScript parameters to C++ parameters"""
        if not params.strip():
            return ''
        
        cpp_params = []
        param_list = params.split(',')
        
        for param in param_list:
            param = param.strip()
            if not param:
                continue
            
            # Handle optional parameters
            is_optional = '?' in param
            param = param.replace('?', '')
            
            if ':' in param:
                param_name, param_type = param.split(':', 1)
                param_name = param_name.strip()
                param_type = param_type.strip()
                cpp_type = self.convert_type(param_type)
                
                if is_optional:
                    cpp_params.append(f'std::optional<{cpp_type}> {param_name}')
                elif cpp_type.startswith('std::'):
                    cpp_params.append(f'const {cpp_type}& {param_name}')
                else:
                    cpp_params.append(f'{cpp_type} {param_name}')
            else:
                cpp_params.append(f'auto {param}')
        
        return ', '.join(cpp_params)
    
    def generate_implementation(self, ts_content: str, namespace: str = 'elizaos') -> str:
        """Generate C++ implementation file from TypeScript content"""
        # Extract function implementations
        impl = f'#include "{{header_name}}"\n\n'
        impl += f'namespace {namespace} {{\n\n'
        
        # Convert function bodies (simplified)
        # In a real transpiler, this would parse and convert function logic
        impl += '// TODO: Implement function bodies\n'
        impl += '// Original TypeScript code has been analyzed\n'
        impl += '// Manual implementation required for complete functionality\n\n'
        
        impl += f'}} // namespace {namespace}\n'
        
        return impl
    
    def should_process_file(self, file_path: Path) -> bool:
        """Determine if a TypeScript file should be processed"""
        # Skip test files
        if 'test' in file_path.stem.lower() or file_path.stem.endswith('.test'):
            return False
        
        # Skip spec files
        if file_path.stem.endswith('.spec'):
            return False
        
        # Skip config files
        if file_path.stem in ['tsconfig', 'vite.config', 'jest.config', 'webpack.config']:
            return False
        
        # Skip type definition files (we'll handle them differently)
        if file_path.suffix == '.d.ts':
            return False
        
        return True
    
    def process_file(self, ts_file: Path) -> bool:
        """Process a single TypeScript file"""
        try:
            # Read TypeScript content
            with open(ts_file, 'r', encoding='utf-8') as f:
                ts_content = f.read()
            
            # Calculate relative path
            rel_path = ts_file.relative_to(self.input_dir)
            
            # Determine output paths
            output_rel_path = rel_path.with_suffix('')
            header_path = self.output_dir / output_rel_path.with_suffix('.hpp')
            impl_path = self.output_dir / output_rel_path.with_suffix('.cpp')
            
            # Create output directory
            header_path.parent.mkdir(parents=True, exist_ok=True)
            
            # Generate header file
            self.log(f"Processing {rel_path} -> {header_path.relative_to(self.output_dir)}")
            header_content = self.generate_header(ts_content)
            
            with open(header_path, 'w', encoding='utf-8') as f:
                f.write(header_content)
            
            # Generate implementation file
            impl_content = self.generate_implementation(ts_content)
            impl_content = impl_content.replace('{header_name}', header_path.name)
            
            with open(impl_path, 'w', encoding='utf-8') as f:
                f.write(impl_content)
            
            self.stats['files_processed'] += 1
            return True
            
        except Exception as e:
            self.log(f"Error processing {ts_file}: {e}", "ERROR")
            self.stats['errors'] += 1
            return False
    
    def transpile(self):
        """Main transpilation process"""
        self.log(f"Starting transpilation from {self.input_dir} to {self.output_dir}")
        
        # Find all TypeScript files
        ts_files = list(self.input_dir.rglob('*.ts')) + list(self.input_dir.rglob('*.tsx'))
        self.log(f"Found {len(ts_files)} TypeScript files")
        
        # Process each file
        for ts_file in ts_files:
            if self.should_process_file(ts_file):
                self.process_file(ts_file)
            else:
                self.log(f"Skipping {ts_file.name}")
                self.stats['files_skipped'] += 1
        
        # Print summary
        self.print_summary()
    
    def print_summary(self):
        """Print transpilation summary"""
        print("\n" + "="*60)
        print("TRANSPILATION SUMMARY")
        print("="*60)
        print(f"Files processed: {self.stats['files_processed']}")
        print(f"Files skipped: {self.stats['files_skipped']}")
        print(f"Errors: {self.stats['errors']}")
        print(f"Output directory: {self.output_dir}")
        print("="*60)
        print("\nNOTE: Generated code is approximate and will require manual fixes.")
        print("This is an experimental rapid transpilation for evaluation purposes.")
        print("="*60)

def main():
    """Main entry point"""
    parser = argparse.ArgumentParser(
        description='Experimental TypeScript to C++ Transpiler',
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="""
Examples:
  # Convert all TypeScript files in current repo to excpp/
  python3 ts_to_cpp_transpiler.py
  
  # Convert specific directory
  python3 ts_to_cpp_transpiler.py --input-dir ./otaku/src --output-dir ./excpp/otaku
  
  # Verbose output
  python3 ts_to_cpp_transpiler.py --verbose
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
        default='excpp',
        help='Output directory for C++ files (default: ./excpp)'
    )
    
    parser.add_argument(
        '--verbose',
        action='store_true',
        help='Enable verbose output'
    )
    
    args = parser.parse_args()
    
    # Create transpiler and run
    transpiler = TypeScriptToCppTranspiler(
        input_dir=args.input_dir,
        output_dir=args.output_dir,
        verbose=args.verbose
    )
    
    transpiler.transpile()

if __name__ == '__main__':
    main()
