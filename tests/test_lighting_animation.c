#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#ifdef _WIN32
#include <direct.h>
#define mkdir(path, mode) _mkdir(path)
#ifndef M_PI
#define M_PI 3.14159265358979323846
#define RESOLUTION 800
#endif
#endif
#include "canvas.h"
#include "math3d.h"
#include "renderer.h"
#include "lighting.h"
#include "animation.h"

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


// Generate cube vertices and edges
void generate_cube(vec3_t** out_verts, int* out_vert_count, int (**out_edges)[2], int* out_edge_count) {
    *out_vert_count = 8;
    *out_edge_count = 12;
    
    *out_verts = (vec3_t*)malloc(8 * sizeof(vec3_t));
    *out_edges = (int(*)[2])malloc(12 * sizeof(int[2]));
    
    // Cube vertices
    (*out_verts)[0] = vec3_from_cartesian(-1.0f, -1.0f, -1.0f);
    (*out_verts)[1] = vec3_from_cartesian( 1.0f, -1.0f, -1.0f);
    (*out_verts)[2] = vec3_from_cartesian( 1.0f,  1.0f, -1.0f);
    (*out_verts)[3] = vec3_from_cartesian(-1.0f,  1.0f, -1.0f);
    (*out_verts)[4] = vec3_from_cartesian(-1.0f, -1.0f,  1.0f);
    (*out_verts)[5] = vec3_from_cartesian( 1.0f, -1.0f,  1.0f);
    (*out_verts)[6] = vec3_from_cartesian( 1.0f,  1.0f,  1.0f);
    (*out_verts)[7] = vec3_from_cartesian(-1.0f,  1.0f,  1.0f);
    
    // Cube edges
    int cube_edges[12][2] = {
        {0, 1}, {1, 2}, {2, 3}, {3, 0},  // bottom face
        {4, 5}, {5, 6}, {6, 7}, {7, 4},  // top face
        {0, 4}, {1, 5}, {2, 6}, {3, 7}   // vertical edges
    };
    
    for (int i = 0; i < 12; i++) {
        (*out_edges)[i][0] = cube_edges[i][0];
        (*out_edges)[i][1] = cube_edges[i][1];
    }
}

// Generate tetrahedron vertices and edges
void generate_tetrahedron(vec3_t** out_verts, int* out_vert_count, int (**out_edges)[2], int* out_edge_count) {
    *out_vert_count = 4;
    *out_edge_count = 6;
    
    *out_verts = (vec3_t*)malloc(4 * sizeof(vec3_t));
    *out_edges = (int(*)[2])malloc(6 * sizeof(int[2]));
    
    // Tetrahedron vertices
    float a = 1.0f / sqrtf(3.0f);
    (*out_verts)[0] = vec3_from_cartesian( a,  a,  a);
    (*out_verts)[1] = vec3_from_cartesian(-a, -a,  a);
    (*out_verts)[2] = vec3_from_cartesian(-a,  a, -a);
    (*out_verts)[3] = vec3_from_cartesian( a, -a, -a);
    
    // Tetrahedron edges
    int tetra_edges[6][2] = {
        {0, 1}, {0, 2}, {0, 3},
        {1, 2}, {1, 3}, {2, 3}
    };
    
    for (int i = 0; i < 6; i++) {
        (*out_edges)[i][0] = tetra_edges[i][0];
        (*out_edges)[i][1] = tetra_edges[i][1];
    }
}

void draw_light_sources(canvas_t* canvas, light_t* lights, int light_count, mat4_t mvp) {
    for (int i = 0; i < light_count; i++) {
        if (lights[i].intensity <= 0.0f) continue;

        vec3_t screen_pos = project_vertex(lights[i].position, mvp, canvas->width, canvas->height);

        if (clip_to_circular_viewport(canvas, screen_pos.x, screen_pos.y)) {
            draw_circle(canvas, (int)screen_pos.x, (int)screen_pos.y, 4, 255);  // 255 = white intensity
        }
    }
}
static vec3_t mat4_transform_point(mat4_t m, vec3_t v) {
    float x = v.x, y = v.y, z = v.z;

    float xp = m.m[0] * x + m.m[4] * y + m.m[8]  * z + m.m[12];
    float yp = m.m[1] * x + m.m[5] * y + m.m[9]  * z + m.m[13];
    float zp = m.m[2] * x + m.m[6] * y + m.m[10] * z + m.m[14];

    return (vec3_t){xp, yp, zp};
}



