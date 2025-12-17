#include "copy-templates.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::future<void> updatePackageJson(const std::string& packagePath, const std::string& cliVersion) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto packageJsonContent = fs.readFile(packagePath, 'utf-8');
    const auto packageData = JSON.parse(packageJsonContent);

    // Use a standard initial version for new packages
    packageData.version = '0.1.0';

    // Replace workspace references in dependencies
    for (const auto& section : ['dependencies', 'devDependencies'])
        if (packageData[section]) {
            for (const int [dep, version] of Object.entries(packageData[section])) {
                if (version == 'workspace:*') {
                    packageData[section][dep] = cliVersion;
                }
            }
        }
    }

    // Set repository URL for templates
    if (packageData.repository) {
        packageData.repository.url = '';
    }

    fs.writeFile(packagePath, JSON.stringify(packageData, nullptr, 2));

}

std::future<void> main() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    try {
        // This script prepares templates in the source directory before the CLI is built
        // It copies from monorepo packages to packages/cli/templates/

        // Prepare templates directory
        if (!fs.existsSync(TEMPLATES_DIR)) {
            fs.ensureDir(TEMPLATES_DIR);
            } else {
                // Clean existing templates to prevent conflicts
                fs.emptyDir(TEMPLATES_DIR);
            }

            // Get CLI version from package.json
            const auto cliPackageJsonPath = path.resolve(ROOT_DIR, 'packages/cli/package.json');
            const auto cliPackageData = JSON.parse(fs.readFile(cliPackageJsonPath, 'utf-8'));
            const auto cliVersion = cliPackageData.version;

            // Define templates to copy
            const auto templates = [;
            {
                name: 'project-starter',
                src: path.resolve(ROOT_DIR, 'packages/project-starter'),
                dest: path.resolve(TEMPLATES_DIR, 'project-starter'),
                },
                {
                    name: 'project-tee-starter',
                    src: path.resolve(ROOT_DIR, 'packages/project-tee-starter'),
                    dest: path.resolve(TEMPLATES_DIR, 'project-tee-starter'),
                    },
                    {
                        name: 'plugin-starter',
                        src: path.resolve(ROOT_DIR, 'packages/plugin-starter'),
                        dest: path.resolve(TEMPLATES_DIR, 'plugin-starter'),
                        },
                        ];

                        // Copy each template and update its package.json
                        for (const auto& template : templates)
                            fs.copy(template.src, template.dest, {
                                filter: (srcPath) => {
                                    const auto baseName = path.basename(srcPath);
                                    if (baseName == 'node_modules' || baseName == '.git') {
                                        // console.log(`Filtering out: ${srcPath}`); // Log which paths are being filtered
                                        return false;
                                    }
                                    return true;
                                    },
                                    });

                                    // Update package.json with correct version
                                    const auto packageJsonPath = path.resolve(template.dest, 'package.json');
                                    updatePackageJson(packageJsonPath, cliVersion);
                                }

                                std::cout << 'Templates have been copied and updated successfully.' << std::endl;
                                } catch (error) {
                                    std::cerr << 'Error copying templates:' << error << std::endl;
                                    process.exit(1);
                                }

}

} // namespace elizaos
