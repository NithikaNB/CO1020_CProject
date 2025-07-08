# libtiny3d - Lightweight 3D Software Renderer in C

[![C](https://img.shields.io/badge/language-C-blue.svg)](https://en.wikipedia.org/wiki/C_(programming_language))
[![License](https://img.shields.io/badge/license-MIT-green.svg)](LICENSE)
[![Build Status](https://img.shields.io/badge/build-passing-brightgreen.svg)](##-building)
[![Version](https://img.shields.io/badge/version-1.0-blue.svg)](##-changelog)

**libtiny3d** is a lightweight, pure-C 3D graphics engine built from scratch, implementing a complete software rendering pipeline without OpenGL or DirectX. Perfect for learning 3D graphics fundamentals or building minimalistic rendering solutions.

## 🎯 Project Overview

This project showcases a custom 3D rendering pipeline that transforms 3D geometric data into 2D images, emphasizing mathematical precision and computational efficiency. It's designed for educational purposes and lightweight applications.

### ✨ Key Features

- 🖼️ **Software Rendering Pipeline**: Full 3D-to-2D transformation with perspective projection.
- 🧮 **Custom Math Engine**: Vector, matrix, and transformation operations for 3D math.
- 🎨 **Floating-Point Canvas**: Sub-pixel precision with bilinear filtering for smooth rendering.
- 📏 **Wireframe Rendering**: Depth-sorted visualization of 3D objects.
- 💡 **Dynamic Lighting**: Lambert lighting model with multiple light sources.
- 🎥 **Smooth Animation**: Bézier curve-based animations with synchronized motion.
- ⚡ **Optimized Algorithms**: DDA line drawing, fast inverse square root, and SLERP interpolation.

## 🏗️ Project Structure

```
libtiny3d/
├── .gitignore                # Ignored files
├── Makefile                  # Build configuration
├── README.md                 # Project documentation
├── build/                    # Compiled binaries
│   └── build.txt             # Build notes
├── demo/                     # Demo application
│   └── main.c                # Main demo source
├── frames/                   # Generated frame outputs (.pgm)
│   └── Frames.txt            # Frame notes
├── include/                  # Header files
│   ├── animation.h           # Animation system
│   ├── canvas.h              # Canvas and drawing operations
│   ├── lighting.h            # Lighting calculations
│   ├── math3d.h              # 3D math utilities
│   └── renderer.h            # Rendering pipeline
├── src/                      # Source files
│   ├── animation.c           # Animation implementation
│   ├── canvas.c              # Canvas and line drawing
│   ├── lighting.c            # Lighting system
│   ├── math3d.c              # Vector and matrix operations
│   └── renderer.c            # Rendering pipeline
└── tests/                    # Unit tests
    ├── test_lighting_animation.c # Lighting and animation tests
    └── test_math.c           # Math operation tests
```

## 🚀 Getting Started

### Prerequisites

- **GCC**: C compiler (`gcc` or compatible).
- **Make**: Build automation tool.
- **FFmpeg**: For generating `.mp4` videos from `.pgm` frames.
- **Standard C Library**: Included with most systems.

For Windows, ensure `gcc` and `make` are installed (e.g., via MinGW or WSL). For Unix-like systems (Linux/macOS), install `gcc`, `make`, and `ffmpeg`:
```bash
# Ubuntu/Debian
sudo apt update
sudo apt install build-essential ffmpeg
```

### Installation

1. **Clone the Repository**:
   ```bash
   git clone <repository-url>
   cd libtiny3d
   ```

2. **Build the Project**:
   ```bash
   make all
   ```

3. **Run the Demo**:
   ```bash
   make run-demo
   ```
   Outputs `soccer_ball_wireframe.mp4` in the project root.

4. **Run Tests**:
   ```bash
   make run-test
   ```
   Outputs `test_math_wireframe.mp4`.

5. **Run Lighting Demo**:
   ```bash
   make run-lighting
   ```
   Outputs `lighting_animation.mp4`.

### Usage Example

```c
#include "canvas.h"
#include "math3d.h"

int main() {
    // Initialize canvas
    canvas_t* canvas = canvas_create(800, 600);

    // Set up 3D transformation
    vec3_t position = {0.0f, 0.0f, -5.0f};
    mat4_t transform = mat4_identity();

    // Render (see demo/main.c for full example)
    // ...

    canvas_destroy(canvas);
    return 0;
}
```

## 🎮 Demo Showcase

The demo (`make run-demo`) generates a video (`soccer_ball_wireframe.mp4`) featuring:
1. 🕒 **Clock Face Pattern**: Radial lines with smooth drawing.
2. 📍 **3D Cube**: Rotations and projections using 3D math.
3. ⚽ **Rotating Soccer Ball**: Wireframe rendering with circular clipping.
4. 💡 **Animated Lighting**: Synchronized lighting and motion.

Run `make demo-only` to execute without video generation for debugging.

## 🧪 Testing

Run `make run-test` to test:
- Vector and matrix operations.
- Coordinate transformations.
- Rendering pipeline accuracy.

Run `make run-lighting` to test lighting and animation systems. Frames are saved as `frameXXX.pgm` in `frames/`.

## 🧮 Technical Details

### Canvas System
- Floating-point coordinates with bilinear filtering.
- DDA algorithm for smooth, configurable line drawing.

### 3D Mathematics
- Vector operations and 4×4 matrix transformations.
- Fast inverse square root for normalization.
- SLERP for smooth rotational interpolation.

### Rendering Pipeline
```
3D Object → World Transform → Camera Transform → Projection → Screen Coordinates
```

### Lighting Model
- Lambert diffuse: `intensity = max(0, dot(surface_normal, light_direction))`.
- Supports multiple light sources and edge-based lighting.

### Animation System
- Cubic Bézier curves for smooth motion.
- Time-based synchronization for consistent frame rates.

## 📊 Performance Optimizations
- ⚡ **Fast Inverse Square Root**: Speeds up vector normalization.
- 📏 **Logarithmic Z-Buffer**: Efficient depth sorting.
- 🖼️ **Bilinear Filtering**: Smooth sub-pixel rendering.
- 💾 **Column-Major Matrices**: Cache-friendly memory layout.

## 🔍 Troubleshooting

**Issue**: `make run-test` reports "No frames generated by test" despite frames in `frames/`.
- **Cause**: Frame names (`frameXXX.pgm`) don’t match the expected pattern (`frame_*.pgm`).
- **Fix**: Update the `run-test` target in `Makefile`:
  ```make
  run-test: $(TEST_TARGET)
      @if not exist $(FRAMEDIR) mkdir $(FRAMEDIR)
      ./$(TEST_TARGET)
      @if exist $(FRAMEDIR)\frame*.pgm (ffmpeg -framerate 60 -i $(FRAMEDIR)\frame%%03d.pgm -pix_fmt yuv420p -c:v libx264 -crf 18 -preset slow $(TEST_MP4_OUTPUT)) else (echo No frames generated by test)
  ```
- **Verify**: Run `make check-frames` to list frames (e.g., `frame000.pgm`).

For Unix-like systems:
```make
run-test: $(TEST_TARGET)
    @mkdir -p $(FRAMEDIR)
    ./$(TEST_TARGET)
    @if ls $(FRAMEDIR)/frame*.pgm >/dev/null 2>&1; then \
        ffmpeg -framerate 60 -i $(FRAMEDIR)/frame%03d.pgm -pix_fmt yuv420p -c:v libx264 -crf 18 -preset slow $(TEST_MP4_OUTPUT); \
    else \
        echo No frames generated by test; \
    fi
```

**Check Frames**:
```bash
dir frames\*.pgm  # Windows
ls frames/*.pgm   # Unix
```

## 👥 Contributors

| Name                        | Student ID | Role                           |
|-----------------------------|------------|--------------------------------|
| D.M.N.N. Bandara            | E/22/044   | Core Engine & Math Systems    |
| K.B.A.D.G.C. Kapurubandara  | E/22/180   | Rendering Pipeline & Animation |

## 📝 License

Licensed under the [MIT License](LICENSE). Developed for CO1020 Computer Systems Programming, University of Peradeniya.

## 🏆 Acknowledgments

- **University of Peradeniya**, Department of Computer Engineering.
- **CO1020 Course** for inspiring this project.
- Graphics algorithms from classic computer graphics literature.

---

**Course**: CO1020 - Computer Systems Programming  
**Institution**: University of Peradeniya  
**Submission Date**: July 7, 2025