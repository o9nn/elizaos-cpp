#include "NavBar.module.css.hpp"
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









struct NavBarProps {
    std::optional<React.ReactNode> children;
};


const NavBar: FC<NavBarProps> = ({ children }) => {
  const { data: session } = useSession()
  const [searchQuery, setSearchQuery] = useState("")

  const handleSearch = (e: React.FormEvent) => {
    e.preventDefault()
    // Add search functionality
  }

  return (
    <nav className="sticky top-0 z-[100] h-[66px] w-full bg-black/25 px-6 py-3.5 backdrop-blur-[69px]">
      <div className="flex h-full items-center justify-end">
        {children}
      </div>
    </nav>
  )
}

default NavBar

} // namespace elizaos
