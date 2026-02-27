#!/usr/bin/env python3
"""
Fix common transpiler errors in C++ code generated from TypeScript.

This script addresses:
1. Arrow function syntax (=>) 
2. process.* calls
3. import.meta references
4. JSON.parse/stringify
5. async/await keywords
6. Template literals
7. Optional chaining (?.)
8. Nullish coalescing (??)
9. Other TypeScript-specific syntax
"""

import os
import re
import sys
from pathlib import Path


def fix_arrow_functions(content: str) -> str:
    """Convert arrow functions to C++ lambdas."""
    # Pattern: (params) => { body } or (params) => expr
    # This is a simplified fix - complex cases may need manual attention
    
    # Replace simple arrow functions with lambdas
    # (x) => x becomes [](auto x) { return x; }
    content = re.sub(
        r'\(([^)]*)\)\s*=>\s*\{',
        r'[&](\1) {',
        content
    )
    
    # Simple expression arrows: (x) => expr  ->  [](auto x) { return expr; }
    content = re.sub(
        r'\(([^)]*)\)\s*=>\s*([^{;]+);',
        r'[&](\1) { return \2; };',
        content
    )
    
    return content


def fix_process_references(content: str) -> str:
    """Replace process.* with C++ equivalents."""
    replacements = [
        (r'process\.argv\.length', 'argc'),
        (r'process\.argv', 'argv'),
        (r'process\.env\.(\w+)\s*=\s*"([^"]*)"', r'/* setenv("\1", "\2") */'),
        (r'process\.env\.(\w+)', r'getEnvVar("\1")'),
        (r'process\.env\["(\w+)"\]', r'getEnvVar("\1")'),
        (r'process\.cwd\(\)', 'std::filesystem::current_path().string()'),
        (r'process\.exit\((\d+)\)', r'std::exit(\1)'),
        (r'process\.stdout', 'std::cout'),
        (r'process\.stderr', 'std::cerr'),
    ]
    
    for pattern, replacement in replacements:
        content = re.sub(pattern, replacement, content)
    
    return content


def fix_import_meta(content: str) -> str:
    """Remove import.meta references."""
    content = re.sub(r'import\.meta\.url', '"__FILE__"', content)
    content = re.sub(r'import\.meta\.\w+', '""', content)
    return content


def fix_json_operations(content: str) -> str:
    """Replace JSON operations with C++ equivalents."""
    content = re.sub(r'/\*\s*JSON\.parse\s*\*/', '/* JSON::parse */', content)
    content = re.sub(r'JSON\.parse\s*\(', 'nlohmann::json::parse(', content)
    content = re.sub(r'JSON\.stringify\s*\(', 'nlohmann::json().dump(', content)
    return content


def fix_async_await(content: str) -> str:
    """Fix async/await syntax issues."""
    # Remove 'await' keyword (already returns future in most transpiled code)
    content = re.sub(r'\bawait\s+', '', content)
    
    # async functions already return std::future in transpiled code
    return content


def fix_template_literals(content: str) -> str:
    """Convert template literals to string concatenation."""
    # Simple case: `text ${var} more` -> "text " + var + " more"
    # This is a basic fix - complex cases need manual attention
    
    def replace_template(match):
        text = match.group(1)
        # Replace ${...} with proper string concat
        result = re.sub(r'\$\{([^}]+)\}', r'" + std::to_string(\1) + "', text)
        return '"' + result + '"'
    
    content = re.sub(r'`([^`]*)`', replace_template, content)
    return content


def fix_optional_chaining(content: str) -> str:
    """Replace optional chaining with explicit checks."""
    # obj?.prop becomes (obj ? obj.prop : default)
    # This is a simplified fix
    content = re.sub(r'(\w+)\?\.([\w.]+)', r'(\1 ? \1.\2 : nullptr)', content)
    return content


def fix_nullish_coalescing(content: str) -> str:
    """Replace ?? with || for nullish coalescing."""
    content = re.sub(r'\s*\?\?\s*', ' || ', content)
    return content


def fix_triple_equals(content: str) -> str:
    """Replace === and !== with == and !=."""
    content = re.sub(r'===', '==', content)
    content = re.sub(r'!==', '!=', content)
    return content


def fix_const_auto(content: str) -> str:
    """Fix const auto declarations."""
    # const x = -> const auto x =
    # But careful not to double-apply
    content = re.sub(r'\bconst\s+auto\s+auto\b', 'const auto', content)
    return content


