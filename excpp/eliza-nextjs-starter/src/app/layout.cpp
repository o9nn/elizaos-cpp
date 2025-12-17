#include "layout.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

void RootLayout() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return (;
    <html suppressHydrationWarning lang="en" className={inter.className}>;
    <body className="min-h-dvh antialiased bg-white text-black scheme-light dark:bg-black dark:text-white dark:scheme-dark selection:!bg-[#fff0dd] dark:selection:!bg-[#3d2b15] overscroll-none">
    <div className="flex min-h-dvh w-full flex-col grow">;
    <div className="flex grow flex-col size-full min-h-dvh">;
    <ThemeProvider;
    attribute="class";
    defaultTheme="system";
    enableSystem;
    disableTransitionOnChange;
    >;
    <Header />;
    {children}
    </ThemeProvider>;
    </div>;
    </div>;
    <ProgressBar />;
    <Toaster />;
    </body>;
    </html>;
    );

}

} // namespace elizaos
