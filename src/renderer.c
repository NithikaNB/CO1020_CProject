// renderer.c - Debug version with fixes
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include "canvas.h"
#include "math3d.h"
#include "renderer.h"

/*************  ✨ Windsurf Command ⭐  *************/
/**
 * Projects a 3D vertex into 2D screen space using the Model-View-Projection (MVP) matrix.
 *
 * This function takes a 3D vertex and applies a transformation using the provided MVP matrix,
 * then performs a perspective divide to convert the vertex into Normalized Device Coordinates (NDC).
 * Finally, it maps the NDC to 2D screen coordinates based on the specified canvas dimensions.
 *
 * @param v The 3D vertex to project.
 * @param mvp The 4x4 Model-View-Projection matrix.
 * @param width The width of the canvas in pixels.
 * @param height The height of the canvas in pixels.
 * @return A vec3_t containing the 2D screen coordinates and depth.
 */

/*******  9051adf6-8e65-4859-8653-1b23cd2563bf  *******/vec3_t project_vertex(vec3_t v, mat4_t mvp, int width, int height) {
    // Debug: Print input vertex
    printf("Projecting vertex: (%.2f, %.2f, %.2f)\n", v.x, v.y, v.z);
    
    // Step 1: Apply Model-View-Projection transformation
    float x = v.x, y = v.y, z = v.z;

    // Apply transformation
    float transformed_x = mvp.m[0] * x + mvp.m[4] * y + mvp.m[8]  * z + mvp.m[12];
    float transformed_y = mvp.m[1] * x + mvp.m[5] * y + mvp.m[9]  * z + mvp.m[13];
    float transformed_z = mvp.m[2] * x + mvp.m[6] * y + mvp.m[10] * z + mvp.m[14];
    float w = mvp.m[3] * x + mvp.m[7] * y + mvp.m[11] * z + mvp.m[15];

    printf("After transformation: (%.2f, %.2f, %.2f, %.2f)\n", transformed_x, transformed_y, transformed_z, w);

    // Perspective divide
    vec3_t ndc = {transformed_x, transformed_y, transformed_z};
    if (w != 0.0f) {
        ndc.x /= w;
        ndc.y /= w;
        ndc.z /= w;
    }

    printf("NDC coordinates: (%.2f, %.2f, %.2f)\n", ndc.x, ndc.y, ndc.z);

    // Step 2: Map from NDC [-1, 1] to canvas [0, width/height]
    vec3_t screen = {
        (ndc.x + 1.0f) * 0.5f * (width  - 1),
        (1.0f - (ndc.y + 1.0f) * 0.5f) * (height - 1), // flip Y
        ndc.z  // depth, used for Z-sorting
    };

    printf("Screen coordinates: (%.2f, %.2f, %.2f)\n\n", screen.x, screen.y, screen.z);

    return screen;
}

bool clip_to_circular_viewport(canvas_t* canvas, float x, float y) {
    // Compute center of the canvas
    float cx = (float)(canvas->width - 1) / 2.0f;
    float cy = (float)(canvas->height - 1) / 2.0f;

    // Radius is half the smallest dimension
    float radius = fminf(canvas->width, canvas->height) / 2.0f;

    // Distance from center
    float dx = x - cx;
    float dy = y - cy;
    float distance_squared = dx * dx + dy * dy;

    // Compare with radius^2
    bool inside = distance_squared <= (radius * radius);
    
    printf("Clipping check: (%.2f, %.2f) -> %s (dist=%.2f, radius=%.2f)\n", 
           x, y, inside ? "INSIDE" : "OUTSIDE", sqrtf(distance_squared), radius);
    
    return inside;
}

// Depth comparator (back-to-front)
int compare_edges(const void* a, const void* b) {
    float z1 = ((edge_depth_t*)a)->depth;
    float z2 = ((edge_depth_t*)b)->depth;
    return (z1 < z2) ? 1 : (z1 > z2) ? -1 : 0;
}

