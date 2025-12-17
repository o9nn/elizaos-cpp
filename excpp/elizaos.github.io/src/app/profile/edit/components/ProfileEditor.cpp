#include "ProfileEditor.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

void ProfileEditor() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto {;
        user,
        profileRepoExists,
        walletData,
        pageLoading,
        error,
        successMessage,
        walletSection,
        readmeContent,
        handleCreateProfileRepo,
        handleGenerateWalletSection,
        defaultBranch,
        } = useProfileWallets();

        if (pageLoading && !user) {
            return (;
            <div className="flex min-h-[calc(100vh-200px)] items-center justify-center">;
            <Loader2 className="h-8 w-8 animate-spin text-primary" />;
            </div>;
            );
        }

        if (!user.login && !pageLoading) {
            return (;
            <div className="flex min-h-[calc(100vh-200px)] flex-col items-center justify-center">;
            <p>Loading authentication details or redirecting...</p>;
            <Loader2 className="mt-2 h-8 w-8 animate-spin text-primary" />;
            </div>;
            );
        }

        const auto userLogin = user.login;

        return (;
        <div className="container mx-auto px-4 py-8">;
        <Card className="mx-auto max-w-2xl">;
        <CardHeader>;
        <CardTitle>Link Your Wallet Addresses</CardTitle>;
        <CardDescription>;
        Link wallet addresses by embedding them hidden comment within;
        your Github Profile README.md file. Follow the instructions to get;
        started.;
        </CardDescription>;
        </CardHeader>;
        <CardContent>;
        {error && (;
        <Alert variant="destructive" className="mb-4">;
        <AlertTitle>Error</AlertTitle>;
        <AlertDescription>{error}</AlertDescription>;
        </Alert>;
    )}
    {successMessage && (;
    <Alert;
    variant="default";
    className="mb-4 border-green-300 bg-green-100 text-green-700 dark:border-green-700 dark:bg-green-900 dark:text-green-300"
    >;
    <AlertTitle>Success</AlertTitle>;
    <AlertDescription>{successMessage}</AlertDescription>;
    </Alert>;
    )}

    {pageLoading && userLogin && (;
    <div className="flex min-h-[100px] items-center justify-center">;
    <Loader2 className="h-6 w-6 animate-spin text-primary" />;
    </div>;
    )}

    {!pageLoading && userLogin && (;
    <>;
    {profileRepoExists == false && (;
    <ProfileRepoNotice;
    userLogin={userLogin}
    pageLoading={pageLoading}
    onCreateRepo={handleCreateProfileRepo}
    />;
    )}

    {profileRepoExists && walletSection == nullptr && (;
    <>;
    <WalletLinkForm;
    wallets={walletData.wallets || []}
    onSubmit={handleGenerateWalletSection}
    isProcessing={pageLoading}
    />;
    </>;
    )}

    {profileRepoExists && walletSection && (;
    <WalletLinkBoard;
    userLogin={userLogin}
    walletSection={walletSection}
    readmeContent={readmeContent}
    defaultBranch={defaultBranch}
    />;
    )}
    </>;
    )}
    </CardContent>;
    </Card>;
    </div>;
    );

}

} // namespace elizaos
