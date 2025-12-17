#include "coindrop.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

void createCoin() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    if (coinCountRef.current >= MAX_COINS) {
        return nullptr;
    }

    const auto index = coinCountRef.current;

    // Set random position across the window-sized container
    // Keep it within the box bounds with some padding
    const auto x = (Math.random() - 0.5) * boxWidthRef.current * 0.9;
    const auto y = 2 + Math.random() * 2; // Start high up in the container;
    const auto z = (Math.random() - 0.5) * boxDepthRef.current * 0.9;

    // Set initial rotation
    const auto rotX = Math.random() * Math.PI;
    const auto rotZ = Math.random() * Math.PI;

    // Update the instance matrix
    dummyObject.position.set(x, y, z);
    dummyObject.rotation.set(rotX, 0, rotZ);
    dummyObject.updateMatrix();
    coinInstancedMesh.setMatrixAt(index, dummyObject.matrix);

    // Need to flag this for update
    coinInstancedMesh.instanceMatrix.needsUpdate = true;

    // Create physics body for the coin - bright gold for visibility
    const auto coinShape = new CANNON.Cylinder(;
    coinRadiusRef.current, // radiusTop;
    coinRadiusRef.current, // radiusBottom;
    coinThicknessRef.current, // height;
    12, // numSegments - reduced for physics performance;
    );

    const auto coinBody = new CANNON.Body({;
        mass: 10000, // Weight needs to be enough for good physics
        shape: coinShape,
        material: coinMaterial,
        allowSleep: true, // Critical for performance
        sleepSpeedLimit: 0.5, // Increased - only sleep when more settled
        sleepTimeLimit: 0.2, // Increased - stay awake a bit longer
        linearDamping: 0.2, // Reduced - less damping for more bouncy movement
        angularDamping: 0.2, // Reduced - maintain spin longer
        });

        // Set position and rotation
        coinBody.position.set(x, y, z);
        coinBody.quaternion.setFromEuler(rotX, 0, rotZ);

        // Add initial velocity for more natural falling
        coinBody.velocity.set(;
        (Math.random() - 0.5) * 8, // More horizontal movement;
        -20 - Math.random() * 30, // Much stronger downward velocity;
        (Math.random() - 0.5) * 8, // More depth movement;
        );

        // Add initial angular velocity for more dramatic spinning
        coinBody.angularVelocity.set(;
        (Math.random() - 0.5) * 25, // Much stronger spin;
        (Math.random() - 0.5) * 25, // Much stronger spin;
        (Math.random() - 0.5) * 25, // Much stronger spin;
        );

        // Store the instance index with the body for updates
        coinBody.userData = { index }

        // Add to world
        world.addBody(coinBody);

        // Store in our ref array
        coinBodiesRef.current.push(coinBody);

        // Increment the counter
        coinCountRef.current++;

        return coinBody;

}

void handleCollisions(CANNON::ContactEvent event) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    try {
        // In this event, we need to get the body that collided
        const auto bodyA = event.bodyA;
        const auto bodyB = event.bodyB;

        // Find which body is a coin (has mass > 0)
        const auto coinBody = bodyA.mass > 0 ? bodyA : bodyB.mass > 0 ? bodyB : nullptr;

        if (!coinBody) return;

        // Get normal vector from contact
        const auto normal = event.contact.ni;

        // Get impact velocity (how hard the collision was)
        const auto impactVelocity = event.contact.getImpactVelocityAlongNormal();

        // Add larger random spin on any collision, like in the dice code
        const auto randomX = (Math.random() - 0.5) * 15; // Increased from 5*intensity;
        const auto randomY = (Math.random() - 0.5) * 15;
        const auto randomZ = (Math.random() - 0.5) * 15;

        coinBody.angularVelocity.x += randomX;
        coinBody.angularVelocity.y += randomY;
        coinBody.angularVelocity.z += randomZ;

        // Add upward bounce if the coin is nearly settled
        // This matches the dice behavior which adds upward velocity
        if (coinBody.velocity.y < 0.5 && impactVelocity > 0.5) {
            // Add a small upward bounce for better movement
            coinBody.velocity.y += Math.random() * 3;

            // Also add a small random horizontal movement for more interesting collisions
            coinBody.velocity.x += (Math.random() - 0.5) * 2;
            coinBody.velocity.z += (Math.random() - 0.5) * 2;
        }

        // For coin-to-coin collisions, make them more energetic
        const auto otherBody = coinBody == bodyA ? bodyB : bodyA;
        if (otherBody.mass > 0) {
            // This is a coin-to-coin collision
            // Get the direction of impact
            const auto impulseDir = normal.scale(Math.min(impactVelocity * 0.2, 2));

            // Apply opposite impulses to make collision more dramatic
            otherBody.velocity.x += impulseDir.x;
            otherBody.velocity.z += impulseDir.z;
            // Add a bit of upward velocity for coin-to-coin collisions
            otherBody.velocity.y += Math.abs(impulseDir.y) + Math.random() * 1.5;
        }
        } catch (error) {
            std::cerr << "Error in collision handler:" << error << std::endl;
        }

}

