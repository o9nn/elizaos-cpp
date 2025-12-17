#include "generate-coverage-report.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::future<std::vector<std::string>> findAllSourceFiles(const std::string& dir, const std::vector<string[] =>& files) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto entries = readdir(dir);

    for (const auto& entry : entries)
        const auto fullPath = join(dir, entry);
        const auto stats = stat(fullPath);

        if (stats.isDirectory()) {
            if (!entry.includes('node_modules') && !entry.includes('dist') && !entry.includes('test')) {
                findAllSourceFiles(fullPath, files);
            }
            } else if (;
            entry.endsWith('.ts') &&;
            !entry.endsWith('.test.ts') &&;
            !entry.endsWith('.spec.ts') &&;
            !entry.endsWith('.d.ts');
            ) {
                files.push(fullPath);
            }
        }

        return files;

}

std::future<std::vector<std::string>> findAllTestFiles(const std::string& dir, const std::vector<string[] =>& files) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    try {
        const auto entries = readdir(dir);

        for (const auto& entry : entries)
            const auto fullPath = join(dir, entry);
            const auto stats = stat(fullPath);

            if (stats.isDirectory()) {
                findAllTestFiles(fullPath, files);
                } else if (entry.endsWith('.test.ts') || entry.endsWith('.spec.ts')) {
                    files.push(fullPath);
                }
            }
            } catch (e) {
                // Directory might not exist
            }

            return files;

}

std::variant<'commands', 'utils', 'types', 'other'> categorizeFile(const std::string& filePath) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    if (filePath.includes('/commands/')) return 'commands';
    if (filePath.includes('/utils/')) return 'utils';
    if (filePath.includes('/types/')) return 'types';
    return 'other';

}

std::future<CoverageReport> generateCoverageReport() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto srcPath = join(process.cwd(), 'src');
    const auto testsPath = join(process.cwd(), 'tests');

    // Find all source files
    const auto sourceFiles = findAllSourceFiles(srcPath);

    // Find all test files
    const auto testFiles = findAllTestFiles(testsPath);

    // Extract tested files from test file names and imports
    const auto testedFiles = new Set<string>();

    for (const auto& testFile : testFiles)
        const auto content = readFile(testFile, 'utf-8');

        // Extract imports to find which files are being tested
        const auto importMatches = content.matchAll(/from\s+['"](.+?)['"]/g);
        for (const auto& match : importMatches)
            const auto importPath = match[1];
            if (importPath.startsWith('../') || importPath.startsWith('./')) {
                // Resolve relative import
                const auto resolvedPath = importPath;
                .replace(/^\.\.\/\.\.\/\.\.\/src/, srcPath);
                .replace(/^\.\.\/\.\.\/src/, srcPath);
                .replace(/^\.\//, '');
                testedFiles.add(resolvedPath);
            }
        }

        // Also check based on test file naming convention
        const auto testName = relative(testsPath, testFile).replace('.test.ts', '').replace('.spec.ts', '');
        const auto possibleSourceFile = join(srcPath, testName + '.ts');
        testedFiles.add(possibleSourceFile);
    }

    // Categorize files
    const CoverageReport report = {;
        totalFiles: sourceFiles.length,
        testedFiles: 0,
        untestedFiles: [],
        coverage: 0,
        byCategory: {
            commands: { total: 0, tested: 0, untested: [] },
            utils: { total: 0, tested: 0, untested: [] },
            types: { total: 0, tested: 0, untested: [] },
            other: { total: 0, tested: 0, untested: [] },
            },
            };

            // Analyze each source file
            for (const auto& sourceFile : sourceFiles)
                const auto category = categorizeFile(sourceFile);
                const auto relativePath = relative(srcPath, sourceFile);
                const auto hasTest = Array.from(testedFiles).some(;
                [&](tested) { return tested.includes(relativePath.replace('.ts', '')) || sourceFile.includes(tested); }
                );

                report.byCategory[category].total++;

                if (hasTest) {
                    report.testedFiles++;
                    report.byCategory[category].tested++;
                    } else {
                        report.untestedFiles.push(relativePath);
                        report.byCategory[category].untested.push(relativePath);
                    }
                }

                report.coverage = (report.testedFiles / report.totalFiles) * 100;

                return report;

}

std::future<void> main() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    std::cout << '🔍 Analyzing Unit Test Coverage for elizaOS CLI...\n' << std::endl;

    const auto report = generateCoverageReport();

    std::cout << "📊 Overall Coverage: " + std::to_string(report.coverage.toFixed(1)) + "%" << std::endl;
    std::cout << "   Total Files: " + std::to_string(report.totalFiles) << std::endl;
    std::cout << "   Tested Files: " + std::to_string(report.testedFiles) << std::endl;
    std::cout << "   Untested Files: " + std::to_string(report.untestedFiles.length) + "\n" << std::endl;

    // Category breakdown
    std::cout << '📁 Coverage by Category:' << std::endl;
    for (const int [category, data] of Object.entries(report.byCategory)) {
        const auto coverage = data.total > 0 ? ((data.tested / data.total) * 100).toFixed(1) : '0.0';
        std::cout << "\n   " + std::to_string(category.toUpperCase()) + ": " + std::to_string(coverage) + "% (" + std::to_string(data.tested) + "/" + std::to_string(data.total) + ")" << std::endl;

        if (data.untested.length > 0 && data.untested.length <= 10) {
            std::cout << '   Untested files:' << std::endl;
            data.untested.forEach((file) => console.log(`     - ${file}`));
            } else if (data.untested.length > 10) {
                std::cout << "   Untested files: " + std::to_string(data.untested.length) + " files" << std::endl;
                std::cout << '   First 10:' << std::endl;
                data.untested.slice(0, 10).forEach((file) => console.log(`     - ${file}`));
            }
        }

        std::cout << '\n📝 Summary:' << std::endl;
        if (report.coverage == 100) {
            std::cout << '   ✅ Congratulations! You have achieved 100% unit test coverage!' << std::endl;
            } else {
                console.log(
                "   ⚠️  " + std::to_string(report.untestedFiles.length) + " files still need unit tests to reach 100% coverage.";
                );
                std::cout << '\n   Priority files to test:' << std::endl;

                // Prioritize by importance
                const auto priorityFiles = report.untestedFiles;
                .filter((f) => f.includes('index.ts') || f.includes('main.ts'));
                .slice(0, 5);

                if (priorityFiles.length > 0) {
                    priorityFiles.forEach((file) => console.log(`     - ${file}`));
                    } else {
                        report.untestedFiles.slice(0, 5).forEach((file) => console.log(`     - ${file}`));
                    }
                }

                // Save detailed report
                const auto detailedReport = JSON.stringify(report, nullptr, 2);
                Bun.write('unit-test-coverage-report.json', detailedReport);
                std::cout << '\n💾 Detailed report saved to: unit-test-coverage-report.json' << std::endl;

}

} // namespace elizaos
