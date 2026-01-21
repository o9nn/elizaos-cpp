#include "audio-capture-stream.hpp"
#include "audio-capture.hpp"
#include "elizaos/core.hpp"
#include "entity-tracker.hpp"
#include "face-recognition.hpp"
#include "florence2-model.hpp"
#include "ocr-service.hpp"
#include "screen-capture.hpp"
#include "types.hpp"
#include "vision-models.hpp"
#include "vision-worker-manager.hpp"
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

// Vision service for camera integration and scene analysis

struct CameraDevice {
    std::string id;
    std::string name;
    () => Promise<Buffer> capture;
};

class VisionService extends Service {
  static override serviceType: ServiceTypeName = VisionServiceType.VISION;
  override capabilityDescription =
    'Provides visual perception through camera integration and scene analysis.';

  private visionConfig: VisionConfig;
  private camera: CameraDevice | null = null;
  private lastFrame: VisionFrame | null = null;
  private lastSceneDescription: SceneDescription | null = null;
  private frameProcessingInterval: NodeJS.Timeout | null = null;
  private screenProcessingInterval: NodeJS.Timeout | null = null;
  private isProcessing = false;
  private isProcessingScreen = false;
  private visionModels: VisionModels;
  private faceRecognition: FaceRecognition;
  private entityTracker: EntityTracker;
  private audioCapture: AudioCaptureService | null = null;
  private streamingAudioCapture: StreamingAudioCaptureService | null = null;

  // Screen vision components
  private screenCapture: ScreenCaptureService;
  private florence2: Florence2Model;
  private ocrService: OCRService;
  private lastScreenCapture: ScreenCapture | null = null;
  private lastEnhancedScene: EnhancedSceneDescription | null = null;

  // Worker manager for high-FPS processing
  private workerManager: VisionWorkerManager | null = null;

  // Add tracking for last update times
  private lastTfUpdateTime = 0;
  private lastVlmUpdateTime = 0;
  private lastTfDescription = '';

  // Default configuration
  private readonly DEFAULT_CONFIG: VisionConfig = {
    pixelChangeThreshold: 50, // 50% change required for VLM update
    updateInterval: 100, // Process frames every 100ms
    enablePoseDetection: false,
    enableObjectDetection: false,
    tfUpdateInterval: 1000, // TensorFlow update every 1 second
    vlmUpdateInterval: 10000, // VLM update every 10 seconds
    tfChangeThreshold: 10, // 10% change triggers TF update
    vlmChangeThreshold: 50, // 50% change triggers VLM update
    visionMode: VisionMode.CAMERA, // Default to camera only
    screenCaptureInterval: 2000, // Screen capture every 2 seconds
    tileSize: 256,
    tileProcessingOrder: 'priority',
    ocrEnabled: true,
    florence2Enabled: true,
  };

  // External media stream handling

    // Load configuration from runtime settings

    // Initialize vision models

    // Initialize face recognition

    // Initialize entity tracker

    // Initialize screen capture

    // Initialize Florence-2

    // Initialize OCR service

    // Check if individual components are enabled

    // Determine vision mode based on enabled components

        // Check if imagesnap is installed
        await execAsync('which imagesnap');
        // Check if fswebcam is installed
        await execAsync('which fswebcam');
        // Check if ffmpeg is available
        await execAsync('where ffmpeg');
      // Tool not found

      // Initialize vision models if enabled

          // Try to initialize TensorFlow models first
          // Fall back to enhanced heuristics

      // Initialize screen vision if enabled

      // Initialize camera if enabled

      // Initialize audio capture if enabled

      // Start processing based on mode

      // Check if we should use worker threads for high-FPS processing

        // Initialize worker manager for high-FPS processing
        // Initialize standard components
        // Initialize Florence-2 if enabled

        // Initialize OCR if enabled

      // Get screen info

    // Check if camera tools are available

    // Find and connect to camera

        // Use new streaming audio with VAD

        // Set up event listeners

          // Store the transcription in memory for context

        // Use original batch audio capture

      // Continue without audio

      // Store transcription in the current scene description

      // You could also create a memory here if needed

    // Start camera processing if enabled

    // Start screen processing if enabled

      // Capture frame from camera

      // Skip if no data

      // Convert to standardized format

      // Validate frame before processing

      // Check if scene has changed significantly

      // Update scene description if change is significant or enough time has passed
      // Always call updateSceneDescription - it will decide what to update based on thresholds