// Fixed lighting setup function
void setup_single_dramatic_light(light_t* lights) {
    // Strong primary light from front-right-top
    lights[0] = light_create(vec3_from_cartesian(6.0f, 4.0f, 5.0f), 
                           vec3_from_cartesian(1.0f, 1.0f, 1.0f), 2.0f);
    
    // Weaker secondary light from opposite side
    lights[1] = light_create(vec3_from_cartesian(-3.0f, 1.0f, -2.0f), 
                           vec3_from_cartesian(0.8f, 0.8f, 1.0f), 0.5f);
    
    // Disable third light
    lights[2] = light_create(vec3_from_cartesian(0.0f, 0.0f, 0.0f), 
                           vec3_from_cartesian(1.0f, 1.0f, 1.0f), 0.0f);
}



// CORRECTED: Proper wireframe rendering with Lambert lighting
void render_wireframe_with_dramatic_lighting(canvas_t* canvas, vec3_t* verts, int vert_count, 
                                           int edges[][2], int edge_count, mat4_t mvp, 
                                           light_t* lights, int light_count) {
    // Project vertices to screen space
    vec3_t* screen_verts = (vec3_t*)malloc(vert_count * sizeof(vec3_t));
    for (int i = 0; i < vert_count; i++) {
        screen_verts[i] = project_vertex(verts[i], mvp, canvas->width, canvas->height);
    }
    
    // Render each edge with proper lighting
    for (int i = 0; i < edge_count; i++) {
        int i0 = edges[i][0];
        int i1 = edges[i][1];
        
        if (i0 >= 0 && i0 < vert_count && i1 >= 0 && i1 < vert_count) {
            // Calculate lighting intensity using corrected Lambert lighting
            //float intensity = calculate_edge_lighting(verts[i0], verts[i1], lights, light_count);
           float intensity = calculate_edge_lighting(verts[i0], verts[i1], lights, light_count);
            intensity = intensity * 1.5f; // Amplify intensity for better visibility
            intensity = fmin(intensity, 1.0f); // Clamp to [0, 1]
            
            // Map lighting intensity to line thickness
            // Base thickness + variable thickness based on lighting
            float thickness = 0.5f + 3.0f * intensity; // Range: 0.5 to 3.5
            
            // Get screen coordinates
            vec3_t v0 = screen_verts[i0];
            vec3_t v1 = screen_verts[i1];
            
            // Check if at least one vertex is visible in the circular viewport
            if (clip_to_circular_viewport(canvas, v0.x, v0.y) || 
                clip_to_circular_viewport(canvas, v1.x, v1.y)) {
                
                // Draw the line with thickness based on lighting
                draw_line_f(canvas, v0.x, v0.y, v1.x, v1.y, thickness);
            }
        }
    }
    
    free(screen_verts);
}


