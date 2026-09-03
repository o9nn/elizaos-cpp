#!/usr/bin/env python3
"""Scan C++ sources/headers for std:: symbols used without the owning header.

libstdc++ pulls many standard headers in transitively, MSVC's STL does not, so
code that compiles on Linux fails on Windows with C2039/C3861. This finds those
cases before CI does.
"""
import json
import os
import re
import sys

# symbol -> required header
SYMBOL_HEADER = {
    # <algorithm>
    'sort': 'algorithm', 'stable_sort': 'algorithm', 'find': 'algorithm',
    'find_if': 'algorithm', 'remove_if': 'algorithm', 'remove': 'algorithm',
    'count_if': 'algorithm', 'transform': 'algorithm', 'for_each': 'algorithm',
    'copy': 'algorithm', 'copy_if': 'algorithm', 'fill': 'algorithm',
    'reverse': 'algorithm', 'unique': 'algorithm', 'lower_bound': 'algorithm',
    'upper_bound': 'algorithm', 'max_element': 'algorithm',
    'min_element': 'algorithm', 'any_of': 'algorithm', 'all_of': 'algorithm',
    'none_of': 'algorithm', 'shuffle': 'algorithm', 'clamp': 'algorithm',
    'partial_sort': 'algorithm', 'nth_element': 'algorithm',
    'set_intersection': 'algorithm', 'set_difference': 'algorithm',
    'generate': 'algorithm',
    # <numeric>
    'accumulate': 'numeric', 'iota': 'numeric', 'inner_product': 'numeric',
    'partial_sum': 'numeric', 'reduce': 'numeric',
    # <iterator>
    'back_inserter': 'iterator', 'inserter': 'iterator',
    'distance': 'iterator', 'advance': 'iterator', 'next': 'iterator',
    'begin': None, 'end': None,  # too noisy, skip
    # <functional>
    'function': 'functional', 'bind': 'functional', 'ref': 'functional',
    'cref': 'functional', 'hash': 'functional', 'greater': 'functional',
    'less': 'functional', 'plus': 'functional',
    # <memory>
    'unique_ptr': 'memory', 'shared_ptr': 'memory', 'weak_ptr': 'memory',
    'make_unique': 'memory', 'make_shared': 'memory',
    'enable_shared_from_this': 'memory',
    # <string>
    'string': 'string', 'to_string': 'string', 'stoi': 'string',
    'stod': 'string', 'stof': 'string', 'stol': 'string', 'stoull': 'string',
    # containers
    'vector': 'vector', 'map': 'map', 'set': 'set',
    'unordered_map': 'unordered_map', 'unordered_set': 'unordered_set',
    'deque': 'deque', 'list': 'list', 'queue': 'queue', 'stack': 'stack',
    'array': 'array', 'priority_queue': 'queue',
    # misc
    'optional': 'optional', 'nullopt': 'optional',
    'variant': 'variant', 'get_if': 'variant', 'holds_alternative': 'variant',
    'any': 'any', 'any_cast': 'any', 'bad_any_cast': 'any',
    'mutex': 'mutex', 'lock_guard': 'mutex', 'unique_lock': 'mutex',
    'recursive_mutex': 'mutex', 'scoped_lock': 'mutex', 'call_once': 'mutex',
    'atomic': 'atomic', 'atomic_int': 'atomic',
    'thread': 'thread', 'this_thread': 'thread',
    'condition_variable': 'condition_variable',
    'ostringstream': 'sstream', 'istringstream': 'sstream',
    'stringstream': 'sstream',
    'ofstream': 'fstream', 'ifstream': 'fstream', 'fstream': 'fstream',
    'cout': 'iostream', 'cerr': 'iostream', 'cin': 'iostream',
    'setw': 'iomanip', 'setprecision': 'iomanip', 'setfill': 'iomanip',
    'numeric_limits': 'limits',
    'runtime_error': 'stdexcept', 'invalid_argument': 'stdexcept',
    'logic_error': 'stdexcept', 'out_of_range': 'stdexcept',
    'exception': 'exception',
    'pair': 'utility', 'make_pair': 'utility', 'move': 'utility',
    'forward': 'utility', 'swap': 'utility',
    'tuple': 'tuple', 'make_tuple': 'tuple', 'tie': 'tuple', 'get': None,
    'chrono': 'chrono',
    'error_code': 'system_error',
    'random_device': 'random', 'mt19937': 'random',
    'uniform_int_distribution': 'random',
    'uniform_real_distribution': 'random', 'normal_distribution': 'random',
    'filesystem': 'filesystem',
    'regex': 'regex', 'regex_match': 'regex', 'regex_search': 'regex',
    'smatch': 'regex', 'regex_replace': 'regex',
    'is_same': 'type_traits', 'enable_if': 'type_traits',
    'decay': 'type_traits', 'is_base_of': 'type_traits',
    'future': 'future', 'promise': 'future', 'async': 'future',
    'packaged_task': 'future',
    'initializer_list': 'initializer_list',
    'bitset': 'bitset',
    'size_t': None, 'uint8_t': None, 'int64_t': None,  # skip cstdint noise
}

