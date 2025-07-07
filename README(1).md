# libtiny3d - 3D Software Renderer Library

[![C](https://img.shields.io/badge/language-C-blue.svg)](https://en.wikipedia.org/wiki/C_(programming_language))
[![License](https://img.shields.io/badge/license-MIT-green.svg)](LICENSE)
[![Build Status](https://img.shields.io/badge/build-passing-brightgreen.svg)](#building)

A lightweight 3D graphics engine built from scratch in pure C, implementing fundamental 3D rendering algorithms without relying on OpenGL or DirectX.

## 🎯 Project Overview

**libtiny3d** is a complete software rendering pipeline that transforms 3D geometric data into 2D images. This project demonstrates the mathematical and computational foundations that power modern 3D graphics by implementing every component from the ground up.

### ✨ Key Features

- **Software Rendering Pipeline**: Complete 3D to 2D transformation pipeline
- **Custom Math Engine**: 3D vectors, 4×4 matrices, and transformation operations
- **Floating-Point Canvas**: Sub-pixel precision drawing with bilinear filtering
- **Wireframe Rendering**: 3D object visualization with depth sorting
- **Dynamic Lighting**: Lambert lighting model with multiple light sources
- **Smooth Animation**: Bézier curve-based animation system with synchronization
- **Advanced Algorithms**: DDA line drawing, fast inverse square root, SLERP interpolation

## 🏗️ Architecture

```
libtiny3d/
├── include/           # Header files
│   ├── tiny3d.h      # Main library interface
│   ├── canvas.h      # Canvas and drawing operations
│   ├── math3d.h      # 3D mathematics utilities
│   ├── renderer.h    # 3D rendering pipeline
│   └── lighting.h    # Lighting calculations
├── src/              # Implementation files
│   ├── canvas.c      # Canvas and line drawing
│   ├── math3d.c      # Vector and matrix operations
│   ├── renderer.c    # 3D rendering pipeline
│   └── lighting.c    # Lighting system
├── tests/            # Unit tests
│   ├── test_math.c   # Math operations testing
│   └── test_pipeline.c # Rendering pipeline tests
├── demo/             # Demo applications
│   └── main.c        # Main demonstration
└── documentation/    # Project documentation
    └── report.pdf    # Detailed implementation report
```

## 🚀 Getting Started

### Prerequisites

- GCC compiler
- Make utility
- Standard C library

### Building

1. **Clone the repository**
   ```bash
   git clone <repository-url>
   cd libtiny3d
   ```

2. **Build the library**
   ```bash
   make
   ```

3. **Run the demo**
   ```bash
   make demo
   ./build/demo
   ```

4. **Run tests**
   ```bash
   make test
   ```

### Usage Example

```c
#include "tiny3d.h"

int main() {
    // Create a canvas
    canvas_t* canvas = canvas_create(800, 600);
    
    // Initialize 3D math
    vec3_t position = {0, 0, -5};
    mat4_t transform = mat4_identity();
    
    // Create and render a 3D object
    // ... (see demo/main.c for complete examples)
    
    canvas_destroy(canvas);
    return 0;
}
```

## 🎮 Demo Features

The included demo showcases:

1. **Clock Face Pattern** - Radial lines demonstrating smooth line drawing
2. **3D Cube Transformation** - Basic 3D math operations and projections
3. **Rotating Soccer Ball** - Complex wireframe rendering with circular clipping
4. **Animated Lighting** - Multiple objects with synchronized movement and lighting

## 🧮 Technical Implementation

### Canvas System
- Floating-point coordinate support with bilinear filtering
- DDA algorithm for smooth line drawing
- Configurable line thickness

### 3D Mathematics
- Cartesian ↔ Spherical coordinate conversion
- Fast inverse square root for vector normalization
- SLERP for smooth rotational interpolation
- Column-major 4×4 matrix operations

### Rendering Pipeline
```
3D Object → World Transform → Camera Transform → Projection → Screen Coordinates
```

### Lighting Model
- Lambert diffuse lighting: `intensity = max(0, dot(surface_normal, light_direction))`
- Multiple light source support
- Edge-based lighting for wireframe rendering

## 🎨 Animation System

- **Bézier Curves**: Cubic Bézier interpolation for smooth motion paths
- **Synchronization**: Time-based animation with consistent frame rates
- **Looping**: Seamless animation cycles

## 🧪 Testing

The project includes comprehensive tests for:
- Vector and matrix operations
- Coordinate transformations
- Rendering pipeline accuracy
- Animation timing and synchronization

## 📊 Performance Considerations

- **Fast Inverse Square Root**: Optimized vector normalization
- **Logarithmic Z-Buffer**: Efficient depth sorting
- **Bilinear Filtering**: Smooth sub-pixel rendering
- **Column-Major Matrices**: Cache-friendly memory layout

## 👥 Team Members

| Name | Student ID | Role |
|------|------------|------|
| D.M.N.N. Bandara | E/22/044 | Core Engine & Math Systems |
| K.B.A.D.G.C. Kapurubandara | E/22/180 | Rendering Pipeline & Animation |

## 📚 Documentation

- **Implementation Report**: `documentation/report.pdf`
- **API Documentation**: Generated from header files
- **Demo Video**: Complete walkthrough of features and implementation

## 🔧 Development

### Project Structure Guidelines
- Follow the established directory structure
- Maintain consistent coding style
- Include comprehensive comments
- Write unit tests for new features

### Build Targets
```bash
make              # Build library and demo
make clean        # Clean build artifacts
make test         # Run all tests
make demo         # Build demo only
make library      # Build library only
```

## 📝 License

This project is part of the CO1020 Computer Systems Programming course at the University of Peradeniya, Department of Computer Engineering.

## 🏆 Acknowledgments

- University of Peradeniya, Department of Computer Engineering
- CO1020 Computer Systems Programming Course
- Mathematical foundations from computer graphics literature

---

**Course**: CO1020 - Computer Systems Programming  
**Institution**: University of Peradeniya  
**Duration**: 4 Weeks  
**Submission Date**: June 30, 2025
