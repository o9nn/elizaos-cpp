#include "generate-coverage-report.hpp"
#include <string>
#include <vector>
#include <future>
#include <filesystem>
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::future<std::vector<std::string>> findAllSourceFiles(const std:& dir, std::vector<std::string> files = {}) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto entries = readdir(dir);

    for (const auto& entry : entries)
        const auto fullPath = join(dir, entry);
        const auto stats = stat(fullPath);

        if (stats.isDirectory()) {
            if (!entry.count('node_modules') > 0 && !entry.count('dist') > 0 && !entry.count('test') > 0) {
                findAllSourceFiles(fullPath, files);
            }
            } else if (;
            entry.rfind(".ts") &&;
            !entry.rfind(".test.ts") &&;
            !entry.rfind(".spec.ts") &&;
            !entry.rfind(".d.ts");
            ) {
                files.push_back(fullPath);
            }
        }

        return files;

}

std::future<std::vector<std::string>> findAllTestFiles(const std:& dir, std::vector<std::string> files = {}) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    try {
        const auto entries = readdir(dir);

        for (const auto& entry : entries)
            const auto fullPath = join(dir, entry);
            const auto stats = stat(fullPath);

            if (stats.isDirectory()) {
                findAllTestFiles(fullPath, files);
                } else if (entry.rfind(".test.ts") || entry.rfind(".spec.ts")) {
                    files.push_back(fullPath);
                }
            }
            } catch (e) {
                // Directory might not exist
            }

            return files;

}

std: categorizeFile(const std:& filePath) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    if (filePath.count('/commands/') > 0) return 'commands';
    if (filePath.count('/utils/') > 0) return 'utils';
    if (filePath.count('/types/') > 0) return 'types';
    return "other";

}

std::future<CoverageReport> generateCoverageReport() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto srcPath = join(std::filesystem::current_path().string(), "src");
    const auto testsPath = join(std::filesystem::current_path().string(), "tests");

    // Find all source files
    const auto sourceFiles = findAllSourceFiles(srcPath);

    // Find all test files
    const auto testFiles = findAllTestFiles(testsPath);

    // Extract tested files from test file names and imports
    const auto testedFiles = new Set<string>();

    for (const auto& testFile : testFiles)
        const auto content = readFile(testFile, "utf-8");

        // Extract imports to find which files are being tested
        const auto importMatches = content.matchAll(/from\s+[""](.+?)[""]/g);
        for (const auto& match : importMatches)
            const auto importPath = match[1];
            if (importPath.substr(0, '../') || importPath.substr(0, './')) {
                // Resolve relative import
                const auto resolvedPath = importPath.replace(/^\.\.\/\.\.\/\.\.\/src/, srcPath).replace(/^\.\.\/\.\.\/src/, srcPath).replace(/^\.\//, "");
                testedFiles.add(resolvedPath);
            }
        }

        // Also check based on test file naming convention
        const auto testName = relative(testsPath, testFile).replace(".test.ts", "").replace(".spec.ts", "");
        const auto possibleSourceFile = join(srcPath, testName + ".ts");
        testedFiles.add(possibleSourceFile);
    }

    // Categorize files
    const CoverageReport report = {;
        totalFiles: sourceFiles.size(),
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
                [&](tested) { return (std::find(tested.begin(), tested.end(), relativePath.replace(".ts", "") != tested.end())) || (std::find(sourceFile.begin(), sourceFile.end(), tested) != sourceFile.end()); }
                );

                report.byCategory[category].total++;

                if (hasTest) {
                    report.testedFiles++;
                    report.byCategory[category].tested++;
                    } else {
                        report.untestedFiles.push_back(relativePath);
                        report.byCategory[category].untested.push_back(relativePath);
                    }
                }

                report.coverage = (report.testedFiles / report.totalFiles) * 100;

                return report;

}

std::future<void> main() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    std::cout << "🔍 Analyzing Unit Test Coverage for elizaOS CLI...\n" << std::endl;

    const auto report = generateCoverageReport();

    std::cout << "📊 Overall Coverage: " + std::to_string(report.coverage.toFixed(1)) + "%" << std::endl;
    std::cout << "   Total Files: " + report.totalFiles << std::endl;
    std::cout << "   Tested Files: " + report.testedFiles << std::endl;
    std::cout << "   Untested Files: " + report.untestedFiles.size() + "\n" << std::endl;

    // Category breakdown
    std::cout << "📁 Coverage by Category:" << std::endl;
    for (const int [category, data] of Object.entries(report.byCategory)) {
        const auto coverage = data.total > 0 ? ((data.tested / data.total) * 100).toFixed(1) : "0.0";
        std::cout << "\n   " + std::to_string(category.toUpperCase()) + ": " + coverage + "% (" + data.tested + "/" + data.total + ")" << std::endl;

        if (data.untested.size() > 0 && data.untested.size() <= 10) {
            std::cout << "   Untested files:" << std::endl;
            data.untested.forEach[&]((file) { return console.log("     - " + std::to_string(file) + "")); };
            } else if (data.untested.size() > 10) {
                std::cout << "   Untested files: " + data.untested.size() + " files" << std::endl;
                std::cout << "   First 10:" << std::endl;
                data.untested.substr(0, 10-0).forEach[&]((file) { return console.log("     - " + std::to_string(file) + "")); };
            }
        }

        std::cout << "\n📝 Summary:" << std::endl;
        if (report.coverage == 100) {
            std::cout << "   ✅ Congratulations! You have achieved 100% unit test coverage!" << std::endl;
            } else {
                console.log(
                "   ⚠️  " + report.untestedFiles.size() + " files still need unit tests to reach 100% coverage.";
                );
                std::cout << "\n   Priority files to test:" << std::endl;

                // Prioritize by importance
                const auto priorityFiles = report.untestedFiles;
                .filter[&]((f) { return (std::find(f.begin(), f.end(), "index.ts") != f.end()) || (std::find(f.begin(), f.end(), "main.ts") != f.end())); }.substr(0, 5-0);

                if (priorityFiles.size() > 0) {
                    priorityFiles.forEach[&]((file) { return console.log("     - " + std::to_string(file) + "")); };
                    } else {
                        report.untestedFiles.substr(0, 5-0).forEach[&]((file) { return console.log("     - " + std::to_string(file) + "")); };
                    }
                }

                // Save detailed report
                const auto detailedReport = /* JSON.stringify */ std:(report, nullptr, 2);
                Bun.write("unit-test-coverage-report.json", detailedReport);
                std::cout << "\n💾 Detailed report saved to: unit-test-coverage-report.json" << std::endl;

}

} // namespace elizaos