SKIP_DIRS = {
    '.git', 'node_modules', 'third_party', '_deps', 'generated', 'archive',
    '__pycache__', '.cache', 'tests', 'test', 'examples', 'docs', 'tools',
    'cpp_merge', 'cpp_generated', 'cpp_hybrid', 'output_multi_full',
}
SKIP_PREFIXES = ('build', 'cmake-build', 'excpp')
CANONICAL_TOP_LEVEL = {'include', 'packages', 'external', 'village', 'src', 'cpp'}
CPP_SUFFIX = r'(?:cpp|cc|cxx|hpp|h)'
CANONICAL_FILE_PATTERNS = (
    re.compile(rf'^include/.+\.{CPP_SUFFIX}$'),
    re.compile(rf'^packages/(?:core|infrastructure|applications|integration|plugins|starters)/[^/]+/src/[^/]+\.{CPP_SUFFIX}$'),
    re.compile(rf'^external/[^/]+/src/[^/]+\.{CPP_SUFFIX}$'),
    re.compile(rf'^village/[^/]+\.{CPP_SUFFIX}$'),
    re.compile(rf'^src/[^/]+\.{CPP_SUFFIX}$'),
    re.compile(r'^packages/integration/autonomous_starter/src/plugin-self-modification/src/database/adapter\.hpp$'),
)


def should_skip_dir(name):
    """Exclude generated/build mirrors without hiding canonical external modules."""
    return name in SKIP_DIRS or name.startswith(SKIP_PREFIXES)


def collect_includes(text):
    return set(re.findall(r'#\s*include\s*<([^>]+)>', text))


# Symbols MSVC actually rejects most often. libstdc++ almost always provides the
# container/string/stream symbols transitively, so scanning for those produces
# thousands of low-value hits. These are the ones that broke the Windows build.
HIGH_RISK = {
    'sort', 'stable_sort', 'find', 'find_if', 'remove_if', 'remove', 'count_if',
    'transform', 'for_each', 'copy', 'copy_if', 'fill', 'reverse', 'unique',
    'lower_bound', 'upper_bound', 'max_element', 'min_element', 'any_of',
    'all_of', 'none_of', 'shuffle', 'clamp', 'partial_sort', 'nth_element',
    'set_intersection', 'set_difference', 'generate',
    'accumulate', 'iota', 'inner_product', 'partial_sum', 'reduce',
    'back_inserter', 'inserter', 'next', 'prev', 'distance',
}


def is_canonical_file(root, path):
    relative = os.path.relpath(path, root).replace(os.sep, '/')
    if relative.startswith('cpp/'):
        relative = relative[4:]
    if '_COMPLETE.' in relative or '_BACKUP.' in relative:
        return False
    return any(pattern.match(relative) for pattern in CANONICAL_FILE_PATTERNS)


def load_compiled_sources(root):
    database = os.path.join(root, 'build-audit', 'compile_commands.json')
    if not os.path.isfile(database):
        return set()
    try:
        with open(database, encoding='utf-8') as handle:
            commands = json.load(handle)
    except (OSError, ValueError):
        return set()
    compiled = set()
    for entry in commands:
        source = entry.get('file', '')
        if not os.path.isabs(source):
            source = os.path.join(entry.get('directory', root), source)
        compiled.add(os.path.realpath(source))
    return compiled


def scan_root(root, findings, high_risk_only, canonical_only):
    root = os.path.abspath(root)
    compiled_sources = load_compiled_sources(root) if canonical_only else set()
    repository_root = os.path.isfile(os.path.join(root, 'CMakeLists.txt'))
    for dirpath, dirnames, filenames in os.walk(root):
        dirnames[:] = [d for d in dirnames if not should_skip_dir(d)]
        if repository_root and os.path.abspath(dirpath) == root:
            dirnames[:] = [d for d in dirnames if d in CANONICAL_TOP_LEVEL]
        for fn in filenames:
            if not fn.endswith(('.cpp', '.hpp', '.h', '.cc', '.cxx')):
                continue
            path = os.path.join(dirpath, fn)
            if canonical_only and not is_canonical_file(root, path):
                continue
            if (canonical_only and compiled_sources and
                    fn.endswith(('.cpp', '.cc', '.cxx')) and
                    os.path.realpath(path) not in compiled_sources):
                continue
            try:
                with open(path, encoding='utf-8', errors='replace') as fh:
                    text = fh.read()
            except OSError:
                continue
            includes = collect_includes(text)
            # strip comments crudely to reduce false positives
            body = re.sub(r'//[^\n]*', '', text)
            body = re.sub(r'/\*.*?\*/', '', body, flags=re.S)
            used = set(re.findall(r'\bstd::([A-Za-z_][A-Za-z0-9_]*)', body))
            for sym in sorted(used):
                header = SYMBOL_HEADER.get(sym)
                if not header:
                    continue
                if header in includes:
                    continue
                if high_risk_only and sym not in HIGH_RISK:
                    continue
                findings.append((path, sym, header))


def main(argv):
    high_risk_only = '--high-risk' in argv
    canonical_only = '--canonical-only' in argv
    roots = [a for a in argv if not a.startswith('--')] or ['.']
    findings = []
    for root in roots:
        scan_root(root, findings, high_risk_only, canonical_only)
    for path, sym, header in findings:
        print(f'{path}: std::{sym} used without <{header}>')
    mode = 'high-risk' if high_risk_only else 'all'
    scope = ', canonical' if canonical_only else ''
    print(f'\nTOTAL ({mode}{scope}): {len(findings)} potential missing includes')
    # Non-zero exit in high-risk mode makes this usable as a CI gate.
    return 1 if (high_risk_only and findings) else 0


if __name__ == '__main__':
    sys.exit(main(sys.argv[1:]))
