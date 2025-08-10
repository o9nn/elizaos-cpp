#!/bin/bash

# Test script for ElizaOS C++ Starter
echo "Testing ElizaOS C++ Starter..."

cd "$(dirname "$0")/build/cpp/eliza_starter"

# Test 1: Check if demo starts
echo "Test 1: Demo startup test"
timeout 3 ./eliza_starter_demo >/dev/null 2>&1
if [ $? -eq 124 ]; then
    echo "✅ Demo starts correctly and waits for input"
else
    echo "❌ Demo failed to start"
    exit 1
fi

# Test 2: Example program
echo "Test 2: Example program test"
./eliza_starter_example >/dev/null 2>&1
if [ $? -eq 0 ]; then
    echo "✅ Example program runs successfully"
else
    echo "❌ Example program failed"
    exit 1
fi

# Test 3: Library linking test
echo "Test 3: Library build test"
if [ -f "./libelizaos-eliza_starter.a" ]; then
    echo "✅ Static library built successfully"
else
    echo "❌ Static library not found"
    exit 1
fi

echo "All tests passed! ✅"
echo "ElizaOS C++ Starter is ready for use."