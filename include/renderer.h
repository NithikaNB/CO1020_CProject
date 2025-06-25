#ifndef RENDERER_H
#define RENDERER_H

#include <stdbool.h>
#include "canvas.h"
#include "math3d.h"

// Struct to store an edge and its average depth
typedef struct {
    int i0, i1;
    float depth;
} edge_depth_t;

// Projects a 3D vertex to 2D screen space
vec3_t project_vertex(vec3_t v, mat4_t mvp, int width, int height);

// Clips a point to a circular viewport
bool clip_to_circular_viewport(canvas_t* canvas, float x, float y);

// Renders a 3D wireframe model with depth sorting
void render_wireframe(canvas_t* canvas, vec3_t* verts, int vert_count, int edges[][2], int edge_count, mat4_t mvp);

// Apply smooth quaternion-based rotation (SLERP between two directions)
mat4_t apply_quaternion_rotation(vec3_t from_dir, vec3_t to_dir, float t);

#endif // RENDERER_H
