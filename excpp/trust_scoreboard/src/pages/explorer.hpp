#include ".views/explorer.hpp"
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

const Explorer: NextPage = (props) => {
  return (
    <div>
      <Head>
        <title>Explorer</title>
        <meta
          name="description"
          content="Explorer"
        />
      </Head>
      <ExplorerView />
    </div>
  );
};

// Add this to explicitly tell Next.js this is not a dynamic page
const getStaticProps = () => {
    return {
      props: {}
    };
  };

default Explorer;

} // namespace elizaos
