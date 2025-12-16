#include ".views/credits.hpp"
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

import type { NextPage } from "next";
;
;

const Credits: NextPage = () => {
  return (
    <div>
      <Head>
        <title>Buy API Credits - Trust Score</title>
        <meta
          name="description"
          content="Purchase API credits for Trust Score access"
        />
      </Head>
      <CreditsView />
    </div>
  );
};

default Credits;
} // namespace elizaos
