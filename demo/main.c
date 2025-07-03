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
#define FRAME_COUNT 300
#define FPS 60
#endif


void generate_soccer_ball(vec3_t** out_verts, int* out_vert_count, int (**out_edges)[2], int* out_edge_count) {
    // Using pre-calculated vertices and edges
    const int VERT_COUNT = 60;
    const int EDGE_COUNT = 174;
    
    // Allocate memory
    *out_verts = malloc(VERT_COUNT * sizeof(vec3_t));
    *out_edges = malloc(EDGE_COUNT * sizeof(int[2]));
    *out_vert_count = VERT_COUNT;
    *out_edge_count = EDGE_COUNT;
    
    vec3_t* verts = *out_verts;
    int (*edges)[2] = *out_edges;
    
    // Pre-calculated vertices
    float vertices[60][3] = {
        {-2.389493, -0.500000, -0.425325},
        {-2.389493, 0.500000, -0.425325},
        {-2.227033, -1.000000, 0.425325},
        {-2.227033, 1.000000, 0.425325},
        {-2.064573, -0.500000, 1.275976},
        {-2.064573, 0.500000, 1.275976},
        {-1.964167, -0.809017, -1.275976},
        {-1.964167, 0.809017, -1.275976},
        {-1.701302, 0.000000, -1.801707},
        {-1.639247, -1.809017, 0.425325},
        {-1.639247, 1.809017, 0.425325},
        {-1.376382, -1.618034, -1.275976},
        {-1.376382, -1.000000, 1.801707},
        {-1.376382, 1.000000, 1.801707},
        {-1.376382, 1.618034, -1.275976},
        {-1.213922, -2.118034, -0.425325},
        {-1.213922, 2.118034, -0.425325},
        {-1.113516, -1.809017, 1.275976},
        {-1.113516, 1.809017, 1.275976},
        {-0.850651, 0.000000, -2.327438},
        {-0.688191, -0.500000, 2.327438},
        {-0.688191, 0.500000, 2.327438},
        {-0.525731, -1.618034, -1.801707},
        {-0.525731, 1.618034, -1.801707},
        {-0.262866, -2.427051, -0.425325},
        {-0.262866, -0.809017, -2.327438},
        {-0.262866, 0.809017, -2.327438},
        {-0.262866, 2.427051, -0.425325},
        {-0.162460, -2.118034, 1.275976},
        {-0.162460, 2.118034, 1.275976},
        {0.162460, -2.118034, -1.275976},
        {0.162460, 2.118034, -1.275976},
        {0.262866, -2.427051, 0.425325},
        {0.262866, -0.809017, 2.327438},
        {0.262866, 0.809017, 2.327438},
        {0.262866, 2.427051, 0.425325},
        {0.525731, -1.618034, 1.801707},
        {0.525731, 1.618034, 1.801707},
        {0.688191, -0.500000, -2.327438},
        {0.688191, 0.500000, -2.327438},
        {0.850651, 0.000000, 2.327438},
        {1.113516, -1.809017, -1.275976},
        {1.113516, 1.809017, -1.275976},
        {1.213922, -2.118034, 0.425325},
        {1.213922, 2.118034, 0.425325},
        {1.376382, -1.618034, 1.275976},
        {1.376382, -1.000000, -1.801707},
        {1.376382, 1.000000, -1.801707},
        {1.376382, 1.618034, 1.275976},
        {1.639247, -1.809017, -0.425325},
        {1.639247, 1.809017, -0.425325},
        {1.701302, 0.000000, 1.801707},
        {1.964167, -0.809017, 1.275976},
        {1.964167, 0.809017, 1.275976},
        {2.064573, -0.500000, -1.275976},
        {2.064573, 0.500000, -1.275976},
        {2.227033, -1.000000, -0.425325},
        {2.227033, 1.000000, -0.425325},
        {2.389493, -0.500000, 0.425325},
        {2.389493, 0.500000, 0.425325},
    };

    // Pre-calculated edges
    int edge_list[174][2] = {
        {0, 1}, {0, 2}, {0, 4}, {0, 5}, {0, 6}, {0, 8}, {0, 9}, {0, 11}, {0, 15},
        {1, 3}, {1, 5}, {1, 7}, {1, 8}, {1, 10}, {1, 14}, {1, 16},
        {2, 4}, {2, 9}, {3, 5}, {3, 10}, {4, 5}, {4, 9}, {4, 12}, {4, 17}, {4, 20}, {4, 21},
        {5, 10}, {5, 13}, {5, 18}, {5, 21}, {6, 8}, {6, 11}, {7, 8}, {7, 14},
        {8, 11}, {8, 14}, {8, 19}, {8, 22}, {8, 23}, {8, 26}, {9, 15}, {9, 17},
        {10, 16}, {10, 18}, {11, 15}, {11, 22}, {12, 17}, {12, 20}, {13, 18}, {13, 21},
        {14, 16}, {14, 23}, {15, 17}, {15, 22}, {15, 24}, {16, 18}, {16, 23}, {16, 27},
        {17, 20}, {17, 24}, {17, 28}, {17, 32}, {17, 33}, {17, 36}, {18, 21}, {18, 27},
        {18, 29}, {18, 34}, {18, 35}, {19, 22}, {19, 25}, {19, 26}, {19, 39}, {20, 21},
        {20, 33}, {21, 33}, {21, 34}, {22, 24}, {22, 25}, {22, 30}, {22, 38}, {22, 41},
        {23, 26}, {23, 27}, {23, 31}, {23, 42}, {24, 30}, {24, 32}, {24, 41}, {24, 49},
        {25, 38}, {25, 39}, {26, 39}, {26, 42}, {27, 31}, {27, 35}, {27, 42}, {27, 50},
        {28, 32}, {28, 36}, {28, 45}, {29, 34}, {29, 35}, {29, 37}, {29, 44}, {29, 48},
        {30, 41}, {31, 42}, {32, 43}, {32, 45}, {32, 49}, {33, 34}, {33, 36}, {33, 40},
        {33, 45}, {33, 51}, {34, 37}, {34, 40}, {34, 48}, {34, 51}, {35, 44}, {35, 50},
        {36, 45}, {37, 48}, {38, 39}, {38, 41}, {38, 46}, {38, 47}, {39, 42}, {39, 47},
        {40, 51}, {41, 46}, {41, 49}, {41, 56}, {42, 47}, {42, 50}, {42, 57}, {43, 45},
        {43, 49}, {43, 56}, {44, 48}, {44, 50}, {44, 57}, {45, 51}, {45, 52}, {45, 56},
        {45, 58}, {46, 47}, {46, 54}, {46, 55}, {46, 56}, {47, 55}, {47, 57}, {48, 51},
        {48, 53}, {48, 57}, {48, 59}, {49, 56}, {50, 57}, {51, 52}, {51, 53}, {51, 58},
        {51, 59}, {52, 58}, {53, 59}, {54, 55}, {54, 56}, {55, 56}, {55, 57}, {56, 57},
        {56, 58}, {57, 58}, {57, 59}, {58, 59}
    };
    
    // Copy vertices
    for (int i = 0; i < VERT_COUNT; i++) {
        verts[i] = vec3_from_cartesian(vertices[i][0], vertices[i][1], vertices[i][2]);
    }
    
    // Copy edges
    for (int i = 0; i < EDGE_COUNT; i++) {
        edges[i][0] = edge_list[i][0];
        edges[i][1] = edge_list[i][1];
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