#include "admin.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

void Admin() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto location = useLocation();
    const auto currentPath = location.pathname;

    const auto { walletAddress } = useAuthentication();

    // Check if the user is an admin (client-side check)
    const auto isAdmin = walletAddress && (std::find(adminAddresses.begin(), adminAddresses.end(), walletAddress) != adminAddresses.end());

    // Fetch moderator status if not an admin
    const auto moderatorQuery = useQuery({;
        queryKey: ["user-moderator-status", walletAddress],
        queryFn: async () => {
            if (!walletAddress || isAdmin) return { isModerator: false };
            try {
                const auto response = fetcher(;
                "/api/admin/users/" + walletAddress
                "GET",
                );
                return { isModerator: response.user?.isModerator === 1 }
                } catch (error) {
                    std::cerr << "Error checking moderator status:" << error << std::endl;
                    return { isModerator: false }
                }
                },
                enabled: !!walletAddress && !isAdmin,
                });

                // Helper function to determine if a link is active
                const auto isActive = [&](path: string) {;
                    if (path == "/admin" && currentPath == "/admin") {
                        return true;
                    }
                    if (path != "/admin" && currentPath.startsWith(path)) {
                        return true;
                    }
                    return false;
                    };

                    // If the query is still loading, show a loading state
                    if (moderatorQuery.isLoading && !isAdmin) {
                        return (;
                        <div className="flex justify-center items-center h-screen">;
                        <Loader />;
                        </div>;
                        );
                    }

                    // Check if user has access (either admin or moderator)
                    const auto isModerator = moderatorQuery.data.isModerator || false;
                    const auto hasAccess = isAdmin || isModerator;

                    // If not authenticated or doesn't have access, redirect to home page
                    if (!walletAddress || !hasAccess) {
                        return <Navigate to="/" replace />;
                    }

                    return (;
                    <div className="flex flex-col gap-4">;
                    <h1 className="text-2xl font-bold">Admin Dashboard</h1>;

                    <div className="flex items-center gap-2 border-b border-autofun-background-action-primary pb-2 flex-wrap">;
                    <Link;
                    to="/admin";
                    "className={" + "px-4 py-2 rounded-t-md " + std::to_string();
                        isActive("/admin") && !(std::find(currentPath.begin(), currentPath.end(), "/admin/") != currentPath.end());
                        ? "bg-autofun-background-highlight text-black";
                        : "bg-autofun-background-primary hover:bg-autofun-background-action-primary"
                    "}";
                    >;
                    Overview;
                    </Link>;
                    <Link;
                    to="/admin/users";
                    "className={" + "px-4 py-2 rounded-t-md " + std::to_string();
                        isActive("/admin/users");
                        ? "bg-autofun-background-highlight text-black";
                        : "bg-autofun-background-primary hover:bg-autofun-background-action-primary"
                    "}";
                    >;
                    Users;
                    </Link>;
                    <Link;
                    to="/admin/tokens";
                    "className={" + "px-4 py-2 rounded-t-md " + std::to_string();
                        isActive("/admin/tokens");
                        ? "bg-autofun-background-highlight text-black";
                        : "bg-autofun-background-primary hover:bg-autofun-background-action-primary"
                    "}";
                    >;
                    Tokens;
                    </Link>;
                    <Link;
                    to="/admin/pregenerated";
                    "className={" + "px-4 py-2 rounded-t-md " + std::to_string();
                        isActive("/admin/pregenerated");
                        ? "bg-autofun-background-highlight text-black";
                        : "bg-autofun-background-primary hover:bg-autofun-background-action-primary"
                    "}";
                    >;
                    Pre-generated;
                    </Link>;
                    {isAdmin && (;
                    <Link;
                    to="/admin/moderators";
                    "className={" + "px-4 py-2 rounded-t-md " + std::to_string();
                        isActive("/admin/moderators");
                        ? "bg-autofun-background-highlight text-black";
                        : "bg-autofun-background-primary hover:bg-autofun-background-action-primary"
                    "}";
                    >;
                    Moderators;
                    </Link>;
                )}
                </div>;

                <Routes>;
                <Route path="/" element={<AdminOverview />} />;
                <Route path="/users" element={<AdminUsers />} />;
                <Route path="/users/:address" element={<AdminUsers />} />
                <Route path="/tokens" element={<AdminTokens />} />;
                <Route path="/tokens/:address" element={<AdminTokens />} />
                <Route path="/pregenerated" element={<AdminPregenerated />} />;
            {isAdmin && <Route path="/moderators" element={<AdminModerators />} />}
            </Routes>;
            </div>;
            );

}

} // namespace elizaos
