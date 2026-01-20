#!/usr/bin/env python3
"""
Consolidate C++ sources from multiple generated folders into unified cpp/ folder.
Strategy:
1. For files that exist in cpp/, keep them unless generated version is significantly better
2. For files only in generated folders, copy the best version to cpp/
3. Normalize paths to match cpp/ naming convention (replace - with _)
"""

import os
import shutil
import json
import re
from pathlib import Path

BASE_DIR = '/home/ubuntu/elizaos-cpp'
CPP_DIR = os.path.join(BASE_DIR, 'cpp')
GENERATED_DIRS = ['cpp_generated', 'excpp', 'excpp1', 'excpp_old', 'excpp_old_backup']

def normalize_module_name(name):
    """Convert module name to cpp/ convention (hyphen to underscore)"""
    return name.replace('-', '_').replace('.', '_')

def get_file_info(filepath):
    """Get file metadata"""
    try:
        with open(filepath, 'r', errors='ignore') as f:
            content = f.read()
            lines = content.count('\n') + 1
            # Check for TODO/FIXME/placeholder markers
            todos = len(re.findall(r'TODO|FIXME|PLACEHOLDER|NOT_IMPLEMENTED', content, re.IGNORECASE))
            # Check for actual implementation (non-empty functions)
            implementations = len(re.findall(r'\{[^}]+\}', content))
            return {
                'lines': lines,
                'size': os.path.getsize(filepath),
                'todos': todos,
                'implementations': implementations,
                'content': content
            }
    except:
        return None

def find_best_version(filename, sources):
    """Find the best version of a file from multiple sources"""
    versions = []
    for source_dir in sources:
        filepath = os.path.join(BASE_DIR, source_dir, filename)
        if os.path.exists(filepath):
            info = get_file_info(filepath)
            if info:
                info['source'] = source_dir
                info['path'] = filepath
                versions.append(info)
    
    if not versions:
        return None
    
    # Score each version: more lines + more implementations - todos
    for v in versions:
        v['score'] = v['lines'] + v['implementations'] * 10 - v['todos'] * 5
    
    # Return best scoring version
    return max(versions, key=lambda x: x['score'])

def consolidate_files():
    """Main consolidation logic"""
    # Load analysis
    with open('/tmp/file_analysis.json', 'r') as f:
        analysis = json.load(f)
    
    stats = {
        'copied': 0,
        'skipped_exists': 0,
        'upgraded': 0,
        'errors': 0,
        'modules_created': set()
    }
    
    # Process files not in cpp/
    files_to_process = [(f, info) for f, info in analysis.items() 
                        if 'cpp' not in info['present_in']]
    
    print(f"Processing {len(files_to_process)} files not in cpp/...")
    
    for filename, info in sorted(files_to_process, key=lambda x: -x[1]['best_lines']):
        try:
            # Get the best version
            best = find_best_version(filename, info['present_in'])
            if not best:
                continue
            
            # Normalize the target path
            parts = filename.split('/')
            if len(parts) > 0:
                parts[0] = normalize_module_name(parts[0])
            
            target_path = os.path.join(CPP_DIR, '/'.join(parts))
            target_dir = os.path.dirname(target_path)
            
            # Create directory if needed
            if not os.path.exists(target_dir):
                os.makedirs(target_dir, exist_ok=True)
                stats['modules_created'].add(parts[0])
            
            # Copy file
            if not os.path.exists(target_path):
                shutil.copy2(best['path'], target_path)
                stats['copied'] += 1
                if stats['copied'] <= 100:
                    print(f"  Copied: {filename} ({best['lines']} lines from {best['source']})")
            else:
                stats['skipped_exists'] += 1
                
        except Exception as e:
            stats['errors'] += 1
            if stats['errors'] <= 10:
                print(f"  Error: {filename}: {e}")
    
    print(f"\n=== CONSOLIDATION COMPLETE ===")
    print(f"Files copied: {stats['copied']}")
    print(f"Files skipped (already exist): {stats['skipped_exists']}")
    print(f"Files upgraded: {stats['upgraded']}")
    print(f"Errors: {stats['errors']}")
    print(f"New modules created: {len(stats['modules_created'])}")
    for m in sorted(stats['modules_created']):
        print(f"  - {m}")
    
    return stats

if __name__ == '__main__':
    consolidate_files()
