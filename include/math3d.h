#ifndef MATH3D_H
#define MATH3D_H

#include <math.h>
#include <stdio.h>

// 3D Vector structure with both Cartesian and spherical coordinates
typedef struct {
    // Cartesian coordinates
    float x, y, z;
    
    // Spherical coordinates (automatically updated)
    float r;     // radius/magnitude
    float theta; // azimuthal angle (around z-axis, from x-axis)
    float phi;   // polar angle (from z-axis)
    
    // Internal flag to track which coordinate system was last modified
    int _cartesian_dirty;
    int _spherical_dirty;
} vec3_t;

// 4x4 Matrix structure (column-major layout)
typedef struct {
    float m[16]; // Column-major: m[col*4 + row]
} mat4_t;

// Vector operations
vec3_t vec3_create(float x, float y, float z);
vec3_t vec3_from_spherical(float r, float theta, float phi);
vec3_t vec3_normalize_fast(vec3_t v);
vec3_t vec3_slerp(vec3_t a, vec3_t b, float t);
void vec3_update_spherical(vec3_t* v);
void vec3_update_cartesian(vec3_t* v);
float vec3_magnitude(vec3_t v);
float vec3_dot(vec3_t a, vec3_t b);
vec3_t vec3_cross(vec3_t a, vec3_t b);
vec3_t vec3_add(vec3_t a, vec3_t b);
vec3_t vec3_subtract(vec3_t a, vec3_t b);
vec3_t vec3_scale(vec3_t v, float s);
void vec3_print(vec3_t v);

// Matrix operations
mat4_t mat4_identity(void);
mat4_t mat4_translate(float tx, float ty, float tz);
mat4_t mat4_scale(float sx, float sy, float sz);
mat4_t mat4_rotate_xyz(float rx, float ry, float rz);
mat4_t mat4_frustum_asymmetric(float left, float right, float bottom, float top, float near, float far);
mat4_t mat4_multiply(mat4_t a, mat4_t b);
vec3_t mat4_transform_point(mat4_t m, vec3_t v);
void mat4_print(mat4_t m);

// Utility functions
float fast_inv_sqrt(float x);
float deg_to_rad(float degrees);
float rad_to_deg(float radians);

// Constants
#define PI 3.14159265359f
#define TWO_PI 6.28318530718f
#define HALF_PI 1.57079632679f

#endif // MATH3D_H