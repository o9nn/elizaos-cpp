#include "agent-log-viewer.hpp"
#include "ui/button.hpp"
#include "ui/label.hpp"
#include "ui/select.hpp"
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
;
;
;
;

default  onValueChange={setLogLevel}>
              <SelectTrigger id="log-level" className="w-32">
                <SelectValue placeholder="Select level" />
              </SelectTrigger>
              <SelectContent>
                <SelectItem value="all">All</SelectItem>
                <SelectItem value="info">Info</SelectItem>
                <SelectItem value="warn">Warning</SelectItem>
                <SelectItem value="error">Error</SelectItem>
                <SelectItem value="debug">Debug</SelectItem>
              </SelectContent>
            </Select>
          </div>
          <Button variant="outline" onClick={() => window.location.reload()}>
            Refresh
          </Button>
        </div>
      </div>

      <div className="flex-1 border rounded-lg overflow-hidden">
        <AgentLogViewer level={logLevel as any} />
      </div>
    </div>
  );
}

} // namespace elizaos