def fix_includes_argv(content: str) -> str:
    """Fix .includes() calls on arrays."""
    # argv.includes("x") -> hasArg("x")
    content = re.sub(
        r'argv\.find\(([^)]+)\)\s*!=\s*\.end\(\)',
        r'hasArg(\1)',
        content
    )
    # Generic .find() fix
    content = re.sub(
        r'\.find\(([^)]+)\)\s*!=\s*\.end\(\)',
        r'.count(\1) > 0',
        content
    )
    return content


def fix_string_methods(content: str) -> str:
    """Fix JavaScript string methods."""
    content = re.sub(r'\.startsWith\s*\(', '.substr(0, ', content)
    content = re.sub(r'\.endsWith\s*\(', '.rfind(', content)
    content = re.sub(r'\.trim\s*\(\)', '', content)  # Remove trim calls
    return content


def fix_typescript_syntax(content: str) -> str:
    """Fix various TypeScript syntax issues."""
    # Remove type annotations after colons in variable declarations
    # let x: string = "value" -> auto x = "value"
    content = re.sub(r':\s*string\b', '', content)
    content = re.sub(r':\s*number\b', '', content)
    content = re.sub(r':\s*boolean\b', '', content)
    content = re.sub(r':\s*any\b', '', content)
    content = re.sub(r':\s*void\b', '', content)
    
    # Remove export/import keywords that might remain
    content = re.sub(r'^\s*export\s+(default\s+)?', '', content, flags=re.MULTILINE)
    
    # Fix 'new Command()' style instantiation
    content = re.sub(r'new\s+(\w+)\s*\(\)', r'std::make_unique<\1>()', content)
    
    # Fix malformed std::vector<std:> to std::vector<std::string>
    content = re.sub(r'std::vector<std:>', 'std::vector<std::string>', content)
    
    # Fix incomplete std: references to std::string
    content = re.sub(r'\bstd:\s+(\w+)', r'std::string \1', content)
    content = re.sub(r'\bstd:(?!\s*:)', 'std::string', content)
    
    # Fix TypeScript class member declarations
    # private token: std::string = '' -> private: std::string token = "";
    content = re.sub(
        r'\s*private\s+(\w+):\s*([\w:]+)\s*=\s*([^;]+);',
        r'\nprivate:\n    \2 \1 = \3;',
        content
    )
    content = re.sub(
        r'\s*public\s+(\w+):\s*([\w:]+)\s*=\s*([^;]+);',
        r'\npublic:\n    \2 \1 = \3;',
        content
    )
    
    # Fix object literal syntax { key: value } to C++ style
    content = re.sub(r'\{\s*(\w+):\s*true\s*\}', r'Config{\1 = true}', content)
    content = re.sub(r'\{\s*(\w+):\s*false\s*\}', r'Config{\1 = false}', content)
    
    # Fix .slice() method
    content = re.sub(r'\.slice\((\d+)\)', r'.substr(\1)', content)
    content = re.sub(r'\.slice\((\d+),\s*(\d+)\)', r'.substr(\1, \2-\1)', content)
    
    # Fix .length property
    content = re.sub(r'\.length\b', '.size()', content)
    
    # Fix undefined checks
    content = re.sub(r'\bundefined\b', 'std::nullopt', content)
    
    # Fix Math.random()
    content = re.sub(r'Math\.random\(\)', '((double)rand() / RAND_MAX)', content)
    
    # Fix constructor keyword
    content = re.sub(r'\bconstructor\s*\(', '/* constructor */ (', content)
    
    # Fix extends keyword (TypeScript inheritance)
    content = re.sub(r'\bextends\s+(\w+)', r': public \1', content)
    
    return content


def fix_chained_method_calls(content: str) -> str:
    """Fix method chaining on new lines."""
    # Pattern like:
    # program
    # .name("x")
    # becomes:
    # program.name("x")
    content = re.sub(r';\s*\n\s*\.([\w]+)\s*\(', r'.\1(', content)
    return content


