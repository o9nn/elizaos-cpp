#!/usr/bin/env python3
"""
Transpiler Refinement Script

This script performs post-transpilation refinement to fix common known errors
in the generated C++ code. It runs after the main transpiler to clean up
any remaining TypeScript artifacts and improve code quality.

Common issues fixed:
1. Remaining console.* calls -> std::cout/std::cerr
2. Remaining await expressions -> proper C++ async handling
3. Arrow function remnants -> lambda or function calls
4. TypeScript-specific syntax (===, !==, ?., ??)
5. Remaining type assertions (as any, as string, etc.)
6. Missing semicolons and brace issues
7. Template literal remnants -> string concatenation

Usage:
    python3 transpiler_refinement.py <directory> [--verbose] [--dry-run]
"""

import os
import re
import sys
import argparse
from pathlib import Path
from typing import List, Dict, Tuple
from dataclasses import dataclass, field


@dataclass
class RefinementStats:
    """Statistics for refinement operations"""
    files_processed: int = 0
    files_modified: int = 0
    total_fixes: int = 0
    fixes_by_type: Dict[str, int] = field(default_factory=dict)
    errors: List[str] = field(default_factory=list)
    notes: List[str] = field(default_factory=list)


class TranspilerRefinement:
    """Post-transpilation refinement processor"""
    
    def __init__(self, verbose: bool = False, dry_run: bool = False):
        self.verbose = verbose
        self.dry_run = dry_run
        self.stats = RefinementStats()
        
        # Define refinement patterns: (pattern, replacement, description)
        self.refinement_patterns = [
            # Console methods
            (r'\bconsole\.log\s*\(([^)]*)\)', r'std::cout << \1 << std::endl', 'console.log -> std::cout'),
            (r'\bconsole\.error\s*\(([^)]*)\)', r'std::cerr << \1 << std::endl', 'console.error -> std::cerr'),
            (r'\bconsole\.warn\s*\(([^)]*)\)', r'std::cerr << "[WARN] " << \1 << std::endl', 'console.warn -> std::cerr'),
            (r'\bconsole\.debug\s*\(([^)]*)\)', r'// DEBUG: \1', 'console.debug -> comment'),
            (r'\bconsole\.info\s*\(([^)]*)\)', r'std::cout << "[INFO] " << \1 << std::endl', 'console.info -> std::cout'),
            
            # TypeScript operators
            (r'===', r'==', '=== -> =='),
            (r'!==', r'!=', '!== -> !='),
            
            # Nullish coalescing and optional chaining
            # Note: ?? -> || is a simplification that may change semantics for falsy values (0, "", false)
            # This is flagged as a known limitation - manual review recommended
            (r'\?\?', r'/* ?? */ ||', '?? -> || (semantic change)'),
            (r'\?\.', r'.', '?. -> .'),
            
            # Type assertions (remove remaining ones)
            (r'\s+as\s+any\b', r'', 'as any removal'),
            (r'\s+as\s+string\b', r'', 'as string removal'),
            (r'\s+as\s+number\b', r'', 'as number removal'),
            (r'\s+as\s+boolean\b', r'', 'as boolean removal'),
            (r'\s+as\s+\w+\[\]', r'', 'as array removal'),
            
            # Await expressions
            (r'\bawait\s+', r'', 'await removal'),
            
            # Promise handling - mark for manual review instead of incorrect auto-fix
            (r'\.then\s*\(\s*\(([^)]*)\)\s*=>\s*\{', r'/* TODO: .then() callback - needs manual async conversion */ {', '.then -> TODO'),
            (r'\.catch\s*\(\s*\(([^)]*)\)\s*=>\s*\{', r'/* TODO: .catch() error handler - needs manual try/catch wrapping */ {', '.catch -> TODO'),
            
            # Arrow functions in certain contexts (simple cases)
            (r'\(\s*\)\s*=>\s*\{', r'[&]() {', 'arrow fn -> lambda'),
            (r'\((\w+)\)\s*=>\s*\{', r'[&](auto \1) {', 'arrow fn with param -> lambda'),
            
            # Template literals remnants - use generic string concatenation
            # Note: Uses + for concatenation; may need manual type handling for non-strings
            (r'`([^`]*)`', lambda m: '"' + re.sub(r'\$\{([^}]+)\}', r'" + (\1) + "', m.group(1)) + '"', 'template literal -> string concat'),
            
            # Remaining typeof
            (r'\btypeof\s+(\w+)\s*===?\s*["\'](\w+)["\']', r'/* typeof \1 == "\2" */', 'typeof check -> comment'),
            
            # Object spread (simplified)
            (r'\.\.\.(\w+)', r'/* spread: \1 */', 'spread operator -> comment'),
            
            # Remove 'export' and 'default export' remnants
            (r'\bexport\s+default\s+', r'', 'export default removal'),
            (r'\bexport\s+', r'', 'export removal'),
            
            # Fix double semicolons
            (r';;', r';', 'double semicolon fix'),
        ]
        
        # Additional validation patterns (things to flag but not auto-fix)
        self.validation_patterns = [
            (r'=>\s*\{', 'Remaining arrow function found'),
            (r'\bimport\s+', 'Remaining import statement'),
            (r'\brequire\s*\(', 'Remaining require() call'),
            (r'\bJSON\.parse\s*\(', 'JSON.parse needs manual conversion'),
            (r'\bJSON\.stringify\s*\(', 'JSON.stringify needs manual conversion'),
            (r'\bObject\.keys\s*\(', 'Object.keys needs manual conversion'),
            (r'\bObject\.values\s*\(', 'Object.values needs manual conversion'),
            (r'\bArray\.from\s*\(', 'Array.from needs manual conversion'),
            (r'\bprocess\.env\b', 'process.env needs manual conversion'),
            (r'\bsetTimeout\s*\(', 'setTimeout needs manual conversion'),
            (r'\bsetInterval\s*\(', 'setInterval needs manual conversion'),
            (r'\bPromise\.(all|race|resolve|reject)\s*\(', 'Promise methods need manual conversion'),
            (r'/\* \?\? \*/', 'Nullish coalescing (??) converted - review for falsy value handling'),
            (r'TODO: \.then\(\)', '.then() callback needs async conversion'),
            (r'TODO: \.catch\(\)', '.catch() error handler needs try/catch wrapping'),
        ]
    
    def log(self, message: str, level: str = "INFO"):
        """Log message if verbose mode is enabled"""
        if self.verbose or level in ["ERROR", "WARN"]:
            print(f"[{level}] {message}")
    
    def refine_content(self, content: str, file_path: str) -> Tuple[str, int]:
        """Apply refinement patterns to content"""
        total_fixes = 0
        refined = content
        
        for pattern, replacement, description in self.refinement_patterns:
            if callable(replacement):
                # Handle lambda replacements
                matches = list(re.finditer(pattern, refined))
                if matches:
                    refined = re.sub(pattern, replacement, refined)
                    count = len(matches)
                else:
                    count = 0
            else:
                # Count matches before replacement
                matches = re.findall(pattern, refined)
                count = len(matches)
                if count > 0:
                    refined = re.sub(pattern, replacement, refined)
            
            if count > 0:
                total_fixes += count
                self.stats.fixes_by_type[description] = self.stats.fixes_by_type.get(description, 0) + count
                self.log(f"  {description}: {count} fixes", "DEBUG")
        
        return refined, total_fixes
    
    def validate_content(self, content: str, file_path: str) -> List[str]:
        """Check for patterns that need manual attention"""
        issues = []
        
        for pattern, description in self.validation_patterns:
            matches = re.findall(pattern, content)
            if matches:
                issues.append(f"{description} ({len(matches)} occurrences)")
        
        return issues
    
    def process_file(self, file_path: Path) -> bool:
        """Process a single C++ file"""
        try:
            with open(file_path, 'r', encoding='utf-8', errors='ignore') as f:
                content = f.read()
            
            # Apply refinements
            refined_content, fixes = self.refine_content(content, str(file_path))
            
            # Validate and collect issues
            issues = self.validate_content(refined_content, str(file_path))
            if issues:
                rel_path = file_path.name
                for issue in issues:
                    self.stats.notes.append(f"{rel_path}: {issue}")
            
            self.stats.files_processed += 1
            self.stats.total_fixes += fixes
            
            if fixes > 0:
                self.stats.files_modified += 1
                self.log(f"Refined {file_path.name}: {fixes} fixes applied")
                
                if not self.dry_run:
                    with open(file_path, 'w', encoding='utf-8') as f:
                        f.write(refined_content)
                else:
                    self.log(f"  (dry-run: not writing changes)")
            
            return True
            
        except Exception as e:
            self.stats.errors.append(f"{file_path}: {str(e)}")
            self.log(f"Error processing {file_path}: {e}", "ERROR")
            return False
    
    def process_directory(self, directory: Path):
        """Process all C++ files in a directory"""
        cpp_files = list(directory.rglob('*.cpp')) + list(directory.rglob('*.hpp'))
        
        self.log(f"Found {len(cpp_files)} C++ files to refine")
        
        for cpp_file in cpp_files:
            self.process_file(cpp_file)
        
        self.print_summary()
        self.generate_notes()
    
    def print_summary(self):
        """Print refinement summary"""
        print("\n" + "=" * 60)
        print("REFINEMENT SUMMARY")
        print("=" * 60)
        print(f"Files processed: {self.stats.files_processed}")
        print(f"Files modified:  {self.stats.files_modified}")
        print(f"Total fixes:     {self.stats.total_fixes}")
        
        if self.stats.fixes_by_type:
            print("\nFixes by type:")
            for fix_type, count in sorted(self.stats.fixes_by_type.items(), key=lambda x: -x[1]):
                print(f"  {fix_type}: {count}")
        
        if self.stats.errors:
            print(f"\nErrors: {len(self.stats.errors)}")
            for error in self.stats.errors[:10]:
                print(f"  - {error}")
            if len(self.stats.errors) > 10:
                print(f"  ... and {len(self.stats.errors) - 10} more")
        
        print("=" * 60)
    
    def generate_notes(self):
        """Generate notes for transpiler improvements"""
        if not self.stats.notes:
            return
        
        print("\n" + "=" * 60)
        print("NOTES FOR TRANSPILER IMPROVEMENTS")
        print("=" * 60)
        
        # Group notes by issue type
        issue_counts = {}
        for note in self.stats.notes:
            # Extract issue type from note
            parts = note.split(': ', 1)
            if len(parts) > 1:
                issue_type = parts[1].split(' (')[0]
                issue_counts[issue_type] = issue_counts.get(issue_type, 0) + 1
        
        print("\nPatterns requiring transpiler improvements:")
        for issue_type, count in sorted(issue_counts.items(), key=lambda x: -x[1]):
            print(f"  - {issue_type}: {count} files affected")
            
            # Add specific recommendations
            if 'arrow function' in issue_type.lower():
                print("    → Recommendation: Improve arrow function to lambda conversion")
            elif 'json' in issue_type.lower():
                print("    → Recommendation: Add nlohmann/json or similar library integration")
            elif 'promise' in issue_type.lower():
                print("    → Recommendation: Map to std::future/std::promise patterns")
            elif 'process.env' in issue_type.lower():
                print("    → Recommendation: Add environment variable handling (std::getenv)")
            elif 'settimeout' in issue_type.lower() or 'setinterval' in issue_type.lower():
                print("    → Recommendation: Add timer handling (std::thread or platform-specific)")
        
        print("\n" + "=" * 60)


def main():
    parser = argparse.ArgumentParser(
        description='Post-transpilation refinement for C++ code',
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="""
Examples:
    python3 transpiler_refinement.py cpp_generated/
    python3 transpiler_refinement.py cpp_generated/ --verbose
    python3 transpiler_refinement.py cpp_generated/ --dry-run
        """
    )
    parser.add_argument('directory', type=Path, help='Directory containing C++ files to refine')
    parser.add_argument('-v', '--verbose', action='store_true', help='Enable verbose output')
    parser.add_argument('-n', '--dry-run', action='store_true', help='Do not write changes, only report what would be fixed')
    
    args = parser.parse_args()
    
    if not args.directory.exists():
        print(f"Error: Directory '{args.directory}' does not exist")
        sys.exit(1)
    
    if not args.directory.is_dir():
        print(f"Error: '{args.directory}' is not a directory")
        sys.exit(1)
    
    refiner = TranspilerRefinement(verbose=args.verbose, dry_run=args.dry_run)
    refiner.process_directory(args.directory)


if __name__ == '__main__':
    main()
