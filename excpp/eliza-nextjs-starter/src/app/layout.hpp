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

import type { Metadata, Viewport } from "next";
;

;
;
import "@/app/globals.css";
;
;
;

const viewport: Viewport = {
  width: "device-width",
  initialScale: 1,
  themeColor: "white",
};

const metadata: Metadata = {
  metadataBase: new URL(siteConfig.url),
  title: `${siteConfig.name} - Learn about the Eliza Agent Framework`,
  description: siteConfig.description,
  openGraph: {
    siteName: siteConfig.name,
    title: "The Documentation for Eliza",
    description: siteConfig.description,
    images: [siteConfig.ogImage],
    type: "website",
    url: siteConfig.url,
    locale: "en_US",
  },
  icons: siteConfig.icons,
  twitter: {
    card: "summary_large_image",
    site: siteConfig.name,
    title: "The Documentation for Eliza",
    description: siteConfig.description,
    images: [siteConfig.ogImage],
    creator: siteConfig.creator,
  },
};

default : Readonly<{
  children: React.ReactNode;
}>) {
  return (
    <html suppressHydrationWarning lang="en" className={inter.className}>
      <body className="min-h-dvh antialiased bg-white text-black scheme-light dark:bg-black dark:text-white dark:scheme-dark selection:!bg-[#fff0dd] dark:selection:!bg-[#3d2b15] overscroll-none">
        <div className="flex min-h-dvh w-full flex-col grow">
          <div className="flex grow flex-col size-full min-h-dvh">
            <ThemeProvider
              attribute="class"
              defaultTheme="system"
              enableSystem
              disableTransitionOnChange
            >
              <Header />
              {children}
            </ThemeProvider>
          </div>
        </div>
        <ProgressBar />
        <Toaster />
      </body>
    </html>
  );
}

} // namespace elizaos
