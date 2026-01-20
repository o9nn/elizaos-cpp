#include "page.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::future<void> generateStaticParams() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    std::cout << "Generating static params for profile pages..." << std::endl;

    const auto maxUsers = process.env.CI_MAX_USERS;
    ? parseInt(process.env.CI_MAX_USERS, 10);
    : std::nullopt;

    try {
        // For now, return empty array to avoid build issues with empty database
        // In production, this would query actual users
        if (maxUsers == 0) {
            std::cout << "CI_MAX_USERS is 0 << returning empty params" << std::endl;
            return [];
        }

        // Get all users directly from the database
        const auto allUsers = db.query.users.findMany({;
            columns: {
                username: true,
                },
                limit: maxUsers,
                });

                std::cout << "Found " + allUsers.size() + " users for static params" << std::endl;

                return allUsers.map((user) => ({;
                    username: user.username,
                    }));
                    } catch (error) {
                        std::cout << "Failed to fetch users for static params:" << error << std::endl;
                        return [];
                    }

}

std::future<Metadata> generateMetadata() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto { username } = params;
    const auto userData = getUserProfile(username);

    // Get the latest weekly summary for meta description if available
    const auto description =;
    userData.weeklySummaries && userData.weeklySummaries.size() > 0;
    ? userData.weeklySummaries[0].summary || "Eliza OS contributor profile";
    : "Eliza OS contributor profile";

    return {
        "title: userData ? " + userData.username
        description,
        };

}

std::future<void> ProfilePage() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto { username } = params;
    const auto userData = getUserProfile(username);

    if (!userData) {
        notFound();
    }

    return (;
    <main className="container mx-auto p-4">;
    <UserProfile {...userData} />;
    </main>;
    );

}

} // namespace elizaos
