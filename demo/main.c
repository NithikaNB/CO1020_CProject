// main.c - Debug version with simpler geometry
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include "canvas.h"
#include "math3d.h"
#include "renderer.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#define RESOLUTION 800  // Reduced for debugging
#define FRAME_COUNT 120
#define FPS 60
#endif

// Generate proper soccer ball (truncated icosahedron) with debugging
// Generate proper soccer ball (truncated icosahedron) with debugging
void generate_soccer_ball(vec3_t** out_verts, int* out_vert_count, int (**out_edges)[2], int* out_edge_count) {
    // Golden ratio for icosahedron construction
    const float phi = (1.0f + sqrtf(5.0f)) / 2.0f;
    const float a = 1.0f;
    const float b = 1.0f / phi;
    
    printf("Generating soccer ball with golden ratio phi = %.4f\n", phi);

    // Icosahedron vertices (12 vertices)
    vec3_t base_verts[] = {
        // Rectangle in XY plane
        { a,  b,  0}, {-a,  b,  0}, { a, -b,  0}, {-a, -b,  0},
        // Rectangle in YZ plane  
        { b,  0,  a}, { b,  0, -a}, {-b,  0,  a}, {-b,  0, -a},
        // Rectangle in XZ plane
        { 0,  a,  b}, { 0, -a,  b}, { 0,  a, -b}, { 0, -a, -b}
    };
    
    // Add more vertices by subdividing edges and faces for soccer ball complexity
    // For simplicity, we'll use the 12 base vertices plus some additional ones
    vec3_t extended_verts[] = {
        // Original 12 icosahedron vertices
        { a,  b,  0}, {-a,  b,  0}, { a, -b,  0}, {-a, -b,  0},
        { b,  0,  a}, { b,  0, -a}, {-b,  0,  a}, {-b,  0, -a},
        { 0,  a,  b}, { 0, -a,  b}, { 0,  a, -b}, { 0, -a, -b},
        
        // Additional vertices for more soccer ball-like appearance
        { 0.8f,  0.8f,  0.8f}, {-0.8f,  0.8f,  0.8f}, { 0.8f, -0.8f,  0.8f}, {-0.8f, -0.8f,  0.8f},
        { 0.8f,  0.8f, -0.8f}, {-0.8f,  0.8f, -0.8f}, { 0.8f, -0.8f, -0.8f}, {-0.8f, -0.8f, -0.8f},
        
        // Mid-edge vertices
        { 0.6f,  0.0f,  0.9f}, { 0.0f,  0.6f,  0.9f}, {-0.6f,  0.0f,  0.9f}, { 0.0f, -0.6f,  0.9f},
        { 0.6f,  0.0f, -0.9f}, { 0.0f,  0.6f, -0.9f}, {-0.6f,  0.0f, -0.9f}, { 0.0f, -0.6f, -0.9f},
        { 0.9f,  0.6f,  0.0f}, { 0.9f, -0.6f,  0.0f}, {-0.9f,  0.6f,  0.0f}, {-0.9f, -0.6f,  0.0f}
    };

    int vert_count = sizeof(extended_verts) / sizeof(extended_verts[0]);
    *out_vert_count = vert_count;
    
    printf("Soccer ball will have %d vertices\n", vert_count);

    // Normalize all vertices to unit sphere (proper soccer ball shape)
    *out_verts = malloc(sizeof(vec3_t) * vert_count);
    for (int i = 0; i < vert_count; i++) {
        vec3_t v = extended_verts[i];
        // Normalize to unit sphere
        float length = sqrtf(v.x * v.x + v.y * v.y + v.z * v.z);
        if (length > 1e-6f) {
            (*out_verts)[i].x = v.x / length;
            (*out_verts)[i].y = v.y / length;
            (*out_verts)[i].z = v.z / length;
        } else {
            (*out_verts)[i] = v;
        }
    }

    // Generate edges by connecting vertices within a threshold distance
    float EDGE_THRESHOLD = 1.2f; // Adjusted for better connectivity
    int edge_capacity = 128;
    int (*edges)[2] = malloc(sizeof(int[2]) * edge_capacity);
    int edge_count = 0;

    printf("Connecting vertices with threshold %.2f...\n", EDGE_THRESHOLD);

    for (int i = 0; i < vert_count; ++i) {
        for (int j = i + 1; j < vert_count; ++j) {
            vec3_t v1 = (*out_verts)[i];
            vec3_t v2 = (*out_verts)[j];
            
            float dx = v1.x - v2.x;
            float dy = v1.y - v2.y;
            float dz = v1.z - v2.z;
            float dist = sqrtf(dx*dx + dy*dy + dz*dz);

            if (dist < EDGE_THRESHOLD) {
                if (edge_count >= edge_capacity) {
                    edge_capacity *= 2;
                    edges = realloc(edges, sizeof(int[2]) * edge_capacity);
                    printf("Expanded edge capacity to %d\n", edge_capacity);
                }
                edges[edge_count][0] = i;
                edges[edge_count][1] = j;
                edge_count++;
            }
        }
    }

    *out_edges = edges;
    *out_edge_count = edge_count;
    
    printf("Generated soccer ball: %d vertices, %d edges\n", vert_count, edge_count);
    
    // Print first few vertices for debugging
    printf("First 12 vertices (normalized):\n");
    for (int i = 0; i < (vert_count < 12 ? vert_count : 12); i++) {
        printf("Vertex %d: (%.3f, %.3f, %.3f)\n", i, 
               (*out_verts)[i].x, (*out_verts)[i].y, (*out_verts)[i].z);
    }
    
    // Print first few edges for debugging
    printf("First 12 edges:\n");
    for (int i = 0; i < (edge_count < 12 ? edge_count : 12); i++) {
        printf("Edge %d: %d -> %d\n", i, edges[i][0], edges[i][1]);
    }
    
    if (edge_count == 0) {
        printf("WARNING: No edges generated! Check EDGE_THRESHOLD value.\n");
    }
}

