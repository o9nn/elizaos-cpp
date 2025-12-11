import type { Metadata, Viewport } from "next";

import { siteConfig } from "@/app/constants";
import "@/app/globals.css";
import { ProgressBar } from "@/app/progress-bar";
import { Toaster } from "@/app/toaster";
import { Header } from "@/components/header";
import { Providers } from "@/components/providers";
import { XShareResume } from "@/components/x-share-resume";
import { IBM_Plex_Mono } from "next/font/google";

export const viewport: Viewport = {
  width: "device-width",
  initialScale: 1,
  themeColor: "white",
};

const ibmPlexMono = IBM_Plex_Mono({
  subsets: ["latin"],
  weight: ["400", "500", "600", "700"],
  variable: "--font-ibm-plex-mono",
});

const appUrl = process.env.NEXT_PUBLIC_URL || "http://localhost:4444";

// Farcaster Mini App frame configuration
const frame = {
  version: "1", // Changed from "next" to match manifest
  imageUrl: `${appUrl}/opengraph-image.png?v=1`, // Added ?v=1 to match manifest
  button: {
    title: "AI OTC Trading", // Changed to match manifest buttonTitle
    action: {
      type: "launch_frame",
      name: "Eliza OTC Desk",
      url: appUrl,
      splashImageUrl: `${appUrl}/splash.png`,
      splashBackgroundColor: "#000000",
    },
  },
};

export const metadata: Metadata = {
  metadataBase: new URL(siteConfig.url),
  title: `${siteConfig.name} - AI-Powered OTC Trading Desk`,
  description:
    "Trade tokens OTC with Eliza AI agent. Negotiate deals on EVM and Solana networks with automated pricing and smart contracts.",
  openGraph: {
    siteName: siteConfig.name,
    title: "Eliza OTC Desk - AI-Powered Token Trading",
    description:
      "Trade tokens OTC with Eliza AI agent. Negotiate deals on EVM and Solana networks.",
    images: [siteConfig.ogImage],
    type: "website",
    url: siteConfig.url,
    locale: "en_US",
  },
  icons: siteConfig.icons,
  twitter: {
    card: "summary_large_image",
    site: siteConfig.name,
    title: "Eliza OTC Desk - AI-Powered Token Trading",
    description:
      "Trade tokens OTC with Eliza AI agent. Negotiate deals on EVM and Solana networks.",
    images: [siteConfig.ogImage],
    creator: siteConfig.creator,
  },
  other: {
    "fc:frame": JSON.stringify(frame),
    // Optionally add fc:miniapp for new format (recommended)
    "fc:miniapp": JSON.stringify({
      ...frame,
      button: {
        ...frame.button,
        action: {
          ...frame.button.action,
          type: "launch_miniapp", // New format type
        },
      },
    }),
  },
};

import { DeploymentValidator } from "@/components/deployment-validator";

export default function RootLayout({
  children,
}: Readonly<{
  children: React.ReactNode;
}>) {
  return (
    <html suppressHydrationWarning lang="en" className={ibmPlexMono.className}>
      <body className="min-h-dvh antialiased bg-white text-black scheme-light dark:bg-surface dark:text-white dark:scheme-dark selection:!bg-[#fff0dd] dark:selection:!bg-[#3d2b15] overscroll-none">
        <DeploymentValidator />
        <Providers>
          <div className="flex h-dvh w-full flex-col overflow-hidden">
            <Header />
            <main className="flex-1 flex flex-col overflow-hidden">
              {children}
            </main>
          </div>
        </Providers>
        <XShareResume />
        <ProgressBar />
        <Toaster />
      </body>
    </html>
  );
}
