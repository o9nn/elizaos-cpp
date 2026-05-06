# ElizaOS C++ Packaging Guide

This document describes how to build and install ElizaOS C++ packages for various platforms.

## Table of Contents

- [Building Packages](#building-packages)
- [Debian/Ubuntu Installation](#debianubuntu-installation)
- [Red Hat/Fedora Installation](#red-hatfedora-installation)
- [Windows Installation](#windows-installation)
- [macOS Installation](#macos-installation)
- [From Source](#from-source)

## Building Packages

### Prerequisites

All platforms require:
- CMake 3.16 or higher
- C++17 compatible compiler
- libcurl development files

### Build All Packages

```bash
# Clone the repository
git clone https://github.com/o9nn/elizaos-cpp.git
cd elizaos-cpp

# Create build directory
mkdir build && cd build

# Configure with CMake
cmake -DCMAKE_BUILD_TYPE=Release ..

# Build the project
cmake --build . --config Release -j$(nproc)

# Create packages
cpack
```

This will generate platform-specific packages in the `build` directory.

### Build Specific Package Types

#### Debian Package (.deb)

```bash
cd build
cpack -G DEB
```

This creates a `.deb` package for Debian/Ubuntu systems.

#### RPM Package (.rpm)

```bash
cd build
cpack -G RPM
```

This creates a `.rpm` package for Red Hat/Fedora/CentOS systems.

#### Windows ZIP

```bash
cd build
cpack -G ZIP
```

#### Windows NSIS Installer

```bash
cd build
cpack -G NSIS
```

Requires NSIS (Nullsoft Scriptable Install System) to be installed.

#### macOS TGZ

```bash
cd build
cpack -G TGZ
```

## Debian/Ubuntu Installation

### From .deb Package

```bash
# Download the package
wget https://github.com/o9nn/elizaos-cpp/releases/latest/download/elizaos-cpp-1.0.0-Linux.deb

# Install
sudo dpkg -i elizaos-cpp-1.0.0-Linux.deb

# Install dependencies if needed
sudo apt-get install -f
```

### Verify Installation

```bash
# Check installed files
dpkg -L elizaos-cpp

# Test the main executable
eliza --version
```

### Uninstall

```bash
sudo apt-get remove elizaos-cpp
```

## Red Hat/Fedora Installation

### From .rpm Package

```bash
# Download the package
wget https://github.com/o9nn/elizaos-cpp/releases/latest/download/elizaos-cpp-1.0.0-Linux.rpm

# Install (Fedora/RHEL 8+)
sudo dnf install ./elizaos-cpp-1.0.0-Linux.rpm

# Or for older systems
sudo yum install ./elizaos-cpp-1.0.0-Linux.rpm
```

### Verify Installation

```bash
# Check installed files
rpm -ql elizaos-cpp

# Test the main executable
eliza --version
```

### Uninstall

```bash
sudo dnf remove elizaos-cpp
# or
sudo yum remove elizaos-cpp
```

## Windows Installation

### Using Chocolatey (Recommended)

```powershell
# Install Chocolatey if not already installed
# See https://chocolatey.org/install

# Install ElizaOS C++
choco install elizaos-cpp
```

### From ZIP Archive

1. Download `elizaos-cpp-1.0.0-win64.zip` from the releases page
2. Extract to a directory (e.g., `C:\Program Files\ElizaOS-CPP`)
3. Add the `bin` directory to your PATH:
   ```powershell
   $env:Path += ";C:\Program Files\ElizaOS-CPP\bin"
   ```
4. Make the change permanent through System Properties → Environment Variables

### From NSIS Installer

1. Download `elizaos-cpp-1.0.0-win64.exe` from the releases page
2. Run the installer
3. Follow the installation wizard
4. The installer will automatically add ElizaOS C++ to your PATH

### Verify Installation

```powershell
eliza --version
```

### Uninstall

#### Chocolatey
```powershell
choco uninstall elizaos-cpp
```

#### NSIS Installer
Use "Add or Remove Programs" in Windows Settings

## macOS Installation

### Using Homebrew (Coming Soon)

```bash
brew tap elizaos/tap
brew install elizaos-cpp
```

### From TGZ Archive

```bash
# Download the package
curl -LO https://github.com/o9nn/elizaos-cpp/releases/latest/download/elizaos-cpp-1.0.0-Darwin.tar.gz

# Extract
tar -xzf elizaos-cpp-1.0.0-Darwin.tar.gz

# Install
cd elizaos-cpp-1.0.0-Darwin
sudo cp -r bin/* /usr/local/bin/
sudo cp -r lib/* /usr/local/lib/
sudo cp -r include/* /usr/local/include/
```

### Verify Installation

```bash
eliza --version
```

## From Source

### Linux/macOS

```bash
# Install dependencies
# Ubuntu/Debian:
sudo apt-get install cmake g++ libcurl4-openssl-dev

# Fedora/RHEL:
sudo dnf install cmake gcc-c++ libcurl-devel

# macOS:
brew install cmake curl

# Clone and build
git clone https://github.com/o9nn/elizaos-cpp.git
cd elizaos-cpp
mkdir build && cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
make -j$(nproc)
sudo make install
```

### Windows

```powershell
# Install Visual Studio 2019 or later with C++ support
# Install CMake from https://cmake.org/download/

# Clone and build
git clone https://github.com/o9nn/elizaos-cpp.git
cd elizaos-cpp
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
cmake --build . --config Release
cmake --install . --prefix "C:\Program Files\ElizaOS-CPP"
```

## Package Contents

All packages include:

### Executables (in `/usr/bin` or `bin/`)
- `eliza` - Main ElizaOS CLI
- `shell_demo` - Shell integration demo
- `agentshell_test` - Agent shell test utility
- Various demo and test executables

### Libraries (in `/usr/lib` or `lib/`)
- `libelizaos-core.a` - Core functionality
- `libelizaos-agentloop.a` - Agent loop system
- `libelizaos-agentmemory.a` - Memory management
- `libelizaos-agentcomms.a` - Communication system
- `libelizaos-agentlogger.a` - Logging system
- And 40+ additional component libraries

### Headers (in `/usr/include/elizaos` or `include/elizaos`)
- Complete C++ header files for all modules
- Enables development of custom agents and extensions

### Documentation (in `/usr/share/doc/elizaos-cpp` or `docs/`)
- README.md - Project overview
- LICENSE - License information
- Additional documentation files

## Troubleshooting

### Missing Dependencies

**Linux:**
```bash
# Ubuntu/Debian
sudo apt-get install libcurl4 libstdc++6

# Fedora/RHEL
sudo dnf install libcurl glibc
```

**macOS:**
```bash
brew install curl
```

**Windows:**
Ensure Visual C++ Redistributable is installed from Microsoft.

### Permission Errors

**Linux/macOS:**
```bash
# Use sudo for installation
sudo dpkg -i package.deb
sudo rpm -i package.rpm
```

**Windows:**
Run installer or PowerShell as Administrator.

### PATH Issues

**Linux/macOS:**
```bash
# Add to ~/.bashrc or ~/.zshrc
export PATH="/usr/local/bin:$PATH"
```

**Windows:**
Add installation directory to System PATH through Environment Variables.

## CI/CD Integration

GitHub Actions automatically builds packages for all platforms on every release. See `.github/workflows/packaging.yml` for details.

### Automated Builds

- **Debian packages**: Built on Ubuntu 20.04 and 22.04
- **RPM packages**: Built on Ubuntu with RPM tools
- **Windows packages**: Built on Windows Server with MSVC
- **macOS packages**: Built on macOS latest

### Release Process

1. Tag a new version: `git tag v1.0.0`
2. Push the tag: `git push origin v1.0.0`
3. GitHub Actions automatically builds and publishes packages
4. Packages are attached to the GitHub Release

## Support

For issues or questions:
- GitHub Issues: https://github.com/o9nn/elizaos-cpp/issues
- Documentation: https://github.com/o9nn/elizaos-cpp/blob/main/README.md