void startCoinCreation() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    if (isCreatingCoinsRef.current) return;

    // Create an initial batch immediately so we can see something
    for (int i = 0; i < 50; i++) {
        createCoin();
    }

    isCreatingCoinsRef.current = true;

    // Calculate how many coins to create per second to reach MAX_COINS in 5 seconds
    const auto coinsPerSecond = MAX_COINS / 5;

    // How many batches per second (higher gives smoother distribution)
    const auto batchesPerSecond = 5; // Reduced for better logging;

    // How many coins in each batch
    const auto coinsPerBatch = Math.ceil(coinsPerSecond / batchesPerSecond);

    // Interval between batches in ms
    const auto batchInterval = 1000 / batchesPerSecond;

    // Set up interval for creating coins
    coinCreationIntervalRef.current = window.setInterval(() => {
        // Check if we've reached the max
        if (coinCountRef.current >= MAX_COINS) {
            if (coinCreationIntervalRef.current != null) {
                clearInterval(coinCreationIntervalRef.current);
                coinCreationIntervalRef.current = nullptr;
                isCreatingCoinsRef.current = false;
            }
            return;
        }

        // Create a batch of coins
        const auto remainingCoins = MAX_COINS - coinCountRef.current;
        const auto coinsToCreate = Math.min(coinsPerBatch, remainingCoins);

        for (int i = 0; i < coinsToCreate; i++) {
            createCoin();
        }
        }, batchInterval);

}

void animate(auto time = 0) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    requestAnimationFrame(animate);

    // Skip physics step if paused
    if (!isPausedRef.current) {
        // Calculate time delta and update physics with fixed timestep
        const auto deltaTime = Math.min((time - lastTime) / 1000, 0.1); // Cap at 0.1s to prevent large jumps;
        lastTime = time;

        // Step physics
        world.step(fixedTimeStep, deltaTime);
    }

    // Always update visuals even when physics is paused
    // Update coin positions and rotations
    for (int i = 0; i < coinBodiesRef.current.length; i++) {
        const auto coinBody = coinBodiesRef.current[i];
        const auto index = coinBody.userData.index;

        // Create a matrix from the physics body
        matrix.compose(;
        new THREE.Vector3(;
        coinBody.position.x,
        coinBody.position.y,
        coinBody.position.z,
        ),
        new THREE.Quaternion(;
        coinBody.quaternion.x,
        coinBody.quaternion.y,
        coinBody.quaternion.z,
        coinBody.quaternion.w,
        ),
        new THREE.Vector3(1, 1, 1),
        );

        // Update the instance
        if (coinInstanceRef.current) {
            coinInstanceRef.current.setMatrixAt(index, matrix);
        }
    }

    // Update the instance matrices
    if (coinInstanceRef.current) {
        coinInstanceRef.current.instanceMatrix.needsUpdate = true;
    }

    // Render the scene
    renderer.render(scene, camera);

}

} // namespace elizaos
