#include "math3d.h"
#include <stdio.h>

// Cube vertices (unit cube centered at origin)
vec3_t cube_vertices[8];

// Cube faces (indices into vertices array)
int cube_faces[6][4] = {
    {0, 1, 2, 3}, // back face
    {4, 7, 6, 5}, // front face
    {0, 4, 5, 1}, // bottom face
    {3, 2, 6, 7}, // top face
    {0, 3, 7, 4}, // left face
    {1, 5, 6, 2}  // right face
};

void initialize_cube() {
    // Initialize cube vertices properly
    float coords[8][3] = {
        {-0.5f, -0.5f, -0.5f}, // 0: back-bottom-left
        { 0.5f, -0.5f, -0.5f}, // 1: back-bottom-right
        { 0.5f,  0.5f, -0.5f}, // 2: back-top-right
        {-0.5f,  0.5f, -0.5f}, // 3: back-top-left
        {-0.5f, -0.5f,  0.5f}, // 4: front-bottom-left
        { 0.5f, -0.5f,  0.5f}, // 5: front-bottom-right
        { 0.5f,  0.5f,  0.5f}, // 6: front-top-right
        {-0.5f,  0.5f,  0.5f}  // 7: front-top-left
    };
    
    for (int i = 0; i < 8; i++) {
        cube_vertices[i] = vec3_create(coords[i][0], coords[i][1], coords[i][2]);
    }
}
    printf("=== %s ===\n", title);
    for (int i = 0; i < 8; i++) {
        printf("Vertex %d: (%.3f, %.3f, %.3f)\n", 
               i, vertices[i].x, vertices[i].y, vertices[i].z);
    }
    printf("\n");
}

void print_cube_vertices(vec3_t* vertices, const char* title) {
    for (int i = 0; i < 8; i++) {
        output[i] = mat4_transform_point(transform, input[i]);
    }
}

void test_vector_operations() {
    printf("=== VECTOR OPERATIONS TEST ===\n\n");
    
    // Test basic vector creation
    vec3_t v1 = vec3_create(1.0f, 0.0f, 0.0f);
    vec3_t v2 = vec3_create(0.0f, 1.0f, 0.0f);
    
    printf("Vector 1: ");
    vec3_print(v1);
    printf("Vector 2: ");
    vec3_print(v2);
    
    // Test spherical coordinates
    vec3_t v3 = vec3_from_spherical(2.0f, deg_to_rad(45.0f), deg_to_rad(60.0f));
    printf("From spherical (r=2, θ=45°, φ=60°): ");
    vec3_print(v3);
    
    // Test normalization
    vec3_t v1_norm = vec3_normalize_fast(v1);
    printf("Normalized v1: ");
    vec3_print(v1_norm);
    
    // Test SLERP
    vec3_t slerp_result = vec3_slerp(v1, v2, 0.5f);
    printf("SLERP between v1 and v2 (t=0.5): ");
    vec3_print(slerp_result);
    
    printf("\n");
}

void test_matrix_operations() {
    printf("=== MATRIX OPERATIONS TEST ===\n\n");
    
    // Test identity matrix
    mat4_t identity = mat4_identity();
    printf("Identity Matrix:\n");
    mat4_print(identity);
    
    // Test translation
    mat4_t translation = mat4_translate(2.0f, 1.0f, -3.0f);
    printf("Translation Matrix (2, 1, -3):\n");
    mat4_print(translation);
    
    // Test scaling
    mat4_t scaling = mat4_scale(2.0f, 0.5f, 1.5f);
    printf("Scale Matrix (2, 0.5, 1.5):\n");
    mat4_print(scaling);
    
    // Test rotation
    mat4_t rotation = mat4_rotate_xyz(45.0f, 30.0f, 60.0f);
    printf("Rotation Matrix (45°, 30°, 60°):\n");
    mat4_print(rotation);
    
    // Test frustum projection
    mat4_t frustum = mat4_frustum_asymmetric(-1.0f, 1.0f, -1.0f, 1.0f, 1.0f, 10.0f);
    printf("Frustum Projection Matrix:\n");
    mat4_print(frustum);
    
    printf("\n");
}

