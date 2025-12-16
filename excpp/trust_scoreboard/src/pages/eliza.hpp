#include ".views/eliza.hpp"
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

const Eliza: NextPage = (props) => {
  return (
    <div>
      <Head>
        <title>Deploy Your Own Eliza Today!</title>
        <meta
          name="description"
          content="Checkout and deploy your own Eliza today!"
        />
      </Head>
      <ElizaView />
    </div>
  );
};

default Eliza;
} // namespace elizaos
