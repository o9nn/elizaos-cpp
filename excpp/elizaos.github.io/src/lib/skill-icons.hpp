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

;

const skillIcons = {
  // Area tags
  core: Code2,
  client: Users,
  plugin: Puzzle,
  docs: FileText,
  infra: Server,
  test: TestTube2,
  tests: TestTube2,
  security: Shield,
  ui: Layout,

  // Role tags
  architect: Building2,
  maintainer: Wrench,
  "feature-dev": Hammer,
  "bug-fix": Bug,
  "bug-fixer": Bug,
  "docs-writer": BookOpen,
  reviewer: Eye,
  devops: Cog,

  // Tech tags
  typescript: FileCode2,
  blockchain: Blocks,
  ai: Brain,
  db: Database,
  database: Database,
  api: Network,
} as const;

using SkillName = keyof typeof skillIcons;

} // namespace elizaos
