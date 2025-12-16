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

;
;
;
;
;
;
;

dotenv.config();

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

  private async fetchOrganizationPackages(): Promise<PackageInfo[]> {
    const packages: PackageInfo[] = [];
    
    try {
      // Search for packages in both @elizaos and @elizaos-plugins scopes
      const searchTerms = ['@elizaos', '@elizaos-plugins'];
      
      for (const searchTerm of searchTerms) {
        console.log(`Searching for packages with "${searchTerm}"...`);
        
        const response = await axios.get(`${this.NPM_REGISTRY_URL}/-/v1/search`, {
          params: {
            text: searchTerm,
            size: 250, // Max results per page
          }
        });

        for (const pkg of response.data.objects || []) {
          const packageInfo = pkg.package;
          if (packageInfo.name && packageInfo.name.startsWith(searchTerm + '/')) {
            // Get detailed package information
            const detailResponse = await axios.get(`${this.NPM_REGISTRY_URL}/${encodeURIComponent(packageInfo.name)}`);
            const detailData = detailResponse.data;
            
            packages.push({
              name: packageInfo.name,
              versions: Object.keys(detailData.versions || {}),
              description: packageInfo.description,
              repository: packageInfo.links?.repository,
              maintainers: packageInfo.maintainers?.map((m: any) => m.username),
              keywords: packageInfo.keywords,
              license: packageInfo.license,
              createdDate: detailData.time?.created,
              modifiedDate: detailData.time?.modified
            });
          }
        }
        
        // Add small delay between searches
        await this.delay(200);
      }
    } catch (error) {
      console.error("Error fetching organization packages:", error);
      throw error;
    }

    // Remove duplicates and sort
    const uniquePackages = packages.filter((pkg, index, self) => 
      index === self.findIndex(p => p.name === pkg.name)
    );
    
    return uniquePackages.sort((a, b) => a.name.localeCompare(b.name));
  }

  private async fetchPackageDownloads(packages: PackageInfo[]): Promise<PackageDownloads[]> {
    const downloads: PackageDownloads[] = [];
    
    for (const pkg of packages) {
      try {
        // Get download stats for different periods
        const [weeklyData, monthlyData, yearlyData] = await Promise.all([
          this.getDownloadStats(pkg.name, "last-week"),
          this.getDownloadStats(pkg.name, "last-month"), 
          this.getDownloadStats(pkg.name, "last-year")
        ]);

        downloads.push({
          packageName: pkg.name,
          totalDownloads: yearlyData.downloads,
          downloadsPeriod: "last-year",
          weeklyDownloads: weeklyData.downloads,
          monthlyDownloads: monthlyData.downloads,
          yearlyDownloads: yearlyData.downloads
        });

        // Add small delay to avoid rate limiting
        await this.delay(100);
      } catch (error: any) {
        console.warn(chalk.yellow(`⚠️ Could not fetch download stats for ${pkg.name}:`, error?.message || 'Unknown error'));
        // Add package with zero downloads if API fails
        downloads.push({
          packageName: pkg.name,
          totalDownloads: 0,
          downloadsPeriod: "last-year",
          weeklyDownloads: 0,
          monthlyDownloads: 0,
          yearlyDownloads: 0
        });
      }
    }

    return downloads;
  }

  private async fetchVersionDownloads(packages: PackageInfo[]): Promise<VersionDownloads[]> {
    const versionDownloads: VersionDownloads[] = [];
    
    for (const pkg of packages) {
      try {
        // Get total downloads for the package (we can't get per-version stats from npm API)
        const downloadData = await this.getDownloadStats(pkg.name, "last-month");
        
        // For each version, we'll estimate based on recency and total downloads
        // Note: npm API doesn't provide per-version download stats
        const versions = pkg.versions.slice(-10); // Get last 10 versions to avoid too much data
        
        for (const version of versions) {
          versionDownloads.push({
            packageName: pkg.name,
            version: version,
            downloads: Math.round(downloadData.downloads / versions.length), // Rough estimate
            downloadsPeriod: "last-month"
          });
        }

        await this.delay(100);
      } catch (error: any) {
        console.warn(chalk.yellow(`⚠️ Could not fetch version download stats for ${pkg.name}`));
      }
    }

    return versionDownloads;
  }

  private async getDownloadStats(packageName: string, period: string): Promise<{ downloads: number }> {
    try {
      const response = await axios.get(`${this.NPM_DOWNLOADS_URL}/point/${period}/${encodeURIComponent(packageName)}`);
      return { downloads: response.data.downloads || 0 };
    } catch (error: any) {
      if (error.response?.status === 404) {
        // Package not found or no download stats
        return { downloads: 0 };
      }
      throw error;
    }
  }

  private async generateExcelReport(
    packages: PackageInfo[], 
    packageDownloads: PackageDownloads[], 
    versionDownloads: VersionDownloads[]
  ): Promise<void> {
    const workbook = XLSX.utils.book_new();

    // Create Package Overview sheet
    const packageOverviewData = packages.map(pkg => {
      const downloads = packageDownloads.find(d => d.packageName === pkg.name);
      return {
        "Package Name": pkg.name,
        "Description": pkg.description || "",
        "Total Versions": pkg.versions.length,
        "Latest Version": pkg.versions[pkg.versions.length - 1] || "",
        "Weekly Downloads": downloads?.weeklyDownloads || 0,
        "Monthly Downloads": downloads?.monthlyDownloads || 0,
        "Yearly Downloads": downloads?.yearlyDownloads || 0,
        "Repository": pkg.repository || "",
        "License": pkg.license || "",
        "Created Date": pkg.createdDate || "",
        "Modified Date": pkg.modifiedDate || "",
        "Keywords": (pkg.keywords || []).join(", "),
        "Maintainers": (pkg.maintainers || []).join(", ")
      };
    });

    const packageSheet = XLSX.utils.json_to_sheet(packageOverviewData);
    XLSX.utils.book_append_sheet(workbook, packageSheet, "Package Overview");

    // Create Package Downloads sheet
    const packageDownloadsData = packageDownloads.map(d => ({
      "Package Name": d.packageName,
      "Weekly Downloads": d.weeklyDownloads,
      "Monthly Downloads": d.monthlyDownloads,
      "Yearly Downloads": d.yearlyDownloads,
      "Total Downloads": d.totalDownloads
    }));

    const downloadsSheet = XLSX.utils.json_to_sheet(packageDownloadsData);
    XLSX.utils.book_append_sheet(workbook, downloadsSheet, "Package Downloads");

    // Create Version Downloads sheet (sample of recent versions)
    const versionDownloadsData = versionDownloads.map(v => ({
      "Package Name": v.packageName,
      "Version": v.version,
      "Estimated Monthly Downloads": v.downloads,
      "Period": v.downloadsPeriod
    }));

    const versionsSheet = XLSX.utils.json_to_sheet(versionDownloadsData);
    XLSX.utils.book_append_sheet(workbook, versionsSheet, "Version Downloads");

    // Create Summary sheet
    const totalPackages = packages.length;
    const totalDownloads = packageDownloads.reduce((sum, d) => sum + d.yearlyDownloads, 0);
    const totalWeeklyDownloads = packageDownloads.reduce((sum, d) => sum + d.weeklyDownloads, 0);
    const totalMonthlyDownloads = packageDownloads.reduce((sum, d) => sum + d.monthlyDownloads, 0);
    const topPackage = packageDownloads.length > 0 ? packageDownloads.reduce((max, d) => d.yearlyDownloads > max.yearlyDownloads ? d : max) : null;

    const summaryData = [
      { "Metric": "Total Packages", "Value": totalPackages },
      { "Metric": "Total Yearly Downloads", "Value": totalDownloads },
      { "Metric": "Total Monthly Downloads", "Value": totalMonthlyDownloads },
      { "Metric": "Total Weekly Downloads", "Value": totalWeeklyDownloads },
      { "Metric": "Average Downloads per Package (Yearly)", "Value": Math.round(totalDownloads / totalPackages) },
      { "Metric": "Most Downloaded Package", "Value": topPackage?.packageName || "N/A" },
      { "Metric": "Most Downloaded Package Downloads", "Value": topPackage?.yearlyDownloads || 0 },
      { "Metric": "Report Generated", "Value": new Date().toISOString() }
    ];

    const summarySheet = XLSX.utils.json_to_sheet(summaryData);
    XLSX.utils.book_append_sheet(workbook, summarySheet, "Summary");

    // Write the Excel file
    const outputPath = path.join(this.OUTPUT_DIR, this.OUTPUT_FILE);
    XLSX.writeFile(workbook, outputPath);
  }

  private printSummary(packages: PackageInfo[], packageDownloads: PackageDownloads[]): void {
    const totalDownloads = packageDownloads.reduce((sum, d) => sum + d.yearlyDownloads, 0);
    const totalWeeklyDownloads = packageDownloads.reduce((sum, d) => sum + d.weeklyDownloads, 0);
    const topPackages = packageDownloads
      .sort((a, b) => b.yearlyDownloads - a.yearlyDownloads)
      .slice(0, 5);

    console.log(chalk.cyan("\n📊 Summary:"));
    console.log(`   Total packages: ${chalk.bold(packages.length)}`);
    console.log(`   Total yearly downloads: ${chalk.bold(totalDownloads.toLocaleString())}`);
    console.log(`   Total weekly downloads: ${chalk.bold(totalWeeklyDownloads.toLocaleString())}`);
    console.log(`   Average downloads per package: ${chalk.bold(Math.round(totalDownloads / packages.length).toLocaleString())}`);
    
    console.log(chalk.cyan("\n🏆 Top 5 packages by yearly downloads:"));
    topPackages.forEach((pkg, index) => {
      console.log(`   ${index + 1}. ${chalk.bold(pkg.packageName)}: ${chalk.green(pkg.yearlyDownloads.toLocaleString())} downloads`);
    });
  }

  private delay(ms: number): Promise<void> {
    return new Promise(resolve => setTimeout(resolve, ms));
  }
}

std::future<void> main();

main().catch((error) => {
  console.error(chalk.red("Fatal error:"), error);
  process.exit(1);
}); 
} // namespace elizaos
