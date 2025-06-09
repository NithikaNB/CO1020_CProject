// test_math.c

#include <stdio.h>
#include "math3d.h"
#include "canvas.h"

// Apply transformation and perspective projection
vec3_t mat4_mul_vec3(const mat4_t* m, vec3_t v) {
    float x = v.x, y = v.y, z = v.z;

    float w =
        m->m[3] * x +
        m->m[7] * y +
        m->m[11] * z +
        m->m[15] * 1.0f;

    vec3_t result = {
        m->m[0] * x + m->m[4] * y + m->m[8]  * z + m->m[12],
        m->m[1] * x + m->m[5] * y + m->m[9]  * z + m->m[13],
        m->m[2] * x + m->m[6] * y + m->m[10] * z + m->m[14]
    };

    if (w != 0.0f) {
        result.x /= w;
        result.y /= w;
        result.z /= w;
    }

    vec3_update_spherical(&result);
    return result;
}

// Convert from NDC to canvas pixel coordinates
void ndc_to_canvas(float ndc_x, float ndc_y, int width, int height, float* cx, float* cy) {
    *cx = (ndc_x + 1.0f) * 0.5f * (width - 1);
    *cy = (1.0f - (ndc_y + 1.0f) * 0.5f) * (height - 1);
}

// Project cube with full transform (including projection) and draw
void draw_projected_cube(const char* filename, mat4_t full_transform, vec3_t cube_verts[8], int edges[12][2], int width, int height) {
    canvas_t* canvas = canvas_create(width, height);
    canvas_clear(canvas);

    vec3_t projected[8];
    for (int i = 0; i < 8; i++) {
        projected[i] = mat4_mul_vec3(&full_transform, cube_verts[i]);
    }

    for (int i = 0; i < 12; i++) {
        vec3_t v0 = projected[edges[i][0]];
        vec3_t v1 = projected[edges[i][1]];

        float x0, y0, x1, y1;
        ndc_to_canvas(v0.x, v0.y, width, height, &x0, &y0);
        ndc_to_canvas(v1.x, v1.y, width, height, &x1, &y1);

        draw_line_f(canvas, x0, y0, x1, y1, 1.5f);
    }

    canvas_save_pgm(canvas, filename);
    canvas_destroy(canvas);
    printf("Saved %s\n", filename);
}

int main() {
    const int width = 256, height = 256;

    // Cube vertices
    vec3_t cube_verts[8] = {
        vec3_from_cartesian(-1, -1, -1),
        vec3_from_cartesian( 1, -1, -1),
        vec3_from_cartesian( 1,  1, -1),
        vec3_from_cartesian(-1,  1, -1),
        vec3_from_cartesian(-1, -1,  1),
        vec3_from_cartesian( 1, -1,  1),
        vec3_from_cartesian( 1,  1,  1),
        vec3_from_cartesian(-1,  1,  1),
    };

    // Cube edges
    int edges[12][2] = {
        {0,1},{1,2},{2,3},{3,0},
        {4,5},{5,6},{6,7},{7,4},
        {0,4},{1,5},{2,6},{3,7}
    };

    // Projection matrix
    mat4_t proj = mat4_frustum(-1, 1, -1, 1, 1, 10);

    // Individual transforms
    mat4_t scale     = mat4_scale(1.5f, 1.0f, 0.5f);
    mat4_t rotate    = mat4_rotate_xyz(0.7f, 1.0f, 0.0f);
    mat4_t translate = mat4_translate(0.0f, 0.0f, -5.0f);

    // Compose: P * (S), P * (R), P * (T)
    draw_projected_cube("projected_scale.pgm", mat4_multiply(proj, scale), cube_verts, edges, width, height);
    draw_projected_cube("projected_rotate.pgm", mat4_multiply(proj, rotate), cube_verts, edges, width, height);
    draw_projected_cube("projected_translate.pgm", mat4_multiply(proj, translate), cube_verts, edges, width, height);

    // Model = T * R * S
    mat4_t model = mat4_multiply(translate, mat4_multiply(rotate, scale));

    draw_projected_cube("projected_model.pgm", mat4_multiply(proj, model), cube_verts, edges, width, height);

    // MVP final
    draw_projected_cube("projected_mvp.pgm", mat4_multiply(proj, model), cube_verts, edges, width, height);

    return 0;
}
