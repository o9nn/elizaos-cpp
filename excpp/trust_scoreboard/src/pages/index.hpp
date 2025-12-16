#include ".views/home/index.hpp"
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

// FILE: src/pages/index.tsx
import type { NextPage } from "next"






const Home: NextPage = () => {
  return (
    <div>
      <Head>
        <title>ai16z - Marc&apos;s Trust Leaderboard</title>
        <meta name="description" content="ai16z - Marc's Trust Leaderboard" />
      </Head>
      <NavBar>
        <Link
          href="https://discord.com/invite/ai16z"
          className="inline-flex items-center rounded-full bg-white/20 px-3.5 py-2 transition-all duration-300 hover:bg-white/30"
        >
          <span className="text-base font-semibold text-white">
            Join Discord
          </span>
        </Link>
      </NavBar>
      <main className="flex flex-1 justify-center">
        <ContentContainer>
          <HomeView />
        </ContentContainer>
      </main>
    </div>
  )
}

default Home

} // namespace elizaos
