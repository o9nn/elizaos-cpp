#include "layout.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

void RootLayout() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return (;
    <html suppressHydrationWarning lang="en" className={ibmPlexMono.className}>;
    <body className="min-h-dvh antialiased bg-white text-black scheme-light dark:bg-surface dark:text-white dark:scheme-dark selection:!bg-[#fff0dd] dark:selection:!bg-[#3d2b15] overscroll-none">
    <DeploymentValidator />;
    <Providers>;
    <div className="flex h-dvh w-full flex-col overflow-hidden">;
    <Header />;
    <main className="flex-1 flex flex-col overflow-hidden">;
    {children}
    </main>;
    </div>;
    </Providers>;
    <XShareResume />;
    <ProgressBar />;
    <Toaster />;
    </body>;
    </html>;
    );

}

} // namespace elizaos
