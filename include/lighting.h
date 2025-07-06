#ifndef LIGHTING_H
#define LIGHTING_H

#include "math3d.h"

// Light structure
typedef struct {
    vec3_t position;    // Light position in world space
    vec3_t color;       // Light color (RGB values 0-1)
    float intensity;    // Light intensity multiplier
} light_t;

// Create a new light
light_t light_create(vec3_t position, vec3_t color, float intensity);

// Calculate lighting intensity for an edge using Lambert lighting
float calculate_edge_lighting(vec3_t v0, vec3_t v1, light_t* lights, int light_count);


#endif // LIGHTING_H