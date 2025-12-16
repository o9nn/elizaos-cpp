#include "components/GameInterface.hpp"
#include "components/StartupFlow.hpp"
#include "components/TauriInitializer.hpp"
#include "utils/blockOldMessages.hpp"
#include "utils/debugWebSockets.hpp"
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
import './App.css';
;
;
;
;
;

// Start WebSocket debugging immediately
debugWebSockets();
blockOldMessages();


  }, [skipStartup]);

  const handleStartupComplete = () => {
    console.log('[App] Startup flow completed, transitioning to main interface');
    setStartupComplete(true);
  };

  if (!startupComplete) {
    console.log('[App] Rendering startup flow');
    return (
      <TauriInitializer>
        <div className="app">
          <StartupFlow onComplete={handleStartupComplete} />
        </div>
      </TauriInitializer>
    );
  }

  console.log('[App] Rendering main game interface');
  return (
    <TauriInitializer>
      <div className="app">
        <GameInterface />
      </div>
    </TauriInitializer>
  );
}

default App;

} // namespace elizaos
