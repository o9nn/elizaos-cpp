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

let neynarClient: any = null;

async );
    neynarClient = new NeynarAPIClient(config);
  }
  return neynarClient;
}

async  = await request.json();

    if (!fid || !title || !body) {
      return NextResponse.json(
        { error: "Missing required parameters: fid, title, body" },
        { status: 400 },
      );
    }

    // Dev mode simulation when API key is not configured
    if (!process.env.NEYNAR_API_KEY) {
      console.log("[Dev] Simulating notification:", { fid, title, body });
      return NextResponse.json({ state: "success", simulated: true });
    }

    const client = await getNeynarClient();
    const result = await client.sendFrameNotification({
      fid: Number(fid),
      title,
      body,
    });

    if (result.success) {
      return NextResponse.json({ state: "success" });
    }

    return NextResponse.json({
      state: result.reason || "error",
      message: result.message,
    });
  } catch (error) {
    console.error("Error sending notification:", error);
    return NextResponse.json(
      { error: "Failed to send notification" },
      { status: 500 },
    );
  }
}

} // namespace elizaos
