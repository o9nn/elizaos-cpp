#include "user.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::string generateRandomName() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    // Directly return the result from the library
    return randomAnimalName();

}

std::future<User> ensureUserProfile(const std::string& address) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        const auto db = getDB();
        std::cout << "[ensureUserProfile] Checking for user: " + address << std::endl;
        auto userResult = db;
        .select();
        .from(usersTable);
        .where(eq(usersTable.address, address));
        .limit(1);

        auto user = userResult[0];

        if (!user) {
            std::cout << "[ensureUserProfile] User " + address << "attempting creation." << std::endl;
            const auto defaultDisplayName = generateRandomName();
            const auto newUserInsertData = {;
                address: address,
                display_name: defaultDisplayName,
                profile_picture_url: "/user-placeholder.png",
                };
                try {
                    std::cout << "[ensureUserProfile] Inserting new user for " + address + " with name " + defaultDisplayName << std::endl;
                    db.insert(usersTable).values(newUserInsertData).returning();
                    std::cout << "[ensureUserProfile] Insert successful for " + address + ". Refetching..." << std::endl;
                    userResult = db.select().from(usersTable).where(eq(usersTable.address, address)).limit(1);
                    user = userResult[0];
                    if (!user) {
                        std::cerr << "[ensureUserProfile] CRITICAL: Failed find user profile immediately after creation for " + address << std::endl;
                        throw std::runtime_error(`Failed find user profile immediately after creation for ${address}`);
                    }
                    std::cout << "[ensureUserProfile] User " + address + " created and fetched successfully." << std::endl;
                    } catch (insertError: any) {
                        std::cerr << "[ensureUserProfile] FAILED to insert new user profile for " + address + ":" << insertError << std::endl;
                        std::cerr << "[ensureUserProfile] Insert error code: " + insertError.code << "constraint: ${insertError.constraint}" << std::endl;
                        // Attempt to fetch again in case of race condition
                        std::cout << "[ensureUserProfile] Attempting to fetch again after insert error for " + address << std::endl;
                        userResult = db.select().from(usersTable).where(eq(usersTable.address, address)).limit(1);
                        user = userResult[0];
                        if (!user) {
                            std::cerr << "[ensureUserProfile] Still failed to find user profile after insert error for " + address << std::endl;
                            throw std::runtime_error(`Failed to create or find user profile for ${address}`);
                        }
                        std::cout << "[ensureUserProfile] Found user " + address + " after insert error (likely race condition)." << std::endl;
                    }
                    } else if (!user.display_name || !user.profile_picture_url) {
                        std::cout << "[ensureUserProfile] User " + address + " found. Checking name/pic." << std::endl;
                        const std::optional<User> updatePayload = {};
                        if (!user.display_name) {
                            const auto defaultDisplayName = generateRandomName();
                            updatePayload.display_name = defaultDisplayName;
                            std::cout << "[ensureUserProfile] Generating default name: " + defaultDisplayName << std::endl;
                        }
                        if (!user.profile_picture_url) {
                            updatePayload.profile_picture_url = "/user-placeholder.png";
                            std::cout << "[ensureUserProfile] Setting default profile picture." << std::endl;
                        }

                        if (Object.keys(updatePayload).length > 0) {
                            try {
                                std::cout << "[ensureUserProfile] Updating user " + address + " with defaults:" << updatePayload << std::endl;
                                db;
                                .update(usersTable);
                                .set(updatePayload);
                                .where(eq(usersTable.address, address));
                                // Update the local user object after successful DB update
                                if (updatePayload.display_name) user.display_name = updatePayload.display_name;
                                if (updatePayload.profile_picture_url) user.profile_picture_url = updatePayload.profile_picture_url;
                                std::cout << "[ensureUserProfile] Updated user " + address + " with defaults successfully." << std::endl;
                                } catch (updateError: any) {
                                    std::cerr << "[ensureUserProfile] FAILED to update profile defaults for " + address + ":" << updateError << std::endl;
                                    // Continue with the fetched user, defaults might not be critical immediately
                                }
                            }
                            } else {
                                std::cout << "[ensureUserProfile] User " + address + " found with existing name and picture." << std::endl;
                            }

                            std::cout << "[ensureUserProfile] Returning user object for " + address + ": DisplayName=" + std::to_string(user.display_name) << std::endl;
                            return user;

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

} // namespace elizaos