    // Validate input data

    // Use sharp to ensure consistent format

    // Validate metadata

      // Convert frame to base64 for VLM

      // Determine if we should update VLM description

        // Use VLM to describe the scene

      // Determine if we should update TensorFlow detections

        // Use advanced computer vision if enabled

        // If no people detected via pose but objects detected, check for person objects
        // Reuse last detection results if not updating
        // Fall back to motion-based detection

      // Face recognition and entity tracking

          // Validate frame data

          // Detect faces in the frame

          // Match faces to people based on bounding box overlap

            // Find the person this face belongs to

                // 50% overlap threshold
                // Recognize or register the face

                  // Register new face

      // Update entity tracker

      // Create scene description

      // Enhanced logging

          // Group objects by type for summary

      // Convert base64 image URL to buffer for Florence-2

        // Use Florence-2 for all image descriptions

      // Fallback to runtime model if Florence-2 is not available

          // Check if we got the unhelpful default response

      // If we got the unhelpful response or an error, provide a basic description based on detected objects

      // Final fallback

    // Divide frame into blocks and detect motion regions
      // Overlap blocks

        // Check motion in this block
          // Sample every other pixel for speed

        // If significant motion in block, consider it an object
          // 30% of sampled pixels show motion

    // Merge adjacent motion blocks into larger objects

    // Filter out very small objects (likely noise)

      // Find all adjacent objects

          // Check if adjacent to any object in cluster

      // Merge cluster into single object

    // Improved classification heuristics

      // Simple heuristic: if height > width * 1.5, likely standing
      // if width > height, likely sitting/lying

      // Estimate facing based on motion direction (simplified)
        // In Phase 1, we'll just use 'unknown' or random assignment
        // Phase 2 will implement proper pose detection

      // Capture screen

      // Process active tile

      // Update enhanced scene description

      // Run Florence-2 analysis if enabled

      // Run OCR if enabled

      // Extract objects from Florence-2 results

    // Aggregate OCR from all processed tiles

    // Generate grid summary

    // Detect focused application (heuristic based on UI elements)

  // Public API methods

    // If worker manager is available, use its high-FPS data

    // Otherwise fall back to standard processing

    // Stop current processing

    // Update configuration

    // Reinitialize based on new mode

    // Initialize components for new mode

    // Start processing for new mode

  // Helper methods for face recognition

  // Public methods for entity tracking

    // Dispose of models

      // Stop current processing

      // Re-parse configuration from current runtime settings

      // If vision is completely disabled, stop everything

      // Reinitialize based on new settings

    // Stop audio services

    // Reset camera

    // Cleanup existing services first

    // Initialize screen vision if enabled

    // Initialize camera if enabled

    // Initialize audio capture if enabled

    // Start processing for new mode

      // Get list of available cameras

      // If camera name is specified, try to find it

      // Use first available camera

        // macOS: Use system_profiler

        // Linux: Use v4l2

        // Windows: Use PowerShell

            // macOS: Use imagesnap
              await execAsync(`imagesnap -d "${info.name}" "${tempFile}"`);
            // Linux: Use fswebcam
            // Windows: Use DirectShow via ffmpeg

          // Read the captured image

          // Clean up temp file

          // Clean up temp file on error

  // External media stream handling

      // Process video frame
        // Store in external streams

        // Process frame through vision pipeline
      // Queue audio for transcription

        // Keep last 10 seconds of audio (assuming ~4KB chunks at 10fps = ~40KB/s)

      // Queue for transcription

      // Process audio transcription queue

        // Use sharp to decode JPEG to raw pixels

      // For other encodings, assume raw data

    // Similar to processFrameData but for external streams

    // Check if scene has changed significantly

    // Update scene description if significant change
      // Get current description synchronously

      // Update internal scene description

    // Update the last frame reference

    // Process in batches to avoid overwhelming the transcription service

    // Combine audio chunks

      // Transcribe using Whisper via Ollama

        // Store transcription in scene description for context
        // Update scene description with audio context

      // Check if Ollama is available and has Whisper model

      // Convert PCM to WAV format for Whisper

      // Send to Ollama for transcription

      // Fallback to mock transcription for testing

    // Simple WAV header for 16-bit PCM, 44100 Hz, mono

    // RIFF header
    // fmt sub-chunk
    // data sub-chunk

  // Get all active external streams

  // Clear external stream

} // namespace elizaos
