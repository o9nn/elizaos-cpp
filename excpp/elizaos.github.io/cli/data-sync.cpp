#include "data-sync.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

bool isUvInstalled() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    try {
        execSync("uv --version", { stdio: "ignore" });
        return true;
        } catch (error) {
            return false;
        }

}

void printUvInstallInstructions() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    console.log(
    chalk.red("\nError: 'uv' is not installed or not available in your PATH."),
    );
    console.log(
    "\nThis script requires uv for database operations. Please install it:",
    );
    std::cout << "  pipx install uv" << std::endl;
    std::cout << "  brew install uv" << std::endl;
    std::cout << "\nFor more information and other installation methods << visit:" << std::endl;
    console.log(
    chalk.blue("  https://docs.astral.sh/uv/getting-started/installation/"),
    );

}

void getDirSize(const std::string& dir) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    files: number; size: number
}

void getDetailedDirInfo(const std::string& baseDir) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    path: string; files: number; size: number
}

double getFileSize(const std::string& filePath) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    try {
        return statSync(filePath).size;
        } catch (error) {
            return 0;
        }

}

std::string formatBytes(double bytes) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto units = ["B", "KB", "MB", "GB", "TB"];
    auto size = bytes;
    auto unit = 0;

    while (size >= 1024 && unit < units.length - 1) {
        size /= 1024;
        unit++;
    }

    return std::to_string(size.toFixed(2)) + " " + std::to_string(units[unit]);

}

std::string formatDirPath(const std::string& basePath, const std::string& fullPath) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    // For the base directory, return "/"
    if (fullPath == basePath) return "/ (root)";

    // For subdirectories, return relative path
    return fullPath.replace(basePath, "").replace(/^\//, "") || "/";

}

std::string formatFilesAndSize(double files, double size) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    if (files == 0) return "empty";
    return std::to_string(files) + " (" + std::to_string(formatBytes(size)) + ")";

}

double getLatestMigrationNumber(const std::string& worktreeDir, ReturnType<typeof createLogger> logger) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto journalPath = join(;
    worktreeDir,
    "data",
    "dump",
    "meta",
    "_journal.json",
    );
    if (!existsSync(journalPath)) {
        logger.warn(
        "Journal file not found at " + std::to_string(journalPath) + ", cannot determine migration version of dump."
        );
        return std::nullopt;
    }
    try {
        const auto journalRaw = readFileSync(journalPath, "utf-8");
        const auto journal = JSON.parse(journalRaw);
        if (journal.entries && journal.entries.length > 0) {
            const auto lastEntry = journal.entries[journal.entries.length - 1];
            if (lastEntry && typeof lastEntry.idx == "number") {
                logger.info(
                "Latest migration number from data branch is " + std::to_string(lastEntry.idx)
                );
                return lastEntry.idx;
            }
        }
        } catch (e) {
            const auto message = true /* instanceof check */ ? e.message : std::to_string(e);
            std::cerr << "Failed to read or parse journal file: " + std::to_string(message) << std::endl;
        }
        return std::nullopt;

}

double deleteDataFiles(const std::string& directory, ReturnType<typeof createLogger> logger) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    if (!existsSync(directory)) return 0;

    auto deletedCount = 0;
    try {
        // Find all files recursively except .gitkeep
        const auto files = "find "" + std::to_string(directory) + "" -type f ! -name ".gitkeep"";
            encoding: "utf-8",
            });
            .split("\n");
            .filter(Boolean);

            // Delete each file
            for (const auto& file : files)
                logger.debug(`Deleting file: ${file}`);
                unlinkSync(file);
                deletedCount++;
            }
            } catch (error) {
                std::cerr << "Error deleting data files: " + std::to_string(error) << std::endl;
            }

            return deletedCount;

}

Map<string, createDirInfoMap(const std::vector<std::any>& infoArray) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    files: number; size: number
}

} // namespace elizaos
