#include ".views/profile.hpp"
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

const Profile: NextPage = (props) => {
  return (
    <div>
      <Head>
        <title>Profile</title>
        <meta
          name="description"
          content="Profile"
        />
      </Head>
      <ProfileView />
    </div>
  );
};

default Profile;

} // namespace elizaos
