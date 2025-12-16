#include ".components/trades/Trades.hpp"
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

import type { NextPage } from "next"






const TradesPage: NextPage = () => {
  return (
    <div>
      <Head>
        <title>ai16z - Marc&apos;s Trades</title>
        <meta
          name="description"
          content="View Marc's trading activity and performance"
        />
      </Head>
      <NavBar>
        <Link
          href="https://solscan.io/account/GypeM9BqKeKGJGTnPxTf1PdVa3UC2LkiYnvvW8CJSNj2#transfers"
          className="inline-flex items-center rounded-full bg-white/20 px-3.5 py-2 transition-all duration-300 hover:bg-white/30"
        >
          <span className="text-base font-semibold text-white">
            View Wallet
          </span>
        </Link>
      </NavBar>
      <main className="flex flex-1 justify-center">
        <ContentContainer>
          <Trades />
        </ContentContainer>
      </main>
    </div>
  )
}

// Add this to explicitly tell Next.js this is not a dynamic page
const getStaticProps = () => {
  return {
    props: {},
  }
}

default TradesPage

} // namespace elizaos
