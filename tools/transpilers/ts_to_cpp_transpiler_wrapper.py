#!/usr/bin/env python3
"""
TypeScript to C++ Transpiler Wrapper
Provides backward compatibility with the original ts_to_cpp_transpiler.py
while using the new multi-transpiler system under the hood.

This wrapper maintains the same CLI interface as the original transpiler
but delegates to the multi-transpiler for better quality output.

Usage:
    python3 ts_to_cpp_transpiler_wrapper.py [--input-dir DIR] [--output-dir DIR] [--verbose] [--parallel]
"""

import sys
import argparse
from pathlib import Path
from multi_transpiler import MultiTranspiler

def main():
    parser = argparse.ArgumentParser(
        description='TypeScript to C++ Transpiler (Multi-Backend)',
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="""
Examples:
  # Transpile directory
  python3 ts_to_cpp_transpiler_wrapper.py --input-dir src/ --output-dir cpp_output/
  
  # Verbose mode
  python3 ts_to_cpp_transpiler_wrapper.py --input-dir src/ --output-dir cpp_output/ --verbose
  
  # Parallel processing (uses multi-backend system)
  python3 ts_to_cpp_transpiler_wrapper.py --input-dir src/ --output-dir cpp_output/ --parallel

Note: This wrapper uses the new multi-transpiler system for better quality output.
      The original ts_to_cpp_transpiler.py is still available as a fallback.
        """
    )
    
    parser.add_argument('--input-dir', type=Path, default=Path('src'),
                        help='Input directory containing TypeScript files (default: src)')
    parser.add_argument('--output-dir', type=Path, default=Path('cpp_output'),
                        help='Output directory for generated C++ files (default: cpp_output)')
    parser.add_argument('--verbose', action='store_true',
                        help='Enable verbose output')
    parser.add_argument('--parallel', action='store_true',
                        help='Enable parallel processing (uses multi-backend system)')
    parser.add_argument('--strategy', type=str, default=None,
                        help='Transpilation strategy (enhanced_hybrid, fast_approximate, typescript2cxx_only, ts2cpp_only)')
    parser.add_argument('--config', type=Path, default=Path('multi_transpiler_config.yaml'),
                        help='Configuration file (default: multi_transpiler_config.yaml)')
    parser.add_argument('--max-workers', type=int, default=4,
                        help='Maximum number of parallel workers (default: 4)')
    
    args = parser.parse_args()
    
    # Validate input directory
    if not args.input_dir.exists():
        print(f"Error: Input directory does not exist: {args.input_dir}")
        sys.exit(1)
    
    # Create output directory
    args.output_dir.mkdir(parents=True, exist_ok=True)
    
    # Initialize multi-transpiler
    config_path = args.config if args.config.exists() else None
    transpiler = MultiTranspiler(config_path=config_path)
    
    if args.verbose:
        print(f"Using multi-transpiler system")
        print(f"Input directory: {args.input_dir}")
        print(f"Output directory: {args.output_dir}")
        if args.strategy:
            print(f"Strategy: {args.strategy}")
        if config_path:
            print(f"Configuration: {config_path}")
        print()
    
    # Transpile
    try:
        transpiler.transpile(
            input_path=args.input_dir,
            output_dir=args.output_dir,
            strategy=args.strategy
        )
        
        if args.verbose:
            print("\nTranspilation completed successfully!")
        
        sys.exit(0)
    except Exception as e:
        print(f"Error during transpilation: {e}")
        if args.verbose:
            import traceback
            traceback.print_exc()
        sys.exit(1)

if __name__ == '__main__':
    main()
