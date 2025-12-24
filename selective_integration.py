#!/usr/bin/env python3
"""
Selective Integration Tool for C++ Adaptation

This tool helps selectively integrate transpiler-generated excpp/ scaffolding
into cpp/ implementations, with options for refinement and validation.
"""

import os
import shutil
import argparse
from pathlib import Path
from typing import List, Optional
import re

class SelectiveIntegrationTool:
    """Tool for integrating excpp/ scaffolding into cpp/ implementations"""
    
    def __init__(self, repo_root: str):
        self.repo_root = Path(repo_root)
        self.excpp_dir = self.repo_root / 'excpp'
        self.cpp_dir = self.repo_root / 'cpp'
    
    def find_matching_files(self, module: str, pattern: str) -> List[Path]:
        """Find files matching pattern in excpp/ module"""
        module_dir = self.excpp_dir / module
        
        if not module_dir.exists():
            print(f"Module {module} not found in excpp/")
            return []
        
        # Handle glob patterns
        import fnmatch
        matches = []
        
        for root, dirs, files in os.walk(module_dir):
            for filename in files:
                if fnmatch.fnmatch(filename, pattern) or fnmatch.fnmatch(str(Path(root) / filename), pattern):
                    matches.append(Path(root) / filename)
        
        return matches
    
    def refine_header(self, content: str) -> str:
        """Refine transpiled header content"""
        lines = content.split('\n')
        refined = []
        
        # Skip auto-generated comments
        skip_next = False
        for line in lines:
            if 'auto-generated' in line.lower() or 'manual refinement required' in line.lower():
                skip_next = True
                continue
            
            if skip_next and line.strip().startswith('//'):
                continue
            
            skip_next = False
            
            # Fix common transpiler issues
            line = line.replace(' as any', '')
            line = line.replace(' as string', '')
            line = line.replace('console.log', '// console.log')
            
            refined.append(line)
        
        return '\n'.join(refined)
    
    def refine_implementation(self, content: str) -> str:
        """Refine transpiled implementation content"""
        lines = content.split('\n')
        refined = []
        
        for line in lines:
            # Skip placeholder comments
            if 'No function implementations found' in line:
                continue
            
            if 'Original TypeScript may contain' in line:
                continue
            
            # Fix common issues
            line = line.replace('std::cout << ', '// std::cout << ')
            line = line.replace('console.log', '// console.log')
            
            refined.append(line)
        
        return '\n'.join(refined)
    
    def copy_and_refine(self, source: Path, dest: Path, refine: bool = True):
        """Copy file from excpp/ to cpp/ with optional refinement"""
        
        # Read source
        try:
            with open(source, 'r', encoding='utf-8') as f:
                content = f.read()
        except Exception as e:
            print(f"Error reading {source}: {e}")
            return
        
        # Refine if requested
        if refine:
            if source.suffix == '.hpp':
                content = self.refine_header(content)
            elif source.suffix == '.cpp':
                content = self.refine_implementation(content)
        
        # Ensure destination directory exists
        dest.parent.mkdir(parents=True, exist_ok=True)
        
        # Write destination
        try:
            with open(dest, 'w', encoding='utf-8') as f:
                f.write(content)
            print(f"Copied: {source.relative_to(self.excpp_dir)} -> {dest.relative_to(self.cpp_dir)}")
        except Exception as e:
            print(f"Error writing {dest}: {e}")
    
    def integrate_module(self, module: str, file_patterns: List[str], refine: bool = True, dry_run: bool = False):
        """Integrate files from excpp/ module into cpp/ module"""
        
        print(f"\n{'='*80}")
        print(f"Integrating module: {module}")
        print(f"{'='*80}\n")
        
        # Ensure cpp module directory exists
        cpp_module_dir = self.cpp_dir / module
        if not cpp_module_dir.exists() and not dry_run:
            cpp_module_dir.mkdir(parents=True)
            print(f"Created: {cpp_module_dir}")
        
        # Process each file pattern
        for pattern in file_patterns:
            print(f"\nPattern: {pattern}")
            matches = self.find_matching_files(module, pattern)
            
            if not matches:
                print(f"  No files matched pattern: {pattern}")
                continue
            
            print(f"  Found {len(matches)} matching files")
            
            for source_file in matches:
                # Calculate destination path
                rel_path = source_file.relative_to(self.excpp_dir / module)
                dest_file = cpp_module_dir / rel_path
                
                if dry_run:
                    print(f"  Would copy: {rel_path}")
                else:
                    self.copy_and_refine(source_file, dest_file, refine)
    
    def show_module_structure(self, module: str):
        """Show structure of excpp/ module"""
        module_dir = self.excpp_dir / module
        
        if not module_dir.exists():
            print(f"Module {module} not found in excpp/")
            return
        
        print(f"\n{'='*80}")
        print(f"Structure of excpp/{module}")
        print(f"{'='*80}\n")
        
        # Count files by type
        hpp_files = list(module_dir.rglob('*.hpp'))
        cpp_files = list(module_dir.rglob('*.cpp'))
        
        print(f"Header files: {len(hpp_files)}")
        print(f"Implementation files: {len(cpp_files)}")
        print(f"Total files: {len(hpp_files) + len(cpp_files)}")
        
        # Show directory tree (first level)
        print("\nDirectory structure:")
        for item in sorted(module_dir.iterdir()):
            if item.is_dir():
                file_count = len(list(item.rglob('*.cpp'))) + len(list(item.rglob('*.hpp')))
                print(f"  {item.name}/ ({file_count} files)")
            else:
                print(f"  {item.name}")
    
    def compare_implementations(self, module: str):
        """Compare cpp/ implementation with excpp/ scaffolding"""
        cpp_module = self.cpp_dir / module
        excpp_module = self.excpp_dir / module
        
        print(f"\n{'='*80}")
        print(f"Comparison: cpp/{module} vs excpp/{module}")
        print(f"{'='*80}\n")
        
        # Count files
        cpp_files = list(cpp_module.rglob('*.cpp')) + list(cpp_module.rglob('*.hpp')) if cpp_module.exists() else []
        excpp_files = list(excpp_module.rglob('*.cpp')) + list(excpp_module.rglob('*.hpp')) if excpp_module.exists() else []
        
        # Count lines
        def count_lines(files):
            total = 0
            for f in files:
                try:
                    with open(f, 'r', encoding='utf-8', errors='ignore') as file:
                        total += sum(1 for line in file if line.strip())
                except:
                    pass
            return total
        
        cpp_lines = count_lines(cpp_files)
        excpp_lines = count_lines(excpp_files)
        
        print(f"cpp/{module}:")
        print(f"  Files: {len(cpp_files)}")
        print(f"  Lines: {cpp_lines:,}")
        
        print(f"\nexcpp/{module}:")
        print(f"  Files: {len(excpp_files)}")
        print(f"  Lines: {excpp_lines:,}")
        
        if cpp_lines > 0:
            ratio = (cpp_lines / excpp_lines * 100) if excpp_lines > 0 else 0
            print(f"\nImplementation ratio: {ratio:.1f}%")
        else:
            print("\nNo cpp/ implementation exists yet")
        
        # Show what's available in excpp/
        if excpp_files:
            print("\nAvailable in excpp/ for integration:")
            
            # Group by directory
            from collections import defaultdict
            by_dir = defaultdict(list)
            
            for f in excpp_files:
                rel = f.relative_to(excpp_module)
                dir_name = rel.parent if rel.parent != Path('.') else Path('root')
                by_dir[dir_name].append(rel.name)
            
            for dir_name in sorted(by_dir.keys()):
                files = by_dir[dir_name]
                print(f"  {dir_name}/ - {len(files)} files")

