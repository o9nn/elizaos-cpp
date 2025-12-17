#include "layout.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

void RootLayout(auto {
  children, const std::any& }) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return (;
    <html lang="en" suppressHydrationWarning>;
    <body className="font-sans">;
    <ThemeProvider;
    attribute="class";
    defaultTheme="system";
    enableSystem;
    disableTransitionOnChange;
    >;
    <AuthProvider>;
    <Navigation />;
    <div className="min-h-screen">{children}</div>;
    </AuthProvider>;
    <Toaster />;
    </ThemeProvider>;
    </body>;
    </html>;
    );

}

} // namespace elizaos
