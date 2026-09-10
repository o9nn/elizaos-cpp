#!/usr/bin/env python3
"""Check for case-insensitive filename collisions.

Exits with code 1 if any two paths differ only by case (would conflict on
Windows/macOS case-insensitive filesystems). Exits with code 0 otherwise.
"""

import os
import sys
from collections import defaultdict


def check_case_collisions(root="."):
    lower_to_paths = defaultdict(list)

    for dirpath, dirnames, filenames in os.walk(root):
        # Skip .git to avoid false positives from pack files etc.
        dirnames[:] = [d for d in dirnames if d != ".git"]

        for filename in filenames:
            filepath = os.path.join(dirpath, filename)
            # Normalise path separators so the comparison is consistent.
            filepath = filepath.replace("\\", "/")
            lower_to_paths[filepath.lower()].append(filepath)

    collisions_found = False
    for _lower_path, paths in sorted(lower_to_paths.items()):
        if len(paths) > 1:
            if not collisions_found:
                print("Case-insensitive path collisions detected:")
                collisions_found = True
            for path in paths:
                print(f"  {path}")

    if collisions_found:
        print(
            "\nERROR: These files would conflict on case-insensitive filesystems"
            " (Windows/macOS). Rename them so they differ by more than case."
        )
        sys.exit(1)

    print("No case collisions found — safe for case-insensitive filesystems.")
    sys.exit(0)


if __name__ == "__main__":
    root = sys.argv[1] if len(sys.argv) > 1 else "."
    check_case_collisions(root)
