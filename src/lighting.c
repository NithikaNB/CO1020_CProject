// Corrected lighting.c implementation - Simple Lambert lighting for edges

#include "lighting.h"
#include <math.h>
#include <stdio.h>

// Create a new light
light_t light_create(vec3_t position, vec3_t color, float intensity) {
    light_t light;
    light.position = position;
    light.color = color;
    light.intensity = intensity;
    return light;
}

// Calculate normalized vector
vec3_t vec3_normalize(vec3_t v) {
    float length = sqrtf(v.x * v.x + v.y * v.y + v.z * v.z);
    if (length < 1e-6f) return vec3_from_cartesian(0, 0, 0);
    return vec3_from_cartesian(v.x / length, v.y / length, v.z / length);
}

// Calculate dot product
float vec3_dot(vec3_t a, vec3_t b) {
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

// Calculate edge direction vector
vec3_t calculate_edge_direction(vec3_t edge_start, vec3_t edge_end) {
    vec3_t edge_dir = vec3_from_cartesian(
        edge_end.x - edge_start.x,
        edge_end.y - edge_start.y,
        edge_end.z - edge_start.z
    );
    return vec3_normalize(edge_dir);
}


static vec3_t vsub(vec3_t a, vec3_t b) {
    return vec3_from_cartesian(a.x - b.x, a.y - b.y, a.z - b.z);
}
static vec3_t vmul(vec3_t v, float s) {
    return vec3_from_cartesian(v.x * s, v.y * s, v.z * s);
}
static float vdot(vec3_t a, vec3_t b) {
    return a.x * b.x + a.y * b.y + a.z * b.z;
}
static float vlen(vec3_t v) {
    return sqrtf(vdot(v, v));
}
static vec3_t vnorm(vec3_t v) {
    float L = vlen(v);
    if (L < 1e-6f) return vec3_from_cartesian(0, 0, 0);
    return vmul(v, 1.0f / L);
}





// float calculate_edge_lighting(vec3_t v0, vec3_t v1, light_t* lights, int light_count) {
//     // Midpoint of the edge
//     vec3_t midpoint = vec3_from_cartesian(
//         0.5f * (v0.x + v1.x),
//         0.5f * (v0.y + v1.y),
//         0.5f * (v0.z + v1.z)
//     );

//     // Edge direction as pseudo-normal
//     vec3_t edge_dir = vnorm(vsub(v1, v0));

//     float total_intensity = 0.0f;

//     for (int i = 0; i < light_count; i++) {
//         vec3_t light_vec = vnorm(vsub(lights[i].position, midpoint));
//         float dot = vdot(edge_dir, light_vec);
//         if (dot > 0.0f) {
//             total_intensity += dot * lights[i].intensity;
//         }
//     }

//     // Clamp between 0 and 1
//     if (total_intensity > 1.0f) total_intensity = 1.0f;
//     return total_intensity;
// }


float calculate_edge_lighting(vec3_t v0, vec3_t v1, light_t* lights, int light_count) {
    // Midpoint of the edge
    vec3_t midpoint = vec3_from_cartesian(
        0.5f * (v0.x + v1.x),
        0.5f * (v0.y + v1.y),
        0.5f * (v0.z + v1.z)
    );

    // Use midpoint as approximate normal (assuming vertices centered around origin)
    vec3_t face_normal = vnorm(midpoint);

    float total_intensity = 0.0f;

    for (int i = 0; i < light_count; i++) {
        vec3_t light_vec = vnorm(vsub(lights[i].position, midpoint));
        float dot = vdot(face_normal, light_vec);
        if (dot > 0.0f) {
            total_intensity += dot * lights[i].intensity * 1.5f; // Amplify for soccer ball
        }
    }

    // Clamp between 0 and 1
    if (total_intensity > 1.0f) total_intensity = 1.0f;
    return total_intensity;
}


// Alternative implementation that considers both edge directions
// (since edges can be viewed from either direction)
float calculate_edge_lighting_bidirectional(vec3_t edge_start, vec3_t edge_end, light_t* lights, int light_count) {
    if (light_count == 0) return 0.1f;
    
    // Calculate edge direction
    vec3_t edge_dir = calculate_edge_direction(edge_start, edge_end);
    
    // Calculate edge midpoint
    vec3_t edge_mid = vec3_from_cartesian(
        (edge_start.x + edge_end.x) * 0.5f,
        (edge_start.y + edge_end.y) * 0.5f,
        (edge_start.z + edge_end.z) * 0.5f
    );
    
    float total_intensity = 0.0f;
    
    for (int i = 0; i < light_count; i++) {
        vec3_t light_vec = vec3_from_cartesian(
            lights[i].position.x - edge_mid.x,
            lights[i].position.y - edge_mid.y,
            lights[i].position.z - edge_mid.z
        );
        
        vec3_t light_dir = vec3_normalize(light_vec);
        
        // Consider both directions of the edge (absolute value)
        // This prevents edges from going completely dark when viewed from behind
        float lambert = fabsf(vec3_dot(edge_dir, light_dir));
        
        total_intensity += lambert * lights[i].intensity;
    }
    
    total_intensity += 0.05f;
    return fminf(1.0f, fmaxf(0.0f, total_intensity));
}