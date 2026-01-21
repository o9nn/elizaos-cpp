#include "registry.hpp"
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

/**
 * WindowedFile class for managing windowed file editing
 * Converted from Python windowed_file.py
 */

class TextNotFound extends Error {
  constructor(message: string) {
    super(message);
    this.name = 'TextNotFound';
  }

struct WindowedFileConfig {
    std::optional<bool> exitOnException;
    std::optional<std::string> path;
};

class WindowedFile {
  private _path: string;
  private _content: string[];
  private _firstLine: number;
  private _window: number;
  public offsetMultiplier: number;
  private exitOnException: boolean;
  private editHistory: string[][] = [];

  constructor(config: WindowedFileConfig = {}) {
    this.exitOnException = config.exitOnException ?? true;
    this.offsetMultiplier = 0.25;

    // Load file path from config or registry
    this._path = config.path || (registry.get('CURRENT_FILE') as string) || '';

    // Load window settings from registry
    this._firstLine = parseInt((registry.get('FIRST_LINE') as string) || '0', 10);
    this._window = parseInt((registry.get('WINDOW') as string) || '10', 10);

    // Load file content if path is set
    if (this._path) {
      this.loadFile();
    } else {
      this._content = [];
    }

    // Ensure firstLine is within bounds after loading content
    // For single-line files, ensure firstLine is 0
    if (this.nLines <= 1) {
      this._firstLine = 0;
    } else {
      this._firstLine = Math.min(this._firstLine, Math.max(0, this.nLines - 1));
    }
  }

    // Handle empty file or file with only newlines
    // If the file is empty or contains only newlines, ensure we have exactly one empty line

    // Print lines above if any

    // Print window content

    // Print lines below if any

    // Replace text

    // Update content

    // Save file

    // Adjust window position (keep it centered on the replacement)
    // Always adjust first line to keep window centered on the replacement
    // This ensures the window shows the replacement area

      // Position window so the target line is offset from top
      // Center the window on the target line

    // Add header

    // Add window content

    // Add footer

    // Save current state for undo

    // Save current state for undo

    // Convert to 0-based index

    // Insert the text at the specified line


} // namespace elizaos
