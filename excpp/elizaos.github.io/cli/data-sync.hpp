#pragma once
#include <algorithm>
#include <cstdint>
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use

/**
 * Data Sync Utility
 *
 * This script synchronizes data from the remote _data branch to a local development environment,
 * allowing developers to work with the latest production data.
 *
 * Based on the same approach used in GitHub Actions:
 * - Fetches the _data branch
 * - Creates a temporary worktree
 * - Copies data files to the local environment
 * - Restores the SQLite database from diffable format
 */

// Helper to check if uv is installed
bool isUvInstalled();

// Helper to print uv installation instructions
void printUvInstallInstructions();

// Helper to get directory size recursively

// Helper to get detailed directory info (up to 2 levels deep)

    // First level subdirectories

      // Skip dump directory

      // Second level subdirectories

        // Skip if can't read second level directories
    // Return empty array if there's an error

// Helper to get file size
double getFileSize(const std::string& filePath);

// Helper to format bytes to human readable size
std::string formatBytes(double bytes);

// Helper to format directory path for display
std::string formatDirPath(const std::string& basePath, const std::string& fullPath);

// Helper to format files and size info
std::string formatFilesAndSize(double files, double size);

// Helper to get latest migration number from data branch journal

// Helper to delete files in a directory while preserving .gitkeep files

// Configure commander

      // Configure logger

      // Check if uv is installed when database operations are needed

      // Step 1: Clean up existing worktree

      // Step 2: Check if the remote branch exists

      // Step 3: Fetch the data branch from remote with depth limit for speed

      // Step 4: Create a worktree for the data branch

      // Pre-restore database in worktree for accurate size comparison

      // Step 5: Check for existing data and get stats for confirmation prompt

        // Get database size comparison

        // Get detailed directory info

        // Create the comparison table using cli-table3

        // Add database row

        // Function to create a map of directory paths to info

        // Create a set of all unique directory paths

        // Add all paths from both current and remote

        // Sort paths for consistent display

        // Add rows for each unique path

        // Print comparison table

        // List potentially affected directories

        // Ask for confirmation

          // Clean up worktree

      // Create data directory if it doesn't exist

      // Force delete existing data files if requested

        // Also remove the database file if it exists

      // Step 6: Copy data from worktree to data directory

        // Copy all files except the database file and dump directory
        // Use a filtering function to exclude .sqlite files and dump directory

      // Step 7: Copy database file and apply final migrations

      // Final cleanup


} // namespace elizaos