def main():
    parser = argparse.ArgumentParser(
        description='Selective integration tool for C++ adaptation',
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="""
Examples:
  # Show structure of otaku module
  python3 selective_integration.py --module otaku --show-structure
  
  # Compare cpp/ and excpp/ implementations
  python3 selective_integration.py --module otaku --compare
  
  # Dry run - show what would be copied
  python3 selective_integration.py --module otaku --files "src/managers/*.hpp" --dry-run
  
  # Actually copy and refine files
  python3 selective_integration.py --module otaku --files "src/managers/*.hpp" "src/managers/*.cpp"
  
  # Copy without refinement (raw transpiler output)
  python3 selective_integration.py --module otaku --files "src/types/*.hpp" --no-refine
        """
    )
    
    parser.add_argument('--repo', default='.', help='Repository root directory')
    parser.add_argument('--module', required=True, help='Module name (e.g., otaku, eliza)')
    parser.add_argument('--show-structure', action='store_true', help='Show excpp/ module structure')
    parser.add_argument('--compare', action='store_true', help='Compare cpp/ vs excpp/ implementations')
    parser.add_argument('--files', nargs='+', help='File patterns to integrate (e.g., "*.hpp" "src/*.cpp")')
    parser.add_argument('--refine', dest='refine', action='store_true', default=True, help='Refine transpiled code (default)')
    parser.add_argument('--no-refine', dest='refine', action='store_false', help='Skip refinement')
    parser.add_argument('--dry-run', action='store_true', help='Show what would be done without actually copying')
    
    args = parser.parse_args()
    
    tool = SelectiveIntegrationTool(args.repo)
    
    if args.show_structure:
        tool.show_module_structure(args.module)
    
    if args.compare:
        tool.compare_implementations(args.module)
    
    if args.files:
        tool.integrate_module(args.module, args.files, args.refine, args.dry_run)
    
    if not (args.show_structure or args.compare or args.files):
        parser.print_help()

if __name__ == '__main__':
    main()