def fix_absolute_include_paths(content: str) -> str:
    """Fix absolute include paths to relative paths."""
    # Replace absolute paths with relative paths
    # Pattern: #include "/home/runner/work/.../file.h"
    content = re.sub(
        r'#include\s+"/[^"]+/([^/"]+\.h(?:pp)?)"',
        r'#include "\1"',
        content
    )
    # Fix header guard names that contain invalid characters
    content = re.sub(
        r'#ifndef\s+(_HOME_[A-Z0-9_-]+)',
        lambda m: '#ifndef ' + m.group(1).replace('-', '_').upper(),
        content
    )
    content = re.sub(
        r'#define\s+(_HOME_[A-Z0-9_-]+)',
        lambda m: '#define ' + m.group(1).replace('-', '_').upper(),
        content
    )
    # Fix invalid include paths like @vitejs/plugin-react.h
    content = re.sub(
        r'#include\s+"@[^"]+\.h(?:pp)?"',
        '// External dependency removed',
        content
    )
    # Fix includes for .h -> .hpp
    content = re.sub(
        r'#include\s+"(\w+)\.h"',
        r'#include "\1.hpp"',
        content
    )
    # Remove invalid using statements like 'using react = _default;'
    content = re.sub(
        r'using\s+\w+\s*=\s*_default\s*;',
        '// Using alias removed (invalid transpilation)',
        content
    )
    return content


def remove_self_includes(content: str, filepath: Path) -> str:
    """Remove self-includes (file including itself)."""
    filename = filepath.name
    # Pattern: #include "filename.hpp" where filename matches current file
    pattern = rf'#include\s+"{re.escape(filename)}"'
    content = re.sub(pattern, f'// Self-include removed: {filename}', content)
    return content


def add_required_includes(content: str) -> str:
    """Add commonly needed includes if not present."""
    includes_needed = []
    
    if 'std::vector' in content and '#include <vector>' not in content:
        includes_needed.append('#include <vector>')
    if 'std::string' in content and '#include <string>' not in content:
        includes_needed.append('#include <string>')
    if 'std::future' in content and '#include <future>' not in content:
        includes_needed.append('#include <future>')
    if 'std::filesystem' in content and '#include <filesystem>' not in content:
        includes_needed.append('#include <filesystem>')
    if 'std::getenv' in content and '#include <cstdlib>' not in content:
        includes_needed.append('#include <cstdlib>')
    if 'std::optional' in content and '#include <optional>' not in content:
        includes_needed.append('#include <optional>')
    if 'std::any' in content and '#include <any>' not in content:
        includes_needed.append('#include <any>')
    if 'std::map' in content and '#include <map>' not in content:
        includes_needed.append('#include <map>')
    if 'std::unordered_map' in content and '#include <unordered_map>' not in content:
        includes_needed.append('#include <unordered_map>')
    
    if includes_needed:
        # Find the last #include line and add after it
        include_match = re.search(r'(#include\s+[<"][^>"]+[>"])', content)
        if include_match:
            insert_pos = include_match.end()
            new_includes = '\n'.join(includes_needed)
            content = content[:insert_pos] + '\n' + new_includes + content[insert_pos:]
        else:
            # Add at the beginning
            content = '\n'.join(includes_needed) + '\n\n' + content
    
    return content


def process_file(filepath: Path) -> bool:
    """Process a single file and apply all fixes."""
    try:
        with open(filepath, 'r', encoding='utf-8', errors='replace') as f:
            original_content = f.read()
    except Exception as e:
        print(f"Error reading {filepath}: {e}")
        return False
    
    content = original_content
    
    # Apply all fixes
    content = fix_absolute_include_paths(content)
    content = remove_self_includes(content, filepath)
    content = fix_arrow_functions(content)
    content = fix_process_references(content)
    content = fix_import_meta(content)
    content = fix_json_operations(content)
    content = fix_async_await(content)
    content = fix_template_literals(content)
    content = fix_optional_chaining(content)
    content = fix_nullish_coalescing(content)
    content = fix_triple_equals(content)
    content = fix_const_auto(content)
    content = fix_includes_argv(content)
    content = fix_string_methods(content)
    content = fix_typescript_syntax(content)
    content = fix_chained_method_calls(content)
    content = add_required_includes(content)
    
    # Only write if content changed
    if content != original_content:
        try:
            with open(filepath, 'w', encoding='utf-8') as f:
                f.write(content)
            return True
        except Exception as e:
            print(f"Error writing {filepath}: {e}")
            return False
    
    return False


def main():
    """Process all .cpp and .hpp files in the directory."""
    if len(sys.argv) > 1:
        root_dir = Path(sys.argv[1])
    else:
        root_dir = Path(__file__).parent
    
    if not root_dir.exists():
        print(f"Error: Directory {root_dir} does not exist")
        sys.exit(1)
    
    files_processed = 0
    files_modified = 0
    
    for ext in ['*.cpp', '*.hpp']:
        for filepath in root_dir.rglob(ext):
            files_processed += 1
            if process_file(filepath):
                files_modified += 1
                print(f"Fixed: {filepath}")
    
    print(f"\nProcessed {files_processed} files, modified {files_modified} files")


if __name__ == '__main__':
    main()
