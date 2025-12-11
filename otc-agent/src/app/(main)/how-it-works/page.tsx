"use client";

import dynamic from "next/dynamic";

// Disable SSR for this page to avoid hydration issues with wallet contexts
const HowItWorksContent = dynamic(() => import("./HowItWorksContent"), {
  ssr: false,
  loading: () => (
    <div className="flex-1 flex items-center justify-center">Loading...</div>
  ),
});

export default function Page() {
  return <HowItWorksContent />;
}
