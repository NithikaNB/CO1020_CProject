#ifndef LIGHTING_H
#define LIGHTING_H

#include "math3d.h"

// Structure to represent a directional light
typedef struct {
    vec3_t direction;  // Light direction (should be normalized)
    float intensity;   // Light intensity (range: 0.0 to 1.0)
} light_t;

// Compute Lambert intensity for an edge and light direction
float compute_lambert_intensity(vec3_t edge_dir, vec3_t light_dir);

#endif