void render_wireframe(canvas_t* canvas, vec3_t* verts, int vert_count, int edges[][2], int edge_count, mat4_t mvp) {
    printf("=== Starting wireframe render ===\n");
    printf("Vertex count: %d, Edge count: %d\n", vert_count, edge_count);
    printf("Canvas size: %dx%d\n", canvas->width, canvas->height);
    
    int width = canvas->width;
    int height = canvas->height;

    // FIX: Allocate for ALL vertices, not edge_count * 2
    vec3_t* projected = malloc(sizeof(vec3_t) * vert_count);
    if (!projected) {
        printf("ERROR: Failed to allocate projected vertices\n");
        return;
    }

    // Initialize all projected vertices
    for (int i = 0; i < vert_count; i++) {
        projected[i] = vec3_from_cartesian(0, 0, 0);
    }

    // Project all vertices (do this once)
    printf("Projecting %d vertices...\n", vert_count);
    for (int i = 0; i < vert_count; i++) {
        printf("Vertex %d: ", i);
        projected[i] = project_vertex(verts[i], mvp, width, height);
    }

    // Store edges with average depth
    edge_depth_t* sorted_edges = malloc(sizeof(edge_depth_t) * edge_count);
    if (!sorted_edges) {
        printf("ERROR: Failed to allocate sorted edges\n");
        free(projected);
        return;
    }

    printf("Processing %d edges...\n", edge_count);
    for (int i = 0; i < edge_count; i++) {
        int i0 = edges[i][0];
        int i1 = edges[i][1];

        // Check bounds
        if (i0 >= vert_count || i1 >= vert_count || i0 < 0 || i1 < 0) {
            printf("ERROR: Edge %d has invalid vertex indices: %d, %d (max: %d)\n", 
                   i, i0, i1, vert_count - 1);
            continue;
        }

        float z0 = projected[i0].z;
        float z1 = projected[i1].z;

        // Optional: use log depth to simulate depth perception
        float logz0 = logf(fabsf(z0) + 1e-3f);
        float logz1 = logf(fabsf(z1) + 1e-3f);

        sorted_edges[i] = (edge_depth_t){
            .i0 = i0,
            .i1 = i1,
            .depth = (logz0 + logz1) / 2.0f
        };
        
        printf("Edge %d: vertices %d->%d, depth %.2f\n", i, i0, i1, sorted_edges[i].depth);
    }

    // Sort edges from back to front
    qsort(sorted_edges, edge_count, sizeof(edge_depth_t), compare_edges);

    // Draw sorted edges
    int drawn_edges = 0;
    printf("Drawing edges...\n");
    for (int i = 0; i < edge_count; i++) {
        int i0 = sorted_edges[i].i0;
        int i1 = sorted_edges[i].i1;

        vec3_t p0 = projected[i0];
        vec3_t p1 = projected[i1];

        printf("Drawing edge %d: (%.1f,%.1f) -> (%.1f,%.1f)\n", i, p0.x, p0.y, p1.x, p1.y);

        // MODIFIED: Only skip if BOTH points are outside (allow partial clipping)
        bool p0_inside = clip_to_circular_viewport(canvas, p0.x, p0.y);
        bool p1_inside = clip_to_circular_viewport(canvas, p1.x, p1.y);
        
        if (!p0_inside && !p1_inside) {
            printf("  -> Skipped (both points outside)\n");
            continue;
        }

        // Draw the line
        draw_line_f(canvas, p0.x, p0.y, p1.x, p1.y, 1.5f);
        drawn_edges++;
        printf("  -> Drawn\n");
    }

    printf("Total edges drawn: %d/%d\n", drawn_edges, edge_count);
    printf("=== Wireframe render complete ===\n\n");

    free(projected);
    free(sorted_edges);
}

// Apply smooth quaternion-based rotation (SLERP between two directions)
mat4_t apply_quaternion_rotation(vec3_t from, vec3_t to, float t) {
    printf("Applying quaternion rotation with t=%.2f\n", t);
    
    // Normalize both
    from = vec3_normalize_fast(from);
    to   = vec3_normalize_fast(to);

    // Slerp between the two
    vec3_t forward = vec3_slerp(from, to, t);
    forward = vec3_normalize_fast(forward);

    // Avoid degenerate 'up' when forward is close to Y
    vec3_t world_up = {0, 1, 0};
    if (fabsf(forward.x) < 1e-3 && fabsf(forward.z) < 1e-3)
        world_up = (vec3_t){0, 0, 1};

    // Right = up × forward
    vec3_t right = {
        world_up.y * forward.z - world_up.z * forward.y,
        world_up.z * forward.x - world_up.x * forward.z,
        world_up.x * forward.y - world_up.y * forward.x
    };
    right = vec3_normalize_fast(right);

    // Recompute true up = forward × right
    vec3_t up = {
        forward.y * right.z - forward.z * right.y,
        forward.z * right.x - forward.x * right.z,
        forward.x * right.y - forward.y * right.x
    };
    up = vec3_normalize_fast(up);

    // Final matrix
    mat4_t result = {
        .m = {
            right.x,   up.x,   -forward.x,   0,
            right.y,   up.y,   -forward.y,   0,
            right.z,   up.z,   -forward.z,   0,
            0,         0,       0,           1
        }
    };

    printf("Rotation matrix created\n");
    return result;
}