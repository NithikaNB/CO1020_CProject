#ifndef MATH3D_H
#define MATH3D_H

#include <stdio.h>
#include <math.h>

// --- vec3 type ---

typedef struct {
    float x, y, z;      // Cartesian coordinates
    float r, theta, phi; // Spherical coordinates (r, polar angle θ, azimuthal angle φ)
} vec3_t;

// vec3 functions
vec3_t vec3_from_cartesian(float x, float y, float z);
vec3_t vec3_from_spherical(float r, float theta, float phi);
void vec3_update_spherical(vec3_t* v);
void vec3_update_cartesian(vec3_t* v);
vec3_t vec3_normalize_fast(vec3_t v);
vec3_t vec3_slerp(vec3_t a, vec3_t b, float t);
void vec3_print_cartesian(const vec3_t* v);
void vec3_print_spherical(const vec3_t* v);

// --- mat4 type ---

typedef struct {
    float m[16]; // 4x4 matrix in column-major order
} mat4_t;

// mat4 functions
mat4_t mat4_identity(void);
mat4_t mat4_translate(float tx, float ty, float tz);
mat4_t mat4_scale(float sx, float sy, float sz);
mat4_t mat4_rotate_xyz(float rx, float ry, float rz);
mat4_t mat4_frustum(float left, float right, float bottom, float top, float near, float far);


// Matrix multiplication (4x4)
mat4_t mat4_multiply(mat4_t a, mat4_t b);


#endif // MATH3D_H
