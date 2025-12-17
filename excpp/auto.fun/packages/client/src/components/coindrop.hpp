#pragma once
#include <algorithm>
#include <any>
#include <cstdint>
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



// @ts-ignore

// Add TypeScript declaration for CANNON to fix the errors
  struct Body {
    std::any userData;
};

  struct Vec3 {
};

  struct Solver {
    double iterations;
};

  struct ContactEquation {
    CANNON::Vec3 ni;
};

  struct ContactEvent {
    CANNON::Body bodyA;
    CANNON::Body bodyB;
    ContactEquation contact;
};

// Declare the debugDropCoins function on the window object
  struct Window {
};

// Constants for physics - match the same constants from header

struct CoinDropProps {
    std::optional<std::string> imageUrl;
};

// Create fireworks-like confetti effect

  // Helper function to create a specific firework burst
    // Define different types of confetti bursts

    // Fire the specified burst type

  // Fire random bursts on an interval

    // Stop launching confetti after 5 seconds
      return clearInterval(burstInterval);

    // Launch multiple bursts per interval

      // Random position
      // Keep y position in upper half of screen for better effect

      // Randomly choose burst type

      // Fire the burst

  // Special "finale" effect at the end
    // Create a grand finale with multiple bursts at once

  // Return cleanup function

// Expose the function globally

  // Add new function to handle cancellation animation

    // Stop any ongoing coin creation

    // Store the world reference

    // Temporarily remove walls from physics world

    // Temporarily disable gravity and collisions

    // Apply initial velocities with a natural curve
      // Disable collision response to prevent coins from getting stuck

      // Keep the current angular velocity for spinning

      // Calculate initial position-based velocities for a natural curve
      // const x = coinBody.position.x;
      // const z = coinBody.position.z;

      // Base velocity components - increased values

      // Calculate initial velocities with a natural curve
      // More horizontal movement for coins higher up

      // Set initial velocity with natural curve

      // Apply an additional downward force to ensure coins keep moving

      // Restore the angular velocity to maintain spin

    // Gradually increase gravity over time for a natural acceleration

    // Restore walls and call onCancel after the animation is complete (2.5 seconds)

  // Expose the flush function globally

  // Effect to set container height to document height
      // Ensure we are running in a browser environment

    // Initial calculation

    // Update on resize

    // Optional: More robust update using ResizeObserver if needed
    // const resizeObserver = new ResizeObserver(updateHeight);
    // resizeObserver.observe(document.documentElement);

    // Cleanup
      // if (resizeObserver) {
      //   resizeObserver.disconnect();
      // }

    // Get container dimensions - use full viewport

    // Set up box dimensions based on screen size - EXACT WINDOW SIZE

    // Set coin size based on window size

    // Scene setup

    // Physics world - updated to be more similar to dice physics

    // Setup materials

    // Define contact behaviors - make more bouncy like the dice in header

    // Camera setup - ORTHOGRAPHIC and OVERHEAD with window-sized frustum
    // Position camera directly overhead

    // Renderer setup

    // Add renderer to DOM

    // Lighting - optimized for overhead view
    // rotate the ambient light down to the floor

    // look at the center of the scene
    // update matrix

    // Floor - invisible but matching window size exactly

    // Floor physics body

    // Walls - invisible, match window edges exactly

    // Back wall (top of screen)

    // Back wall physics body

    // Front wall (bottom of screen)

    // Front wall physics body

    // Left wall

    // Left wall physics body

    // Right wall

    // Right wall physics body

    // Load textures for coins
      // Start coin creation once texture is loaded

      // Removed: createConfettiFireworks() - this will now be called externally

    // Create coin geometry using the radius from ref

    // create a roughness map from sobel edge detection
      // Create a canvas to process the texture

      // If we can't get a 2d context, return null

      // Create an image to draw the texture

      // Return a new promise that resolves with the roughness texture
          // Set canvas size to match image

          // Draw the image to canvas

          // Get image data for processing

          // Create output array for the edge detection result

          // Sobel kernels for edge detection

          // Apply Sobel filter

              // Apply convolution

                  // Use average of RGB for grayscale

              // Calculate gradient magnitude

              // Calculate index in the output array

              // Normalize and set edge intensity for roughness (invert for proper roughness effect)

              // Set RGB to edge value for roughness map

          // Create new image data with edge detection results

          // Create a Three.js texture from the canvas

          // Resolve the promise with the generated texture

        // Set the image source to the texture's image
          // Set colorspace to SRGB before loading

          // If texture image isn't available, resolve with null

    // Create materials for the coin - gold color with the logo

    // Create a fake normal map from the coin texture
      // Create a canvas to process the texture

      // If we can't get a 2d context, return null

      // Create an image to draw the texture

      // Return a new promise that resolves with the normal texture
          // Set canvas size to match image

          // Draw the image to canvas

          // Get image data for processing

          // Create output array for the normal map

          // Sobel kernels for edge detection

          // Apply Sobel filter to generate normal map

              // Apply convolution

                  // Use average of RGB for grayscale

              // Calculate index in the output array

              // Normal maps store XYZ normals in RGB channels
              // Convert gradients to normal map by normalizing
              // For normal maps: R=X, G=Y, B=Z
              // Scale X and Y (red and green) by strength factor

              // Calculate normal vector components
              // R: X normal component (128 is neutral/flat)
              // G: Y normal component (128 is neutral/flat)
              // B: Z normal component (always positive for coin surface, higher value = more raised)
              // Alpha channel

          // Create new image data with normal map

          // Create a Three.js texture from the canvas

          // Resolve the promise with the generated texture

        // Set the image source to the texture's image
          // If texture image isn't available, resolve with null

    // Apply roughness map after texture loads

    // Apply normal map after texture loads

    // Create instanced mesh for efficient rendering
    // Disable frustum culling so offscreen coins are still rendered

    // Store the instanced mesh in the ref

    // Create a temporary matrix and dummy object for updates

    // Pre-position all instances far off screen initially

    // Function to create and drop a single coin
    void createCoin();

    // Collision handler for more realistic physics - similar to dice in frontpage-header
    void handleCollisions(CANNON::ContactEvent event);

    // Add collision event listener

    // Function to create coins gradually over 5 seconds
    void startCoinCreation();

    // Animation loop

    // Add to the animation loop to log active coins periodically
    void animate(auto time);

    // Start animation

    // Handle window resize - adjust box dimensions to match new window size

      // Update orthographic camera to match new dimensions

      // Update renderer size

      // Update box dimensions to match new window size

      // Update coin size to scale with new window size

    // Add resize listener

    // Cleanup on unmount
      // Clear any ongoing coin creation

      // Clean up THREE.js resources


} // namespace elizaos
