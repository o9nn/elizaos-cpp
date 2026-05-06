"use client";

import Card from "@/components/Card";
import Image from "next/image";
import { useRouter } from "next/navigation";
import { useMultiWallet } from "@/components/multiwallet";
import { useCallback } from "react";

export default function HowItWorksContent() {
  const router = useRouter();
  useMultiWallet(); // Hook used for side effects only

  // Simple wallet connect - just use Privy login
  const handleOpenConsignmentForm = useCallback(() => {
    // navigate to /consign
    router.push("/consign");
  }, [router]);

  const handleOpenTradingDesk = useCallback(() => {
    router.push("/");
  }, [router]);

  const handleViewDeals = useCallback(() => {
    router.push("/my-deals");
  }, [router]);

  return (
    <div className="relative flex flex-col px-4 sm:px-6 py-10 flex-1 overflow-y-auto">
      {/* Background with gradient overlay */}
      <div className="absolute inset-0">
        {/* Background */}
        <div className="absolute inset-0 bg-surface" />

        {/* Background image positioned on the right */}
        <div className="absolute inset-0 flex justify-end">
          <Image
            src="/how-it-works/how-it-works-bg.png"
            alt="How it works background"
            width={1200}
            height={900}
            className="object-cover h-auto"
            priority
          />
        </div>

        {/* Gradient overlay - black on left fading to transparent on right */}
        <div
          className="absolute inset-0 bg-gradient-to-r from-black via-black to-transparent"
          style={{
            background:
              "linear-gradient(to right, rgba(16, 16, 16, 1) 0%, #000000 55%, rgba(0,0,0,0.3) 75%)",
          }}
        />
      </div>

      {/* Content */}
      <div className="z-10 flex flex-col items-start justify-center h-full">
        <div className="flex items-center mb-10">
          <Image
            src="/how-it-works/text.svg"
            alt="How it works text"
            height={220}
            width={950}
            draggable={false}
            className="select-none w-auto"
          />
        </div>

        {/* New heading text */}
        <div className="flex mb-12 gap-4 mt-8 place-self-center lg:place-self-start flex-col lg:flex-row">
          <Card
            number="1"
            title="List A Token"
            description="Consign your tokens at a discount with a lockup period."
            button={"List A Token"}
            onClick={handleOpenConsignmentForm}
          />
          <Card
            number="2"
            title="Negotiate"
            description="Make an offer, choose a discount and lockup."
            button="Open Trading Desk"
            onClick={handleOpenTradingDesk}
          />
          <Card
            number="3"
            title="Buy and hold"
            description="Your tokens are available after the lockup period ends."
            button="View My Deals"
            onClick={handleViewDeals}
          />
        </div>
      </div>
      <div
        className="absolute bottom-0 right-0 w-full h-2/3 z-20 pointer-events-none"
        style={{
          background: `radial-gradient(ellipse 80% 100% at 100% 100%, var(--brand-primary) 0%, rgba(247, 91, 30, 0.6) 0%, rgba(247, 91, 30, 0.3) 0%, transparent 75%)`,
          filter: "blur(2px)",
        }}
      />
    </div>
  );
}