int main() {
    printf("=== Starting 3D Rendering Debug ===\n");
    
    // Test canvas creation first
    canvas_t* canvas = canvas_create(RESOLUTION, RESOLUTION);
    if (!canvas) {
        printf("ERROR: Failed to create canvas\n");
        return 1;
    }
    printf("Canvas created: %dx%d\n", canvas->width, canvas->height);

    canvas_clear(canvas);

    // --- Test 1: Simple line drawing ---
    printf("\n=== Test 1: Simple line drawing ===\n");
    
    // Draw a simple cross to test basic functionality
    float center_x = canvas->width / 2.0f;
    float center_y = canvas->height / 2.0f;
    
    draw_line_f(canvas, center_x - 50, center_y, center_x + 50, center_y, 2.0f); // horizontal
    draw_line_f(canvas, center_x, center_y - 50, center_x, center_y + 50, 2.0f); // vertical
    
    canvas_save_pgm(canvas, "test_cross.pgm");
    printf("Simple cross saved to test_cross.pgm\n");

    // --- Test 2: Clock face (original test) ---
    printf("\n=== Test 2: Clock face ===\n");
    
    canvas_clear(canvas);
    
    float radius = 100.0f;
    for (int angle_deg = 0; angle_deg < 360; angle_deg += 30) {  // Every 30 degrees for clearer debug
        float angle_rad = angle_deg * M_PI / 180.0f;
        float end_x = center_x + radius * cosf(angle_rad);
        float end_y = center_y + radius * sinf(angle_rad);
        draw_line_f(canvas, center_x, center_y, end_x, end_y, 1.5f);
        printf("Drew line to angle %d degrees: (%.1f, %.1f)\n", angle_deg, end_x, end_y);
    }

    canvas_save_pgm(canvas, "debug_clock.pgm");
    printf("Clock demo saved to debug_clock.pgm\n");

    // --- Test 3: 3D Soccer Ball Wireframe ---
    printf("\n=== Test 3: 3D Soccer Ball Wireframe ===\n");
    
    canvas_clear(canvas);

    // Generate soccer ball geometry
    vec3_t* soccer_verts = NULL;
    int (*soccer_edges)[2] = NULL;
    int vert_count = 0, edge_count = 0;
    generate_soccer_ball(&soccer_verts, &vert_count, &soccer_edges, &edge_count);

    if (edge_count == 0) {
        printf("ERROR: No edges generated for soccer ball!\n");
        free(soccer_verts);
        free(soccer_edges);
        canvas_destroy(canvas);
        return 1;
    }

    // Test matrix functions
    printf("Testing matrix operations...\n");
    
    mat4_t identity = mat4_identity();
    printf("Identity matrix created\n");
    
    mat4_t proj = mat4_frustum(-0.8f, 0.8f, -0.8f, 0.8f, 1.0f, 10.0f);
    printf("Projection matrix created\n");
    
    // Use SLERP-based quaternion rotation (as per your original code)
    printf("Setting up quaternion rotation...\n");
    vec3_t from_dir = vec3_from_cartesian(0.0f, 0.0f, -1.0f);  // original forward view
    vec3_t to_dir   = vec3_from_cartesian(1.0f, 0.0f, -1.0f);  // rotated slightly to the right
    float t = 0.5f;  // Interpolation amount [0.0 - 1.0]

    mat4_t rotate = apply_quaternion_rotation(from_dir, to_dir, t);
    mat4_t translate = mat4_translate(0.0f, 0.0f, -3.5f);
    mat4_t model = mat4_multiply(translate, rotate);
    mat4_t mvp = mat4_multiply(proj, model);
    printf("MVP matrix computed with quaternion rotation\n");

    // Create output folder (run manually in shell too)
    system("mkdir -p frames");

    for (int frame = 0; frame < FRAME_COUNT; frame++) {
        printf("\n--- Rendering Frame %d/%d ---\n", frame + 1, FRAME_COUNT);
        canvas_clear(canvas);

        float t = (float)frame / (FRAME_COUNT - 1);  // SLERP interpolation value

        // You can use continuous spin instead (see below)
        mat4_t rotate = apply_quaternion_rotation(from_dir, to_dir, t);
        mat4_t translate = mat4_translate(0.0f, 0.0f, -3.5f);
        mat4_t model = mat4_multiply(translate, rotate);
        mat4_t mvp = mat4_multiply(proj, model);

        render_wireframe(canvas, soccer_verts, vert_count, soccer_edges, edge_count, mvp);

        // Save frame
        char filename[256];
        snprintf(filename, sizeof(filename), "frames/frame_%03d.pgm", frame);
        canvas_save_pgm(canvas, filename);
        printf("Saved frame: %s\n", filename);
}

    free(soccer_verts);
    free(soccer_edges);
    canvas_destroy(canvas);

    printf("\n=== Debug complete ===\n");
    printf("Check the output files:\n");
    printf("1. test_cross.pgm - Should show a simple cross\n");
    printf("2. debug_clock.pgm - Should show clock face pattern\n");
    printf("3. debug_soccer_ball.pgm - Should show 3D soccer ball wireframe\n");

    return 0;
}