int main() {
    struct stat st = {0};
    if (stat("frames", &st) == -1) {
        mkdir("frames", 0755);
    }

    const int FPS = 30;
    const int DURATION_SECONDS = 15;
    const int TOTAL_FRAMES = FPS * DURATION_SECONDS;
    const float FRAME_TIME = 1.0f / FPS;

    const int WIDTH = RESOLUTION;
    const int HEIGHT = RESOLUTION;

    printf("Generating %d frames for %d seconds at %d fps...\n", TOTAL_FRAMES, DURATION_SECONDS, FPS);

    canvas_t* canvas = canvas_create(WIDTH, HEIGHT);
    if (!canvas) {
        printf("Failed to create canvas\n");
        return 1;
    }

    vec3_t* soccer_verts;
    int soccer_vert_count;
    int (*soccer_edges)[2];
    int soccer_edge_count;
    generate_soccer_ball(&soccer_verts, &soccer_vert_count, &soccer_edges, &soccer_edge_count);

    vec3_t* cube_verts;
    int cube_vert_count;
    int (*cube_edges)[2];
    int cube_edge_count;
    generate_cube(&cube_verts, &cube_vert_count, &cube_edges, &cube_edge_count);

    vec3_t* tetra_verts;
    int tetra_vert_count;
    int (*tetra_edges)[2];
    int tetra_edge_count;
    generate_tetrahedron(&tetra_verts, &tetra_vert_count, &tetra_edges, &tetra_edge_count);

    // Setup lighting
    light_t lights[3];
    setup_single_dramatic_light(lights);

    // Create animation paths
    animation_path_t soccer_path = path_create(
        vec3_from_cartesian(-4.0f, 0.0f, 0.0f),
        vec3_from_cartesian(-2.0f, 3.0f, 2.0f),
        vec3_from_cartesian(2.0f, -2.0f, 1.0f),
        vec3_from_cartesian(4.0f, 0.0f, 0.0f),
        DURATION_SECONDS
    );

    animation_path_t cube_path = path_create(
        vec3_from_cartesian(0.0f, 3.0f, 0.0f),
        vec3_from_cartesian(3.0f, 2.0f, -2.0f),
        vec3_from_cartesian(-2.0f, -1.0f, 2.0f),
        vec3_from_cartesian(0.0f, -3.0f, 0.0f),
        DURATION_SECONDS
    );

    animation_path_t tetra_path = path_create(
        vec3_from_cartesian(0.0f, 0.0f, 3.0f),
        vec3_from_cartesian(-3.0f, 0.0f, 1.0f),
        vec3_from_cartesian(1.0f, 2.0f, -1.0f),
        vec3_from_cartesian(0.0f, 0.0f, -3.0f),
        DURATION_SECONDS
    );

    // Setup projection and view matrices
    mat4_t projection = mat4_frustum(-2.5f, 2.5f, -2.0f, 2.0f, 3.0f, 20.0f);
    mat4_t view = mat4_translate(0.0f, 0.0f, -10.0f);

    // Transform lights into view space once
    light_t lights_in_view[3];
    for (int i = 0; i < 3; i++) {
        vec3_t pos = lights[i].position;
        vec3_t pos_in_view = mat4_transform_point(view, pos);
        lights_in_view[i] = lights[i];
        lights_in_view[i].position = pos_in_view;
    }

    // Main animation loop
    for (int frame = 0; frame < TOTAL_FRAMES; frame++) {
        float time = frame * FRAME_TIME;
        canvas_clear(canvas);

        // Get positions from animation paths
        vec3_t soccer_pos = path_evaluate(soccer_path, time);
        vec3_t cube_pos = path_evaluate(cube_path, time);
        vec3_t tetra_pos = path_evaluate(tetra_path, time);

        // Calculate rotations
        vec3_t rotation_soccer = vec3_from_cartesian(time * 2.0f, time * 1.5f, time * 1.0f);
        vec3_t rotation_cube = vec3_from_cartesian(0.0f, time * 1.5f, 0.0f);
        vec3_t rotation_tetra = vec3_from_cartesian(time, time, time);

        // Render soccer ball
        mat4_t soccer_model = mat4_multiply(
            mat4_translate(soccer_pos.x, soccer_pos.y, soccer_pos.z),
            mat4_rotate_xyz(rotation_soccer.x, rotation_soccer.y, rotation_soccer.z)
        );
        mat4_t soccer_mvp = mat4_multiply(projection, mat4_multiply(view, soccer_model));

        vec3_t* soccer_transformed = (vec3_t*)malloc(soccer_vert_count * sizeof(vec3_t));


        // for (int i = 0; i < soccer_vert_count; i++) {
        //     vec3_t v = soccer_verts[i];
        //     // Apply rotation
        //     float cy = cosf(rotation_soccer.y), sy = sinf(rotation_soccer.y);
        //     float x = v.x * cy + v.z * sy;
        //     float z = -v.x * sy + v.z * cy;
        //     v.x = x + soccer_pos.x;
        //     v.y += soccer_pos.y;
        //     v.z = z + soccer_pos.z;
        //     soccer_transformed[i] = v;
        // }

            for (int i = 0; i < soccer_vert_count; i++) {
        vec3_t v = soccer_verts[i];
        // Apply full 3D rotation
        float cx = cosf(rotation_soccer.x), sx = sinf(rotation_soccer.x);
        float cy = cosf(rotation_soccer.y), sy = sinf(rotation_soccer.y);
        float cz = cosf(rotation_soccer.z), sz = sinf(rotation_soccer.z);
        
        vec3_t temp = v;
        // X rotation
        v.y = temp.y * cx - temp.z * sx;
        v.z = temp.y * sx + temp.z * cx;
        temp = v;
        // Y rotation
        v.x = temp.x * cy + temp.z * sy;
        v.z = -temp.x * sy + temp.z * cy;
        temp = v;
        // Z rotation
        v.x = temp.x * cz - temp.y * sz;
        v.y = temp.x * sz + temp.y * cz;
        
        // Apply translation
        v.x += soccer_pos.x;
        v.y += soccer_pos.y;
        v.z += soccer_pos.z;
        soccer_transformed[i] = v;
    }

        render_wireframe_with_dramatic_lighting(canvas, soccer_transformed, soccer_vert_count,
                                              soccer_edges, soccer_edge_count, soccer_mvp, lights_in_view, 2);
        free(soccer_transformed);

        // Render cube
        mat4_t cube_model = mat4_multiply(
            mat4_translate(cube_pos.x, cube_pos.y, cube_pos.z),
            mat4_rotate_xyz(rotation_cube.x, rotation_cube.y, rotation_cube.z)
        );
        mat4_t cube_mvp = mat4_multiply(projection, mat4_multiply(view, cube_model));

        vec3_t* cube_transformed = (vec3_t*)malloc(cube_vert_count * sizeof(vec3_t));
        for (int i = 0; i < cube_vert_count; i++) {
            vec3_t v = cube_verts[i];
            float c = cosf(rotation_cube.y), s = sinf(rotation_cube.y);
            float x = v.x * c + v.z * s;
            float z = -v.x * s + v.z * c;
            v.x = x + cube_pos.x;
            v.y += cube_pos.y;
            v.z = z + cube_pos.z;
            cube_transformed[i] = v;
        }
        render_wireframe_with_dramatic_lighting(canvas, cube_transformed, cube_vert_count,
                                              cube_edges, cube_edge_count, cube_mvp, lights_in_view, 2);
        free(cube_transformed);

        // Render tetrahedron
        mat4_t tetra_model = mat4_multiply(
            mat4_translate(tetra_pos.x, tetra_pos.y, tetra_pos.z),
            mat4_rotate_xyz(rotation_tetra.x, rotation_tetra.y, rotation_tetra.z)
        );
        mat4_t tetra_mvp = mat4_multiply(projection, mat4_multiply(view, tetra_model));

        vec3_t* tetra_transformed = (vec3_t*)malloc(tetra_vert_count * sizeof(vec3_t));
        for (int i = 0; i < tetra_vert_count; i++) {
            vec3_t v = tetra_verts[i];
            // Apply full 3D rotation
            float cx = cosf(rotation_tetra.x), sx = sinf(rotation_tetra.x);
            float cy = cosf(rotation_tetra.y), sy = sinf(rotation_tetra.y);
            float cz = cosf(rotation_tetra.z), sz = sinf(rotation_tetra.z);
            
            vec3_t temp = v;
            // X rotation
            v.y = temp.y * cx - temp.z * sx;
            v.z = temp.y * sx + temp.z * cx;
            temp = v;
            // Y rotation
            v.x = temp.x * cy + temp.z * sy;
            v.z = -temp.x * sy + temp.z * cy;
            temp = v;
            // Z rotation
            v.x = temp.x * cz - temp.y * sz;
            v.y = temp.x * sz + temp.y * cz;
            
            // Apply translation
            v.x += tetra_pos.x;
            v.y += tetra_pos.y;
            v.z += tetra_pos.z;
            tetra_transformed[i] = v;
        }
        render_wireframe_with_dramatic_lighting(canvas, tetra_transformed, tetra_vert_count,
                                              tetra_edges, tetra_edge_count, tetra_mvp, lights_in_view, 2);
        free(tetra_transformed);

        // Save frame
        char filename[256];
        snprintf(filename, sizeof(filename), "frames/frame_%04d.pgm", frame);
        //draw_light_sources(canvas, lights_in_view, 3, mat4_multiply(projection, view));
        canvas_save_pgm(canvas, filename);

        // Progress update
        if (frame % 30 == 0) {
            printf("Generated frame %d/%d (%.1f%%)...\n", frame, TOTAL_FRAMES,
                   100.0f * frame / TOTAL_FRAMES);
        }
    }

    printf("Animation complete! Generated %d frames.\n", TOTAL_FRAMES);
    printf("To create video: ffmpeg -r %d -i frames/frame_%%04d.pgm -vcodec libx264 -pix_fmt yuv420p output.mp4\n", FPS);

    // Cleanup
    canvas_destroy(canvas);
    free(soccer_verts);
    free(soccer_edges);
    free(cube_verts);
    free(cube_edges);
    free(tetra_verts);
    free(tetra_edges);

    return 0;
}