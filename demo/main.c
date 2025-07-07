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

#define PHI ((1.0f + sqrtf(5.0f)) * 0.5f)
#define T   (1.0f / 3.0f)

#endif


void generate_soccer_ball(vec3_t** out_verts, int* out_vert_count, int (**out_edges)[2], int* out_edge_count) {
    // Constants
    #define MAX_EDGES_SB   30
    #define MAX_VERTS_SB   60
    #define MAX_FACES_SB   (20+12)
    
    // Edge structure for truncation
    typedef struct { 
        int v1, v2, new_v1, new_v2; 
    } Edge_SB;
    
    // Pentagon and hexagon face structures
    typedef struct { 
        int nv, idx[6]; 
    } Hex_SB;
    typedef struct { 
        int nv, idx[5]; 
    } Pen_SB;
    
    // Vector operations (working with vec3_t)
    vec3_t vadd_sb(vec3_t a, vec3_t b) { 
        return vec3_from_cartesian(a.x + b.x, a.y + b.y, a.z + b.z); 
    }
    vec3_t vsub_sb(vec3_t a, vec3_t b) { 
        return vec3_from_cartesian(a.x - b.x, a.y - b.y, a.z - b.z); 
    }
    vec3_t vmul_sb(vec3_t v, float s) { 
        return vec3_from_cartesian(v.x * s, v.y * s, v.z * s); 
    }
    float vdot_sb(vec3_t a, vec3_t b) { 
        return a.x * b.x + a.y * b.y + a.z * b.z; 
    }
    vec3_t vcross_sb(vec3_t a, vec3_t b) {
        return vec3_from_cartesian(
            a.y * b.z - a.z * b.y,
            a.z * b.x - a.x * b.z,
            a.x * b.y - a.y * b.x
        );
    }
    float vlen_sb(vec3_t v) { 
        return sqrtf(vdot_sb(v, v)); 
    }
    vec3_t vnorm_sb(vec3_t v) { 
        float L = vlen_sb(v); 
        if (L < 1e-6f) return vec3_from_cartesian(0, 0, 0);
        return vmul_sb(v, 1.0f / L); 
    }
    
    // Raw icosahedron vertices
    vec3_t icoV[12];
    
    // Raw icosahedron faces
    int icoF[20][3] = {
        {0,11,5},{0,5,1},{0,1,7},{0,7,10},{0,10,11},
        {1,5,9},{5,11,4},{11,10,2},{10,7,6},{7,1,8},
        {3,9,4},{3,4,2},{3,2,6},{3,6,8},{3,8,9},
        {4,9,5},{2,4,11},{6,2,10},{8,6,7},{9,8,1}
    };
    
    // Edge tracking
    Edge_SB edges[MAX_EDGES_SB];
    int edgeCount = 0;
    
    // Output vertices
    vec3_t verts[MAX_VERTS_SB];
    int vertCount = 0;
    
    // Face structures
    Hex_SB hexes[20];
    Pen_SB pens[12];
    
    // Find or add edge function
    int find_or_add_edge(int a, int b, float t) {
        if (a > b) { 
            int tmp = a; a = b; b = tmp; 
        }
        
        // Check if edge already exists
        for (int i = 0; i < edgeCount; i++) {
            if (edges[i].v1 == a && edges[i].v2 == b) return i;
        }
        
        // Add new edge
        if (edgeCount >= MAX_EDGES_SB) {
            printf("ERROR: Too many edges!\n");
            return -1;
        }
        
        edges[edgeCount].v1 = a;
        edges[edgeCount].v2 = b;
        
        // Create two new vertices on this edge
        vec3_t A = icoV[a];
        vec3_t B = icoV[b];
        vec3_t pA = vadd_sb(vmul_sb(A, 1.0f - t), vmul_sb(B, t));
        vec3_t pB = vadd_sb(vmul_sb(B, 1.0f - t), vmul_sb(A, t));
        
        verts[vertCount] = pA; 
        edges[edgeCount].new_v1 = vertCount++;
        verts[vertCount] = pB; 
        edges[edgeCount].new_v2 = vertCount++;
        
        return edgeCount++;
    }
    
    // Pentagon sorting function
    void sort_pentagon(int pi) {
        int *idx = pens[pi].idx;
        
        // Calculate centroid
        vec3_t C = vec3_from_cartesian(0.0f, 0.0f, 0.0f);
        for (int i = 0; i < 5; i++) {
            C = vadd_sb(C, verts[idx[i]]);
        }
        C = vmul_sb(C, 1.0f / 5.0f);
        
        // Approximate normal
        vec3_t N = vnorm_sb(C);
        
        // Build basis vectors
        vec3_t ref = (fabsf(N.x) < 0.9f) ? vec3_from_cartesian(1.0f, 0.0f, 0.0f) : vec3_from_cartesian(0.0f, 1.0f, 0.0f);
        vec3_t U = vnorm_sb(vcross_sb(N, ref));
        vec3_t V = vcross_sb(N, U);
        
        // Compute angles
        float ang[5];
        int order[5];
        for (int i = 0; i < 5; i++) {
            vec3_t w = vsub_sb(verts[idx[i]], C);
            ang[i] = atan2f(vdot_sb(w, V), vdot_sb(w, U));
            order[i] = i;
        }
        
        // Insertion sort by angle
        for (int i = 1; i < 5; i++) {
            int key = order[i];
            float ka = ang[key];
            int j = i - 1;
            while (j >= 0 && ang[order[j]] > ka) {
                order[j + 1] = order[j];
                j--;
            }
            order[j + 1] = key;
        }
        
        // Reorder indices
        int tmp[5];
        for (int i = 0; i < 5; i++) {
            tmp[i] = idx[order[i]];
        }
        for (int i = 0; i < 5; i++) {
            idx[i] = tmp[i];
        }
    }
    
    // 1) Build unit icosahedron
    float phi = (1.0f + sqrtf(5.0f)) / 2.0f;
    icoV[0]  = vec3_from_cartesian(-1.0f, phi, 0.0f);
    icoV[1]  = vec3_from_cartesian( 1.0f, phi, 0.0f);
    icoV[2]  = vec3_from_cartesian(-1.0f,-phi, 0.0f);
    icoV[3]  = vec3_from_cartesian( 1.0f,-phi, 0.0f);
    icoV[4]  = vec3_from_cartesian( 0.0f,-1.0f, phi);
    icoV[5]  = vec3_from_cartesian( 0.0f, 1.0f, phi);
    icoV[6]  = vec3_from_cartesian( 0.0f,-1.0f,-phi);
    icoV[7]  = vec3_from_cartesian( 0.0f, 1.0f,-phi);
    icoV[8]  = vec3_from_cartesian( phi, 0.0f,-1.0f);
    icoV[9]  = vec3_from_cartesian( phi, 0.0f, 1.0f);
    icoV[10] = vec3_from_cartesian(-phi, 0.0f,-1.0f);
    icoV[11] = vec3_from_cartesian(-phi, 0.0f, 1.0f);
    
    // Normalize all vertices
    for (int i = 0; i < 12; i++) {
        icoV[i] = vnorm_sb(icoV[i]);
    }
    
    float t = 1.0f / 3.0f;  // Truncation parameter
    
    // 2) Truncate all edges -> build edge table & new vertices
    for (int f = 0; f < 20; f++) {
        int a = icoF[f][0], b = icoF[f][1], c = icoF[f][2];
        find_or_add_edge(a, b, t);
        find_or_add_edge(b, c, t);
        find_or_add_edge(c, a, t);
    }
    
    // 3) Build hexagon faces (one per original face)
    for (int f = 0; f < 20; f++) {
        int a = icoF[f][0], b = icoF[f][1], c = icoF[f][2];
        int eAB = find_or_add_edge(a, b, t);
        int eBC = find_or_add_edge(b, c, t);
        int eCA = find_or_add_edge(c, a, t);
        
        Hex_SB *h = &hexes[f];
        h->nv = 6;
        h->idx[0] = (a < b) ? edges[eAB].new_v1 : edges[eAB].new_v2;
        h->idx[1] = (a < b) ? edges[eAB].new_v2 : edges[eAB].new_v1;
        h->idx[2] = (b < c) ? edges[eBC].new_v1 : edges[eBC].new_v2;
        h->idx[3] = (b < c) ? edges[eBC].new_v2 : edges[eBC].new_v1;
        h->idx[4] = (c < a) ? edges[eCA].new_v1 : edges[eCA].new_v2;
        h->idx[5] = (c < a) ? edges[eCA].new_v2 : edges[eCA].new_v1;
    }
    
    // 4) Build pentagon faces (one per original vertex)
    int pi = 0;
    for (int v = 0; v < 12; v++) {
        Pen_SB *p = &pens[pi];
        p->nv = 0;
        
        // Find all edges connected to this vertex
        for (int e = 0; e < edgeCount; e++) {
            if (edges[e].v1 == v) {
                p->idx[p->nv++] = edges[e].new_v1;
            } else if (edges[e].v2 == v) {
                p->idx[p->nv++] = edges[e].new_v2;
            }
        }
        
        if (p->nv != 5) {
            printf("ERROR: Vertex %d has %d cuts, expected 5\n", v, p->nv);
            return;
        }
        
        sort_pentagon(pi);
        pi++;
    }
    
    // 5) Generate edge list for wireframe rendering
    int total_edges = 0;
    // Count edges: 12 pentagons * 5 edges + 20 hexagons * 6 edges
    total_edges = 12 * 5 + 20 * 6;
    
    // Allocate memory for output
    *out_verts = (vec3_t*)malloc(vertCount * sizeof(vec3_t));
    *out_edges = (int(*)[2])malloc(total_edges * sizeof(int[2]));
    
    if (!*out_verts || !*out_edges) {
        printf("ERROR: Memory allocation failed\n");
        return;
    }
    
    // Copy vertices
    for (int i = 0; i < vertCount; i++) {
        (*out_verts)[i] = verts[i];
    }
    *out_vert_count = vertCount;
    
    // Generate edges
    int edge_idx = 0;
    
    // Pentagon edges
    for (int i = 0; i < 12; i++) {
        for (int j = 0; j < 5; j++) {
            (*out_edges)[edge_idx][0] = pens[i].idx[j];
            (*out_edges)[edge_idx][1] = pens[i].idx[(j + 1) % 5];
            edge_idx++;
        }
    }
    
    // Hexagon edges
    for (int i = 0; i < 20; i++) {
        for (int j = 0; j < 6; j++) {
            (*out_edges)[edge_idx][0] = hexes[i].idx[j];
            (*out_edges)[edge_idx][1] = hexes[i].idx[(j + 1) % 6];
            edge_idx++;
        }
    }
    
    *out_edge_count = edge_idx;
    
    printf("Generated soccer ball: %d vertices, %d edges\n", vertCount, edge_idx);
    
    #undef MAX_EDGES_SB
    #undef MAX_VERTS_SB
    #undef MAX_FACES_SB
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

    
    // Draw a simple cross to test basic functionality
    float center_x = canvas->width / 2.0f;
    float center_y = canvas->height / 2.0f;
    
    //draw_line_f(canvas, center_x - 50, center_y, center_x + 50, center_y, 2.0f); // horizontal
    //draw_line_f(canvas, center_x, center_y - 50, center_x, center_y + 50, 2.0f); // vertical
    
    //canvas_save_pgm(canvas, "test_cross.pgm");
    //printf("Simple cross saved to test_cross.pgm\n");

    // --- Test 2: Clock face (original test) ---
    printf("\n=== Clock face ===\n");
    
    canvas_clear(canvas);
    
    float radius = 100.0f;
    for (int angle_deg = 0; angle_deg < 360; angle_deg += 15) {  // Every 15 degrees for clearer debug
        float angle_rad = angle_deg * M_PI / 180.0f;
        float end_x = center_x + radius * cosf(angle_rad);
        float end_y = center_y + radius * sinf(angle_rad);
        draw_line_f(canvas, center_x, center_y, end_x, end_y, 1.5f);
        printf("Drew line to angle %d degrees: (%.1f, %.1f)\n", angle_deg, end_x, end_y);
    }

    canvas_save_pgm(canvas, "Clock.pgm");
    printf("Clock saved to Clock.pgm\n");

    // --- Test 3: 3D Soccer Ball Wireframe ---
    printf("\n=== 3D Soccer Ball Wireframe ===\n");
    
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
    system("mkdir frames");

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