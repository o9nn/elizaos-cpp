#include "/home/runner/work/elizaos-cpp/elizaos-cpp/plugins-automation/src/npmDownloadStats.h"

std::shared_ptr<Promise<void>> NpmDownloadStatsGenerator::main()
{
    console->log(chalk->blue(std::string("🚀 Starting npm download statistics generation for @elizaos")));
    try
    {
        std::async([=]() { fs->ensureDir(this->OUTPUT_DIR); });
        auto spinner = ora(std::string("Fetching packages from @elizaos organization..."))->start();
        auto packages = std::async([=]() { this->fetchOrganizationPackages(); });
        spinner->succeed(std::string("Found ") + packages->get_length() + std::string(" packages in @elizaos organization"));
        if (packages->get_length() == 0) {
            console->log(chalk->yellow(std::string("⚠️ No packages found in @elizaos organization")));
            return std::shared_ptr<Promise<void>>();
        }
        auto downloadSpinner = ora(std::string("Fetching download statistics..."))->start();
        auto packageDownloads = std::async([=]() { this->fetchPackageDownloads(packages); });
        auto versionDownloads = std::async([=]() { this->fetchVersionDownloads(packages); });
        downloadSpinner->succeed(std::string("Download statistics fetched successfully"));
        auto excelSpinner = ora(std::string("Generating Excel report..."))->start();
        std::async([=]() { this->generateExcelReport(packages, packageDownloads, versionDownloads); });
        excelSpinner->succeed(std::string("Excel report generated: ") + path->join(this->OUTPUT_DIR, this->OUTPUT_FILE) + string_empty);
        console->log(chalk->green(std::string("✅ npm download statistics generation completed successfully!")));
        this->printSummary(packages, packageDownloads);
    }
    catch (const any& error)
    {
        console->error(chalk->red(std::string("❌ Error generating npm download statistics:")), error);
        process->exit(1);
    }
}

std::shared_ptr<Promise<array<std::shared_ptr<PackageInfo>>>> NpmDownloadStatsGenerator::fetchOrganizationPackages()
{
    auto packages = array<std::shared_ptr<PackageInfo>>();
    try
    {
        auto searchTerms = array<string>{ std::string("@elizaos"), std::string("@elizaos-plugins") };
        for (auto& searchTerm : searchTerms)
        {
            console->log(std::string("Searching for packages with "") + searchTerm + std::string(""..."));
            auto response = std::async([=]() { axios->get(string_empty + this->NPM_REGISTRY_URL + std::string("/-/v1/search"), object{
                object::pair{std::string("params"), object{
                    object::pair{std::string("text"), searchTerm}, 
                    object::pair{std::string("size"), 250}
                }}
            }); });
            for (auto& pkg : OR((response->data->objects), (array<any>())))
            {
                auto packageInfo = pkg["package"];
                if (AND((packageInfo["name"]), (packageInfo["name"]["startsWith"](searchTerm + std::string("/"))))) {
                    auto detailResponse = std::async([=]() { axios->get(string_empty + this->NPM_REGISTRY_URL + std::string("/") + encodeURIComponent(packageInfo["name"]) + string_empty); });
                    auto detailData = detailResponse->data;
                    packages->push(object{
                        object::pair{std::string("name"), packageInfo["name"]}, 
                        object::pair{std::string("versions"), Object->keys(OR((detailData->versions), (object{})))}, 
                        object::pair{std::string("description"), packageInfo["description"]}, 
                        object::pair{std::string("repository"), packageInfo["links"]["repository"]}, 
                        object::pair{std::string("maintainers"), packageInfo["maintainers"]["map"]([=](auto m) mutable
                        {
                            return m["username"];
                        }
                        )}, 
                        object::pair{std::string("keywords"), packageInfo["keywords"]}, 
                        object::pair{std::string("license"), packageInfo["license"]}, 
                        object::pair{std::string("createdDate"), detailData->time->created}, 
                        object::pair{std::string("modifiedDate"), detailData->time->modified}
                    });
                }
            }
            std::async([=]() { this->delay(200); });
        }
    }
    catch (const any& error)
    {
        console->error(std::string("Error fetching organization packages:"), error);
        throw any(error);
    }
    auto uniquePackages = packages->filter([=](auto pkg, auto index, auto self) mutable
    {
        return index == self->findIndex([=](auto p) mutable
        {
            return p->name == pkg->name;
        }
        );
    }
    );
    return uniquePackages->sort([=](auto a, auto b) mutable
    {
        return a->name->localeCompare(b->name);
    }
    );
}

