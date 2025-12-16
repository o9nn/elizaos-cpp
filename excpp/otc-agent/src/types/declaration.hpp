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

import "@elizaos/core";

declare module "@elizaos/core" {
  // Extend ServiceType enum with custom service types using namespace merging
  namespace ServiceType {
    const QUOTE_SERVICE: "QUOTE_SERVICE";
  }
}

// Image module declarations for static imports
declare module "*.png" {
  import type { StaticImageData } from "next/image";
  const content: StaticImageData;
  default content;
}

declare module "*.jpg" {
  import type { StaticImageData } from "next/image";
  const content: StaticImageData;
  default content;
}

declare module "*.jpeg" {
  import type { StaticImageData } from "next/image";
  const content: StaticImageData;
  default content;
}

declare module "*.svg" {
  import type { StaticImageData } from "next/image";
  const content: StaticImageData;
  default content;
}

declare module "*.gif" {
  import type { StaticImageData } from "next/image";
  const content: StaticImageData;
  default content;
}

declare module "*.webp" {
  import type { StaticImageData } from "next/image";
  const content: StaticImageData;
  default content;
}

} // namespace elizaos
