#include "providers.hpp"
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




import type { Metadata } from 'next'
import './globals.css'

const metadata: Metadata = {
  title: "Eliza's World (a16z)",
  description: "Live 'Observatory' dashboard showcasing the emergent network of autonomous AI agents born from Eliza's core.",
}

default : {
  children: React.ReactNode
}) {
  return (
    <html lang="en">
      <body>
        <Analytics />
        <Providers>
          <NavHeader />
          {children}
        </Providers>
      </body>
    </html>
  )
} 
} // namespace elizaos
