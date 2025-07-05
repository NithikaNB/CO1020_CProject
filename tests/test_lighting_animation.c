#include <stdio.h>
#include "lighting.h"
#include "animation.h"

int main() {
    vec3_t edge = {1, 1, 0};
    vec3_t light = {0, 0, -1};

    float intensity = compute_lambert_intensity(edge, light);
    printf("Lambert Intensity: %.2f\n", intensity);  // Should be between 0.0 and 1.0

    vec3_t p0 = {0, 0, 0};
    vec3_t p1 = {1, 2, 0};
    vec3_t p2 = {2, 3, 0};
    vec3_t p3 = {4, 0, 0};

    float t = 0.5f;
    vec3_t pt = bezier(p0, p1, p2, p3, t);
    printf("Bezier point at t=0.5: (%.2f, %.2f, %.2f)\n", pt.x, pt.y, pt.z);

    return 0;
}