std::shared_ptr<Promise<array<std::shared_ptr<PackageDownloads>>>> NpmDownloadStatsGenerator::fetchPackageDownloads(array<std::shared_ptr<PackageInfo>> packages)
{
    auto downloads = array<std::shared_ptr<PackageDownloads>>();
    for (auto& pkg : packages)
    {
        try
        {
            auto [weeklyData, monthlyData, yearlyData] = std::async([=]() { Promise->all(std::tuple<std::shared_ptr<Promise<object>>, std::shared_ptr<Promise<object>>, std::shared_ptr<Promise<object>>>{ this->getDownloadStats(pkg->name, std::string("last-week")), this->getDownloadStats(pkg->name, std::string("last-month")), this->getDownloadStats(pkg->name, std::string("last-year")) }); });
            downloads->push(object{
                object::pair{std::string("packageName"), pkg->name}, 
                object::pair{std::string("totalDownloads"), yearlyData["downloads"]}, 
                object::pair{std::string("downloadsPeriod"), std::string("last-year")}, 
                object::pair{std::string("weeklyDownloads"), weeklyData["downloads"]}, 
                object::pair{std::string("monthlyDownloads"), monthlyData["downloads"]}, 
                object::pair{std::string("yearlyDownloads"), yearlyData["downloads"]}
            });
            std::async([=]() { this->delay(100); });
        }
        catch (const any& error)
        {
            console->warn(chalk->yellow(std::string("⚠️ Could not fetch download stats for ") + pkg->name + std::string(":"), OR((error["message"]), (std::string("Unknown error")))));
            downloads->push(object{
                object::pair{std::string("packageName"), pkg->name}, 
                object::pair{std::string("totalDownloads"), 0}, 
                object::pair{std::string("downloadsPeriod"), std::string("last-year")}, 
                object::pair{std::string("weeklyDownloads"), 0}, 
                object::pair{std::string("monthlyDownloads"), 0}, 
                object::pair{std::string("yearlyDownloads"), 0}
            });
        }
    }
    return downloads;
}

std::shared_ptr<Promise<array<std::shared_ptr<VersionDownloads>>>> NpmDownloadStatsGenerator::fetchVersionDownloads(array<std::shared_ptr<PackageInfo>> packages)
{
    auto versionDownloads = array<std::shared_ptr<VersionDownloads>>();
    auto& __array6196_7160 = packages;
    for (auto __indx6196_7160 = 0_N; __indx6196_7160 < __array6196_7160->get_length(); __indx6196_7160++)
    {
        auto& pkg = const_(__array6196_7160)[__indx6196_7160];
        {
            try
            {
                auto downloadData = std::async([=]() { this->getDownloadStats(pkg->name, std::string("last-month")); });
                auto versions = pkg->versions->slice(-10);
                auto& __array6630_6988 = versions;
                for (auto __indx6630_6988 = 0_N; __indx6630_6988 < __array6630_6988->get_length(); __indx6630_6988++)
                {
                    auto& version = const_(__array6630_6988)[__indx6630_6988];
                    {
                        versionDownloads->push(object{
                            object::pair{std::string("packageName"), pkg->name}, 
                            object::pair{std::string("version"), version}, 
                            object::pair{std::string("downloads"), Math->round(downloadData["downloads"] / versions->get_length())}, 
                            object::pair{std::string("downloadsPeriod"), std::string("last-month")}
                        });
                    }
                }
                std::async([=]() { this->delay(100); });
            }
            catch (const any& error)
            {
                console->warn(chalk->yellow(std::string("⚠️ Could not fetch version download stats for ") + pkg->name + string_empty));
            }
        }
    }
    return versionDownloads;
}

std::shared_ptr<Promise<object>> NpmDownloadStatsGenerator::getDownloadStats(string packageName, string period)
{
    try
    {
        auto response = std::async([=]() { axios->get(string_empty + this->NPM_DOWNLOADS_URL + std::string("/point/") + period + std::string("/") + encodeURIComponent(packageName) + string_empty); });
        return object{
            object::pair{std::string("downloads"), OR((response->data->downloads), (0))}
        };
    }
    catch (const any& error)
    {
        if (error["response"]["status"] == 404) {
            return object{
                object::pair{std::string("downloads"), 0}
            };
        }
        throw any(error);
    }
}

