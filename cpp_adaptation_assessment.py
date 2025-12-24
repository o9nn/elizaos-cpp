#!/usr/bin/env python3
"""
C++ Adaptation Completeness Assessment Tool

This script analyzes the ElizaOS C++ adaptation by comparing:
1. Original TypeScript source files
2. C++ implementations in cpp/ folder
3. Transpiler-generated scaffolding in excpp/ folder

It generates a comprehensive report on completeness, gaps, and next steps.
"""

import os
import json
from pathlib import Path
from typing import Dict, List, Tuple, Optional
from dataclasses import dataclass, asdict
from collections import defaultdict

@dataclass
class FileMetrics:
    """Metrics for a single file"""
    path: str
    lines: int
    functions: int
    classes: int
    has_implementation: bool
    
@dataclass
class ModuleAssessment:
    """Assessment of a single module"""
    name: str
    ts_files: int
    ts_lines: int
    cpp_files: int
    cpp_lines: int
    excpp_files: int
    excpp_lines: int
    completeness_score: float
    status: str  # "complete", "partial", "minimal", "missing"
    notes: List[str]

class AdaptationAssessor:
    """Main assessment class"""
    
    def __init__(self, repo_root: str):
        self.repo_root = Path(repo_root)
        self.ts_modules = defaultdict(list)
        self.cpp_modules = {}
        self.excpp_modules = {}
        
    def count_lines(self, filepath: Path) -> int:
        """Count non-empty lines in a file"""
        try:
            with open(filepath, 'r', encoding='utf-8', errors='ignore') as f:
                return sum(1 for line in f if line.strip())
        except:
            return 0
    
    def estimate_functions(self, filepath: Path, language: str) -> int:
        """Estimate number of functions in a file"""
        try:
            with open(filepath, 'r', encoding='utf-8', errors='ignore') as f:
                content = f.read()
                if language == 'ts':
                    # Count function declarations (simple heuristic)
                    return content.count('function ') + content.count('const ') + content.count('async ')
                elif language == 'cpp':
                    # Count function definitions
                    return content.count('::') + content.count('() {')
        except:
            return 0
    
    def scan_typescript_sources(self):
        """Scan TypeScript source files"""
        print("Scanning TypeScript sources...")
        
        # Find all TS files, excluding node_modules, tests, and build artifacts
        exclude_dirs = {'node_modules', 'dist', 'build', '.git', '__pycache__'}
        exclude_patterns = {'.test.', '.spec.', '.config.', 'vite.config'}
        
        for ts_file in self.repo_root.rglob('*.ts'):
            # Skip excluded directories
            if any(ex in ts_file.parts for ex in exclude_dirs):
                continue
            
            # Skip test and config files
            if any(pat in ts_file.name for pat in exclude_patterns):
                continue
            
            # Determine module name (first directory in path)
            relative = ts_file.relative_to(self.repo_root)
            module_name = relative.parts[0] if len(relative.parts) > 0 else 'root'
            
            metrics = FileMetrics(
                path=str(relative),
                lines=self.count_lines(ts_file),
                functions=self.estimate_functions(ts_file, 'ts'),
                classes=0,  # Not counted for now
                has_implementation=True
            )
            
            self.ts_modules[module_name].append(metrics)
    
    def scan_cpp_implementations(self):
        """Scan C++ implementations in cpp/ folder"""
        print("Scanning C++ implementations...")
        
        cpp_dir = self.repo_root / 'cpp'
        if not cpp_dir.exists():
            return
        
        for module_dir in cpp_dir.iterdir():
            if not module_dir.is_dir():
                continue
            
            module_name = module_dir.name
            files = []
            total_lines = 0
            
            # Find all .cpp and .hpp files
            for ext in ['*.cpp', '*.hpp']:
                for cpp_file in module_dir.rglob(ext):
                    lines = self.count_lines(cpp_file)
                    total_lines += lines
                    
                    # Check if it's a placeholder
                    has_impl = 'placeholder' not in cpp_file.name
                    
                    files.append(FileMetrics(
                        path=str(cpp_file.relative_to(self.repo_root)),
                        lines=lines,
                        functions=self.estimate_functions(cpp_file, 'cpp'),
                        classes=0,
                        has_implementation=has_impl
                    ))
            
            self.cpp_modules[module_name] = {
                'files': files,
                'total_lines': total_lines,
                'file_count': len(files)
            }
    
    def scan_excpp_scaffolding(self):
        """Scan transpiler-generated excpp/ scaffolding"""
        print("Scanning excpp/ transpiler scaffolding...")
        
        excpp_dir = self.repo_root / 'excpp'
        if not excpp_dir.exists():
            return
        
        for module_dir in excpp_dir.iterdir():
            if not module_dir.is_dir():
                continue
            
            module_name = module_dir.name
            files = []
            total_lines = 0
            
            # Find all .cpp and .hpp files
            for ext in ['*.cpp', '*.hpp']:
                for excpp_file in module_dir.rglob(ext):
                    lines = self.count_lines(excpp_file)
                    total_lines += lines
                    
                    files.append(FileMetrics(
                        path=str(excpp_file.relative_to(self.repo_root)),
                        lines=lines,
                        functions=0,
                        classes=0,
                        has_implementation=lines > 20  # Heuristic: > 20 lines might have impl
                    ))
            
            self.excpp_modules[module_name] = {
                'files': files,
                'total_lines': total_lines,
                'file_count': len(files)
            }
    
    def calculate_completeness(self, module_name: str) -> ModuleAssessment:
        """Calculate completeness for a module"""
        
        # Get TypeScript metrics
        ts_files = self.ts_modules.get(module_name, [])
        ts_lines = sum(f.lines for f in ts_files)
        
        # Get C++ metrics
        cpp_data = self.cpp_modules.get(module_name, {'files': [], 'total_lines': 0, 'file_count': 0})
        cpp_lines = cpp_data['total_lines']
        cpp_files_count = cpp_data['file_count']
        
        # Get excpp metrics
        excpp_data = self.excpp_modules.get(module_name, {'files': [], 'total_lines': 0, 'file_count': 0})
        excpp_lines = excpp_data['total_lines']
        excpp_files_count = excpp_data['file_count']
        
        # Calculate completeness score (0-100)
        completeness = 0.0
        status = "missing"
        notes = []
        
        if cpp_lines > 0:
            # Base completeness on line count ratio
            if ts_lines > 0:
                ratio = min(cpp_lines / ts_lines, 1.0)
                completeness = ratio * 100
            else:
                # No TS reference, judge by absolute size
                if cpp_lines > 1000:
                    completeness = 100
                elif cpp_lines > 500:
                    completeness = 80
                elif cpp_lines > 200:
                    completeness = 60
                else:
                    completeness = 30
            
            # Determine status
            if completeness >= 80:
                status = "complete"
                notes.append(f"Fully implemented ({cpp_lines} lines)")
            elif completeness >= 40:
                status = "partial"
                notes.append(f"Partial implementation ({cpp_lines}/{ts_lines} lines)")
            else:
                status = "minimal"
                notes.append(f"Minimal implementation ({cpp_lines} lines)")
            
            # Check for placeholders
            has_placeholder = any('placeholder' in f.path for f in cpp_data['files'])
            if has_placeholder:
                notes.append("Contains placeholder files")
        else:
            status = "missing"
            notes.append("No C++ implementation found")
            
            if excpp_lines > 0:
                notes.append(f"Transpiler scaffolding available ({excpp_lines} lines)")
        
        return ModuleAssessment(
            name=module_name,
            ts_files=len(ts_files),
            ts_lines=ts_lines,
            cpp_files=cpp_files_count,
            cpp_lines=cpp_lines,
            excpp_files=excpp_files_count,
            excpp_lines=excpp_lines,
            completeness_score=completeness,
            status=status,
            notes=notes
        )
    
    def generate_report(self) -> str:
        """Generate comprehensive assessment report"""
        
        # Collect all module names
        all_modules = set(self.ts_modules.keys()) | set(self.cpp_modules.keys()) | set(self.excpp_modules.keys())
        all_modules = sorted(all_modules)
        
        # Assess each module
        assessments = []
        for module_name in all_modules:
            assessment = self.calculate_completeness(module_name)
            assessments.append(assessment)
        
        # Sort by completeness (complete first, then partial, minimal, missing)
        status_order = {"complete": 0, "partial": 1, "minimal": 2, "missing": 3}
        assessments.sort(key=lambda a: (status_order[a.status], -a.completeness_score))
        
        # Generate report
        report = []
        report.append("=" * 80)
        report.append("C++ ADAPTATION COMPLETENESS ASSESSMENT")
        report.append("=" * 80)
        report.append("")
        
        # Summary statistics
        complete = sum(1 for a in assessments if a.status == "complete")
        partial = sum(1 for a in assessments if a.status == "partial")
        minimal = sum(1 for a in assessments if a.status == "minimal")
        missing = sum(1 for a in assessments if a.status == "missing")
        
        total = len(assessments)
        completion_rate = (complete / total * 100) if total > 0 else 0
        
        report.append("SUMMARY")
        report.append("-" * 80)
        report.append(f"Total Modules Analyzed: {total}")
        report.append(f"  ✅ Complete: {complete} ({complete/total*100:.1f}%)")
        report.append(f"  🟡 Partial: {partial} ({partial/total*100:.1f}%)")
        report.append(f"  🟠 Minimal: {minimal} ({minimal/total*100:.1f}%)")
        report.append(f"  ❌ Missing: {missing} ({missing/total*100:.1f}%)")
        report.append(f"\nOverall Completion Rate: {completion_rate:.1f}%")
        report.append("")
        
        # Total lines
        total_ts_lines = sum(a.ts_lines for a in assessments)
        total_cpp_lines = sum(a.cpp_lines for a in assessments)
        total_excpp_lines = sum(a.excpp_lines for a in assessments)
        
        report.append(f"Total TypeScript Lines: {total_ts_lines:,}")
        report.append(f"Total C++ Lines (cpp/): {total_cpp_lines:,}")
        report.append(f"Total Transpiled Lines (excpp/): {total_excpp_lines:,}")
        report.append("")
        
        # Detailed module breakdown
        report.append("=" * 80)
        report.append("MODULE-BY-MODULE ANALYSIS")
        report.append("=" * 80)
        report.append("")
        
        for status_filter in ["complete", "partial", "minimal", "missing"]:
            filtered = [a for a in assessments if a.status == status_filter]
            if not filtered:
                continue
            
            status_icon = {
                "complete": "✅",
                "partial": "🟡",
                "minimal": "🟠",
                "missing": "❌"
            }[status_filter]
            
            report.append(f"\n{status_icon} {status_filter.upper()} MODULES ({len(filtered)})")
            report.append("-" * 80)
            
            for assessment in filtered:
                report.append(f"\n{assessment.name}")
                report.append(f"  Completeness: {assessment.completeness_score:.1f}%")
                report.append(f"  TypeScript: {assessment.ts_files} files, {assessment.ts_lines:,} lines")
                report.append(f"  C++ (cpp/): {assessment.cpp_files} files, {assessment.cpp_lines:,} lines")
                report.append(f"  Excpp (transpiled): {assessment.excpp_files} files, {assessment.excpp_lines:,} lines")
                
                if assessment.notes:
                    report.append(f"  Notes:")
                    for note in assessment.notes:
                        report.append(f"    - {note}")
        
        # Next steps recommendations
        report.append("\n\n")
        report.append("=" * 80)
        report.append("RECOMMENDATIONS & NEXT STEPS")
        report.append("=" * 80)
        report.append("")
        
        # High-priority missing modules
        high_priority_missing = [a for a in assessments if a.status == "missing" and a.ts_lines > 500]
        if high_priority_missing:
            report.append("HIGH-PRIORITY MISSING MODULES (>500 TS lines):")
            for a in high_priority_missing[:5]:  # Top 5
                report.append(f"  - {a.name}: {a.ts_lines:,} TS lines, {a.excpp_lines:,} excpp scaffolding available")
        
        report.append("")
        
        # Modules with excpp scaffolding but no cpp implementation
        has_scaffolding = [a for a in assessments if a.cpp_lines == 0 and a.excpp_lines > 100]
        if has_scaffolding:
            report.append("MODULES WITH TRANSPILER SCAFFOLDING (excpp/ > 100 lines, no cpp/):")
            for a in has_scaffolding[:10]:  # Top 10
                report.append(f"  - {a.name}: {a.excpp_lines:,} excpp lines ready to refine")
        
        report.append("")
        
        # Minimal implementations that could use excpp scaffolding
        minimal_can_improve = [a for a in assessments if a.status == "minimal" and a.excpp_lines > a.cpp_lines]
        if minimal_can_improve:
            report.append("MINIMAL IMPLEMENTATIONS THAT CAN BE ENHANCED WITH EXCPP:")
            for a in minimal_can_improve[:5]:  # Top 5
                report.append(f"  - {a.name}: {a.cpp_lines} cpp lines, {a.excpp_lines} excpp lines available")
        
        return "\n".join(report)
    
    def save_json_report(self, output_path: str):
        """Save detailed report as JSON"""
        all_modules = set(self.ts_modules.keys()) | set(self.cpp_modules.keys()) | set(self.excpp_modules.keys())
        assessments = [self.calculate_completeness(m) for m in sorted(all_modules)]
        
        data = {
            'summary': {
                'total_modules': len(assessments),
                'complete': sum(1 for a in assessments if a.status == "complete"),
                'partial': sum(1 for a in assessments if a.status == "partial"),
                'minimal': sum(1 for a in assessments if a.status == "minimal"),
                'missing': sum(1 for a in assessments if a.status == "missing"),
            },
            'modules': [asdict(a) for a in assessments]
        }
        
        with open(output_path, 'w') as f:
            json.dump(data, f, indent=2)
        
        print(f"\nJSON report saved to: {output_path}")

def main():
    """Main entry point"""
    import argparse
    
    parser = argparse.ArgumentParser(description='Assess C++ adaptation completeness')
    parser.add_argument('--repo', default='.', help='Repository root directory')
    parser.add_argument('--json', help='Output JSON report to file')
    parser.add_argument('--output', help='Output text report to file')
    
    args = parser.parse_args()
    
    # Create assessor and run analysis
    assessor = AdaptationAssessor(args.repo)
    
    # Scan all sources
    assessor.scan_typescript_sources()
    assessor.scan_cpp_implementations()
    assessor.scan_excpp_scaffolding()
    
    # Generate report
    report = assessor.generate_report()
    
    # Output report
    print(report)
    
    if args.output:
        with open(args.output, 'w') as f:
            f.write(report)
        print(f"\nReport saved to: {args.output}")
    
    if args.json:
        assessor.save_json_report(args.json)

if __name__ == '__main__':
    main()
