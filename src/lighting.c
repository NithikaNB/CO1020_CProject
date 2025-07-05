#include "lighting.h"
#include <math.h>

// Compute Lambertian reflection intensity
float compute_lambert_intensity(vec3_t edge_dir, vec3_t light_dir) {
    vec3_t edge_n = vec3_normalize_fast(edge_dir);   // normalize edge direction
    vec3_t light_n = vec3_normalize_fast(light_dir); // normalize light direction

    // Compute dot product
    float dot = edge_n.x * light_n.x + edge_n.y * light_n.y + edge_n.z * light_n.z;

    // Clamp to 0 for Lambertian shading
    return fmaxf(0.0f, dot);
}