std::shared_ptr<Promise<void>> NpmDownloadStatsGenerator::generateExcelReport(array<std::shared_ptr<PackageInfo>> packages, array<std::shared_ptr<PackageDownloads>> packageDownloads, array<std::shared_ptr<VersionDownloads>> versionDownloads)
{
    auto workbook = XLSX->utils->book_new();
    auto packageOverviewData = packages->map([=](auto pkg) mutable
    {
        auto downloads = packageDownloads->find([=](auto d) mutable
        {
            return d->packageName == pkg->name;
        }
        );
        return object{
            object::pair{std::string("Package Name"), pkg->name}, 
            object::pair{std::string("Description"), OR((pkg->description), (string_empty))}, 
            object::pair{std::string("Total Versions"), pkg->versions->get_length()}, 
            object::pair{std::string("Latest Version"), OR((const_(pkg->versions)[pkg->versions->get_length() - 1]), (string_empty))}, 
            object::pair{std::string("Weekly Downloads"), OR((downloads->weeklyDownloads), (0))}, 
            object::pair{std::string("Monthly Downloads"), OR((downloads->monthlyDownloads), (0))}, 
            object::pair{std::string("Yearly Downloads"), OR((downloads->yearlyDownloads), (0))}, 
            object::pair{std::string("Repository"), OR((pkg->repository), (string_empty))}, 
            object::pair{std::string("License"), OR((pkg->license), (string_empty))}, 
            object::pair{std::string("Created Date"), OR((pkg->createdDate), (string_empty))}, 
            object::pair{std::string("Modified Date"), OR((pkg->modifiedDate), (string_empty))}, 
            object::pair{std::string("Keywords"), (OR((pkg->keywords), (array<any>())))->join(std::string(", "))}, 
            object::pair{std::string("Maintainers"), (OR((pkg->maintainers), (array<any>())))->join(std::string(", "))}
        };
    }
    );
    auto packageSheet = XLSX->utils->json_to_sheet(packageOverviewData);
    XLSX->utils->book_append_sheet(workbook, packageSheet, std::string("Package Overview"));
    auto packageDownloadsData = packageDownloads->map([=](auto d) mutable
    {
        return (object{
            object::pair{std::string("Package Name"), d->packageName}, 
            object::pair{std::string("Weekly Downloads"), d->weeklyDownloads}, 
            object::pair{std::string("Monthly Downloads"), d->monthlyDownloads}, 
            object::pair{std::string("Yearly Downloads"), d->yearlyDownloads}, 
            object::pair{std::string("Total Downloads"), d->totalDownloads}
        });
    }
    );
    auto downloadsSheet = XLSX->utils->json_to_sheet(packageDownloadsData);
    XLSX->utils->book_append_sheet(workbook, downloadsSheet, std::string("Package Downloads"));
    auto versionDownloadsData = versionDownloads->map([=](auto v) mutable
    {
        return (object{
            object::pair{std::string("Package Name"), v->packageName}, 
            object::pair{std::string("Version"), v->version}, 
            object::pair{std::string("Estimated Monthly Downloads"), v->downloads}, 
            object::pair{std::string("Period"), v->downloadsPeriod}
        });
    }
    );
    auto versionsSheet = XLSX->utils->json_to_sheet(versionDownloadsData);
    XLSX->utils->book_append_sheet(workbook, versionsSheet, std::string("Version Downloads"));
    auto totalPackages = packages->get_length();
    shared totalDownloads = packageDownloads->reduce([=](auto sum, auto d) mutable
    {
        return sum + d->yearlyDownloads;
    }
    , 0);
    auto totalWeeklyDownloads = packageDownloads->reduce([=](auto sum, auto d) mutable
    {
        return sum + d->weeklyDownloads;
    }
    , 0);
    auto totalMonthlyDownloads = packageDownloads->reduce([=](auto sum, auto d) mutable
    {
        return sum + d->monthlyDownloads;
    }
    , 0);
    auto topPackage = (packageDownloads->get_length() > 0) ? any(packageDownloads->reduce([=](auto max, auto d) mutable
    {
        return (d->yearlyDownloads > max->yearlyDownloads) ? d : max;
    }
    )) : any(nullptr);
    auto summaryData = array<object>{ object{
        object::pair{std::string("Metric"), std::string("Total Packages")}, 
        object::pair{std::string("Value"), totalPackages}
    }, object{
        object::pair{std::string("Metric"), std::string("Total Yearly Downloads")}, 
        object::pair{std::string("Value"), totalDownloads}
    }, object{
        object::pair{std::string("Metric"), std::string("Total Monthly Downloads")}, 
        object::pair{std::string("Value"), totalMonthlyDownloads}
    }, object{
        object::pair{std::string("Metric"), std::string("Total Weekly Downloads")}, 
        object::pair{std::string("Value"), totalWeeklyDownloads}
    }, object{
        object::pair{std::string("Metric"), std::string("Average Downloads per Package (Yearly)")}, 
        object::pair{std::string("Value"), Math->round(totalDownloads / totalPackages)}
    }, object{
        object::pair{std::string("Metric"), std::string("Most Downloaded Package")}, 
        object::pair{std::string("Value"), OR((topPackage->packageName), (std::string("N/A")))}
    }, object{
        object::pair{std::string("Metric"), std::string("Most Downloaded Package Downloads")}, 
        object::pair{std::string("Value"), OR((topPackage->yearlyDownloads), (0))}
    }, object{
        object::pair{std::string("Metric"), std::string("Report Generated")}, 
        object::pair{std::string("Value"), ((std::make_shared<Date>()))->toISOString()}
    } };
    auto summarySheet = XLSX->utils->json_to_sheet(summaryData);
    XLSX->utils->book_append_sheet(workbook, summarySheet, std::string("Summary"));
    auto outputPath = path->join(this->OUTPUT_DIR, this->OUTPUT_FILE);
    XLSX->writeFile(workbook, outputPath);
    return std::shared_ptr<Promise<void>>();
}

