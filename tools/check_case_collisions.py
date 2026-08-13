#!/usr/bin/env python3
"""Detect case-insensitive path collisions in the current Git repository.

This protects Windows and other case-insensitive filesystems from ambiguous
checkouts such as `Foo/Bar` vs `foo/bar`, which can break configure/build
steps and package generation.
"""

from __future__ import annotations

import subprocess
import sys
from collections import defaultdict
from pathlib import Path


def main() -> int:
    try:
        repository = str(Path.cwd().resolve())
        result = subprocess.run(
            ["git", "-c", f"safe.directory={repository}", "ls-files"],
            check=True,
            capture_output=True,
            text=True,
        )
    except subprocess.CalledProcessError as exc:
        print("error: unable to enumerate tracked files with git ls-files", file=sys.stderr)
        print(exc.stderr, file=sys.stderr)
        return 2

    buckets: dict[str, list[str]] = defaultdict(list)
    for raw_path in result.stdout.splitlines():
        path = raw_path.strip()
        if not path:
            continue
        buckets[path.lower()].append(path)

    collisions = [sorted(paths) for paths in buckets.values() if len(paths) > 1]
    if not collisions:
        print("No case-insensitive path collisions detected.")
        return 0

    print("Case-insensitive path collisions detected:", file=sys.stderr)
    for group in sorted(collisions, key=lambda g: [p.lower() for p in g]):
        print("- collision group:", file=sys.stderr)
        for path in group:
            print(f"    {path}", file=sys.stderr)

    print(
        "\nThese collisions break checkouts and builds on Windows and other "
        "case-insensitive filesystems.",
        file=sys.stderr,
    )
    return 1


if __name__ == "__main__":
    raise SystemExit(main())
