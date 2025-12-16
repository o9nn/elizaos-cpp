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

/**
 * Sends a standardized error response
 */
const sendError = (
  res: express.Response,
  status: number,
  code: string,
  message: string,
  details?: string
) => {
  res.status(status).json({
    success: false,
    error: {
      code,
      message,
      ...(details && { details }),
    },
  });
};

/**
 * Sends a standardized success response
 */
const sendSuccess = (res: express.Response, data: any, status = 200) => {
  res.status(status).json({
    success: true,
    data,
  });
};

} // namespace elizaos
