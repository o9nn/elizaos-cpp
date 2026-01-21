#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#pragma once

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



struct PackageInfo {
    std::string name;
    std::vector<std::string> versions;
    std::optional<std::string> description;
    std::optional<std::string> repository;
    std::optional<std::vector<std::string>> maintainers;
    std::optional<std::vector<std::string>> keywords;
    std::optional<std::string> license;
    std::optional<std::string> createdDate;
    std::optional<std::string> modifiedDate;
};

struct VersionDownloads {
    std::string packageName;
    std::string version;
    double downloads;
    std::string downloadsPeriod;
};

struct PackageDownloads {
    std::string packageName;
    double totalDownloads;
    std::string downloadsPeriod;
    double weeklyDownloads;
    double monthlyDownloads;
    double yearlyDownloads;
};

class NpmDownloadStatsGenerator {
  private readonly NPM_REGISTRY_URL = "https://registry.npmjs.org";
  private readonly NPM_DOWNLOADS_URL = "https://api.npmjs.org/downloads";

  private readonly OUTPUT_DIR = "./assets";
  private readonly OUTPUT_FILE = "elizaos-npm-download-stats.xlsx";

  async main(): Promise<void> {
    console.log(chalk.blue("🚀 Starting npm download statistics generation for @elizaos"));
    
    try {
      // Ensure output directory exists
      await fs.ensureDir(this.OUTPUT_DIR);

      // Get all packages from @elizaos organization
      const spinner = ora("Fetching packages from @elizaos organization...").start();
      const packages = await this.fetchOrganizationPackages();
      spinner.succeed(`Found ${packages.length} packages in @elizaos organization`);

      if (packages.length === 0) {
        console.log(chalk.yellow("⚠️ No packages found in @elizaos organization"));
        return;
      }

      // Get download statistics for all packages
      const downloadSpinner = ora("Fetching download statistics...").start();
      const packageDownloads = await this.fetchPackageDownloads(packages);
      const versionDownloads = await this.fetchVersionDownloads(packages);
      downloadSpinner.succeed("Download statistics fetched successfully");

      // Generate Excel report
      const excelSpinner = ora("Generating Excel report...").start();
      await this.generateExcelReport(packages, packageDownloads, versionDownloads);
      excelSpinner.succeed(`Excel report generated: ${path.join(this.OUTPUT_DIR, this.OUTPUT_FILE)}`);

      console.log(chalk.green("✅ npm download statistics generation completed successfully!"));
      
      // Print summary
      this.printSummary(packages, packageDownloads);

    } catch (error) {
      console.error(chalk.red("❌ Error generating npm download statistics:"), error);
      process.exit(1);
    }
  }

      // Search for packages in both @elizaos and @elizaos-plugins scopes

            // Get detailed package information

        // Add small delay between searches

    // Remove duplicates and sort

        // Get download stats for different periods

        // Add small delay to avoid rate limiting
        // Add package with zero downloads if API fails

        // Get total downloads for the package (we can't get per-version stats from npm API)
        
        // For each version, we'll estimate based on recency and total downloads
        // Note: npm API doesn't provide per-version download stats

        // Package not found or no download stats

    // Create Package Overview sheet

    // Create Package Downloads sheet

    // Create Version Downloads sheet (sample of recent versions)

    // Create Summary sheet

    // Write the Excel file

std::future<void> main();

} // namespace elizaos
