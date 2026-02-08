# NeonAegis - Circular Defense Shooter

A high-intensity arcade shooter built with C++ and Raylib. Defend the core from incoming enemies, upgrade your turret, and survive as long as possible.

## 📁 Project Structure

```text
.
├── CMakeLists.txt      # Build configuration
├── README.md           # This file
└── src
    └── main.cpp        # Game source code
```

## 🛠️ Prerequisites

Before running, ensure you have these installed:

- C++ Compiler (GCC, Clang, or MSVC)
- CMake (Version 3.21 or higher)
- Git

## 🚀 How to Build & Run

### 🍎 macOS

**Install Dependencies (Recommended via Homebrew):**

```bash
brew install cmake raylib
```

**Build the Game:**

```bash
mkdir build
cd build
cmake ..
make
```

**Run:**

```bash
./NeonAegis
```

### 🐧 Linux (Ubuntu/Debian)

**Install Dependencies:**

```bash
sudo apt update
sudo apt install build-essential git cmake libasound2-dev libx11-dev libxrandr-dev libxi-dev libgl1-mesa-dev libglu1-mesa-dev libxcursor-dev libxinerama-dev
```

**Build the Game:**

```bash
mkdir build
cd build
cmake ..
make
```

**Run:**

```bash
./NeonAegis
```

### 🪟 Windows

**Option A: Visual Studio (Easiest)**

1. Install Visual Studio 2022 with "Desktop development with C++" workload
2. Open the project folder directly in Visual Studio
3. Right-click `CMakeLists.txt` → Build
4. Press the Green "Play" button

**Option B: VS Code (Recommended)**

1. Install VS Code, C/C++ and CMake Tools extensions
2. Install MinGW or Visual Studio Build Tools
3. Open the folder in VS Code
4. Press `Ctrl+Shift+P` → CMake: Configure
5. Press `F7` to build and `F5` to run

> **Note:** CMakeLists.txt automatically downloads Raylib on Windows, so the first build may take longer.

## 🎮 Controls

- **Mouse:** Aim Turret
- **Left Click:** Shoot
- **[B]:** Open Shop (Buy upgrades)
- **[M]:** Pause Game
- **[S]:** Settings (in Menu)