void demo_cube_transformations() {
    printf("=== CUBE TRANSFORMATION DEMO ===\n\n");
    
    // Initialize cube vertices
    initialize_cube();
    
    vec3_t transformed_vertices[8];
    
    // Original cube
    print_cube_vertices(cube_vertices, "Original Cube");
    
    // 1. Translation
    mat4_t translate_matrix = mat4_translate(2.0f, 1.0f, 0.0f);
    transform_cube(cube_vertices, transformed_vertices, translate_matrix);
    print_cube_vertices(transformed_vertices, "Translated Cube (2, 1, 0)");
    
    // 2. Scaling
    mat4_t scale_matrix = mat4_scale(2.0f, 1.5f, 0.5f);
    transform_cube(cube_vertices, transformed_vertices, scale_matrix);
    print_cube_vertices(transformed_vertices, "Scaled Cube (2, 1.5, 0.5)");
    
    // 3. Rotation
    mat4_t rotate_matrix = mat4_rotate_xyz(45.0f, 30.0f, 0.0f);
    transform_cube(cube_vertices, transformed_vertices, rotate_matrix);
    print_cube_vertices(transformed_vertices, "Rotated Cube (45°, 30°, 0°)");
    
    // 4. Combined transformation (Scale -> Rotate -> Translate)
    mat4_t combined = mat4_multiply(translate_matrix, 
                                   mat4_multiply(rotate_matrix, scale_matrix));
    transform_cube(cube_vertices, transformed_vertices, combined);
    print_cube_vertices(transformed_vertices, "Combined Transform (Scale->Rotate->Translate)");
    
    // 5. Perspective projection
    mat4_t projection = mat4_frustum_asymmetric(-1.0f, 1.0f, -1.0f, 1.0f, 2.0f, 10.0f);
    
    // Move cube away from camera first
    mat4_t move_back = mat4_translate(0.0f, 0.0f, -5.0f);
    mat4_t view_transform = mat4_multiply(projection, move_back);
    
    transform_cube(cube_vertices, transformed_vertices, view_transform);
    print_cube_vertices(transformed_vertices, "Perspective Projected Cube");
}

void demo_animation_simulation() {
    printf("=== ANIMATION SIMULATION ===\n\n");
    
    vec3_t start_dir = vec3_create(1.0f, 0.0f, 0.0f);
    vec3_t end_dir = vec3_create(0.0f, 1.0f, 0.0f);
    
    printf("Animating rotation from X-axis to Y-axis:\n");
    for (int frame = 0; frame <= 10; frame++) {
        float t = frame / 10.0f;
        vec3_t current = vec3_slerp(start_dir, end_dir, t);
        printf("Frame %2d (t=%.1f): ", frame, t);
        vec3_print(current);
    }
    printf("\n");
}

void benchmark_operations() {
    printf("=== PERFORMANCE BENCHMARK ===\n\n");
    
    // This is a simple demonstration - in real applications you'd use proper timing
    vec3_t test_vec = vec3_create(3.0f, 4.0f, 5.0f);
    
    printf("Testing fast inverse square root vs standard:\n");
    printf("Vector magnitude: %.6f\n", vec3_magnitude(test_vec));
    
    vec3_t normalized_fast = vec3_normalize_fast(test_vec);
    printf("Fast normalized: ");
    vec3_print(normalized_fast);
    
    float standard_inv_sqrt = 1.0f / sqrtf(test_vec.x * test_vec.x + test_vec.y * test_vec.y + test_vec.z * test_vec.z);
    printf("Standard 1/sqrt result: %.6f\n", standard_inv_sqrt);
    printf("Fast inv_sqrt result: %.6f\n", fast_inv_sqrt(test_vec.x * test_vec.x + test_vec.y * test_vec.y + test_vec.z * test_vec.z));
    printf("\n");
}

int main() {
    printf("3D MATH ENGINE DEMONSTRATION\n");
    printf("============================\n\n");
    
    // Initialize cube vertices first
    initialize_cube();
    
    test_vector_operations();
    test_matrix_operations();
    demo_cube_transformations();
    demo_animation_simulation();
    benchmark_operations();
    
    printf("Demo completed successfully!\n");
    return 0;
}