void NpmDownloadStatsGenerator::printSummary(array<std::shared_ptr<PackageInfo>> packages, array<std::shared_ptr<PackageDownloads>> packageDownloads)
{
    auto totalDownloads = packageDownloads->reduce([=](auto sum, auto d) mutable
    {
        return sum + d->yearlyDownloads;
    }
    , 0);
    auto totalWeeklyDownloads = packageDownloads->reduce([=](auto sum, auto d) mutable
    {
        return sum + d->weeklyDownloads;
    }
    , 0);
    auto topPackages = packageDownloads->sort([=](auto a, auto b) mutable
    {
        return b->yearlyDownloads - a->yearlyDownloads;
    }
    )->slice(0, 5);
    console->log(chalk->cyan(std::string("\
📊 Summary:")));
    console->log(std::string("   Total packages: ") + chalk->bold(packages->get_length()) + string_empty);
    console->log(std::string("   Total yearly downloads: ") + chalk->bold(totalDownloads->toLocaleString()) + string_empty);
    console->log(std::string("   Total weekly downloads: ") + chalk->bold(totalWeeklyDownloads->toLocaleString()) + string_empty);
    console->log(std::string("   Average downloads per package: ") + chalk->bold(Math->round(totalDownloads / packages->get_length())->toLocaleString()) + string_empty);
    console->log(chalk->cyan(std::string("\
🏆 Top 5 packages by yearly downloads:")));
    topPackages->forEach([=](auto pkg, auto index) mutable
    {
        console->log(std::string("   ") + (index + 1) + std::string(". ") + chalk->bold(pkg->packageName) + std::string(": ") + chalk->green(pkg->yearlyDownloads->toLocaleString()) + std::string(" downloads"));
    }
    );
}

std::shared_ptr<Promise<void>> NpmDownloadStatsGenerator::delay(double ms)
{
    return std::make_shared<Promise>([=](auto resolve) mutable
    {
        return setTimeout(resolve, ms);
    }
    );
}

std::shared_ptr<Promise<void>> main()
{
    auto generator = std::make_shared<NpmDownloadStatsGenerator>();
    std::async([=]() { generator->main(); });
    return std::shared_ptr<Promise<void>>();
};



void Main(void)
{
    dotenv->config();
    main()->_catch([=](auto error) mutable
    {
        console->error(chalk->red(std::string("Fatal error:")), error);
        process->exit(1);
    }
    );
}

MAIN
