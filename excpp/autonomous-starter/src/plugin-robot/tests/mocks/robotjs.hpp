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

// Mock screen capture data
const mockScreenCapture = {
  image: Buffer.from("mock-screenshot-data"),
  width: 1920,
  height: 1080,
  byteWidth: 7680,
  bitsPerPixel: 32,
  bytesPerPixel: 4,
};

// Mock robotjs module
default {
  getScreenSize: vi.fn(() => ({ width: 1920, height: 1080 })),
  screen: {
    capture: vi.fn(() => mockScreenCapture),
  },
  moveMouse: vi.fn(),
  mouseClick: vi.fn(),
  typeString: vi.fn(),
  keyTap: vi.fn(),
  keyToggle: vi.fn(),
  getMousePos: vi.fn(() => ({ x: 100, y: 100 })),
  getPixelColor: vi.fn(() => "ffffff"),
  setMouseDelay: vi.fn(),
  setKeyboardDelay: vi.fn(),
};

// Export named functions for compatibility
const getScreenSize = vi.fn(() => ({ width: 1920, height: 1080 }));
const screen = {
  capture: vi.fn(() => mockScreenCapture),
};
const moveMouse = vi.fn();
const mouseClick = vi.fn();
const typeString = vi.fn();
const keyTap = vi.fn();
const keyToggle = vi.fn();
const getMousePos = vi.fn(() => ({ x: 100, y: 100 }));
const getPixelColor = vi.fn(() => "ffffff");
const setMouseDelay = vi.fn();
const setKeyboardDelay = vi.fn();

} // namespace elizaos
