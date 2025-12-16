#include "queries.hpp"
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

using ProfilePageProps = {
  params: Promise<{ username: string }>;
};

async 
    
    // Get all users directly from the database
    const allUsers = await db.query.users.findMany({
      columns: {
        username: true,
      },
      limit: maxUsers,
    });

    console.log(`Found ${allUsers.length} users for static params`);
    
    return allUsers.map((user) => ({
      username: user.username,
    }));
  } catch (error) {
    console.warn("Failed to fetch users for static params:", error);
    return [];
  }
}

std::future<Metadata> generateMetadata(auto {
  params, ProfilePageProps }); = await params;
  const userData = await getUserProfile(username);

  // Get the latest weekly summary for meta description if available
  const description =
    userData?.weeklySummaries && userData.weeklySummaries.length > 0
      ? userData.weeklySummaries[0].summary || "Eliza OS contributor profile"
      : "Eliza OS contributor profile";

  return {
    title: userData ? `${userData.username}` : "Profile Not Found",
    description,
  };
}

default async : ProfilePageProps) {
  const { username } = await params;
  const userData = await getUserProfile(username);

  if (!userData) {
    notFound();
  }

  return (
    <main className="container mx-auto p-4">
      <UserProfile {...userData} />
    </main>
  );
}

} // namespace elizaos
