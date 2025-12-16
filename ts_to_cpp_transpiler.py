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

Version 2.1 - Performance improvements and bug fixes

Usage:
    python3 ts_to_cpp_transpiler.py [--input-dir DIR] [--output-dir DIR] [--verbose] [--parallel]
"""

import os
import sys
import re
import argparse
from pathlib import Path
from typing import List, Dict, Tuple, Optional
import json
from concurrent.futures import ThreadPoolExecutor, as_completed
import time

class TypeScriptToCppTranspiler:
    """Main transpiler class for converting TypeScript to C++"""
    
    def __init__(self, input_dir: str, output_dir: str, verbose: bool = False, parallel: bool = False, max_workers: int = 4):
        self.input_dir = Path(input_dir)
        self.output_dir = Path(output_dir)
        self.verbose = verbose
        self.parallel = parallel
        self.max_workers = max_workers
        self.stats = {
            'files_processed': 0,
            'files_skipped': 0,
            'errors': 0,
            'warnings': 0,
            'headers_generated': 0,
            'implementations_generated': 0,
            'start_time': None,
            'end_time': None
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
        
        # Compiled regex patterns for performance
        self._compiled_patterns = {}
        self._compile_patterns()
    
    def _compile_patterns(self):
        """Pre-compile frequently used regex patterns for better performance"""
        self._compiled_patterns = {
            'interface': re.compile(r'interface\s+(\w+)\s*\{'),
            'class': re.compile(r'(?:export\s+)?class\s+(\w+)(?:\s+extends\s+\w+)?(?:\s+implements\s+[\w,\s]+)?\s*\{'),
            'function': re.compile(r'(export\s+)?(async\s+)?function\s+(\w+)\s*\(([^)]*)\)\s*(?::\s*([^{]+))?\s*\{'),
            'type_alias': re.compile(r'type\s+(\w+)\s*=\s*([^;]+);'),
            'import': re.compile(r'import\s+(?:{([^}]+)}|(\w+)|\*\s+as\s+(\w+))\s+from\s+["\']([^"\']+)["\']'),
            'generic': re.compile(r'(\w+)<(.+)>'),
            'property': re.compile(r'^(\w+)(\?)?:\s*([^;,{}\(\)]+?)$'),
            'ts_code_patterns': [
                re.compile(r'\breturn\s+\{'),
                re.compile(r'\bawait\s+'),
                re.compile(r'\bconsole\.'),
                re.compile(r'\bprocess\.'),
                re.compile(r'=>\s*\{'),
                re.compile(r'\bJSON\.'),
                re.compile(r'\brequire\('),
                re.compile(r'\bimport\s+'),
                re.compile(r'===|!=='),
                re.compile(r'\bconst\s+\w+\s*=\s*\{'),
                re.compile(r'\blet\s+\w+\s*=\s*\{'),
                re.compile(r'\.then\('),
                re.compile(r'\.catch\('),
            ]
        }
    
    def log(self, message: str, level: str = "INFO"):
        """Log message if verbose mode is enabled"""
        if self.verbose or level == "ERROR":
            print(f"[{level}] {message}")
    
    def convert_type(self, ts_type: str) -> str:
        """Convert TypeScript type to C++ type"""
        # Clean up the type string
        ts_type = ts_type.strip()
        
        # Handle React-specific types FIRST (before union splitting)
        if ts_type.startswith('React.'):
            return self.convert_react_type(ts_type)
        
        # Handle object literal types like { path: string; hiddenTools?: string[] }
        if ts_type.startswith('{') and ts_type.endswith('}'):
            return 'std::any'  # Simplified - could be a struct
        
        # Handle union types like string | null
        if '|' in ts_type and not '=>' in ts_type:
            types = [self.convert_type(t.strip()) for t in ts_type.split('|')]
            # Simplify null unions to optional
            if 'null' in ts_type.lower():
                non_null_types = [t for t in types if t not in ['null', 'nullptr', 'std::nullopt']]
                if len(non_null_types) == 1:
                    base_type = non_null_types[0]
                    # Don't double-wrap optionals
                    if not base_type.startswith('std::optional<'):
                        return f'std::optional<{base_type}>'
                    return base_type
            # Remove redundant nullptr and nullopt from variant
            types = [t for t in types if t not in ['nullptr', 'std::nullopt']]
            if len(types) == 1:
                return types[0]
            return f'std::variant<{", ".join(types)}>'
        
        # Handle function types like (value: string) => void
        if '=>' in ts_type:
            return self.convert_function_type(ts_type)
        
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
        
        # Handle enum member access like ChannelType.DM
        if '.' in ts_type:
            parts = ts_type.split('.')
            if len(parts) == 2:
                # Convert EnumType.Member to EnumType::Member
                return f'{parts[0]}::{parts[1]}'
        
        # Direct type mapping
        if ts_type in self.type_mappings:
            return self.type_mappings[ts_type]
        
        return ts_type
    
    def convert_function_type(self, ts_type: str) -> str:
        """Convert TypeScript function type to std::function"""
        # Pattern: (param1: type1, param2: type2) => returnType
        match = re.match(r'\(([^)]*)\)\s*=>\s*(.+)', ts_type)
        if match:
            params_str = match.group(1)
            return_type = match.group(2).strip()
            
            # Parse parameters
            cpp_param_types = []
            if params_str.strip():
                params = params_str.split(',')
                for param in params:
                    if ':' in param:
                        param_type = param.split(':', 1)[1].strip()
                        cpp_param_types.append(self.convert_type(param_type))
                    else:
                        cpp_param_types.append('auto')
            
            # Convert return type
            cpp_return = self.convert_type(return_type)
            
            # Build std::function type
            if cpp_param_types:
                return f'std::function<{cpp_return}({", ".join(cpp_param_types)})>'
            else:
                return f'std::function<{cpp_return}()>'
        
        return 'std::function<void()>'
    
    def convert_react_type(self, ts_type: str) -> str:
        """Convert React-specific types to C++ equivalents"""
        # React.RefObject<T> -> std::shared_ptr<T> or std::optional<std::shared_ptr<T>>
        if ts_type.startswith('React.RefObject<'):
            inner = ts_type[16:-1]  # Extract T from React.RefObject<T>
            # Check if inner is a union with null
            if '| null' in inner or 'null |' in inner:
                # Remove null from the type
                base_type = inner.replace('| null', '').replace('null |', '').strip()
                return f'std::optional<std::shared_ptr<{base_type}>>'
            inner_cpp = self.convert_type(inner)
            return f'std::shared_ptr<{inner_cpp}>'
        
        # React.FC or React.FunctionComponent -> void (function pointer)
        if 'React.FC' in ts_type or 'React.FunctionComponent' in ts_type:
            return 'void'
        
        # React.ChangeEvent<T>, FormEvent<T>, etc. -> EventType<T>
        if 'Event<' in ts_type:
            # Extract the generic parameter
            match = re.search(r'React\.(\w+Event)<([^>]+)>', ts_type)
            if match:
                event_type = match.group(1)
                param = match.group(2)
                return f'{event_type}<{param}>'
            return 'EventType'
        
        # React.ChangeEvent, FormEvent, etc. (without generics)
        if 'React.' in ts_type and 'Event' in ts_type:
            event_type = ts_type.replace('React.', '')
            return event_type
        
        # React.KeyboardEvent, MouseEvent, etc.
        if 'React.' in ts_type:
            return ts_type.replace('React.', '')
        
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
        includes.add('#pragma once')  # For header files (will be moved to first line)
        includes.add('#include <string>')
        includes.add('#include <vector>')
        includes.add('#include <memory>')
        includes.add('#include <optional>')
        includes.add('#include <functional>')
        includes.add('#include <unordered_map>')
        
        # Track if we need additional includes based on usage
        self._needs_any = False
        self._needs_variant = False
        self._needs_future = False
        
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
    
    def add_conditional_includes(self, includes: List[str], content: str) -> List[str]:
        """Add includes based on content analysis"""
        includes_set = set(includes)
        
        # Check for std::any usage
        if 'std::any' in content:
            includes_set.add('#include <any>')
        
        # Check for std::variant usage
        if 'std::variant' in content:
            includes_set.add('#include <variant>')
        
        # Check for std::future usage
        if 'std::future' in content:
            includes_set.add('#include <future>')
        
        # Sort includes properly: pragma once first, then system, then project
        result = []
        if '#pragma once' in includes_set:
            result.append('#pragma once')
            includes_set.remove('#pragma once')
        
        # Add system includes (starts with <)
        system_includes = sorted([i for i in includes_set if i.startswith('#include <')])
        result.extend(system_includes)
        
        # Add project includes (starts with #include ")
        project_includes = sorted([i for i in includes_set if i.startswith('#include "')])
        result.extend(project_includes)
        
        return result
    
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
    
    def convert_class_body(self, name: str, body: str) -> str:
        """Convert TypeScript class body to C++ class with proper brace matching"""
        cpp_class = f'class {name} {{\npublic:\n'
        
        # Extract constructor using brace matching
        constructor_pattern = r'constructor\s*\(([^)]*)\)\s*\{'
        constructor_match = re.search(constructor_pattern, body)
        
        if constructor_match:
            params_str = constructor_match.group(1)
            # Convert constructor parameters
            cpp_params = self.convert_params(params_str)
            cpp_class += f'    {name}({cpp_params});\n'
        
        # Extract method signatures using brace matching
        method_pattern = r'(?:public|private|protected)?\s*(?:async\s+)?(?:get\s+)?(\w+)\s*\(([^)]*)\)\s*(?::\s*([^{]+))?\s*\{'
        pos = 0
        methods = []
        
        while pos < len(body):
            match = re.search(method_pattern, body[pos:])
            if not match:
                break
            
            method_name = match.group(1)
            if method_name == 'constructor':
                # Skip past constructor body
                brace_start = pos + match.end() - 1
                brace_count = 1
                i = brace_start + 1
                while i < len(body) and brace_count > 0:
                    if body[i] == '{':
                        brace_count += 1
                    elif body[i] == '}':
                        brace_count -= 1
                    i += 1
                pos = i
                continue
            
            params_str = match.group(2)
            return_type = match.group(3).strip() if match.group(3) else 'void'
            
            # Check if method is async or a getter
            prefix = body[max(0, pos + match.start() - 30):pos + match.start()]
            is_async = 'async' in prefix
            is_getter = 'get ' in body[pos + match.start():pos + match.start() + 10]
            
            # Convert parameters
            cpp_params = self.convert_params(params_str)
            
            # Convert return type
            cpp_return = self.convert_type(return_type)
            if is_async and cpp_return != 'void':
                cpp_return = f'std::future<{cpp_return}>'
            
            # Getters should be const methods
            const_suffix = ' const' if is_getter and not cpp_params else ''
            methods.append(f'    {cpp_return} {method_name}({cpp_params}){const_suffix};')
            
            # Skip past method body
            brace_start = pos + match.end() - 1
            brace_count = 1
            i = brace_start + 1
            while i < len(body) and brace_count > 0:
                if body[i] == '{':
                    brace_count += 1
                elif body[i] == '}':
                    brace_count -= 1
                i += 1
            pos = i
        
        # Add methods to class
        if methods:
            for method in methods:
                cpp_class += method + '\n'
        
        # Extract member variables - only from class body, not from inside methods
        # We'll be more conservative and only extract clear property declarations at the class level
        properties = []
        
        # Look for simple property declarations like "path: string;"
        # Split by methods/constructor to get only class-level declarations
        parts = re.split(r'(?:constructor|get|(?:async\s+)?[\w]+)\s*\([^)]*\)\s*\{', body)
        if parts:
            class_level_code = parts[0]  # Only the part before any methods/constructor
            property_pattern = r'^\s*(?:private|public|protected)?\s+(\w+):\s*([^;={]+);'
            for line in class_level_code.split('\n'):
                prop_match = re.match(property_pattern, line)
                if prop_match:
                    prop_name = prop_match.group(1)
                    prop_type = prop_match.group(2).strip()
                    
                    # Skip if this looks like a method
                    if '(' in prop_type:
                        continue
                    
                    cpp_type = self.convert_type(prop_type)
                    properties.append(f'    {cpp_type} {prop_name}_;')
        
        if properties:
            cpp_class += '\nprivate:\n'
            for prop in properties:
                cpp_class += prop + '\n'
        
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
        
        # Convert classes (similar approach with proper brace matching)
        class_converted = []
        class_pattern = r'(?:export\s+)?class\s+(\w+)(?:\s+extends\s+\w+)?(?:\s+implements\s+[\w,\s]+)?\s*\{'
        pos = 0
        while True:
            match = re.search(class_pattern, content[pos:])
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
                class_body = content[brace_start+1:i-1]
                class_str = self.convert_class_body(name, class_body)
                class_converted.append((start, i, class_str))
            
            pos = i
        
        # Apply class replacements in reverse order
        for start, end, replacement in reversed(class_converted):
            content = content[:start] + replacement + content[end:]
        
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
            
            # Skip lines that contain obvious TypeScript/JavaScript code
            typescript_patterns = [
                r'\breturn\s+\{',  # return { ... }
                r'\bawait\s+',     # await ...
                r'\bconsole\.',    # console.log, etc.
                r'\bprocess\.',    # process.env, etc.
                r'=>\s*\{',        # arrow functions with bodies
                r'\bJSON\.',       # JSON.stringify, etc.
                r'\brequire\(',    # require(...)
                r'\bimport\s+',    # import statements (should be removed earlier but check again)
                r'===|!==',        # strict equality (common in TS/JS)
                r'\bconst\s+\w+\s*=\s*\{',  # const x = { ... }
                r'\blet\s+\w+\s*=\s*\{',    # let x = { ... }
                r'\.then\(',       # Promise.then
                r'\.catch\(',      # Promise.catch
            ]
            
            # Check if line contains TypeScript code
            is_typescript_code = any(re.search(pattern, stripped) for pattern in typescript_patterns)
            if is_typescript_code:
                continue
            
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
                
                # Inside struct, only keep valid member declarations
                # Skip lines that look like code
                if not is_typescript_code and (
                    stripped.startswith('std::') or  # C++ types
                    stripped.startswith('const ') or  # const members
                    stripped.endswith(';') or  # declarations
                    stripped.endswith('{') or stripped.endswith('}') or  # braces
                    re.match(r'^\w+:', stripped) or  # labels (rare but valid)
                    not stripped  # empty lines
                ):
                    filtered_lines.append(line)
                
                if struct_brace_count <= 0 and '}' in line:
                    in_struct = False
                continue
            
            # Keep using statements (type aliases)
            if re.match(r'^\s*using\s+\w+\s*=', line):
                filtered_lines.append(line)
                continue
            
            # Keep function declarations (end with ; and look like C++ signatures)
            if re.match(r'^\s*(?:std::)?\w+(?:<[^>]+>)?\s+\w+\s*\([^)]*\)\s*(?:const\s*)?;', line):
                filtered_lines.append(line)
                continue
            
            # Keep blank lines for readability
            if not stripped:
                filtered_lines.append(line)
                continue
        
        content = '\n'.join(filtered_lines)
        
        # Clean up extra blank lines
        content = re.sub(r'\n\s*\n\s*\n+', '\n\n', content)
        
        # Add conditional includes based on content
        includes = self.add_conditional_includes(includes, content)
        
        # Build header with proper include ordering
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
        
        # Parse members more carefully - handle multiple properties per line
        lines = body.split('\n')
        for line in lines:
            line = line.strip()
            if not line or line.startswith('//') or line.startswith('/*') or line.startswith('*'):
                continue
            
            # Skip lines that are just braces or empty
            if line in ['{', '}', ';']:
                continue
            
            # Try to match individual property declarations
            # Handle format: propertyName?: Type;
            # Also handle: propertyName: Type, propertyName2: Type2
            
            # Split by semicolon first to handle multiple statements
            statements = line.split(';')
            for statement in statements:
                statement = statement.strip()
                if not statement:
                    continue
                
                # Match property: type pattern
                # More strict pattern to avoid matching code
                match = re.match(r'^(\w+)(\?)?:\s*([^;,{}\(\)]+?)$', statement)
                if match:
                    prop_name = match.group(1)
                    is_optional = match.group(2) == '?'
                    prop_type = match.group(3).strip()
                    
                    # Skip invalid types and keywords
                    if prop_type in ['true', 'false', 'function', 'class', 'interface']:
                        continue
                    
                    # Skip if type looks like it contains code (parentheses, braces, etc)
                    if any(char in prop_type for char in ['{', '}', '(']):
                        continue
                    
                    cpp_type = self.convert_type(prop_type)
                    
                    # Make sure the converted type is valid
                    if cpp_type and not any(invalid in cpp_type for invalid in ['undefined', 'export', 'import']):
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
    
    def validate_generated_header(self, header_content: str, file_path: str) -> list:
        """Validate generated header file and return list of issues found"""
        issues = []
        
        # Check for TypeScript code patterns that shouldn't be in C++
        ts_patterns = {
            r'=> void': 'Arrow function syntax detected',
            r'=> string': 'Arrow function syntax detected',
            r'=> number': 'Arrow function syntax detected',
            r'\bawait\s+': 'await keyword detected (TypeScript)',
            r'\basync function': 'async function keyword detected (should be converted)',
            r'\bconsole\.': 'console object detected (JavaScript)',
            r'\bprocess\.': 'process object detected (Node.js)',
            r'===|!==': 'Strict equality operators detected (JavaScript)',
            r'\.then\(': 'Promise.then detected',
            r'\.catch\(': 'Promise.catch detected',
        }
        
        for pattern, message in ts_patterns.items():
            if re.search(pattern, header_content):
                issues.append(f"{message}: found pattern '{pattern}'")
        
        # Check for basic C++ requirements
        if '#pragma once' not in header_content:
            issues.append("Missing '#pragma once' directive")
        
        if 'namespace elizaos' not in header_content:
            issues.append("Missing 'namespace elizaos' declaration")
        
        # Check for mismatched braces
        open_braces = header_content.count('{')
        close_braces = header_content.count('}')
        if open_braces != close_braces:
            issues.append(f"Brace mismatch: {open_braces} opening, {close_braces} closing")
        
        return issues
    
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
            
            # Validate generated header
            issues = self.validate_generated_header(header_content, str(header_path))
            if issues:
                self.stats['warnings'] += len(issues)
                self.log(f"Validation warnings for {header_path.name}:", "WARN")
                for issue in issues[:3]:  # Show first 3 issues
                    self.log(f"  - {issue}", "WARN")
                if len(issues) > 3:
                    self.log(f"  ... and {len(issues) - 3} more issues", "WARN")
            
            with open(header_path, 'w', encoding='utf-8') as f:
                f.write(header_content)
            
            self.stats['headers_generated'] += 1
            
            # Generate implementation file
            impl_content = self.generate_implementation(ts_content)
            impl_content = impl_content.replace('{header_name}', header_path.name)
            
            with open(impl_path, 'w', encoding='utf-8') as f:
                f.write(impl_content)
            
            self.stats['implementations_generated'] += 1
            self.stats['files_processed'] += 1
            return True
            
        except Exception as e:
            self.log(f"Error processing {ts_file}: {e}", "ERROR")
            self.stats['errors'] += 1
            return False
    
    def transpile(self):
        """Main transpilation process"""
        self.stats['start_time'] = time.time()
        self.log(f"Starting transpilation from {self.input_dir} to {self.output_dir}")
        
        # Find all TypeScript files
        ts_files = list(self.input_dir.rglob('*.ts')) + list(self.input_dir.rglob('*.tsx'))
        self.log(f"Found {len(ts_files)} TypeScript files")
        
        # Filter files to process
        files_to_process = [f for f in ts_files if self.should_process_file(f)]
        files_to_skip = [f for f in ts_files if not self.should_process_file(f)]
        
        self.log(f"Will process {len(files_to_process)} files")
        self.log(f"Will skip {len(files_to_skip)} files")
        self.stats['files_skipped'] = len(files_to_skip)
        
        # Process files (parallel or sequential)
        if self.parallel and len(files_to_process) > 1:
            self.log(f"Using parallel processing with {self.max_workers} workers")
            self._process_parallel(files_to_process)
        else:
            self._process_sequential(files_to_process)
        
        self.stats['end_time'] = time.time()
        
        # Print summary
        self.print_summary()
    
    def _process_sequential(self, files: List[Path]):
        """Process files sequentially"""
        for ts_file in files:
            self.process_file(ts_file)
    
    def _process_parallel(self, files: List[Path]):
        """Process files in parallel using thread pool"""
        with ThreadPoolExecutor(max_workers=self.max_workers) as executor:
            # Submit all tasks
            future_to_file = {executor.submit(self.process_file, f): f for f in files}
            
            # Collect results
            for future in as_completed(future_to_file):
                file = future_to_file[future]
                try:
                    future.result()
                except Exception as e:
                    self.log(f"Error processing {file}: {e}", "ERROR")
    
    def print_summary(self):
        """Print transpilation summary"""
        print("\n" + "="*60)
        print("TRANSPILATION SUMMARY")
        print("="*60)
        print(f"TypeScript files processed: {self.stats['files_processed']}")
        print(f"TypeScript files skipped: {self.stats['files_skipped']}")
        print(f"Header files generated (.hpp): {self.stats['headers_generated']}")
        print(f"Implementation files generated (.cpp): {self.stats['implementations_generated']}")
        print(f"Validation warnings: {self.stats['warnings']}")
        print(f"Errors: {self.stats['errors']}")
        
        # Performance metrics
        if self.stats['start_time'] and self.stats['end_time']:
            duration = self.stats['end_time'] - self.stats['start_time']
            files_per_sec = self.stats['files_processed'] / duration if duration > 0 else 0
            print(f"\nPerformance:")
            print(f"  Duration: {duration:.2f} seconds")
            print(f"  Processing rate: {files_per_sec:.1f} files/second")
            if self.parallel:
                print(f"  Mode: Parallel ({self.max_workers} workers)")
            else:
                print(f"  Mode: Sequential")
        
        print(f"\nOutput directory: {self.output_dir}")
        print("="*60)
        print("\nNOTE: Generated code is approximate and will require manual fixes.")
        print("This is an experimental rapid transpilation for evaluation purposes.")
        if self.stats['warnings'] > 0:
            print(f"\n⚠️  {self.stats['warnings']} validation warnings detected.")
            print("Review the generated files for TypeScript code leakage or conversion issues.")
        print("="*60)

def main():
    """Main entry point"""
    parser = argparse.ArgumentParser(
        description='Experimental TypeScript to C++ Transpiler (v2.1)',
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="""
Examples:
  # Convert all TypeScript files in current repo to excpp/
  python3 ts_to_cpp_transpiler.py
  
  # Convert specific directory
  python3 ts_to_cpp_transpiler.py --input-dir ./otaku/src --output-dir ./excpp/otaku
  
  # Verbose output
  python3 ts_to_cpp_transpiler.py --verbose
  
  # Use parallel processing for faster conversion
  python3 ts_to_cpp_transpiler.py --parallel --max-workers 8
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
    
    parser.add_argument(
        '--parallel',
        action='store_true',
        help='Enable parallel processing for faster conversion'
    )
    
    parser.add_argument(
        '--max-workers',
        type=int,
        default=4,
        help='Maximum number of parallel workers (default: 4, only used with --parallel)'
    )
    
    args = parser.parse_args()
    
    # Create transpiler and run
    transpiler = TypeScriptToCppTranspiler(
        input_dir=args.input_dir,
        output_dir=args.output_dir,
        verbose=args.verbose,
        parallel=args.parallel,
        max_workers=args.max_workers
    )
    
    transpiler.transpile()

if __name__ == '__main__':
    main()
